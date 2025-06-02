//
// Created by 조언약 on 6/2/25.
//

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
    // 파일 크기를 구해서 버퍼에 한 번에 읽어오는 방식
    std::string content;
    in.seekg(0, std::ios::end);
    auto size = in.tellg();
    in.seekg(0, std::ios::beg);
    content.resize(static_cast<size_t>(size));
    in.read(&content[0], size);
    ok = true;
    return content;
}
