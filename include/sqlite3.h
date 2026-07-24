#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#define SQLITE_OK 0
#define SQLITE_DONE 101
#define SQLITE_OPEN_READWRITE 0x00000002
#define SQLITE_OPEN_CREATE 0x00000004

typedef struct sqlite3 sqlite3;
typedef struct sqlite3_stmt sqlite3_stmt;
typedef int (*sqlite3_callback)(void*, int, char**, char**);

int sqlite3_open_v2(const char* filename, sqlite3** ppDb, int flags, const char* zVfs);
int sqlite3_close(sqlite3* db);
int sqlite3_busy_timeout(sqlite3* db, int ms);
int sqlite3_exec(sqlite3* db, const char* sql, sqlite3_callback callback, void* data, char** errmsg);
int sqlite3_prepare_v2(sqlite3* db, const char* zSql, int nByte, sqlite3_stmt** ppStmt, const char** pzTail);
int sqlite3_bind_text(sqlite3_stmt* stmt, int index, const char* value, int n, void (*destructor)(void*));
int sqlite3_bind_int(sqlite3_stmt* stmt, int index, int value);
int sqlite3_step(sqlite3_stmt* stmt);
int sqlite3_finalize(sqlite3_stmt* stmt);
int sqlite3_reset(sqlite3_stmt* stmt);
int sqlite3_clear_bindings(sqlite3_stmt* stmt);
const unsigned char* sqlite3_column_text(sqlite3_stmt* stmt, int index);
int sqlite3_column_int(sqlite3_stmt* stmt, int index);
const char* sqlite3_errmsg(sqlite3* db);
void sqlite3_free(void* ptr);

#ifdef __cplusplus
}
#endif
