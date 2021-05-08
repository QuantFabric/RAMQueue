#include "ConcurrentHashMap.hpp"
#include "Utils.hpp"

#define N 100000000

struct MarketData
{
    char Ticker[16];
    double OpenPrice;
    double LastPrice;
    double ClosePrice;
};

double getdetlatimeofday(struct timeval *begin, struct timeval *end)
{
    return (end->tv_sec + end->tv_usec * 1.0 / 1000000) -
           (begin->tv_sec + begin->tv_usec * 1.0 / 1000000);
}

int main(int ragc, char *argv[])
{
    // producer
    std::unordered_map<std::string, int> tickerIndexMap;
    tickerIndexMap["SZ000001"] = 0;
    tickerIndexMap["SZ000002"] = 1;
    tickerIndexMap["SZ000003"] = 2;
    ConcurrentHashMap<MarketData> conMap(3, 0XFF000001, tickerIndexMap);

    struct timeval begin, end;
    gettimeofday(&begin, NULL);
    for (size_t i = 0; i < N; i++)
    {
        MarketData market;
        strncpy(market.Ticker, "SZ000001", sizeof(market.Ticker));
        market.OpenPrice = 1.0;
        conMap.insert("SZ000001", market);

        strncpy(market.Ticker, "SZ000002", sizeof(market.Ticker));
        market.OpenPrice = 2.0;
        conMap.insert("SZ000002", market);

        strncpy(market.Ticker, "SZ000003", sizeof(market.Ticker));
        market.OpenPrice = 3.0;
        conMap.insert("SZ000003", market);
    }

    gettimeofday(&end, NULL);
    double tm = getdetlatimeofday(&begin, &end);
    printf("producer tid=%lu %f MB/s %f msg/s elapsed= %f size= %u\n", pthread_self(), 3 * N * sizeof(MarketData) * 1.0 / (tm * 1024 * 1024), 3 * N * 1.0 / tm, tm, 3 * N);

    // consumer

    // std::unordered_map<std::string, int> tickerIndexMap;
    // tickerIndexMap["SZ000001"] = 0;
    // tickerIndexMap["SZ000002"] = 1;
    // tickerIndexMap["SZ000003"] = 2;
    // ConcurrentHashMap<MarketData> conMap(3, 0XFF000001, tickerIndexMap);

    // struct timeval begin, end;
    // gettimeofday(&begin, NULL);
    // for (size_t i = 0; i < N; i++)
    // {
    //     MarketData market;
    //     std::string ticker = "SZ000001";
    //     conMap.get(ticker, market);
    //     // printf("Ticker %s OpenPrice %2.f\n", market.Ticker, market.OpenPrice);

    //     ticker = "SZ000002";
    //     conMap.get(ticker, market);
    //     // printf("Ticker %s OpenPrice %2.f\n", market.Ticker, market.OpenPrice);

    //     ticker = "SZ000003";
    //     conMap.get(ticker, market);
    //     // printf("Ticker %s OpenPrice %2.f\n", market.Ticker, market.OpenPrice);
    // }
    // gettimeofday(&end, NULL);
    // double tm = getdetlatimeofday(&begin, &end);
    // printf("consumer tid=%lu %f MB/s %f msg/s elapsed= %f size= %u\n", pthread_self(), 3 * N * sizeof(MarketData) * 1.0 / (tm * 1024 * 1024), 3 * N * 1.0 / tm, tm, 3 * N);

    // wait
    getchar();

    return 0;
}

// g++ -O3  -std=c++11 ConcurrentHashMapTest.cpp -o producer
// g++ -O3  -std=c++11 ConcurrentHashMapTest.cpp -o consumer

// 删除共享内存
// ipcs | grep user |grep 0xff | awk '{print $1}' | xargs -d '\n' ipcrm -M