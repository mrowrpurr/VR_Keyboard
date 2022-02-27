#include <format>
#include <RE/C/ConsoleLog.h>

#include "VRKeyboard.h"

namespace VRKeyboard {
    namespace Papyrus {

        class MyCallback : public RE::BSScript::IStackCallbackFunctor {
            public:
                MyCallback() {}
                void operator()(RE::BSScript::Variable a_result) {
                    // DO SOMETHING!
                }
                bool CanSave() { return false; }
                void SetObject(const RE::BSTSmartPointer<RE::BSScript::Object>&) {}
        };

        void GetKeyboardInput(RE::BSScript::IVirtualMachine*, RE::VMStackID stackId, StaticFunctionTag*, std::string_view callbackFn) {
            logger::info("Invoked GetKeyboardInput()");

            auto* vm = RE::BSScript::Internal::VirtualMachine::GetSingleton();

            RE::BSSpinLockGuard lock(vm->runningStacksLock);

            const auto matchingStacks = vm->allRunningStacks.find(stackId);
            if (matchingStacks == vm->allRunningStacks.end()) return;
            const auto& stack = matchingStacks->second;
            auto variable = stack->top->previousFrame->self;

            #undef GetObject
            const auto& object = variable.GetObject();
            const auto className = object->GetTypeInfo()->GetName();
            const auto handle = object->GetHandle();

            auto callback = new MyCallback();
            auto callbackPtr = RE::BSTSmartPointer<RE::BSScript::IStackCallbackFunctor>(callback);

            VRKeyboard::Open([vm, handle, className, callbackFn, &callbackPtr] (std::string text) {
                logger::info("Hi from inside of lambda");
                logger::info("The text is: '{}'", text);
                const auto& consoleLog = RE::ConsoleLog::GetSingleton();
                consoleLog->Print(text.c_str());
                consoleLog->Print(std::format("Got text! '{}'", text).c_str());
                auto args = RE::FunctionArguments<std::string>(std::basic_string(text));
                vm->DispatchMethodCall(handle, className, callbackFn, &args, callbackPtr);
            });

            // vm->DispatchMethodCall(handle, className, callbackFn, &args, callbackPtr);
        };

        bool BIND(RE::BSScript::IVirtualMachine* vm) {
            auto className = "VRKeyboard";
            vm->RegisterFunction("GetKeyboardInput", className, GetKeyboardInput);
            logger::info("BIND GetKeyboardInput() OK");
            return true;
        };
    };
}