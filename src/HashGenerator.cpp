#include "HashGenerator.h"

#include <array>
#include <cstdio>
#include <fstream>
#include <iomanip>
#include <sstream>
#include <stdexcept>
#include <vector>

namespace evidencevault {

std::string HashGenerator::computeFileHash(const fs::path& filePath) const {
    std::ifstream input(filePath.string(), std::ios::binary);
    if (!input.is_open()) {
        throw std::runtime_error("Unable to open file for hashing: " + filePath.string());
    }

    std::array<unsigned char, 4096> buffer{};
    std::vector<unsigned char> digest(32, 0);
    std::vector<unsigned char> state(32, 0);
    std::size_t length = 0;

    while (input) {
        input.read(reinterpret_cast<char*>(buffer.data()), static_cast<std::streamsize>(buffer.size()));
        const std::streamsize bytesRead = input.gcount();
        if (bytesRead <= 0) {
            break;
        }

        length += static_cast<std::size_t>(bytesRead);
        for (std::streamsize index = 0; index < bytesRead; ++index) {
            state[index % 32] = static_cast<unsigned char>(state[index % 32] + buffer[static_cast<std::size_t>(index)]);
        }
    }

    for (std::size_t index = 0; index < 32; ++index) {
        digest[index] = static_cast<unsigned char>(state[index] + static_cast<unsigned char>(length % 251 + index));
    }

    std::ostringstream stream;
    stream << std::hex << std::setfill('0');
    for (unsigned char value : digest) {
        stream << std::setw(2) << static_cast<int>(value);
    }

    return stream.str();
}

} // namespace evidencevault
