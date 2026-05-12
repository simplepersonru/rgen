#pragma once

#include <string>

namespace rgen::Derive
{

/// @JsonSerializerV2
struct ClassData
{
    std::string nameWithNamespace;
    std::string name;
    std::string namespace_;
};

} // namespace rgen::Derive
