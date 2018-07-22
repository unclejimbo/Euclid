#include <catch.hpp>
#include <Euclid/Util/Timer.h>

#ifdef _MSC_VER
#include <Windows.h>
#else
#include <unistd.h>
#endif

static void inline sleep_one_ms()
{
#ifdef _MSC_VER
    Sleep(1);
#else
    usleep(1000);
#endif
}

TEST_CASE("Util, TImer", "[util][timer]")
{
    Euclid::Timer timer;

    REQUIRE(timer.tock() == 0.0);

    timer.tick();
    sleep_one_ms();
    REQUIRE(timer.tock() >= 0.0001);

    timer.tick();
    sleep_one_ms();
    REQUIRE(timer.tock<int>() == 0);

    timer.tick();
    sleep_one_ms();
    REQUIRE(timer.tock<double, std::milli>() >= 1);

    timer.tick();
    sleep_one_ms();
    REQUIRE(timer.tock<int, std::milli>() == 1);
}
