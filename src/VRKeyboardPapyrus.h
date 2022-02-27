#include <format>
#include <RE/C/ConsoleLog.h>

#include "VRKeyboard.h"

namespace VRKeyboard {
    namespace Papyrus {

        class MethodDispatchCallback : public RE::BSScript::IStackCallbackFunctor {
            public:
                MethodDispatchCallback() {}
                void operator()(RE::BSScript::Variable a_result) {
                    // DO SOMETHING!
                }
                bool CanSave() { return false; }
                void SetObject(const RE::BSTSmartPointer<RE::BSScript::Object>&) {}
        };

        // void GetKeyboardInputAsync(RE::BSScript::IVirtualMachine*, RE::VMStackID stackId, StaticFunctionTag*, std::string_view callbackFn, std::string_view startingText, std::string_view lineMode, std::string_view style) {
        //     logger::info("Invoked GetKeyboardInput() callback:'{}' startingText:'{}' lineMode:'{}' style:'{}'", callbackFn, startingText, lineMode, style);

        //     auto* vm = RE::BSScript::Internal::VirtualMachine::GetSingleton();

        //     RE::BSSpinLockGuard lock(vm->runningStacksLock);

        //     const auto matchingStacks = vm->allRunningStacks.find(stackId);
        //     if (matchingStacks == vm->allRunningStacks.end()) return;
        //     const auto& stack = matchingStacks->second;
        //     auto variable = stack->top->previousFrame->self;

        //     #undef GetObject
        //     const auto& object = variable.GetObject();
        //     const auto className = object->GetTypeInfo()->GetName();
        //     const auto handle = object->GetHandle();

        //     auto callback = new MethodDispatchCallback(); // TODO <-- rename me
        //     auto callbackPtr = RE::BSTSmartPointer<RE::BSScript::IStackCallbackFunctor>(callback);

        //     VRKeyboard::Open([vm, handle, className, callbackFn, &callbackPtr] (std::string text) {
        //         const auto& consoleLog = RE::ConsoleLog::GetSingleton();
        //         consoleLog->Print(text.c_str());
        //         consoleLog->Print(std::format("Got text! '{}'", text).c_str()); // TODO <--- remove log ehre
        //         auto args = RE::FunctionArguments<std::string>(std::basic_string(text));
        //         vm->DispatchMethodCall(handle, className, callbackFn, &args, callbackPtr);
        //     });
        // };

        const std::string_view inputSize_singleLine = "single"sv;
        const std::string_view inputSize_multiLine = "multi"sv;
        VRKeyboard::InputSize InputSizeForString(std::string_view text) {
            logger::info("Trying string converstion stuffs...");
            std::string lowercase = std::string(text.data());
            logger::info("Gonna make this lowercase: {}", text);
            std::transform(lowercase.begin(), lowercase.end(), lowercase.begin(), [](unsigned char c){ return std::tolower(c); });
            logger::info("Oh nevermind, I guess the transform worked.");
            if (lowercase == inputSize_singleLine) {
                return VRKeyboard::InputSize::SingleLine;
            } else if (lowercase == inputSize_multiLine) {
                return VRKeyboard::InputSize::MultiLine;
            } else {
                return VRKeyboard::InputSize::SingleLine;
            }
        }

        const std::string_view inputMode_normal = "normal"sv;
        const std::string_view inputMode_password = "password"sv;
        VRKeyboard::InputMode InputModeForString(std::string_view text) {
            logger::info("Trying more string converstion stuffs...");
            std::string lowercase = std::string(text.data());
            logger::info("Gonna make this lowercase: {}", text);
            std::transform(lowercase.begin(), lowercase.end(), lowercase.begin(), [](unsigned char c){ return std::tolower(c); });
            logger::info("Oh nevermind, I guess the transform worked.");
            if (lowercase == inputMode_normal) {
                return VRKeyboard::InputMode::Normal;
            } else if (lowercase == inputMode_password) {
                return VRKeyboard::InputMode::Password;
            } else {
                return VRKeyboard::InputMode::Normal;
            }
        }

        // bool function OpenKeyboard(string startingText = "", string lineMode = "single", string style = "normal") global native
        // bool OpenKeyboard(StaticFunctionTag*, std::string startingText = "", std::string lineMode = "single", std::string style = "normal") {
        bool OpenKeyboard(StaticFunctionTag*, std::string_view startingText, std::string_view lineMode, std::string_view style) {
            logger::info("Papyrus OpenKeyboard with starting text {} and line mode {} and style {}", startingText, lineMode, style);
            auto vrLineMode = InputSizeForString(lineMode);
            logger::info("In the middle! By the way, line mode is: {}", vrLineMode);
            auto vrStyle = InputModeForString(style);
            logger::info("Oh, I guess we made it this far? Also style is: {}", vrStyle);
            // return VRKeyboard::Open(startingText, InputSizeForString(lineMode), InputModeForString(style));
            return VRKeyboard::Open(startingText, vrLineMode, vrStyle);
        }

        std::string PollForFinishedKeyboardEntry(StaticFunctionTag*) {
            return VRKeyboard::PollForCompletedKeyboardText();
        }

        bool BIND(RE::BSScript::IVirtualMachine* vm) {
            const auto className = "VRKeyboard";
            vm->RegisterFunction("OpenKeyboard", className, OpenKeyboard);
            vm->RegisterFunction("PollForFinishedKeyboardEntry", className, PollForFinishedKeyboardEntry);
            logger::info("BIND GetKeyboardInput() OK");
            return true;
        };
    };
}