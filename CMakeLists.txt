cmake_minimum_required(VERSION 3.20)

project(EvidenceVault LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

set(EVIDENCE_VAULT_INCLUDE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/include")
set(EVIDENCE_VAULT_SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/src")

add_executable(evidence_vault_config_test
    "${EVIDENCE_VAULT_SOURCE_DIR}/main.cpp"
    "${EVIDENCE_VAULT_SOURCE_DIR}/ConfigManager.cpp"
)

target_include_directories(evidence_vault_config_test
    PRIVATE
        "${EVIDENCE_VAULT_INCLUDE_DIR}"
        "${EVIDENCE_VAULT_SOURCE_DIR}"
)
