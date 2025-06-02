//
// Created by 조언약 on 6/2/25.
//

#ifndef UTIL_H
#define UTIL_H

#include <string>

// 확장자(ex: "html")에 대응하는 MIME 타입 반환
// 등록되지 않은 확장자는 "application/octet-stream" 반환
std::string mime_type(const std::string& ext);

// 파일 경로를 받아서 내용을 읽어와서 반환
// ok를 참조 인자로 받아, 읽기에 성공하면 ok = true, 실패하면 ok = false
std::string read_file(const std::string& path, bool& ok);


#endif //UTIL_H
