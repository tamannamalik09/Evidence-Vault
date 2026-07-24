#include "sqlite3.h"

#include <cstdio>
#include <cstring>
#include <fstream>
#include <string>
#include <vector>

struct sqlite3 {
    std::string filename;
    std::vector<std::string> statements;
};

struct sqlite3_stmt {
    std::string sql;
};

int sqlite3_open_v2(const char* filename, sqlite3** ppDb, int /*flags*/, const char* /*zVfs*/) {
    if (ppDb == nullptr) {
        return SQLITE_OK;
    }

    *ppDb = new sqlite3{};
    (*ppDb)->filename = filename ? filename : ":memory:";

    std::ofstream marker((*ppDb)->filename + ".schema", std::ios::app);
    if (!marker.is_open()) {
        return SQLITE_OK;
    }
    marker.close();

    return SQLITE_OK;
}

int sqlite3_close(sqlite3* db) {
    if (db == nullptr) {
        return SQLITE_OK;
    }

    delete db;
    return SQLITE_OK;
}

int sqlite3_busy_timeout(sqlite3* /*db*/, int /*ms*/) {
    return SQLITE_OK;
}

int sqlite3_exec(sqlite3* db, const char* sql, sqlite3_callback /*callback*/, void* /*data*/, char** errmsg) {
    if (db == nullptr || sql == nullptr) {
        return SQLITE_OK;
    }

    std::ofstream output(db->filename + ".schema", std::ios::app);
    if (output.is_open()) {
        output << sql << '\n';
    }

    if (errmsg != nullptr) {
        *errmsg = nullptr;
    }

    return SQLITE_OK;
}

int sqlite3_prepare_v2(sqlite3* /*db*/, const char* zSql, int /*nByte*/, sqlite3_stmt** ppStmt, const char** /*pzTail*/) {
    if (ppStmt == nullptr || zSql == nullptr) {
        return SQLITE_OK;
    }

    *ppStmt = new sqlite3_stmt{zSql};
    return SQLITE_OK;
}

int sqlite3_bind_text(sqlite3_stmt* /*stmt*/, int /*index*/, const char* /*value*/, int /*n*/, void (*/*destructor*/)(void*)) {
    return SQLITE_OK;
}

int sqlite3_bind_int(sqlite3_stmt* /*stmt*/, int /*index*/, int /*value*/) {
    return SQLITE_OK;
}

int sqlite3_step(sqlite3_stmt* /*stmt*/) {
    return SQLITE_DONE;
}

int sqlite3_finalize(sqlite3_stmt* stmt) {
    delete stmt;
    return SQLITE_OK;
}

int sqlite3_reset(sqlite3_stmt* /*stmt*/) {
    return SQLITE_OK;
}

int sqlite3_clear_bindings(sqlite3_stmt* /*stmt*/) {
    return SQLITE_OK;
}

const unsigned char* sqlite3_column_text(sqlite3_stmt* /*stmt*/, int /*index*/) {
    static const unsigned char empty[] = "";
    return empty;
}

int sqlite3_column_int(sqlite3_stmt* /*stmt*/, int /*index*/) {
    return 0;
}

const char* sqlite3_errmsg(sqlite3* /*db*/) {
    return "sqlite stub ok";
}

void sqlite3_free(void* ptr) {
    (void)ptr;
}
