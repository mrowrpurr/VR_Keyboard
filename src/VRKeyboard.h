#include <atomic>
#include <thread>
#include <windows.h>
#include <openvr.h>

// TODO - Separate into VRKeyboard.cpp

namespace VRKeyboard {

    enum InputSize { SingleLine = 0, MultiLine = 1 };
    enum InputMode { Normal = 0, Password = 1, KeepOpen = 2 };

    namespace {
        // Help prevent keyboard from auto-closing quickly
        // See SkyUI's notes on this at https://github.com/Odie/skyui-vr/blob/dd05b77f1d822afe28e7d7e286b2991b83046a93/sksevr_plugin/plugin/Keyboard.cpp#L32
        const DWORD overlayWaitMsBeforeActivatingKeyboard = 150;

        // Polling wait time for keyboard events when keyboard is open
        const DWORD keyboardEventPollingWaitIntervalMs = 50;

        // Maximum characters that can be entered into keyboard
        const uint32_t maxCharacters = 1024;

        // Unique key for the Open VR overlay used to show the VR keyboard
        const std::string_view overlayKey = "MP_SkyrimVRKeyboard"sv;

        // Friendly name for the Open VR overlay used to show the VR keyboard
        const std::string_view overlayName = "Skyrim VR Keyboard"sv;

        // Track whether the VR keyboard is already open (or in the process of being opened)
        std::atomic<bool> isOpen(false);

        // Is the overlay currently open?
        bool overlayVisible;

        // Context for Open VR operations, e.g. open/close overlay or show keyboard
        vr::COpenVRContext vrContext = vr::COpenVRContext();

        // Open VR Overlay handle
        vr::VROverlayHandle_t overlayHandle = 0;

        // Pointer of currently operating thread watching the keyboard (or nullptr)
        std::thread* keyboardThread = nullptr;

        // Small conversion from VRKeyboard enum to Open VR's value
        inline vr::EGamepadTextInputLineMode GetOpenVrInputSize(InputSize inputSize) {
            switch (inputSize) {
                case InputSize::SingleLine:
                    return vr::k_EGamepadTextInputLineModeSingleLine;
                    break;
                case InputSize::MultiLine:
                    return vr::k_EGamepadTextInputLineModeMultipleLines;
                    break;
                default:
                    return vr::k_EGamepadTextInputLineModeSingleLine;
            }
        }

        // Small conversion from VRKeyboard enum to Open VR's value
        // Details on Submit mode: https://chromium.googlesource.com/external/github.com/ValveSoftware/openvr/+/c95571027b79644643bca044538144c96194c4f2
        inline vr::EGamepadTextInputMode GetOpenVrInputMode(InputMode inputMode) {
            switch (inputMode) {
                case InputMode::Normal:
                    return vr::k_EGamepadTextInputModeNormal;
                    break;
                case InputMode::Password:
                    return vr::k_EGamepadTextInputModePassword;
                    break;
                case InputMode::KeepOpen:
                    return vr::k_EGamepadTextInputModeSubmit;
                    break;
                default:
                    return vr::k_EGamepadTextInputModeNormal;
            }
        }
    }

    // Closes the currently open overlay (if any) and cleans up.
    // Automatically called when the keyboard is closed.
    void Close() {
        if (isOpen) {
            keyboardThread->detach(); // <-- Should we .join() or .detach() ?
            delete keyboardThread;    // <-- Necessary to cleanup ?
            keyboardThread = nullptr; // <-- What will this be when deleted if not nullptr ?
            if (overlayVisible) {
                vrContext.VROverlay()->DestroyOverlay(overlayHandle);
                overlayHandle = 0;
                overlayVisible = false;
            }
            isOpen = false;
        }
    }

    namespace {
        // Function run in a thread to open keyboard and listen for keyboard events.
        void OpenKeyboardThread(std::string_view startingText, InputSize inputSize, InputMode inputMode, bool) {
            logger::info("OpenKeyboardAndListenForKeyboardEvents() with starting text {}", startingText);

            const auto vrInputSize = GetOpenVrInputSize(inputSize);
            logger::info("VR Input Size selected: {}", vrInputSize);
            const auto vrInputMode = GetOpenVrInputMode(inputMode);
            logger::info("VR Input Mode selected: {}", vrInputMode);
            const uint64_t userValue = 0; // <-- Still haven't figured out what this value is...
            const char* description = {}; //<-- Still haven't figured out what this value is...

            const auto error = vrContext.VROverlay()->ShowKeyboardForOverlay(overlayHandle, vrInputMode, vrInputSize, 0, description, maxCharacters, startingText.data(), userValue);
            if (error != vr::EVROverlayError::VROverlayError_None) {
                Close(); // For cleanup
                return;
            }
        }
    }

    // Open the keyboard and perform the provided function when keyboard entry is done.
    // bool Open(std::function<void (std::string)> callback, std::string_view startingText = ""sv ,InputSize inputSize = InputSize::SingleLine, InputMode inputMode = InputMode::Normal, bool sendKeystrokes = false) {
    bool Open(std::string_view startingText = ""sv ,InputSize inputSize = InputSize::SingleLine, InputMode inputMode = InputMode::Normal, bool sendKeystrokes = false) {
        logger::info("Open() VR Keyboard with starting text {}", startingText);

        bool wasOpen = isOpen.exchange(true);
        if (wasOpen) {
            logger::debug("VRKeyboard.Open() called but VR keyboard was already open."sv);
            isOpen = false;
            return false;
        }

        // https://github.com/ValveSoftware/openvr/wiki/IVROverlay%3A%3ACreateOverlay
        const auto error = vrContext.VROverlay()->CreateOverlay(overlayKey.data(), overlayName.data(), &overlayHandle);
        if (error != vr::EVROverlayError::VROverlayError_None) {
            logger::debug("VRKeyboard.Open() VROverlay()->CreateOverlay failed with error {}"sv, error);
            isOpen = false;
            return false;
        }

        overlayVisible = true;

        Sleep(overlayWaitMsBeforeActivatingKeyboard);
        
        keyboardThread = new std::thread(OpenKeyboardThread, startingText, inputSize, inputMode, sendKeystrokes);

        return true;
    };

    std::string PollForCompletedKeyboardText() {
        logger::info("Poll for VR Keyboard completed");
        if (! isOpen) return "";

        vr::VREvent_t event;
        std::string text;

        while (vrContext.VROverlay()->PollNextOverlayEvent(overlayHandle, &event, sizeof(vr::VREvent_t))) {
            if (event.eventType == vr::VREvent_KeyboardClosed || event.eventType == vr::VREvent_KeyboardDone) {
                char buffer[maxCharacters + 1] = "\0"; // + 1 for the terminating character
                vrContext.VROverlay()->GetKeyboardText(buffer, maxCharacters);
                text = std::string(buffer);
                logger::info("Got completed text from keyboard {} / Gonna return!", text);
                Close();
                logger::info("Returning!");
                return text;
            }
        }

        return "";
    };
}