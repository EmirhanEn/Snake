#include "asset_files.hpp"
#include <fstream>
#include <limits>
#ifdef _WIN32
#define WIN32_LEAN_AND_MEAN
#define NOMINMAX
#include <windows.h>
#endif
namespace orchard {
std::filesystem::path AssetFiles::executableDirectory() {
#ifdef _WIN32
    std::vector<wchar_t> buffer(32768);
    DWORD length=GetModuleFileNameW(nullptr,buffer.data(),static_cast<DWORD>(buffer.size()));
    if(length && length<buffer.size()) return std::filesystem::path(std::wstring(buffer.data(),length)).parent_path();
#endif
    return {};
}
std::vector<unsigned char> AssetFiles::read(const char* name) const {
    std::ifstream f(folder/std::filesystem::u8path(name),std::ios::binary|std::ios::ate);
    if(!f) return {};
    const auto size=f.tellg();
    if(size<=0 || size>128*1024*1024) return {};
    std::vector<unsigned char> bytes(static_cast<std::size_t>(size));
    f.seekg(0);f.read(reinterpret_cast<char*>(bytes.data()),static_cast<std::streamsize>(bytes.size()));
    if(!f) return {};
    return bytes;
}
std::vector<std::string> AssetFiles::missing() const {
    static const char* files[]={
        "images/mascot.png","images/sprites.png","images/backdrop.png","images/grass-board.png",
        "images/ui-card.png","images/ui-button.png","images/snake-skin.png",
        "fonts/BalsamiqSans-Regular.ttf","fonts/BalsamiqSans-Bold.ttf",
        "audio/orchard-loop.wav","audio/eat.wav","audio/click.wav","audio/countdown.wav","audio/lose.wav","audio/win.wav"
    };
    std::vector<std::string> missing;
    for(const auto* name:files) {std::error_code ec;auto size=std::filesystem::file_size(folder/name,ec);if(ec||size==0) missing.push_back(name);}
    return missing;
}
}
