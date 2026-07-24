#pragma once

#include <stdexcept>
#include <string>

namespace evidencevault {

class EvidenceVaultException : public std::runtime_error {
public:
    using std::runtime_error::runtime_error;
};

class DatabaseException : public EvidenceVaultException {
public:
    using EvidenceVaultException::EvidenceVaultException;
};

} // namespace evidencevault
