#pragma once
#include <filesystem>
#include <string>
#include <vector>
namespace orchard {
class AssetFiles {
public:
    explicit AssetFiles(std::filesystem::path root) : folder(std::move(root)) {}
    static std::filesystem::path executableDirectory();
    std::vector<unsigned char> read(const char* relative) const;
    std::vector<std::string> missing() const;
    const std::filesystem::path& root() const { return folder; }
private:
    std::filesystem::path folder;
};
}
