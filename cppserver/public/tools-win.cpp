#include "tools.h"
#include <Shlwapi.h>
#include <direct.h>  
#include <io.h>  
#include <windows.h>
#pragma comment(lib, "shlwapi.lib")

namespace tools {
    namespace system {
        u64 getCurrentProcessId() {
            return ::GetCurrentProcessId();
        }
        
        u64 getCurrentThreadID() {
            return ::GetCurrentThreadId();
        }
    }

    namespace memery {
        const shareMemeryInfo * getShareMemory(const std::string & path, const std::string & name, const int size) {
            SMID handle = ::CreateFileMapping(INVALID_HANDLE_VALUE, nullptr, PAGE_READWRITE, 0, size, name.c_str());
            if (nullptr == handle) {
                if (GetLastError() == ERROR_ALREADY_EXISTS) {
                    handle = ::OpenFileMapping(FILE_MAP_ALL_ACCESS, 0, name.c_str());
                    if (nullptr == handle) {
                        return nullptr;
                    }
                } else {
                    return nullptr;
                }
            }

            void * vm = MapViewOfFile(handle, FILE_MAP_ALL_ACCESS, 0, 0, size);
            return tnew shareMemeryInfo(handle, vm);
        }

        void returnShareMemory(const shareMemeryInfo * info) {
            ::UnmapViewOfFile(info->_vm);
            ::CloseHandle(info->_smid);
            tdel info;
        }
    }

    namespace file {
        const char * getApppath() {
            static char * s_path = nullptr;
            static std::string static_path_str;
            if (nullptr == s_path) {
                s_path = tnew char[256];
                char link[256];
                ZeroMemory(link, sizeof(link));
                memset(s_path, 0, 256);
                GetModuleFileName(nullptr, s_path, 256);
                PathRemoveFileSpec(s_path);
                static_path_str = s_path;
                tools::stringReplase(static_path_str, "\\", "/");
            }

            return static_path_str.c_str();;
        }

        bool exists(const std::string & path) {
            return _access(path.c_str(), 0) == 0;
        }

        bool mkdir(const char * path) {
            if (::PathIsDirectory(path)) {
                return true;
            }
            return ::CreateDirectory(path, 0);
        }

        bool deldir(const char * path) {
            if (::PathIsDirectory(path)) {
                ::RemoveDirectory(path);
                return true;
            }

            return false;
        }

        bool delfile(const char * path) {
            return remove(path) == 0;
        }

        bool getfiles(const char * dicpath, const char * extension, OUT opaths & paths, OUT onames & names, OUT s32 & count) {
            WIN32_FIND_DATA finder;
            HANDLE error;
            int filecount = 0;
            char path[512] = { 0 };
            count = 0;

            safesprintf(path, sizeof(path), "%s/*.*", dicpath);
            error = FindFirstFile(path, &finder);
            if (INVALID_HANDLE_VALUE == error) {
                return false;
            }

            while (::FindNextFile(error, &finder)) {
                if (strcmp(finder.cFileName, ".") == 0
                    || strcmp(finder.cFileName, "..") == 0) {
                    continue;
                }

                safesprintf(path, "%s/%s", dicpath, finder.cFileName);
                if (finder.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) {
                    s32 nCount = 0;
                    getfiles(path, extension, paths, names, nCount);
                    count += nCount;
                }
                else {
                    if (0 == strcmp(extension, PathFindExtension(finder.cFileName))
                        || 0 == strcmp(extension, "*")) {
                        paths.push_back(path);
                        PathRemoveExtension(finder.cFileName);
                        names.push_back(finder.cFileName);
                        count++;
                    }
                }
            }

            return true;
        }
    }

    const std::string toUtf8(const char* font) {
        s32 len = MultiByteToWideChar(CP_ACP, 0, font, -1, nullptr, 0);
        wchar_t * wstr = (wchar_t *)alloca((len + 1) * sizeof(wchar_t));
        memset(wstr, 0, len + 1);
        MultiByteToWideChar(CP_ACP, 0, font, -1, wstr, len);
        len = WideCharToMultiByte(CP_UTF8, 0, wstr, -1, nullptr, 0, nullptr, nullptr);
        char * str = (char *)alloca(len + 1);
        memset(str, 0, len + 1);
        WideCharToMultiByte(CP_UTF8, 0, wstr, -1, str, len, nullptr, nullptr);
        return std::string(str);
    }

    const std::string toMultiString(const wchar_t * pwszUnicode) {
        int iSize;
        char * pszMultiByte;

        iSize = WideCharToMultiByte(CP_ACP, 0, pwszUnicode, -1, nullptr, 0, nullptr, nullptr);
        pszMultiByte = (char*)alloca(iSize * sizeof(char));
        WideCharToMultiByte(CP_ACP, 0, pwszUnicode, -1, pszMultiByte, iSize, nullptr, nullptr);
        return pszMultiByte;
    }
}
