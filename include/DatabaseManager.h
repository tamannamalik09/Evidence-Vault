#pragma once

#include "ConfigManager.h"
#include "sqlite3.h"

#include <string>

namespace evidencevault {

class DatabaseManager {
public:
    explicit DatabaseManager(const fs::path& databasePath = fs::path("database/evidence_vault.db"));
    ~DatabaseManager();

    void open();
    void close();
    bool isOpen() const;

    void initializeSchema();
    sqlite3_stmt* prepareStatement(const std::string& sql);
    int executeStatement(const std::string& sql);

    static std::string schemaSql();

private:
    fs::path databasePath_;
    sqlite3* db_ = nullptr;
    bool isOpen_ = false;
};

} // namespace evidencevault
