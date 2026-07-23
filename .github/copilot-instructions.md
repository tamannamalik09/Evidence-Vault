# Evidence Vault (C++) – Project Specification

## Project Name
Evidence Vault

## Objective
Build a desktop application in modern C++ that securely stores digital evidence (images, videos, audio files, and documents). The application preserves the original file, computes a SHA-256 hash to verify integrity, stores metadata, and allows users to search and export evidence records.

**The application must never modify the original uploaded file.**

This project is intended for educational purposes to demonstrate secure file handling, hashing, metadata management, and software engineering principles.

---

## Technology Stack

**Language:** C++17 or C++20
**IDE:** Visual Studio Code
**Build System:** CMake (minimum version 3.20)

**Libraries:**
- SQLite3 (database)
- OpenSSL (SHA-256 hashing)
- nlohmann/json (JSON support, used for export and config)
- std::filesystem
- chrono
- fstream

**Design Principles:**
- Object-Oriented Programming
- Modular architecture
- Clean code
- SOLID principles
- RAII and consistent exception handling (see Error Handling Strategy below)

---

## Folder Structure

```
EvidenceVault/
├── src/
├── include/
├── database/
├── storage/
│   ├── images/
│   ├── videos/
│   ├── audio/
│   └── documents/
├── exports/
├── logs/
├── config/
│   └── config.json
├── CMakeLists.txt
└── README.md
```

---

## Configuration

All paths must be configurable via `config/config.json`, not hardcoded. On first run, if the file doesn't exist, generate it with sane defaults relative to the executable's working directory.

```json
{
  "storage_root": "./storage",
  "database_path": "./database/evidence_vault.db",
  "exports_path": "./exports",
  "logs_path": "./logs",
  "max_file_size_mb": 500,
  "allowed_extensions": [".jpg", ".jpeg", ".png", ".mp4", ".mov", ".mp3", ".wav", ".pdf", ".txt", ".docx"]
}
```

A `ConfigManager` class loads this at startup and is injected (not global) into components that need paths.

---

## Concurrency Model

This is a single-user console application with no multithreading required for v1. To keep behavior predictable and avoid corruption:

- Only one write operation (upload, delete, verify-with-update) may run at a time — enforce this simply by the console being single-threaded and synchronous (no background threads in v1).
- SQLite must be opened in `SERIALIZED` threading mode with a busy timeout (e.g., `sqlite3_busy_timeout(db, 5000);`) as a defensive measure even though the app is single-threaded, in case this evolves later.
- Document this constraint in the README so future GUI/multi-user extensions know concurrency was explicitly deferred.

---

## Core Features

### 1. Upload Evidence

Allow users to upload: Images, Videos, Audio, PDF, Text documents.

When uploaded:
- Validate extension against `allowed_extensions` in config (reject anything not on the list, case-insensitive match).
- Validate file size against `max_file_size_mb`.
- Compute SHA-256 hash of the source file **before** copying.
- **Duplicate detection: by hash, not filename.** If the computed hash already exists in the `Evidence` table, reject the upload with a clear message: `"Duplicate evidence detected: matches Evidence ID EV-2026-000042. Upload rejected."` Do not allow a bypass flag in v1 — duplicates are always rejected.
- Copy the file into the correct storage subdirectory (by type). Never modify the original at the source path.
- Preserve the original file extension in the stored filename.
- Generate a unique Evidence ID, sequential and zero-padded: `EV-<YEAR>-<6-digit-sequence>`, e.g. `EV-2026-000001`. Sequence resets per year and is derived from a dedicated counter table (not from `MAX(id)+1`, to avoid race conditions and gaps-on-delete issues).
- Stored filename format: `<EvidenceID>_<original_basename><original_extension>` to keep files traceable on disk without opening the DB.

### 2. Metadata Collection

For every uploaded file, store:

| Field | Notes |
|---|---|
| Evidence ID | Primary identifier, e.g. `EV-2026-000001` |
| Original Filename | As provided by user |
| Stored Filename | As generated above |
| File Type | Derived category: image/video/audio/document |
| File Extension | Lowercased, including the dot |
| File Size | Bytes |
| Upload Date | ISO 8601 date |
| Upload Time | ISO 8601 time, stored with the date as a single UTC timestamp |
| Category | One of the fixed categories, or custom |
| Description | Free text, optional |
| Location | Free text, optional |
| Tags | Many-to-many, see schema |
| SHA-256 Hash | Computed at upload, immutable after |
| Verification Status | `Verified` / `File Modified` / `Not Yet Verified` |

