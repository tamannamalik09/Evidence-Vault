#include "MetadataExtractor.h"

#include <chrono>
#include <ctime>
#include <string>

namespace evidencevault {

namespace {
std::string formatUtcTimestamp() {
    const auto now = std::chrono::system_clock::now();
    const auto time = std::chrono::system_clock::to_time_t(now);
    const std::tm* utcTime = std::gmtime(&time);

    char buffer[32] = {};
    std::strftime(buffer, sizeof(buffer), "%Y-%m-%dT%H:%M:%SZ", utcTime);
    return buffer;
}
}

Evidence MetadataExtractor::extractFromPath(const std::string& originalFilename, const std::string& storedFilename, const std::string& fileExtension, long long fileSizeBytes) const {
    const std::string timestamp = formatUtcTimestamp();

    const std::string fileType = fileExtension == ".jpg" || fileExtension == ".jpeg" || fileExtension == ".png" ? "image"
        : fileExtension == ".mp4" || fileExtension == ".mov" ? "video"
        : fileExtension == ".mp3" || fileExtension == ".wav" ? "audio"
        : "document";

    return Evidence{
        "",
        originalFilename,
        storedFilename,
        fileType,
        fileExtension,
        fileSizeBytes,
        timestamp,
        "Documents",
        "",
        "",
        {},
        "",
        "Not Yet Verified",
        "",
        timestamp,
        ""
    };
}

} // namespace evidencevault
