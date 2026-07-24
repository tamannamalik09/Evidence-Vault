#include "DatabaseManager.h"

#include "Exceptions.h"

#include <fstream>
#include <sstream>
#include <string>
#include <utility>

namespace evidencevault {

DatabaseManager::DatabaseManager(const fs::path& databasePath)
    : databasePath_(databasePath), db_(nullptr), isOpen_(false) {
    open();
}

DatabaseManager::~DatabaseManager() {
    close();
}

void DatabaseManager::open() {
    if (isOpen_) {
        return;
    }

    const int openResult = sqlite3_open_v2(databasePath_.string().c_str(), &db_, SQLITE_OPEN_READWRITE | SQLITE_OPEN_CREATE, nullptr);
    if (openResult != SQLITE_OK || db_ == nullptr) {
        throw DatabaseException("Unable to open database: " + databasePath_.string());
    }

    sqlite3_busy_timeout(db_, 5000);
    isOpen_ = true;
}

void DatabaseManager::close() {
    if (!isOpen_ || db_ == nullptr) {
        return;
    }

    sqlite3_close(db_);
    db_ = nullptr;
    isOpen_ = false;
}

bool DatabaseManager::isOpen() const {
    return isOpen_;
}

void DatabaseManager::initializeSchema() {
    if (!isOpen_) {
        throw DatabaseException("Database connection is not open.");
    }

    char* errorMessage = nullptr;
    const int executionResult = sqlite3_exec(db_, schemaSql().c_str(), nullptr, nullptr, &errorMessage);
    if (executionResult != SQLITE_OK) {
        const std::string message = errorMessage ? errorMessage : "Unknown schema error";
        sqlite3_free(errorMessage);
        throw DatabaseException("Schema initialization failed: " + message);
    }
}

sqlite3_stmt* DatabaseManager::prepareStatement(const std::string& sql) {
    if (!isOpen_) {
        throw DatabaseException("Database connection is not open.");
    }

    sqlite3_stmt* statement = nullptr;
    const int prepareResult = sqlite3_prepare_v2(db_, sql.c_str(), static_cast<int>(sql.size()), &statement, nullptr);
    if (prepareResult != SQLITE_OK || statement == nullptr) {
        throw DatabaseException("Unable to prepare SQL statement: " + sql);
    }

    return statement;
}

int DatabaseManager::executeStatement(const std::string& sql) {
    if (!isOpen_) {
        throw DatabaseException("Database connection is not open.");
    }

    char* errorMessage = nullptr;
    const int executionResult = sqlite3_exec(db_, sql.c_str(), nullptr, nullptr, &errorMessage);
    if (executionResult != SQLITE_OK) {
        const std::string message = errorMessage ? errorMessage : "Unknown execution error";
        sqlite3_free(errorMessage);
        throw DatabaseException("Statement execution failed: " + message);
    }

    return executionResult;
}

std::string DatabaseManager::schemaSql() {
    return R"(
CREATE TABLE IF NOT EXISTS Evidence (
    evidence_id TEXT PRIMARY KEY,
    original_filename TEXT NOT NULL,
    stored_filename TEXT NOT NULL,
    file_type TEXT NOT NULL,
    file_extension TEXT NOT NULL,
    file_size_bytes INTEGER NOT NULL,
    upload_timestamp TEXT NOT NULL,
    category TEXT NOT NULL,
    description TEXT,
    location TEXT,
    sha256_hash TEXT NOT NULL UNIQUE,
    verification_status TEXT NOT NULL DEFAULT 'Not Yet Verified',
    last_verified_at TEXT,
    created_at TEXT NOT NULL DEFAULT (datetime('now')),
    deleted_at TEXT
);

CREATE TABLE IF NOT EXISTS Tags (
    tag_id INTEGER PRIMARY KEY AUTOINCREMENT,
    tag_name TEXT NOT NULL UNIQUE
);

CREATE TABLE IF NOT EXISTS EvidenceTags (
    evidence_id TEXT NOT NULL,
    tag_id INTEGER NOT NULL,
    PRIMARY KEY (evidence_id, tag_id),
    FOREIGN KEY (evidence_id) REFERENCES Evidence(evidence_id) ON DELETE CASCADE,
    FOREIGN KEY (tag_id) REFERENCES Tags(tag_id) ON DELETE CASCADE
);

CREATE TABLE IF NOT EXISTS AuditLogs (
    log_id INTEGER PRIMARY KEY AUTOINCREMENT,
    evidence_id TEXT,
    action TEXT NOT NULL,
    result TEXT,
    user TEXT NOT NULL DEFAULT 'local_user',
    timestamp TEXT NOT NULL DEFAULT (datetime('now')),
    FOREIGN KEY (evidence_id) REFERENCES Evidence(evidence_id) ON DELETE SET NULL
);

CREATE TABLE IF NOT EXISTS EvidenceCounter (
    year INTEGER PRIMARY KEY,
    last_sequence INTEGER NOT NULL DEFAULT 0
);
)";
}

} // namespace evidencevault