### 3. SHA-256 Integrity

- Hash is generated immediately after upload and stored — **never overwritten**.
- **Verify Evidence** action:
  - Recalculates the hash of the file currently on disk in storage (not the original source, which may no longer exist).
  - Compares against the stored hash.
  - Displays `Verified` or `File Modified`.
  - Updates `Verification Status` and `LastVerifiedAt` timestamp in the DB — this is a status update field, distinct from the immutable original hash, so it's fine to update.
  - Every verification attempt (pass or fail) is written to `AuditLogs`.

### 4. Database (SQLite)

Use prepared statements for every query. Enable foreign keys (`PRAGMA foreign_keys = ON;`).

```sql
CREATE TABLE Evidence (
    evidence_id        TEXT PRIMARY KEY,       -- e.g. 'EV-2026-000001'
    original_filename   TEXT NOT NULL,
    stored_filename      TEXT NOT NULL,
    file_type           TEXT NOT NULL CHECK(file_type IN ('image','video','audio','document')),
    file_extension      TEXT NOT NULL,
    file_size_bytes     INTEGER NOT NULL,
    upload_timestamp    TEXT NOT NULL,          -- ISO 8601 UTC
    category            TEXT NOT NULL,
    description         TEXT,
    location            TEXT,
    sha256_hash         TEXT NOT NULL UNIQUE,
    verification_status TEXT NOT NULL DEFAULT 'Not Yet Verified'
                         CHECK(verification_status IN ('Verified','File Modified','Not Yet Verified')),
    last_verified_at    TEXT,
    created_at          TEXT NOT NULL DEFAULT (datetime('now')),
    deleted_at          TEXT                    -- soft delete; NULL = active
);

CREATE TABLE Tags (
    tag_id      INTEGER PRIMARY KEY AUTOINCREMENT,
    tag_name    TEXT NOT NULL UNIQUE
);

CREATE TABLE EvidenceTags (
    evidence_id TEXT NOT NULL,
    tag_id      INTEGER NOT NULL,
    PRIMARY KEY (evidence_id, tag_id),
    FOREIGN KEY (evidence_id) REFERENCES Evidence(evidence_id) ON DELETE CASCADE,
    FOREIGN KEY (tag_id) REFERENCES Tags(tag_id) ON DELETE CASCADE
);

CREATE TABLE AuditLogs (
    log_id       INTEGER PRIMARY KEY AUTOINCREMENT,
    evidence_id  TEXT,                          -- nullable: some actions aren't tied to one record
    action       TEXT NOT NULL CHECK(action IN ('Upload','Verification','Export','Delete')),
    result       TEXT,                           -- e.g. 'Success', 'Verified', 'File Modified', 'Failed'
    user         TEXT NOT NULL DEFAULT 'local_user',
    timestamp    TEXT NOT NULL DEFAULT (datetime('now')),
    FOREIGN KEY (evidence_id) REFERENCES Evidence(evidence_id) ON DELETE SET NULL
);

CREATE TABLE EvidenceCounter (
    year         INTEGER PRIMARY KEY,
    last_sequence INTEGER NOT NULL DEFAULT 0
);
```

**Delete Record** is a soft delete (`deleted_at` timestamp set), not a row removal — this preserves audit trail integrity. The physical file in storage is retained; a "hard delete/purge" feature is out of scope for v1.

### 5. Search

Allow search by any combination of: Evidence ID, filename (original or stored), category, date range, tags, location. Build the query dynamically with parameterized placeholders — never string-concatenate user input into SQL.

### 6. Categories

Fixed set: `Police Action`, `Medical`, `Speech`, `Crowd`, `Property Damage`, `Documents`, `Other`.
Custom categories are allowed as free text but stored as-provided (not validated against the fixed list) — flag in UI when a category doesn't match the standard set, so it's visibly "custom."

### 7. Audit Log

Every one of these actions writes a row to `AuditLogs`: Upload, Verification, Export, Delete. Fields: timestamp, action, evidence_id (nullable), result, user (hardcoded to `"local_user"` for v1 — no auth system yet).

### 8. Export Report

Plain text export per evidence record, written to `exports/`, filename `<EvidenceID>_report.txt`:

