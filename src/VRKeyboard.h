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

        // Use normal input mode. Alternative modes include password entry and submit-style (for chat).
        const vr::EGamepadTextInputMode keyboardInputMode_Normal = vr::k_EGamepadTextInputModeNormal;

        // Use single-line input. Note: I wasn't able to get multi-line input working.
        const vr::EGamepadTextInputLineMode keyboardTextInput_SingleLine = vr::k_EGamepadTextInputLineModeSingleLine;

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
        void OpenKeyboardThread(std::string_view startingText) {
            const uint64_t userValue = 0; // <-- Still haven't figured out what this value is...
            const char* description = {}; //<-- Still haven't figured out what this value is...

            const auto error = vrContext.VROverlay()->ShowKeyboardForOverlay(overlayHandle, keyboardInputMode_Normal, keyboardTextInput_SingleLine, 0, description, maxCharacters, startingText.data(), userValue);
            if (error != vr::EVROverlayError::VROverlayError_None) {
                Close(); // For cleanup
                return;
            }
        }
    }

    // Open the keyboard and perform the provided function when keyboard entry is done.
    // bool Open(std::function<void (std::string)> callback, std::string_view startingText = ""sv ,InputSize inputSize = InputSize::SingleLine, InputMode inputMode = InputMode::Normal, bool sendKeystrokes = false) {
    bool Open(std::string_view startingText = ""sv) {
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
        
        keyboardThread = new std::thread(OpenKeyboardThread, startingText);

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
                Close();
                return text;
            }
        }

        return "";
    };
}