#include "../include/util.h"
#include <unordered_map>
#include <fstream>

// 간단한 MIME 맵
static const std::unordered_map<std::string, std::string> mime_map = {
    {"html", "text/html"},
    {"htm",  "text/html"},
    {"css",  "text/css"},
    {"js",   "application/javascript"},
    {"json", "application/json"},
    {"png",  "image/png"},
    {"jpg",  "image/jpeg"},
    {"jpeg", "image/jpeg"},
    {"gif",  "image/gif"},
    {"svg",  "image/svg+xml"},
    {"txt",  "text/plain"}
};

std::string mime_type(const std::string& ext) {
    auto it = mime_map.find(ext);
    if (it != mime_map.end()) {
        return it->second;
    }
    return "application/octet-stream";
}

std::string read_file(const std::string& path, bool& ok) {
    std::ifstream in(path, std::ios::binary);
    if (!in.is_open()) {
        ok = false;
        return {};
    }

    // 파일 크기를 미리 구한 뒤, 그 크기만큼 string 버퍼를 확보
    std::string content;

    // 파일 포인터를 끝으로 이동
    in.seekg(0, std::ios::end);

    // 현재 위치 == 파일 끝 == 파일 크기
    auto size = in.tellg();

    // 다시 파일 처음으로 위치 이동
    in.seekg(0, std::ios::beg);

    // content 버퍼 크기를 'size'만큼 확보
    content.resize(static_cast<size_t>(size));

    // read() 로 한 번에 size 바이트를 읽어 content 버퍼에 채움
    in.read(&content[0], size);

    ok = true;
    return content;
}
