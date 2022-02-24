#pragma once

#include <iostream>
#include <format>

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
    return response(std::format("Hi Skyrim, I am C++. The time is currently {}", now));
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