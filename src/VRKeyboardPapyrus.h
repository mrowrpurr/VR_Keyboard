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

        void GetInput(RE::BSScript::IVirtualMachine*, RE::VMStackID stackId, StaticFunctionTag*, std::string callbackFn) {
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

            auto args = RE::FunctionArguments<std::string>("Hello this is the keyboard text");
            auto callback = new MyCallback();
            auto callbackPtr = RE::BSTSmartPointer<RE::BSScript::IStackCallbackFunctor>(callback);

            // // Later... somewhere once we get the keyboard input... from another thread...
            vm->DispatchMethodCall(handle, className, callbackFn, &args, callbackPtr);
        };

        bool BIND(RE::BSScript::IVirtualMachine* vm) {
            auto className = "VRKeyboard";
            vm->RegisterFunction("GetInput", className, VRKeyboard::Papyrus::GetInput);
            return true;
        };
    };
}