```
==================================================
EVIDENCE VAULT — EXPORT REPORT
==================================================
Evidence ID:        EV-2026-000001
Original Filename:  IMG_0231.jpg
Stored Filename:    EV-2026-000001_IMG_0231.jpg
File Type:          image
File Size:          4,213,552 bytes
Category:           Police Action
Location:            (optional)
Upload Timestamp:   2026-07-20T14:32:00Z
--------------------------------------------------
SHA-256 Hash:       <hash>
Verification:       Verified (last checked 2026-07-23T09:00:00Z)
--------------------------------------------------
Description:
<description text>

Tags: crowd, downtown, protest
==================================================
Report generated: <current timestamp>
==================================================
```

Future: PDF export (out of scope for v1, keep `ReportGenerator` interface abstract enough to add a `PdfReportGenerator` later).

### 9. User Interface

Console menu:

```
===========================
       Evidence Vault
===========================
1. Upload Evidence
2. View Evidence
3. Search
4. Verify File
5. Export Report
6. Delete Record
7. Statistics
8. Exit
===========================
```

### 10. Statistics

Display: Total Files, Images, Videos, Audio, Documents, Storage Used (human-readable, e.g. MB/GB), Verified Files count, Not Yet Verified count, File Modified count. Exclude soft-deleted records from all counts.

---

## Classes

Separate header (`include/`) and implementation (`src/`) files for each:

- `Evidence` — data model / POD-like struct with validation
- `ConfigManager` — loads/saves `config.json`
- `DatabaseManager` — all SQLite access, prepared statements only
- `HashGenerator` — SHA-256 via OpenSSL
- `FileManager` — copy/validate files, path handling via `std::filesystem`
- `MetadataExtractor` — derives file_type from extension, size, etc.
- `SearchEngine` — builds and runs parameterized search queries
- `AuditLogger` — writes to `AuditLogs` table
- `ReportGenerator` — abstract base; `TextReportGenerator` implementation for v1
- `Application` — orchestrates the console menu and wires components together
- `Utilities` — small free functions (ID formatting, timestamp formatting, byte-size formatting)

---

## Error Handling Strategy

Consistency matters more than the specific choice. Use this pattern throughout:

- Define a single custom exception hierarchy in `include/Exceptions.h`:
  - `EvidenceVaultException` (base, inherits `std::runtime_error`)
  - `FileNotFoundException`, `InvalidFileTypeException`, `DuplicateEvidenceException`, `DatabaseException`, `HashComputationException`, `PermissionDeniedException`
- Lower-level components (`FileManager`, `HashGenerator`, `DatabaseManager`) throw these specific exceptions; they never print to console directly.
- `Application` is the only layer that catches exceptions, at the top of each menu action, and translates them into a user-facing message. No exception escapes `main()`.
- `main()` has a final catch-all (`catch (const std::exception&)`) as a last resort so the app never crashes to a raw terminate.
- Explicitly handle: missing file, invalid path, unsupported file extension, database failure (e.g. locked/corrupt), hash computation failure, duplicate upload (by hash), permission denied (e.g. read-only source, no write access to storage dir).

---

## Coding Standards

- Modern C++ (C++17/20 features used idiomatically: `std::optional`, `std::filesystem`, structured bindings).
- No global variables — pass dependencies via constructor injection.
- Smart pointers (`std::unique_ptr`) for owned resources; raw pointers/references for non-owning views.
- RAII for file handles, DB connections, OpenSSL contexts.
- Comment non-obvious logic, not restating the obvious.
- Header/implementation separation for every class.
- Keep functions small and single-purpose.

---

## Future Extensions (Design Only — Do Not Implement Now)

Keep architecture flexible for: GUI, cloud backup, user authentication, face blurring, AI transcription, OCR, video summarization, digital signatures, blockchain timestamping, web dashboard.

---

## Development Strategy

**Do NOT generate the entire project at once.** Implement module by module, in this order, and confirm each compiles cleanly before moving to the next:

1. Folder structure + config.json loading
2. CMake configuration
3. Database layer (schema creation, DatabaseManager, prepared statement helpers)
4. Evidence class
5. Hash generator
6. File manager (validation, copy, path handling)
7. Upload feature (ties 3–6 together, including duplicate detection and ID generation)
8. Search feature
9. Verification feature
10. Export reports
11. Statistics
12. Audit logging (wire into all above actions once each exists)

After each module compiles and runs, move to the next. Do not skip ahead.