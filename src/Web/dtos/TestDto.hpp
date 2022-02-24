#pragma once

#include "oatpp/core/macro/codegen.hpp"
#include "oatpp/core/Types.hpp"

#include OATPP_CODEGEN_BEGIN(DTO)

class TestDto : public oatpp::DTO {
  
  DTO_INIT(TestDto, DTO)
  
  DTO_FIELD(String, text);
  
};

#include OATPP_CODEGEN_END(DTO)
