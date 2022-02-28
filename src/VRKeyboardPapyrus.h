#include <format>
#include <RE/C/ConsoleLog.h>

#include "VRKeyboard.h"

namespace VRKeyboard {
    namespace Papyrus {

        bool OpenKeyboard(StaticFunctionTag*, std::string_view startingText) {
            return VRKeyboard::Open(startingText);
        }

        std::string PollForFinishedKeyboardEntry(StaticFunctionTag*) {
            return VRKeyboard::PollForCompletedKeyboardText();
        }

        bool BIND(RE::BSScript::IVirtualMachine* vm) {
            const auto className = "VRKeyboard";
            vm->RegisterFunction("OpenKeyboard", className, OpenKeyboard);
            vm->RegisterFunction("PollForFinishedKeyboardEntry", className, PollForFinishedKeyboardEntry);
            return true;
        }
    }
}