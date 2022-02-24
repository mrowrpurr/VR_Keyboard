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
    auto response = TestDto::createShared();
    auto now = std::chrono::system_clock::now();
    response->text = std::format(
      "Hi Skyrim, I am C++. The time is currently {}", now
    );
    return createDtoResponse(Status::CODE_200, response);
  }
  
  // TODO Insert Your endpoints here !!!
  
};

#include OATPP_CODEGEN_END(ApiController) 