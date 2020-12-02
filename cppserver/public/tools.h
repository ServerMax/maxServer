#ifndef __tools_h__
#define __tools_h__

#include "multisys.h"
#include <math.h>
#include <chrono>
#include <string>
#include <vector>

#define tmin(a,b) (((a) < (b)) ? (a) : (b))
#define float_is_zero(f) ((f) > -0.000001f && (f) < 0.000001f)

namespace tools {
    namespace memery {
        const int Kb = 1024;
        const int Mb = 1024 * Kb;
        const int Gb = 1024 * Mb;

        inline void safeMemset(void *__restrict __dest, size_t __max, int val, size_t __n) {
            tassert(__n <= __max, "over flow");
            memset(__dest, val, (__max >= __n) ? (__n) : (__max));
        }

        inline void safeMemcpy(void *__restrict __dest, size_t __max, const void *__restrict __src, size_t __n) {
            tassert(__n <= __max, "over flow");
            memcpy(__dest, __src, (__max >= __n) ? (__n) : (__max));
        }

        inline void safeMemmove(void *__restrict __dest, size_t __max, const void *__restrict __src, size_t __n) {
            tassert(__n <= __max, "over flow");
            memmove(__dest, __src, (__max >= __n) ? (__n) : (__max));
        }

        inline bool safeStrcmp(const char *__restrict __dest, const char *__restrict __src) {
            return !strcmp(__dest, __src);
        }
        
#ifdef WIN32
        typedef void * SMID;
#else
        typedef int SMID;
#endif //WIN32
        
        struct shareMemeryInfo {
            SMID _smid;
            void * const _vm;
            shareMemeryInfo(SMID smid, void * vm) : _smid(smid), _vm(vm) {}
        };
        
        const shareMemeryInfo * getShareMemory(const std::string & path, const std::string & name, const int size);
        void returnShareMemory(const shareMemeryInfo * info);
    }
    
    namespace system {
        u64 getCurrentProcessId();
        u64 getCurrentThreadID();
    }

    namespace time {
        enum {
            second = 1000,
            minute = (60 * second),
            hour = (60 * minute),
            day = (24 * hour)
        };

        inline bool safeLocaltime(struct tm & tm, const time_t & tt) {
            const struct tm * p = localtime(&tt);
            if (nullptr == p) {
                return false;
            }
            
            memery::safeMemcpy(&tm, sizeof(tm), p, sizeof(tm));
            return true;
        }
        
        inline u64 getMillisecond() {
            return std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count();
        }
        
        inline u64 getMicrosecond() {
            return std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::high_resolution_clock::now().time_since_epoch()).count();
        }

        inline std::string getCurrentTimeString(const char * format = "%4d-%02d-%02d %02d:%02d:%02d") {
            char strtime[64] = { 0 };
            auto tt = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
            struct tm tm;
            safeLocaltime(tm, tt);
            safesprintf(strtime, sizeof(strtime), format,
                (int)tm.tm_year + 1900, (int)tm.tm_mon + 1, (int)tm.tm_mday,
                (int)tm.tm_hour, (int)tm.tm_min, (int)tm.tm_sec);
            return strtime;
        }
        // second
        inline u64 transformationTime(const std::string strtime, const char * format = "%4d-%02d-%02d %02d:%02d:%02d") {
            char *cha = (char*)strtime.data(); 
            struct tm ptm;
            s32 year, month, day, hour, minute, second;
            sscanf(cha, format, &year, &month, &day, &hour, &minute, &second);
            ptm.tm_year = year - 1900;
            ptm.tm_mon = month - 1;
            ptm.tm_mday = day;
            ptm.tm_hour = hour;
            ptm.tm_min = minute;
            ptm.tm_sec = second;
            return mktime(&ptm);
        }

        inline u64 getWeekDay(const s64 tick) {
            auto mTime = std::chrono::milliseconds(tick);
            auto tp = std::chrono::time_point<std::chrono::system_clock, std::chrono::milliseconds>(mTime);
            auto tt = std::chrono::system_clock::to_time_t(tp);
            struct tm tm;
            safeLocaltime(tm, tt);
            return tm.tm_wday;
        }

