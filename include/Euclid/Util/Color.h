/** Color utilities.
 *
 *  This package provides tools to help with colors in visualisation.
 *
 *  @defgroup PkgColor Color
 *  @ingroup PkgUtil
 */
#pragma once

#include <vector>
#include <igl/colormap.h>

namespace Euclid
{
/**@{*/

/** Color map.
 *
 *  Map values to colors.
 *
 *  @param cm Color map type.
 *  @param values Input values.
 *  @param colors Output colors
 *  @param to255 Set to true if you want to output colors to range [0, 255],
 *  default to range [0, 1].
 *  @param alpha Set to true if you want to generate rgba colors instead of rgb.
 *  The alpha value is set to totally opaque.
 *  @param inverse Set to true if you want to invert the color map so that the
 *  smallest value is mapped to the color corresponding to the largest one and
 *  vice versa.
 */
template<typename T1, typename T2>
void colormap(const igl::ColorMapType& cm,
              const std::vector<T1>& values,
              std::vector<T2>& colors,
              bool to255 = false,
              bool alpha = false,
              bool inverse = false);

/** Generate a set of random colors.
 *
 *  @param n Number of colors to generate.
 *  @param colors Output colors.
 *  @param to255 Set to true if you want to output colors to range [0, 255],
 *  default to range [0, 1].
 *  @param alpha Set to true if you want to generate rgba colors instead of rgb.
 *  The alpha value is set to totally opaque.
 */
template<typename T>
void rnd_colors(unsigned n,
                std::vector<T>& colors,
                bool to255 = false,
                bool alpha = false);

/**@}*/
} // namespace Euclid

#include "src/Color.cpp"
