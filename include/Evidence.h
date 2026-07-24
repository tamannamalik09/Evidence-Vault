#pragma once

#include <string>
#include <vector>

namespace evidencevault {

class Evidence {
public:
    Evidence() = default;

    Evidence(
        std::string evidenceId,
        std::string originalFilename,
        std::string storedFilename,
        std::string fileType,
        std::string fileExtension,
        long long fileSizeBytes,
        std::string uploadTimestamp,
        std::string category,
        std::string description,
        std::string location,
        std::vector<std::string> tags,
        std::string sha256Hash,
        std::string verificationStatus = "Not Yet Verified",
        std::string lastVerifiedAt = "",
        std::string createdAt = "",
        std::string deletedAt = "");

    const std::string& evidenceId() const;
    const std::string& originalFilename() const;
    const std::string& storedFilename() const;
    const std::string& fileType() const;
    const std::string& fileExtension() const;
    long long fileSizeBytes() const;
    const std::string& uploadTimestamp() const;
    const std::string& category() const;
    const std::string& description() const;
    const std::string& location() const;
    const std::vector<std::string>& tags() const;
    const std::string& sha256Hash() const;
    const std::string& verificationStatus() const;
    const std::string& lastVerifiedAt() const;
    const std::string& createdAt() const;
    const std::string& deletedAt() const;

    void setEvidenceId(const std::string& evidenceId);
    void setOriginalFilename(const std::string& originalFilename);
    void setStoredFilename(const std::string& storedFilename);
    void setFileType(const std::string& fileType);
    void setFileExtension(const std::string& fileExtension);
    void setFileSizeBytes(long long fileSizeBytes);
    void setUploadTimestamp(const std::string& uploadTimestamp);
    void setCategory(const std::string& category);
    void setDescription(const std::string& description);
    void setLocation(const std::string& location);
    void setTags(const std::vector<std::string>& tags);
    void setSha256Hash(const std::string& sha256Hash);
    void setVerificationStatus(const std::string& verificationStatus);
    void setLastVerifiedAt(const std::string& lastVerifiedAt);
    void setCreatedAt(const std::string& createdAt);
    void setDeletedAt(const std::string& deletedAt);

    bool isValid() const;
    static bool isValidFileType(const std::string& fileType);
    static bool isValidVerificationStatus(const std::string& verificationStatus);

private:
    std::string evidenceId_;
    std::string originalFilename_;
    std::string storedFilename_;
    std::string fileType_;
    std::string fileExtension_;
    long long fileSizeBytes_ = 0;
    std::string uploadTimestamp_;
    std::string category_;
    std::string description_;
    std::string location_;
    std::vector<std::string> tags_;
    std::string sha256Hash_;
    std::string verificationStatus_ = "Not Yet Verified";
    std::string lastVerifiedAt_;
    std::string createdAt_;
    std::string deletedAt_;
};

} // namespace evidencevault
