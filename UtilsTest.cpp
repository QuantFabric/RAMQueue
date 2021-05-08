#include <stdio.h>

#include "Utils.hpp"

int main()
{
    printf("current time: %luns\n", Utils::getTimeNs());

    printf("current time: %luus\n", Utils::getTimeUs());

    printf("current time: %lums\n", Utils::getTimeMs());

    printf("current time: %lus\n", Utils::getTimeSec());

    printf("current time: %s\n", Utils::getCurrentTimeSec());

    printf("current time: %s\n", Utils::getCurrentTimeMs());

    printf("current time: %s\n", Utils::getCurrentTimeUs());

    printf("current time: %s\n", Utils::getCurrentTimeNs());

}
// g++ -O3 UtilsTest.cpp -o test


