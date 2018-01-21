#include <Euclid/ImgProc/Histogram.h>
#include <catch.hpp>

TEST_CASE("Pckage: ImgProc/Histogram", "[histogram]")
{
    std::vector<float> hist1{1.0f, 1.0f, 1.0f, 1.0f};
    std::vector<float> hist2{0.0f, 0.0f, 0.0f, 0.0f};
    cv::Mat hist3(128, 128, CV_8UC1);
    cv::Mat hist4(128, 128, CV_8UC1);
    std::vector<cv::Mat> pyramid1{hist3};
    std::vector<cv::Mat> pyramid2{hist4};

    SECTION("Function: diffusion_distance")
    {
        // Just to make sure everything compiles.
        Euclid::diffusion_distance(hist1, hist2);
        Euclid::diffusion_distance(pyramid1, pyramid2);
    }
}
