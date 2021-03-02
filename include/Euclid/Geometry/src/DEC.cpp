#include <queue>
#include <vector>
#include <Euclid/Geometry/TriMeshGeometry.h>

namespace Euclid
{

template<typename Mesh>
int halfedge_orientation(const Mesh& mesh, halfedge_t<Mesh> h)
{
    auto vimap = get(boost::vertex_index, mesh);
    auto s = source(h, mesh);
    auto t = target(h, mesh);
    auto sidx = get(vimap, s);
    auto tidx = get(vimap, t);
    if (sidx < tidx) {
        return -1;
    }
    else {
        return 1;
    }
}

template<typename Mesh, typename T>
void d0(const Mesh& mesh, Eigen::SparseMatrix<T>& op)
{
    using Triplet = Eigen::Triplet<T>;
    using Triplets = std::vector<Triplet>;
    auto vimap = get(boost::vertex_index, mesh);
    auto eimap = get(boost::edge_index, mesh);

    Triplets values;
    values.reserve(num_halfedges(mesh));
    for (auto e : edges(mesh)) {
        auto h = halfedge(e, mesh);
        auto vs = source(h, mesh);
        auto vt = target(h, mesh);
        auto is = get(vimap, vs);
        auto it = get(vimap, vt);
        auto ie = get(eimap, e);
        values.emplace_back(ie, is, -1);
        values.emplace_back(ie, it, 1);
    }
    op.resize(num_edges(mesh), num_vertices(mesh));
    op.setFromTriplets(values.begin(), values.end());
    op.makeCompressed();
}

template<typename Mesh, typename T>
void d1(const Mesh& mesh, Eigen::SparseMatrix<T>& op)
{
    using Triplet = Eigen::Triplet<T>;
    using Triplets = std::vector<Triplet>;
    auto eimap = get(boost::edge_index, mesh);
    auto fimap = get(boost::face_index, mesh);

    Triplets values;
    values.reserve(num_faces(mesh) * 3);
    for (auto f : faces(mesh)) {
        auto fidx = get(fimap, f);
        for (auto h : CGAL::halfedges_around_face(f, mesh)) {
            auto eidx = get(eimap, edge(h, mesh));
            values.emplace_back(fidx, eidx, halfedge_orientation(h, mesh));
        }
    }
    op.resize(num_faces(mesh), num_edges(mesh));
    op.setFromTriplets(values.begin(), values.end());
    op.makeCompressed();
}

template<typename Mesh, typename T>
void star1(const Mesh& mesh, Eigen::SparseMatrix<T>& op)
{
    op = cotangent_matrix<Mesh, T>(mesh);
}

template<typename Mesh, typename DerivedA, typename DerivedB, typename DerivedC>
void wedge1(const Mesh& mesh,
            const Eigen::MatrixBase<DerivedA>& w1,
            const Eigen::MatrixBase<DerivedB>& w2,
            Eigen::MatrixBase<DerivedC>& result)
{
    assert(w1.rows() == w2.rows());
    assert(w1.cols() == 1);
    assert(w2.cols() == 1);
    auto eimap = get(boost::edge_index, mesh);
    auto fimap = get(boost::face_index, mesh);

    result.derived().resize(num_faces(mesh), 1);
    for (auto f : faces(mesh)) {
        Eigen::Matrix<typename DerivedC::Scalar, 3, 3> W;
        size_t k = 0;
        for (auto h : halfedges_around_face(halfedge(f, mesh), mesh)) {
            auto e = edge(h, mesh);
            auto eidx = get(eimap, e);
            auto sign = halfedge_orientation(mesh, h);
            W(0, k) = w1(eidx) * sign;
            W(1, k) = w2(eidx) * sign;
            W(2, k) = 1.0;
            ++k;
        }
        result(get(fimap, f), 0) = W.determinant() / 6.0;
    }
}

template<typename Mesh, typename DerivedA, typename DerivedB, typename DerivedC>
void star_wedge1(const Mesh& mesh,
                 const Eigen::MatrixBase<DerivedA>& w1,
                 const Eigen::MatrixBase<DerivedB>& w2,
                 Eigen::MatrixBase<DerivedC>& result)
{
    assert(w1.rows() == w2.rows());
    assert(w1.cols() == 1);
    assert(w2.cols() == 1);
    auto eimap = get(boost::edge_index, mesh);
    auto fimap = get(boost::face_index, mesh);

    result.derived().resize(num_faces(mesh), 1);
    for (auto f : mesh.faces()) {
        auto s = 0.0;
        for (auto h : halfedges_around_face(halfedge(f, mesh), mesh)) {
            auto e = edge(h, mesh);
            auto eidx = get(eimap, e);
            s += w1(eidx) * w2(eidx) * cotangent_weight(e, mesh);
        }
        result(get(fimap, f), 0) = s * 0.5;
    }
}

} // namespace Euclid
