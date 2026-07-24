#pragma once

#include "ConfigManager.h"
#include "Evidence.h"

#include <string>
#include <vector>

namespace evidencevault {

class FileManager {
public:
    explicit FileManager(const fs::path& storageRoot = fs::path("storage"));

    void ensureStorageLayout() const;
    fs::path buildStoragePath(const Evidence& evidence) const;
    fs::path copyFileToStorage(const fs::path& sourcePath, const Evidence& evidence) const;
    bool isAllowedExtension(const std::string& extension) const;
    long long fileSizeBytes(const fs::path& path) const;

private:
    fs::path storageRoot_;
    std::vector<std::string> categoryFolders_;
};

} // namespace evidencevault
