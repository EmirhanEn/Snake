#include "asset_files.hpp"
#include <fstream>
#include <iostream>
using namespace orchard;
namespace fs=std::filesystem;
int main(int argc,char** argv) {
    auto folder=fs::current_path()/fs::path(L"asset-test-çilek-şirin");
    fs::create_directories(folder);
    {std::ofstream f(folder/"sample.bin",std::ios::binary);f.write("PNG\0ABC",7);}
    AssetFiles files(folder);
    auto bytes=files.read("sample.bin");
    bool ok=bytes.size()==7 && bytes[3]==0 && bytes[6]=='C';
    if(!ok) std::cerr<<"FAIL: Unicode file paths must load exact binary contents\n";
    if(!files.read("missing.bin").empty()) {ok=false;std::cerr<<"FAIL: missing file must not produce data\n";}
    if(AssetFiles::executableDirectory().empty()) {ok=false;std::cerr<<"FAIL: executable directory must be available\n";}
    if(files.missing().empty()) {ok=false;std::cerr<<"FAIL: incomplete asset pack must identify missing files\n";}
    fs::remove(folder/"sample.bin");fs::remove(folder);
    if(argc>1) {
        AssetFiles pack(fs::u8path(argv[1]));
        for(const auto& file:pack.missing()) {ok=false;std::cerr<<"FAIL: missing pack asset: "<<file<<'\n';}
    }
    if(ok) std::cout<<"Asset paths and binary file loading passed\n";
    return ok?0:1;
}