        inline std::string getTimeString(const s64 tick, const char * format = "%4d-%02d-%02d %02d:%02d:%02d"){
            char strtime[128];
            auto mTime = std::chrono::milliseconds(tick);
            auto tp = std::chrono::time_point<std::chrono::system_clock, std::chrono::milliseconds>(mTime);
            auto tt = std::chrono::system_clock::to_time_t(tp);
            struct tm tm;
            safeLocaltime(tm, tt);
            safesprintf(strtime, sizeof(strtime), format,
                (int)tm.tm_year + 1900, (int)tm.tm_mon + 1, (int)tm.tm_mday,
                (int)tm.tm_hour, (int)tm.tm_min, (int)tm.tm_sec);
            return strtime;
        }

        inline std::string getTimeStringM_D_H_M(const s64 tick, const char * format = "%02d-%02d %02d:%02d") {
            char strtime[128];
            auto mTime = std::chrono::milliseconds(tick);
            auto tp = std::chrono::time_point<std::chrono::system_clock, std::chrono::milliseconds>(mTime);
            auto tt = std::chrono::system_clock::to_time_t(tp);
            struct tm tm;
            safeLocaltime(tm, tt);
            safesprintf(strtime, sizeof(strtime), format,
                (int)tm.tm_mon + 1, (int)tm.tm_mday,
                (int)tm.tm_hour, (int)tm.tm_min);
            return strtime;
        }

        inline std::string getQuantumTimeString(const s64 tick, const char * format = "%02d:%02d:%02d") {
            int h = tick / hour;
            int min = (tick - h * hour) / minute;
            int sec = (tick - h * hour - min * minute) % minute / second;
            char strtime[128];
            safesprintf(strtime, sizeof(strtime), format, h, min, sec);
            return strtime;
        }

        inline std::string getMinuteSecondTimeString(const s64 tick, const char * format = "%02d:%02d") {
            int min = tick / minute;
            int sec = (tick - min * minute) % minute / second;
            char strtime[128];
            safesprintf(strtime, sizeof(strtime), format, min, sec);
            return strtime;
        }

