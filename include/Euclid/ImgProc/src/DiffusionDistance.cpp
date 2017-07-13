// Reference:
// [1] Diffusion Distance for Histogram Comparison
//     Haibin Ling, Kazunori Okada
//     CVPR'2006
#include <iostream>
#include <cmath>

namespace Euclid
{

template<typename T>
inline double diffusion_distance(
	const std::vector<T>& hist1,
	const std::vector<T>& hist2,
	unsigned level,
	float sigma)
{
	if (level == 0) {
		return 0.0;
	}
	if (hist1.size() != hist2.size()) {
		std::cerr << "Input histograms should be of the same size" << std::endl;
		return -1.0;
	}
	auto sz = static_cast<int>(hist1.size());
	auto width = static_cast<int>(std::sqrt(sz));
	if (width * width != sz) {
		std::cerr << "Input histograms should have square size" << std::endl;
		return -1.0;
	}
	auto h1 = cv::Mat_<T>(hist1).reshape(0, width).clone();
	auto h2 = cv::Mat_<T>(hist2).reshape(0, width).clone();
	
	double distance = 0.0;
	while (level-- >= 1) {
		// Compute the histogram distance of this layer
		cv::Mat_<T> diff;
		cv::absdiff(h1, h2, diff);
		distance += cv::sum(diff)[0];

		if (level == 0 || h1.rows <= 2 || distance == 0.0) {
			break;
		}

		// Gaussian blur and downsampling
		cv::GaussianBlur(h1, h1, cv::Size(3, 3), sigma);
		cv::GaussianBlur(h2, h2, cv::Size(3, 3), sigma);
		cv::resize(h1, h1, cv::Size(), 0.5, 0.5, cv::INTER_NEAREST);
		cv::resize(h2, h2, cv::Size(), 0.5, 0.5, cv::INTER_NEAREST);
	}
	return distance;
}

template<typename T>
double diffusion_distance(
	const std::vector<cv::Mat_<T>>& pyramid1,
	const std::vector<cv::Mat_<T>>& pyramid2)
{
	if (pyramid1.size() != pyramid2.size()) {
		std::cerr << "Input pyramids should be of the same size" << std::endl;
		return -1.0;
	}

	double distance = 0.0;
	for (auto i = 0; i < pyramid1.size(); ++i) {
		cv::Mat_<T> diff;
		cv::absdiff(pyramid1[i], pyramid2[i], diff);
		auto difference = cv::sum(diff)[0];
		if (difference == 0.0) {
			break;
		}
		distance += difference;
	}
	return distance;
}

} // namespace Euclid