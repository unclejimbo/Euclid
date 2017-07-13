/*************************************************
* Package Overview:                              *
* Image processing utilities, OpenCV is required *
*************************************************/
#pragma once
#include <opencv2/imgproc.hpp>
#include <vector>

namespace Euclid
{

// Compute for a pair of histograms
template<typename T>
double diffusion_distance(
	const std::vector<T>& hist1,
	const std::vector<T>& hist2,
	unsigned level = 5,
	float sigma = 0.5f);

// Compute for a pair of precomputed histogram pyramids
template<typename T>
double diffusion_distance(
	const std::vector<cv::Mat_<T>>& pyramid1,
	const std::vector<cv::Mat_<T>>& pyramid2);

} // namespace Euclid

#include "src/DiffusionDistance.cpp"
