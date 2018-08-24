#include <catch2/catch.hpp>
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
    auto t1 = timer.tock();
    REQUIRE(t1 == Approx(0.001).margin(0.001));

    timer.tick();
    sleep_one_ms();
    auto t2 = timer.tock<int>();
    REQUIRE(t2 == 0);

    timer.tick();
    sleep_one_ms();
    auto t3 = timer.tock<double, std::milli>();
    REQUIRE(t3 == Approx(1.0).margin(1.0));

    timer.tick();
    sleep_one_ms();
    auto t4 = timer.tock<int, std::milli>();
    REQUIRE(t4 == Approx(1).margin(1));
}
