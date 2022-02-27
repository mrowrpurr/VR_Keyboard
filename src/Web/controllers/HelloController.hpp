#pragma once

#include <iostream>
#include <format>

// #include "skse64/GameVR.h"

#include "Web/dtos/TestDto.hpp"

#include "oatpp/web/server/api/ApiController.hpp"
#include "oatpp/core/macro/codegen.hpp"
#include "oatpp/core/macro/component.hpp"

#include OATPP_CODEGEN_BEGIN(ApiController)

class HelloController : public oatpp::web::server::api::ApiController {
public:
  HelloController(OATPP_COMPONENT(std::shared_ptr<ObjectMapper>, objectMapper)) : oatpp::web::server::api::ApiController(objectMapper) {}

public:
  
  ENDPOINT("GET", "/", root) {
    auto now = std::chrono::system_clock::now();


	// namespace vr = vr_1_0_12;

    // auto vrContext = vr::COpenVRContext();
    // vr::VROverlayHandle_t keyboardOverlayHandle = 0;
    // std::thread* keyboardHandlerThread = nullptr;
    

    #ifdef FOO
      return response(std::format("FOO! Hi Skyrim, I am C++. I will run in VR shortly! The time is currently {}", now));
    #else
      return response(std::format("Welcome to this mod. We will make the VR keyboard work now! The time is currently {}", now));
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