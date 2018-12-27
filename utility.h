#ifndef UTILITY_H
#define UTILITY_H

#include <vector>
#include <string>

std::vector<std::string> stringSplit(const std::string& line, char sep = ' ');

std::string stringTrimL(const std::string& input);

std::string stringTrimR(const std::string& input);

std::string stringTrim(const std::string& input);

// directories should be separated by backslashes '\\'
std::vector<std::string> listFiles(const std::string& dir);

// wrapper for reading content and do base64 convertion
bool readBase64Content(const std::string& file, std::string& content);

#endif
