#pragma once

#include "ConfigManager.h"

#include <string>

namespace evidencevault {

class HashGenerator {
public:
    HashGenerator() = default;

    std::string computeFileHash(const fs::path& filePath) const;
};

} // namespace evidencevault