        static s64 getTimeDay() {
            auto tt = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
            struct tm* ptm = localtime(&tt);
            ptm->tm_hour = 0;
            ptm->tm_min = 0;
            ptm->tm_sec = 0;
            return  mktime(ptm);
        }
    }

    namespace file {
        const char * getApppath();

        bool exists(const std::string & path);
        bool mkdir(const char * path);
        bool deldir(const char * path);
        bool delfile(const char * path);

        typedef std::vector<std::string> opaths, onames;
        bool getfiles(const char * dir, const char * ex, OUT opaths & paths, OUT onames & names, OUT s32 & count);
    }
    
    template <typename t>
    struct oHashFunction {
        size_t operator()(const t & src) const {
            return (size_t)src;
        }
    };

    template <typename t>
    struct oCompareFunction {
        bool operator()(const t & src, const t & dst) const {
            return src == dst;
        }
    };

    typedef std::vector<std::string> osplitres;
    inline s32 split(const std::string & src, const std::string & commas, osplitres & res) {
        if (src == "") {
            return 0;
        }
        
        std::string strs = src + commas;

        size_t pos = strs.find(commas);
        size_t size = strs.size();

        while (pos != std::string::npos) {
            std::string x = strs.substr(0, pos);
            if (x != "") {
                res.push_back(x.c_str());
            }
            strs = strs.substr(pos + commas.size(), size);
            pos = strs.find(commas);
        }

        return res.size();
    }

    const std::string toUtf8(const char* font);
    const std::string toMultiString(const wchar_t * pwszUnicode);

    inline bool stringAsBool(const char* b) {
        return !strcmp(b, "true");
    }

    inline const char * boolAsString(const bool b) {
        if (b) {
            return "true";
        }

        return "false";
    }

    inline s32 stringAsInt(const char * value) {
        tassert(value, "null point");
        return atoi(value);
    }

    inline float stringAsFloat(const char * value) {
        tassert(value, "null point");
        return atof(value);
    }

    inline s64 stringAsInt64(const char * value) {
        tassert(value, "null point");
        return atoi64(value);
    }

    inline std::string int64AsString(const s64 value) {
        char str[128] = { 0 };
        safesprintf(str, sizeof(str), "%lld", value);
        return str;
    }

    inline std::string unsigned64AsString(const u64 value) {
        char str[128] = { 0 };
        safesprintf(str, sizeof(str), "%llu", value);
        return str;
    }
    
    inline std::string intAsString(const s32 value) {
        char str[128] = { 0 };
        safesprintf(str, sizeof(str), "%d", value);
        return str;
    }
    
    inline std::string floatAsString(const double value) {
        char str[128] = { 0 };
        safesprintf(str, sizeof(str), "%f", value);
        return str;
    }

    inline void stringReplase(std::string & content, const std::string & src, const std::string & dst) {
        std::string::size_type pos = 0;
        std::string::size_type a = src.size();
        std::string::size_type b = dst.size();
        while ((pos = content.find(src, pos)) != std::string::npos) {
            content.replace(pos, a, dst);
            pos += b;
        }
    }

    static u64 static_seed = 0;
    inline void setSeed() {
        static_seed = time::getMicrosecond();
    }

    inline u32 rand(u32 range) {
        if (0 == range) { 
            return 0; 
        }
        if (static_seed == 0) {
            static_seed = time::getMicrosecond();
            // setSeed();
        }

        static_seed = (((static_seed = static_seed * 214013L + 2531011L) >> 16) & 0x7fff);
        return static_seed % range;
    }

    namespace code {
        inline unsigned char toHex(unsigned char x)
        {
            return  x > 9 ? x + 55 : x + 48;
        }

        inline unsigned char fromHex(unsigned char x)
        {
            unsigned char y;
            if (x >= 'A' && x <= 'Z') y = x - 'A' + 10;
            else if (x >= 'a' && x <= 'z') y = x - 'a' + 10;
            else if (x >= '0' && x <= '9') y = x - '0';
            return y;
        }

        inline std::string urlEncode(const std::string& str)
        {
            std::string strTemp = "";
            size_t length = str.length();
            for (size_t i = 0; i < length; i++)
            {
                if (isalnum((unsigned char)str[i]) ||
                    (str[i] == '-') ||
                    (str[i] == '_') ||
                    (str[i] == '.') ||
                    (str[i] == '~'))
                    strTemp += str[i];
                else if (str[i] == ' ')
                    strTemp += "+";
                else
                {
                    strTemp += '%';
                    strTemp += toHex((unsigned char)str[i] >> 4);
                    strTemp += toHex((unsigned char)str[i] % 16);
                }
            }
            return strTemp;
        }

        inline std::string urlDecode(const std::string& str)
        {
            std::string strTemp = "";
            size_t length = str.length();
            for (size_t i = 0; i < length; i++)
            {
                if (str[i] == '+') strTemp += ' ';
                else if (str[i] == '%')
                {
                    unsigned char high = fromHex((unsigned char)str[++i]);
                    unsigned char low = fromHex((unsigned char)str[++i]);
                    strTemp += high * 16 + low;
                }
                else strTemp += str[i];
            }
            return strTemp;
        }
    }
}

inline std::string & operator<<(std::string & target, const std::string & value) {
    target += value;
    return target;
}

inline std::string & operator<<(std::string & target, const s32 value) {
    target += tools::intAsString(value);
    return target;
}

inline std::string & operator<<(std::string & target, const s64 value) {
    target += tools::int64AsString(value);
    return target;
}

inline std::string & operator<<(std::string & target, const u64 value) {
    target += tools::int64AsString(value);
    return target;
}

inline std::string & operator<<(std::string & target, const double value) {
    target += tools::floatAsString(value);
    return target;
}

#endif //__tools_h__
