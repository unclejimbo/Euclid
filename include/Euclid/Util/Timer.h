/** Timer utility.
 *
 *  @defgroup PkgTimer Timer
 *  @ingroup PkgUtil
 */
#pragma once

#include <chrono>

namespace Euclid
{
/** @{*/

class Timer
{
public:
    void tick()
    {
        _started = true;
        _start = std::chrono::high_resolution_clock::now();
    }

    template<typename Rep = double, typename Period = std::ratio<1>>
    Rep tock()
    {
        if (!_started) { return Rep(0); }
        else {
            auto t = std::chrono::high_resolution_clock::now();
            _started = false;
            return std::chrono::duration_cast<
                       std::chrono::duration<Rep, Period>>(t - _start)
                .count();
        }
    }

private:
    bool _started = false;
    std::chrono::time_point<std::chrono::high_resolution_clock> _start;
};

/** @}*/
} // namespace Euclid
