#include "Evidence.h"

#include <algorithm>
#include <cctype>

namespace evidencevault {

Evidence::Evidence(
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
    std::string verificationStatus,
    std::string lastVerifiedAt,
    std::string createdAt,
    std::string deletedAt)
    : evidenceId_(std::move(evidenceId)),
      originalFilename_(std::move(originalFilename)),
      storedFilename_(std::move(storedFilename)),
      fileType_(std::move(fileType)),
      fileExtension_(std::move(fileExtension)),
      fileSizeBytes_(fileSizeBytes),
      uploadTimestamp_(std::move(uploadTimestamp)),
      category_(std::move(category)),
      description_(std::move(description)),
      location_(std::move(location)),
      tags_(std::move(tags)),
      sha256Hash_(std::move(sha256Hash)),
      verificationStatus_(std::move(verificationStatus)),
      lastVerifiedAt_(std::move(lastVerifiedAt)),
      createdAt_(std::move(createdAt)),
      deletedAt_(std::move(deletedAt)) {
}

const std::string& Evidence::evidenceId() const { return evidenceId_; }
const std::string& Evidence::originalFilename() const { return originalFilename_; }
const std::string& Evidence::storedFilename() const { return storedFilename_; }
const std::string& Evidence::fileType() const { return fileType_; }
const std::string& Evidence::fileExtension() const { return fileExtension_; }
long long Evidence::fileSizeBytes() const { return fileSizeBytes_; }
const std::string& Evidence::uploadTimestamp() const { return uploadTimestamp_; }
const std::string& Evidence::category() const { return category_; }
const std::string& Evidence::description() const { return description_; }
const std::string& Evidence::location() const { return location_; }
const std::vector<std::string>& Evidence::tags() const { return tags_; }
const std::string& Evidence::sha256Hash() const { return sha256Hash_; }
const std::string& Evidence::verificationStatus() const { return verificationStatus_; }
const std::string& Evidence::lastVerifiedAt() const { return lastVerifiedAt_; }
const std::string& Evidence::createdAt() const { return createdAt_; }
const std::string& Evidence::deletedAt() const { return deletedAt_; }

void Evidence::setEvidenceId(const std::string& evidenceId) { evidenceId_ = evidenceId; }
void Evidence::setOriginalFilename(const std::string& originalFilename) { originalFilename_ = originalFilename; }
void Evidence::setStoredFilename(const std::string& storedFilename) { storedFilename_ = storedFilename; }
void Evidence::setFileType(const std::string& fileType) { fileType_ = fileType; }
void Evidence::setFileExtension(const std::string& fileExtension) { fileExtension_ = fileExtension; }
void Evidence::setFileSizeBytes(long long fileSizeBytes) { fileSizeBytes_ = fileSizeBytes; }
void Evidence::setUploadTimestamp(const std::string& uploadTimestamp) { uploadTimestamp_ = uploadTimestamp; }
void Evidence::setCategory(const std::string& category) { category_ = category; }
void Evidence::setDescription(const std::string& description) { description_ = description; }
void Evidence::setLocation(const std::string& location) { location_ = location; }
void Evidence::setTags(const std::vector<std::string>& tags) { tags_ = tags; }
void Evidence::setSha256Hash(const std::string& sha256Hash) { sha256Hash_ = sha256Hash; }
void Evidence::setVerificationStatus(const std::string& verificationStatus) { verificationStatus_ = verificationStatus; }
void Evidence::setLastVerifiedAt(const std::string& lastVerifiedAt) { lastVerifiedAt_ = lastVerifiedAt; }
void Evidence::setCreatedAt(const std::string& createdAt) { createdAt_ = createdAt; }
void Evidence::setDeletedAt(const std::string& deletedAt) { deletedAt_ = deletedAt; }

bool Evidence::isValid() const {
    return !evidenceId_.empty() &&
           !originalFilename_.empty() &&
           !storedFilename_.empty() &&
           !fileType_.empty() &&
           !fileExtension_.empty() &&
           fileSizeBytes_ >= 0 &&
           !uploadTimestamp_.empty() &&
           !category_.empty() &&
           !sha256Hash_.empty() &&
           isValidFileType(fileType_) &&
           isValidVerificationStatus(verificationStatus_);
}

bool Evidence::isValidFileType(const std::string& fileType) {
    const std::string normalized = fileType;
    return normalized == "image" || normalized == "video" || normalized == "audio" || normalized == "document";
}

bool Evidence::isValidVerificationStatus(const std::string& verificationStatus) {
    const std::string normalized = verificationStatus;
    return normalized == "Verified" || normalized == "File Modified" || normalized == "Not Yet Verified";
}

} // namespace evidencevault
