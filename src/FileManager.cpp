#include "FileManager.h"

#include <algorithm>
#include <fstream>
#include <stdexcept>
#include <string>

namespace evidencevault {

FileManager::FileManager(const fs::path& storageRoot)
    : storageRoot_(storageRoot),
      categoryFolders_{"images", "videos", "audio", "documents"} {
}

void FileManager::ensureStorageLayout() const {
    for (const auto& folder : categoryFolders_) {
        const auto path = storageRoot_ / fs::path(folder);
        if (!fs::exists(path)) {
            fs::create_directories(path);
        }
    }
}

fs::path FileManager::buildStoragePath(const Evidence& evidence) const {
    const std::string normalizedType = evidence.fileType();
    const std::string folder = normalizedType == "image" ? "images"
        : normalizedType == "video" ? "videos"
        : normalizedType == "audio" ? "audio"
        : "documents";

    return storageRoot_ / fs::path(folder) / fs::path(evidence.storedFilename());
}

fs::path FileManager::copyFileToStorage(const fs::path& sourcePath, const Evidence& evidence) const {
    const auto destinationPath = buildStoragePath(evidence);
    ensureStorageLayout();

    std::ifstream source(sourcePath.string(), std::ios::binary);
    if (!source.is_open()) {
        throw std::runtime_error("Unable to open source file for storage copy: " + sourcePath.string());
    }

    std::ofstream destination(destinationPath.string(), std::ios::binary);
    if (!destination.is_open()) {
        throw std::runtime_error("Unable to write destination file: " + destinationPath.string());
    }

    destination << source.rdbuf();
    return destinationPath;
}

bool FileManager::isAllowedExtension(const std::string& extension) const {
    const std::string normalized = extension;
    return normalized == ".jpg" || normalized == ".jpeg" || normalized == ".png" || normalized == ".mp4"
        || normalized == ".mov" || normalized == ".mp3" || normalized == ".wav" || normalized == ".pdf"
        || normalized == ".txt" || normalized == ".docx";
}

long long FileManager::fileSizeBytes(const fs::path& path) const {
    std::ifstream input(path.string(), std::ios::binary | std::ios::ate);
    if (!input.is_open()) {
        throw std::runtime_error("Unable to open file to determine size: " + path.string());
    }

    return static_cast<long long>(input.tellg());
}

} // namespace evidencevault
