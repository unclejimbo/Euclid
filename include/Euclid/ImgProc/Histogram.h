/** Histogram.
 *
 *  This package contains histogram related utilities.
 *  @defgroup PkgHistogram Histogram
 *  @ingroup PkgImgProc
 */
#pragma once

#include <vector>

#include <opencv2/imgproc.hpp>

namespace Euclid
{
/** @{*/

/** Diffusion distance between two histograms.
 *
 *  #### Reference
 *  [1]H. Ling and K. Okada,
 *  “Diffusion distance for histogram comparison,”
 *  in Computer Vision and Pattern Recognition, 2006, vol. 1, pp. 246–253.
 */
template<typename T>
double diffusion_distance(const std::vector<T>& hist1,
                          const std::vector<T>& hist2,
                          unsigned level = 5,
                          float sigma = 0.5f);

/** Diffusion distance between two histogram pyramids.
 *
 *  #### Reference
 *  [1]H. Ling and K. Okada,
 *  “Diffusion distance for histogram comparison,”
 *  in Computer Vision and Pattern Recognition, 2006, vol. 1, pp. 246–253.
 */
double diffusion_distance(const std::vector<cv::Mat>& pyramid1,
                          const std::vector<cv::Mat>& pyramid2);

/** @}*/
} // namespace Euclid

#include "src/DiffusionDistance.cpp"
