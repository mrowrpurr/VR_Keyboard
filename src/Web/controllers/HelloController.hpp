#pragma once

#include <iostream>
#include <format>
#include <thread>
#include <windows.h>
#include <openvr.h>
#include <oatpp/web/server/api/ApiController.hpp>
#include <oatpp/core/macro/codegen.hpp>
#include <oatpp/core/macro/component.hpp>
#include <RE/C/ConsoleLog.h>

#include "Web/dtos/TestDto.hpp"

#include OATPP_CODEGEN_BEGIN(ApiController)

class HelloController : public oatpp::web::server::api::ApiController {
public:
  HelloController(OATPP_COMPONENT(std::shared_ptr<ObjectMapper>, objectMapper)) : oatpp::web::server::api::ApiController(objectMapper) {}

  vr::VROverlayHandle_t keyboardOverlayHandle = 0;
  std::thread* keyboardHandlerThread = nullptr;
  vr::COpenVRContext vrContext = vr::COpenVRContext();

  void ListenToVrKeyboard() {
    vr::EVROverlayError err;
    err = vrContext.VROverlay()->ShowKeyboardForOverlay(keyboardOverlayHandle, vr::k_EGamepadTextInputModeNormal, vr::k_EGamepadTextInputLineModeSingleLine, 0, "", 128, "", 0);
    if (err != vr::EVROverlayError::VROverlayError_None) {
      return;
    }
    Sleep(100);

    vr::VREvent_t event;
    std::string text;

    while (true) {
      if (vrContext.VROverlay()->PollNextOverlayEvent(keyboardOverlayHandle, &event, sizeof(vr::VREvent_t))) {
        if (event.eventType == vr::VREvent_KeyboardClosed || event.eventType == vr::VREvent_KeyboardDone) {
          char buffer[128 + 1] = "\0";
          vrContext.VROverlay()->GetKeyboardText(buffer, 128);
          text = std::string(buffer);
          break;
        }
      } else {
        Sleep(50);
      }
    }

    keyboardHandlerThread->detach();
    delete keyboardHandlerThread;
    keyboardHandlerThread = nullptr;
    vrContext.VROverlay()->DestroyOverlay(keyboardOverlayHandle);
    keyboardOverlayHandle = 0;

    auto output = std::format("Keyboard text entered: '{}'", text);
    auto* consoleLog = RE::ConsoleLog::GetSingleton();
    consoleLog->Print(output.c_str());
  }

  ENDPOINT("GET", "/", root) {
    auto now = std::chrono::system_clock::now();
    
    vr::EVROverlayError err;
    err = vrContext.VROverlay()->CreateOverlay("The Key", "The Title", &keyboardOverlayHandle);
    if (err != vr::EVROverlayError::VROverlayError_None) {
      return response("Could not create VR overlay!");
    }

    keyboardHandlerThread = new std::thread([this] { this->ListenToVrKeyboard(); });

    #ifdef FOO
      return response(std::format("FOO! Hi Skyrim, I am C++. I will run in VR shortly! The time is currently {}", now));
    #else
      return response(std::format("Welcome to this mod. I think we showed the keyboard??? The time is currently {}", now));
    #endif
  }
  
  ENDPOINT("GET", "/spells/{spellName}", searchSpells, PATH(String, spellName)) {
    const auto nameQuery = spellName->data();
    const auto dataHandler = RE::TESDataHandler::GetSingleton();
    const auto& spells = dataHandler->GetFormArray<RE::SpellItem>();
    int found = 0;

    for (const auto& spell : spells) {
      std::string name(spell->GetName());
      if (name.find(nameQuery) != std::string::npos)
        found++;
    }

    return response(std::format("Found {} spells", found)); // 
  }
  
  std::shared_ptr<oatpp::web::protocol::http::outgoing::Response> response(std::string text) {
    auto response = TestDto::createShared();
    response->text = text;
    return createDtoResponse(Status::CODE_200, response);
  }
};

#include OATPP_CODEGEN_END(ApiController) 