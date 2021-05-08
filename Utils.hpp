#include <time.h>

namespace Utils
{

/********************************
struct timespec
{
    time_t tv_sec;
    long int tv_nsec;
};

struct tm {
    int tm_sec;         
    int tm_min;         
    int tm_hour;       
    int tm_mday;        
    int tm_mon;         
    int tm_year;        
    int tm_wday;        
    int tm_yday;        
    int tm_isdst;
};
********************************/

unsigned long getTimeNs()
{
    struct timespec timeStamp={0, 0};
    clock_gettime(CLOCK_REALTIME, &timeStamp);
    return timeStamp.tv_sec * 1e9 + timeStamp.tv_nsec;;
}

unsigned long getTimeUs()
{
    return getTimeNs() / 1e3;
}

unsigned long getTimeMs()
{
    return getTimeNs() / 1e6;
}

unsigned long getTimeSec()
{
    struct timespec timeStamp={0, 0};
    clock_gettime(CLOCK_REALTIME, &timeStamp);
    return timeStamp.tv_sec;  
}

const char* getCurrentTimeSec()
{
    time_t current = getTimeSec();
    struct tm* timeStamp = localtime(&current);
    static char szBuffer[64] = {0};
    strftime(szBuffer, sizeof(szBuffer), "%Y-%m-%d %H:%M:%S", timeStamp);
    return szBuffer;
}

const char* getCurrentTimeMs()
{
    unsigned long n = Utils::getTimeMs();
    time_t current = n / 1e3;
    struct tm* timeStamp = localtime(&current);
    static char szBuffer[64] = {0};
    strftime(szBuffer, sizeof(szBuffer), "%Y-%m-%d %H:%M:%S", timeStamp);
    unsigned long mod = 1e3;
    unsigned long ret = n % mod;
    sprintf(szBuffer, "%s.%03u", szBuffer, ret);
    return szBuffer;
}

const char* getCurrentTimeUs()
{
    unsigned long n = Utils::getTimeUs();
    time_t current = n / 1e6;
    struct tm* timeStamp = localtime(&current);
    static char szBuffer[64] = {0};
    strftime(szBuffer, sizeof(szBuffer), "%Y-%m-%d %H:%M:%S", timeStamp);
    unsigned long mod = 1e6;
    unsigned long ret = n % mod;
    sprintf(szBuffer, "%s.%06u", szBuffer, ret);
    return szBuffer;
}

const char* getCurrentTimeNs()
{
    unsigned long n = Utils::getTimeNs();
    time_t current = n / 1e9;
    struct tm* timeStamp = localtime(&current);
    static char szBuffer[64] = {0};
    strftime(szBuffer, sizeof(szBuffer), "%Y-%m-%d %H:%M:%S", timeStamp);
    unsigned long mod = 1e9;
    unsigned long ret = n % mod;
    sprintf(szBuffer, "%s.%09u", szBuffer, ret);
    return szBuffer;
}

}
