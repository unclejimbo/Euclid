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

/** A simple timer.
 *
 */
class Timer
{
public:
    /** Start the timer.
     *
     *  The timer is reset when tick is called.
     */
    void tick()
    {
        _started = true;
        _start = std::chrono::high_resolution_clock::now();
    }

    /** Stop the timer and return the elapsed time.
     *
     *  You could control the representation and precesion of the timer using
     *  the template arguments. The default is to count seconds in double
     *  format, like 3.14159 seconds. You could use, for example, `tock<int,
     *  std::milli>()` to return integer valued milliseconds. See std::chrono
     *  for detailed explainations of these parameters. If tick is not called,
     *  tock will return 0.
     *
     *  @tparam Rep The representational type of the elapsed time, default to
     *  double.
     *  @tparam Period The unit of the elapsed time, default to second.
     */
    template<typename Rep = double, typename Period = std::ratio<1>>
    Rep tock()
    {
        auto t = std::chrono::high_resolution_clock::now();
        if (!_started) {
            return Rep(0);
        }
        else {
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
