/*******************************************************
* Package Overview:                                    *
* Extend the functionalities of CGAL's Kernel Geometry *
*******************************************************/
#pragma once
#include <CGAL/Vector_3.h>
#include <Eigen/Dense>

namespace Euclid
{

template<typename Kernel>
typename Kernel::FT length(const CGAL::Vector_3<Kernel>& vec);

template<typename Kernel>
void normalize(CGAL::Vector_3<Kernel>& vec);

template<typename Kernel>
CGAL::Vector_3<Kernel> normalized(const CGAL::Vector_3<Kernel>& vec);

template<typename Kernel>
typename Kernel::FT 
area(const CGAL::Point_3<Kernel>& p1,
	const CGAL::Point_3<Kernel>& p2,
	const CGAL::Point_3<Kernel>& p3);

template<typename FT, typename CGALVec>
Eigen::Matrix<FT, 3, 1> cgal_to_eigen(const CGALVec& vec);

template<typename CGALVec, typename FT>
CGALVec eigen_to_cgal(const Eigen::Matrix<FT, 3, 1>& vec);

} // namespace Euclid

#include "src/KernelGeometry.cpp"
