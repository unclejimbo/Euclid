#include <catch2/catch.hpp>
#include <Euclid/Util/Timer.h>

#ifdef _MSC_VER
#include <Windows.h>
#else
#include <unistd.h>
#endif

static void inline sleep_one_tenth_second()
{
#ifdef _MSC_VER
    Sleep(100);
#else
    usleep(100000);
#endif
}

TEST_CASE("Util, TImer", "[util][timer]")
{
    Euclid::Timer timer;

    REQUIRE(timer.tock() == 0.0);

    timer.tick();
    sleep_one_tenth_second();
    auto t1 = timer.tock();
    auto target1 = Approx(0.1).margin(0.02);
    REQUIRE(t1 == target1);

    timer.tick();
    sleep_one_tenth_second();
    auto t2 = timer.tock<int>();
    REQUIRE(t2 == 0);

    timer.tick();
    sleep_one_tenth_second();
    auto t3 = timer.tock<double, std::deci>();
    auto target3 = Approx(1.0).margin(0.2);
    REQUIRE(t3 == target3);

    timer.tick();
    sleep_one_tenth_second();
    auto t4 = timer.tock<int, std::deci>();
    REQUIRE(t4 == 1);
}
