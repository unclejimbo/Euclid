/*********************************************************
* Package Overview                                       *
* (O)beject (O)rientated (B)oundingbox uses pca to build *
* an object aligned bounding box                         *
*********************************************************/
#pragma once
#include <Eigen/Dense>

namespace Euclid
{

template<typename Mesh>
class OBB
{
	using Vec3 = Eigen::Vector3f;
	using Mat3 = Eigen::Matrix3f;

public:
	explicit OBB(const Mesh& mesh);
	explicit OBB(const std::vector<Vec3>& vertices);
	~OBB();

	Vec3 center() const;
	std::array<Vec3, 3> directions() const;
	Vec3 halfsize() const;
	float radius() const;
	Vec3 lbb() const;
	Vec3 lbf() const;
	Vec3 ltb() const;
	Vec3 ltf() const;
	Vec3 rbb() const;
	Vec3 rbf() const;
	Vec3 rtb() const;
	Vec3 rtf() const;

private:
	Mat3 _directions;
	Vec3 _center;
	Vec3 _halfsize;
};

} // namespace Euclid

#include "src/OBB.cpp"
