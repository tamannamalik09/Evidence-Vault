#include "ConfigManager.h"

#include <algorithm>
#include <fstream>
#include <iostream>
#include <sstream>
#include <stdexcept>
#include <string>

namespace {
std::string trim(const std::string& value) {
    const auto begin = value.find_first_not_of(" \t\r\n");
    if (begin == std::string::npos) {
        return "";
    }

    const auto end = value.find_last_not_of(" \t\r\n");
    return value.substr(begin, end - begin + 1);
}
}

ConfigManager::ConfigManager(fs::path configPath)
    : configPath_(std::move(configPath)) {
    saveDefaultIfMissing();
    load();
}

void ConfigManager::saveDefaultIfMissing() {
    if (fs::exists(configPath_)) {
        return;
    }

    fs::create_directories(configPath_.parent_path());

    std::ofstream output(configPath_.string());
    if (!output.is_open()) {
        throw std::runtime_error("Unable to create config file: " + configPath_.string());
    }

    output << defaultConfigJson();
    output.close();
}

void ConfigManager::load() {
    const auto content = readFileContents(configPath_);

    storageRoot_ = fs::path(extractStringValue(content, "storage_root"));
    databasePath_ = fs::path(extractStringValue(content, "database_path"));
    exportsPath_ = fs::path(extractStringValue(content, "exports_path"));
    logsPath_ = fs::path(extractStringValue(content, "logs_path"));
    maxFileSizeMb_ = extractIntValue(content, "max_file_size_mb");
    allowedExtensions_ = extractStringArray(content, "allowed_extensions");

    normalizeRelativePaths();
}

const fs::path& ConfigManager::storageRoot() const {
    return storageRoot_;
}

const fs::path& ConfigManager::databasePath() const {
    return databasePath_;
}

const fs::path& ConfigManager::exportsPath() const {
    return exportsPath_;
}

const fs::path& ConfigManager::logsPath() const {
    return logsPath_;
}

int ConfigManager::maxFileSizeMb() const {
    return maxFileSizeMb_;
}

const std::vector<std::string>& ConfigManager::allowedExtensions() const {
    return allowedExtensions_;
}

std::string ConfigManager::defaultConfigJson() {
    return R"({
  "storage_root": "./storage",
  "database_path": "./database/evidence_vault.db",
  "exports_path": "./exports",
  "logs_path": "./logs",
  "max_file_size_mb": 500,
  "allowed_extensions": [".jpg", ".jpeg", ".png", ".mp4", ".mov", ".mp3", ".wav", ".pdf", ".txt", ".docx"]
})";
}

std::string ConfigManager::readFileContents(const fs::path& path) {
    std::ifstream input(path.string());
    if (!input.is_open()) {
        throw std::runtime_error("Unable to open config file: " + path.string());
    }

    std::stringstream buffer;
    buffer << input.rdbuf();
    return buffer.str();
}

std::string ConfigManager::extractStringValue(const std::string& json, const std::string& key) {
    const std::string marker = "\"" + key + "\"";
    const auto keyPos = json.find(marker);
    if (keyPos == std::string::npos) {
        throw std::runtime_error("Missing configuration key: " + key);
    }

    const auto colonPos = json.find(':', keyPos);
    const auto quoteOpen = json.find('"', colonPos);
    if (quoteOpen == std::string::npos) {
        throw std::runtime_error("Malformed configuration value for key: " + key);
    }

    const auto quoteClose = json.find('"', quoteOpen + 1);
    if (quoteClose == std::string::npos) {
        throw std::runtime_error("Malformed configuration value for key: " + key);
    }

    return json.substr(quoteOpen + 1, quoteClose - quoteOpen - 1);
}

std::vector<std::string> ConfigManager::extractStringArray(const std::string& json, const std::string& key) {
    const std::string marker = "\"" + key + "\"";
    const auto keyPos = json.find(marker);
    if (keyPos == std::string::npos) {
        throw std::runtime_error("Missing configuration key: " + key);
    }

    const auto arrayOpen = json.find('[', keyPos);
    const auto arrayClose = json.find(']', arrayOpen);
    if (arrayOpen == std::string::npos || arrayClose == std::string::npos) {
        throw std::runtime_error("Malformed array configuration for key: " + key);
    }

    const auto payload = json.substr(arrayOpen + 1, arrayClose - arrayOpen - 1);
    std::vector<std::string> values;

    std::stringstream stream(payload);
    std::string token;
    while (std::getline(stream, token, ',')) {
        token = trim(token);
        if (token.empty()) {
            continue;
        }

        if (token.front() == '"' && token.back() == '"') {
            token = token.substr(1, token.size() - 2);
        }

        values.push_back(token);
    }

    return values;
}

int ConfigManager::extractIntValue(const std::string& json, const std::string& key) {
    const std::string marker = "\"" + key + "\"";
    const auto keyPos = json.find(marker);
    if (keyPos == std::string::npos) {
        throw std::runtime_error("Missing configuration key: " + key);
    }

    const auto colonPos = json.find(':', keyPos);
    const auto numberStart = json.find_first_of("-0123456789", colonPos);
    if (numberStart == std::string::npos) {
        throw std::runtime_error("Malformed configuration value for key: " + key);
    }

    const auto numberEnd = json.find_first_not_of("-0123456789", numberStart);
    const auto payload = json.substr(numberStart, numberEnd == std::string::npos ? std::string::npos : numberEnd - numberStart);
    return std::stoi(payload);
}

void ConfigManager::normalizeRelativePaths() {
    const auto basePath = configPath_.parent_path();

    if (storageRoot_.is_relative()) {
        storageRoot_ = basePath / storageRoot_;
    }
    if (databasePath_.is_relative()) {
        databasePath_ = basePath / databasePath_;
    }
    if (exportsPath_.is_relative()) {
        exportsPath_ = basePath / exportsPath_;
    }
    if (logsPath_.is_relative()) {
        logsPath_ = basePath / logsPath_;
    }
}
