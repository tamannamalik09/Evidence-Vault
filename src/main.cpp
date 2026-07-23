#include "ConfigManager.h"

#include <iostream>

int main() {
    try {
        ConfigManager config("config/config.json");

        std::cout << "Configuration loaded successfully.\n";
        std::cout << "Storage root: " << config.storageRoot().string() << '\n';
        std::cout << "Database path: " << config.databasePath().string() << '\n';
        std::cout << "Exports path: " << config.exportsPath().string() << '\n';
        std::cout << "Logs path: " << config.logsPath().string() << '\n';
        std::cout << "Max file size (MB): " << config.maxFileSizeMb() << '\n';
        std::cout << "Allowed extensions: ";
        for (const auto& extension : config.allowedExtensions()) {
            std::cout << extension << ' ';
        }
        std::cout << '\n';
    } catch (const std::exception& exception) {
        std::cerr << "Failed to load configuration: " << exception.what() << '\n';
        return 1;
    }

    return 0;
}
