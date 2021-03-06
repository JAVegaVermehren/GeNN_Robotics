#pragma once
#include <string>

#ifdef _WIN32
#include <shlwapi.h>
#pragma comment(lib, "shlwapi.lib")
#else
#include <sys/stat.h>
#include <unistd.h>
#endif

namespace OS::FileSystem {
inline bool
fileExists(const std::string &filePath)
{
#ifdef _WIN32
    return PathFileExists((LPCWSTR) filePath.c_str());
#else
    struct stat buffer;
    return (stat(filePath.c_str(), &buffer) == 0);
#endif
}
}
