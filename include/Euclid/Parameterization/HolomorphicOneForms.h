#pragma once

#include <CGAL/boost/graph/Seam_mesh.h>
#include <CGAL/Surface_mesh_parameterization/Error_code.h>
#include <Eigen/Dense>
#include <Euclid/Topology/Chain.h>

namespace Euclid
{
/**@name Algorithms.
 *
 * Parameterization algorithms.
 */
/**@{ @ingroup PkgParameterization*/

/**Global conformal parameterization using holomorphic 1-forms.
 *
 * Compute the holomorphic basis.
 *
 * @param[in] mesh Input mesh, genus has to be greater than 0.
 * @param[out] primal Primal basis.
 * @param[out] conjugate Conjugate basis.
 *
 * **Reference**
 *
 * [1] Gu, X., Yau, S.T.
 * Global Conformal Surface Parameterization.
 */
template<typename Mesh, typename DerivedA, typename DerivedB>
void holomorphic_one_form_basis(const Mesh& mesh,
                                Eigen::MatrixBase<DerivedA>& primal,
                                Eigen::MatrixBase<DerivedB>& conjugate);

/**Global conformal parameterization using holomorphic 1-forms.
 *
 * Compute the holomorphic basis.
 *
 * @param[in] mesh Input mesh, genus has to be greater than 0.
 * @param[in] homology_generators Precomputed homology generators.
 * @param[out] primal Primal basis.
 * @param[out] conjugate Conjugate basis.
 *
 * **Reference**
 *
 * [1] Gu, X., Yau, S.T.
 * Global Conformal Surface Parameterization.
 */
template<typename Mesh, typename DerivedA, typename DerivedB>
void holomorphic_one_form_basis(const Mesh& mesh,
                                const VertexChains<Mesh>& homology_generators,
                                Eigen::MatrixBase<DerivedA>& primal,
                                Eigen::MatrixBase<DerivedB>& conjugate);
/**Global conformal parameterization using holomorphic 1-forms.
 *
 * Compute uv parameterization with holomorphic 1-forms. This class is
 * compatible with CGAL::Surface_mesh_parameterization::parameterize().
 *
 * **Reference**
 *
 * [1] Gu, X., Yau, S.T.
 * Global Conformal Surface Parameterization.
 */
template<typename Mesh, typename SEM, typename SVM>
class Holomorphic_one_forms_parameterizer3
{
public:
    using TriangleMesh = CGAL::Seam_mesh<Mesh, SEM, SVM>;
    using halfedge_descriptor =
        typename boost::graph_traits<TriangleMesh>::halfedge_descriptor;

public:
    /**Constructor.
     *
     * @param mesh Input mesh.
     */
    explicit Holomorphic_one_forms_parameterizer3(const Mesh& mesh);

    /**Constructor.
     *
     * @param mesh Input mesh.
     * @param homology_generators Precomputed homology generators.
     */
    Holomorphic_one_forms_parameterizer3(
        const Mesh& mesh,
        const VertexChains<Mesh>& homology_generators);

    /**Set the coeffecients for the basis.
     *
     * The coeffs are linearly combined with the basis to compute the 1-forms
     * for parameterization.
     *
     * @param coeffs The coefficients, must be a 2G x 1 matrix where G is the
     * genus of the mesh.
     */
    template<typename Derived>
    void set_coeffs(const Eigen::MatrixBase<Derived>& coeffs);

    /**Parameterize the surface.
     *
     * Only CGAL::Seam_mesh is supported. Note some parameters are not used
     * internally but only serves to be consistent with the CGAL interface.
     *
     * @param mesh The seam mesh.
     * @param bhd A boundary halfedge, not used.
     * @param uvmap A property map to store the uv coordinates.
     * @param vimap A property map to store the vertex index, not used.
     * @param vpmap A property map to store if a vertex has been parameterized.
     */
    template<typename VertexUVMap,
             typename VertexIndexMap,
             typename VertexParameterizedMap>
    CGAL::Surface_mesh_parameterization::Error_code parameterize(
        TriangleMesh& mesh,
        halfedge_descriptor bhd,
        VertexUVMap uvmap,
        VertexIndexMap vimap,
        VertexParameterizedMap vpmap);

private:
    using Scalar = typename CGAL::Kernel_traits<typename boost::property_traits<
        typename boost::property_map<Mesh, boost::vertex_point_t>::type>::
                                                    value_type>::Kernel::FT;
    using Vector = Eigen::Matrix<Scalar, Eigen::Dynamic, 1>;
    using Matrix = Eigen::Matrix<Scalar, Eigen::Dynamic, Eigen::Dynamic>;

private:
    const Mesh& _underlying_mesh;
    Matrix _primal;
    Matrix _conjugate;
    Matrix _one_forms;
};

/**@}*/
} // namespace Euclid

#include "src/HolomorphicOneForms.cpp"
