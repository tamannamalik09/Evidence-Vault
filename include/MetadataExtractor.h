#pragma once

#include "Evidence.h"

#include <string>

namespace evidencevault {

class MetadataExtractor {
public:
    MetadataExtractor() = default;

    Evidence extractFromPath(const std::string& originalFilename, const std::string& storedFilename, const std::string& fileExtension, long long fileSizeBytes) const;
};

} // namespace evidencevault
