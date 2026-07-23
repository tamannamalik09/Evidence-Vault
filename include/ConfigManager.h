#pragma once

#include <cstdlib>
#include <fstream>
#include <string>
#include <vector>

namespace fs {
class path {
public:
    path() = default;
    path(const std::string& value) : value_(value) {}
    path(const char* value) : value_(value) {}

    const std::string& string() const { return value_; }

    bool is_relative() const {
        return value_.find(':') == std::string::npos;
    }

    path parent_path() const {
        const auto separator = value_.find_last_of("/\\");
        if (separator == std::string::npos) {
            return path();
        }

        return path(value_.substr(0, separator));
    }

    friend path operator/(const path& lhs, const path& rhs) {
        if (lhs.string().empty()) {
            return rhs;
        }
        if (rhs.string().empty()) {
            return lhs;
        }

        const auto lhsEndsWithSeparator = lhs.string().back() == '/' || lhs.string().back() == '\\';
        const auto rhsStartsWithSeparator = rhs.string().front() == '/' || rhs.string().front() == '\\';
        if (lhsEndsWithSeparator || rhsStartsWithSeparator) {
            return path(lhs.string() + rhs.string());
        }

        return path(lhs.string() + "/" + rhs.string());
    }

private:
    std::string value_;
};

inline bool exists(const path& input) {
    std::ifstream stream(input.string());
    return stream.good();
}

inline void create_directories(const path& input) {
    if (input.string().empty()) {
        return;
    }

    std::string command = "mkdir " + input.string();
    std::system(command.c_str());
}
}

class ConfigManager {
public:
    explicit ConfigManager(fs::path configPath = fs::path("config/config.json"));

    void load();
    void saveDefaultIfMissing();

    const fs::path& storageRoot() const;
    const fs::path& databasePath() const;
    const fs::path& exportsPath() const;
    const fs::path& logsPath() const;
    int maxFileSizeMb() const;
    const std::vector<std::string>& allowedExtensions() const;

private:
    fs::path configPath_;
    fs::path storageRoot_;
    fs::path databasePath_;
    fs::path exportsPath_;
    fs::path logsPath_;
    int maxFileSizeMb_ = 500;
    std::vector<std::string> allowedExtensions_;

    static std::string defaultConfigJson();
    static std::string readFileContents(const fs::path& path);
    static std::string extractStringValue(const std::string& json, const std::string& key);
    static std::vector<std::string> extractStringArray(const std::string& json, const std::string& key);
    static int extractIntValue(const std::string& json, const std::string& key);
    void normalizeRelativePaths();
};
