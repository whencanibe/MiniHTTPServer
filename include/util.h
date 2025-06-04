//
// Created by 조언약 on 6/2/25.
//

#ifndef UTIL_H
#define UTIL_H

#include <string>

// Returns the MIME type corresponding to the given file extension (e.g., "html")
std::string mime_type(const std::string& ext);

// Reads the contents of the file at the given path and returns it as a string
std::string read_file(const std::string& path, bool& ok);


#endif //UTIL_H
