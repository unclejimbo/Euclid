#include <algorithm>
#include <queue>
#include <boost/math/constants/constants.hpp>
#include <Eigen/Sparse>
#include <Euclid/Geometry/TriMeshGeometry.h>
#include <Euclid/Math/Numeric.h>
#include <Euclid/Topology/MeshTopology.h>

namespace Euclid
{

namespace _impl
{

template<typename Mesh, typename VertexCurvatureMap>
bool check_gauss_bonnet(const Mesh& mesh, VertexCurvatureMap vcm)
{
    using Scalar =
        typename boost::property_traits<VertexCurvatureMap>::value_type;
    Scalar sum = 0;
    for (auto v : vertices(mesh)) {
        sum += get(vcm, v);
    }
    auto chi = euler_characteristic(mesh);
    return eq_almost(sum, boost::math::constants::two_pi<Scalar>() * chi);
}

template<typename Mesh, typename VertexRadiusMap, typename EdgeWeightMap>
decltype(auto) elen_from_circle_packing_metric(
    const Mesh& mesh,
    typename boost::graph_traits<Mesh>::edge_descriptor e,
    VertexRadiusMap vrm,
    EdgeWeightMap ewm)
{
    auto h = halfedge(e, mesh);
    auto s = source(h, mesh);
    auto t = target(h, mesh);
    auto rs = get(vrm, s);
    auto rt = get(vrm, t);
    auto cosw = get(ewm, e);
    auto l2 = rs * rs + rt * rt + 2 * rs * rt * cosw;
    l2 = std::max(l2, static_cast<decltype(l2)>(0));
    return std::sqrt(l2);
}

template<typename T>
T cos12(T l1, T l2, T l3)
{
    auto cos = (l1 * l1 + l2 * l2 - l3 * l3) / (2 * l1 * l2);
    cos = std::min(cos, static_cast<T>(1));
    cos = std::max(cos, static_cast<T>(-1));
    return cos;
}

template<typename Mesh, typename T, typename EIMap>
T corner_angle(const Mesh& mesh,
               typename boost::graph_traits<Mesh>::halfedge_descriptor h1,
               const std::vector<T>& lengths,
               EIMap eimap)
{
    auto h2 = next(h1, mesh);
    auto h3 = prev(h1, mesh);
    auto e1 = edge(h1, mesh);
    auto e2 = edge(h2, mesh);
    auto e3 = edge(h3, mesh);
    auto l1 = lengths[get(eimap, e1)];
    auto l2 = lengths[get(eimap, e2)];
    auto l3 = lengths[get(eimap, e3)];
    auto c = _impl::cos12(l1, l2, l3);
    return std::acos(c);
}

template<typename Mesh, typename T, typename EIMap>
T gaussian(const Mesh& mesh,
           typename boost::graph_traits<Mesh>::vertex_descriptor v,
           const std::vector<T>& lengths,
           EIMap eimap)
{
    T k;
    if (CGAL::is_border(v, mesh)) {
        k = boost::math::constants::pi<T>();
    }
    else {
        k = boost::math::constants::two_pi<T>();
    }
    for (auto h : halfedges_around_target(v, mesh)) {
        if (!CGAL::is_border(h, mesh)) {
            k -= corner_angle(mesh, h, lengths, eimap);
        }
    }
    return k;
}

template<typename Mesh,
         typename VertexRadiusMap,
         typename EdgeIndexMap,
         typename T>
void solve_power_circle(const Mesh& mesh,
                        typename boost::graph_traits<Mesh>::face_descriptor f,
                        VertexRadiusMap vrm,
                        EdgeIndexMap eim,
                        const std::vector<T>& lengths,
                        std::vector<T>& weights)
{
    // construct local coordinates for a triangle
    auto h = *(halfedges_around_face(halfedge(f, mesh), mesh).first);
    auto v = target(h, mesh);
    T l1 = lengths[get(eim, edge(h, mesh))];
    T x1 = 0;
    T y1 = 0;
    T r1 = get(vrm, v);

    h = next(h, mesh);
    v = target(h, mesh);
    T l2 = lengths[get(eim, edge(h, mesh))];
    T x2 = l2;
    T y2 = 0;
    T r2 = get(vrm, v);

    h = next(h, mesh);
    v = target(h, mesh);
    T l3 = lengths[get(eim, edge(h, mesh))];
    T cos = cos12(l1, l2, l3);
    T sin = std::sqrt(1 - cos * cos);
    T x3 = cos * l1;
    T y3 = sin * l1;
    T r3 = get(vrm, v);

    // solve the power circle in local coordinates
    auto x1_2 = x1 * x1;
    auto y1_2 = y1 * y1;
    auto r1_2 = r1 * r1;
    auto x2_2 = x2 * x2;
    auto y2_2 = y2 * y2;
    auto r2_2 = r2 * r2;
    auto x3_2 = x3 * x3;
    auto y3_2 = y3 * y3;
    auto r3_2 = r3 * r3;
    auto a = -x1_2 + x2_2 - y1_2 + y2_2 + r1_2 - r2_2;
    auto b = -x1_2 + x3_2 - y1_2 + y3_2 + r1_2 - r3_2;
    auto c = 2 * ((y1 - y2) * (x3 - x1) - (y1 - y3) * (x2 - x1));

    auto y = ((x2 - x1) * b - (x3 - x1) * a) / c;
    auto x = (2 * (y1 - y3) * y + b) / (2 * (x3 - x1));

    // calculate the height from the center to each edge
    auto d1 = std::sqrt((x1 - x) * (x1 - x) + (y1 - y) * (y1 - y));
    auto d2 = std::sqrt((x2 - x) * (x2 - x) + (y2 - y) * (y2 - y));
    auto d3 = std::sqrt((x3 - x) * (x3 - x) + (y3 - y) * (y3 - y));

    auto cos1 = cos12(d3, l1, d1);
    auto sin1 = std::sqrt(1 - cos1 * cos1);
    auto height1 = d3 * sin1;
    h = next(h, mesh);
    weights[get(eim, edge(h, mesh))] += height1;

    auto cos2 = cos12(d1, l2, d2);
    auto sin2 = std::sqrt(1 - cos2 * cos2);
    auto height2 = d1 * sin2;
    h = next(h, mesh);
    weights[get(eim, edge(h, mesh))] += height2;

    auto cos3 = cos12(d2, l3, d3);
    auto sin3 = std::sqrt(1 - cos3 * cos3);
    auto height3 = d2 * sin3;
    h = next(h, mesh);
    weights[get(eim, edge(h, mesh))] += height3;
}

template<typename Mesh,
         typename VertexRadiusMap,
         typename EdgeWeightMap,
         typename VertexIndexMap,
         typename EdgeIndexMap,
         typename T>
void hessian(const Mesh& mesh,
             VertexRadiusMap vrm,
             EdgeWeightMap ewm,
             VertexIndexMap vim,
             EdgeIndexMap eim,
             const std::vector<T>& lengths,
             Eigen::SparseMatrix<T>& H)
{
    using Triplet = Eigen::Triplet<T>;
    using Triplets = std::vector<Triplet>;

    std::vector<T> weights(num_edges(mesh), 0);
    for (auto f : faces(mesh)) {
        solve_power_circle(mesh, f, vrm, eim, lengths, weights);
    }

    Triplets triplets;
    triplets.reserve(num_halfedges(mesh) + num_vertices(mesh));
    for (auto v : vertices(mesh)) {
        T sum = 0;
        auto vi_idx = get(vim, v);
        for (auto h : halfedges_around_target(v, mesh)) {
            auto vj_idx = get(vim, source(h, mesh));
            auto eidx = get(eim, edge(h, mesh));
            auto w = weights[eidx] / lengths[eidx];
            sum += w;
            triplets.emplace_back(vi_idx, vj_idx, -w);
        }
        triplets.emplace_back(vi_idx, vi_idx, sum);
    }

    H.resize(num_vertices(mesh), num_vertices(mesh));
    H.setFromTriplets(triplets.begin(), triplets.end());
    H.makeCompressed();
}

template<typename Mesh,
         typename VertexRadiusMap,
         typename EdgeWeightMap,
         typename SEM,
         typename SVM,
         typename VertexUVMap,
         typename VertexParameterizedMap>
void embed_circle_packing_metric(const Mesh& mesh,
                                 VertexRadiusMap vrm,
                                 EdgeWeightMap ewm,
                                 CGAL::Seam_mesh<Mesh, SEM, SVM>& seam_mesh,
                                 VertexUVMap uvm,
                                 VertexParameterizedMap vpm)
{
    using Seam_mesh = CGAL::Seam_mesh<Mesh, SEM, SVM>;
    using fd = typename boost::graph_traits<Seam_mesh>::face_descriptor;
    using Point_2 = typename boost::property_traits<VertexUVMap>::value_type;
    using Point_3 = typename CGAL::Kernel_traits<Point_2>::Kernel::Point_3;
    using FT = typename CGAL::Kernel_traits<Point_2>::Kernel::FT;

    // mesh and seam_mesh have the same number of faces
    // seam_mesh does not have face_index, use mesh's
    auto fimap = get(boost::face_index, mesh);
    std::vector<bool> visited(num_faces(seam_mesh), false);
    std::queue<fd> queue;

    // flatten the root face
    auto froot = *(faces(seam_mesh).first);
    auto hroot = halfedge(froot, seam_mesh);
    auto vroot = target(hroot, seam_mesh);
    auto l0 = _impl::elen_from_circle_packing_metric(
        mesh, edge(hroot.tmhd, mesh), vrm, ewm);
    put(uvm, vroot, Point_2(0, 0));
    put(vpm, vroot, true);
    hroot = next(hroot, seam_mesh);
    vroot = target(hroot, seam_mesh);
    auto l1 = _impl::elen_from_circle_packing_metric(
        mesh, edge(hroot.tmhd, mesh), vrm, ewm);
    put(uvm, vroot, Point_2(l1, 0));
    put(vpm, vroot, true);
    hroot = next(hroot, seam_mesh);
    vroot = target(hroot, seam_mesh);
    auto l2 = _impl::elen_from_circle_packing_metric(
        mesh, edge(hroot.tmhd, mesh), vrm, ewm);
    auto cos0 = _impl::cos12(l0, l1, l2);
    auto sin0 = std::sqrt(1 - cos0 * cos0);
    put(uvm, vroot, Point_2(cos0 * l0, sin0 * l0));
    put(vpm, vroot, true);
    visited[get(fimap, face(hroot.tmhd, mesh))] = true;
    queue.push(froot);
    size_t nv = 3;
    size_t nf = 1;

    // propogate the flattening
    while (!queue.empty()) {
        auto f = queue.front();
        queue.pop();
        for (auto h :
             halfedges_around_face(halfedge(f, seam_mesh), seam_mesh)) {
            auto hoppo = opposite(h, seam_mesh);
            if (!CGAL::is_border(hoppo, seam_mesh)) {
                auto foppo_idx = get(fimap, face(hoppo.tmhd, mesh));
                if (visited[foppo_idx]) {
                    continue;
                }
                visited[foppo_idx] = true;
                queue.push(face(hoppo, seam_mesh));
                ++nf;

                auto vk = target(next(hoppo, seam_mesh), seam_mesh);
                if (get(vpm, vk)) {
                    continue;
                }
                put(vpm, vk, true);
                ++nv;

                // use the intersection point of two circles as the next
                // flattened vertex's uv-coordinate
                auto vs = source(hoppo, seam_mesh);
                auto vt = target(hoppo, seam_mesh);
                auto ps = get(uvm, vs);
                auto pt = get(uvm, vt);
                auto ls = _impl::elen_from_circle_packing_metric(
                    mesh, edge(prev(hoppo.tmhd, mesh), mesh), vrm, ewm);
                auto lt = _impl::elen_from_circle_packing_metric(
                    mesh, edge(next(hoppo.tmhd, mesh), mesh), vrm, ewm);
                auto d = std::sqrt((pt - ps).squared_length());
                auto a = (ls * ls - lt * lt + d * d) / (2 * d);
                auto h =
                    std::sqrt(std::max(ls * ls - a * a, static_cast<FT>(0)));
                auto pm = ps + a * (pt - ps) / d;
                auto u = pm.x() + h * (pt.y() - ps.y()) / d;
                auto v = pm.y() - h * (pt.x() - ps.x()) / d;
                assert(!std::isnan(u));
                assert(!std::isnan(v));

                // check orientation
                Point_3 pi(ps.x(), ps.y(), 0);
                Point_3 pj(pt.x(), pt.y(), 0);
                Point_3 pk(u, v, 0);
                if (CGAL::cross_product(pj - pi, pk - pi).z() > 0) {
                    put(uvm, vk, Point_2(u, v));
                }
                else {
                    u = pm.x() - h * (pt.y() - ps.y()) / d;
                    v = pm.y() + h * (pt.x() - ps.x()) / d;
                    put(uvm, vk, Point_2(u, v));
                }
            }
        }
    }
    assert(nv == num_vertices(seam_mesh));
    assert(nf == num_faces(seam_mesh));
}

} // namespace _impl

template<typename Mesh, typename VertexRadiusMap, typename EdgeWeightMap>
void circle_packing_metric(Mesh& mesh, VertexRadiusMap vrm, EdgeWeightMap ewm)
{
    using Scalar = typename boost::property_traits<VertexRadiusMap>::value_type;
    auto eimap = get(boost::edge_index, mesh);

    // compute edge lengths
    std::vector<Scalar> lens;
    lens.reserve(num_edges(mesh));
    for (auto e : edges(mesh)) {
        auto l = edge_length(e, mesh);
        lens.push_back(l);
    }

    // compute vertex radii
    for (auto v : vertices(mesh)) {
        Scalar radius;
        bool initialized = false;
        for (auto h : halfedges_around_target(v, mesh)) {
            if (!CGAL::is_border(h, mesh)) {
                auto e1 = get(eimap, edge(h, mesh));
                auto e2 = get(eimap, edge(next(h, mesh), mesh));
                auto e3 = get(eimap, edge(prev(h, mesh), mesh));
                auto r = (lens[e1] + lens[e2] - lens[e3]) / 2;
                if (!initialized) {
                    radius = r;
                    initialized = true;
                }
                else {
                    radius = std::min(radius, r);
                }
            }
        }
        put(vrm, v, radius);
    }

    // compute edge weights
    for (auto e : edges(mesh)) {
        auto h = halfedge(e, mesh);
        auto s = source(h, mesh);
        auto t = target(h, mesh);
        auto rs = get(vrm, s);
        auto rt = get(vrm, t);
        auto l = lens[get(eimap, e)];
        // don't use cos12 since it's okay if cosw > 1
        auto cosw = (l * l - rs * rs - rt * rt) / (2 * rs * rt);
        put(ewm, e, cosw);
    }
}

template<typename Mesh,
         typename VertexRadiusMap,
         typename EdgeWeightMap,
         typename SEM,
         typename SVM,
         typename VertexUVMap>
void embed_circle_packing_metric(const Mesh& mesh,
                                 VertexRadiusMap vrm,
                                 EdgeWeightMap ewm,
                                 CGAL::Seam_mesh<Mesh, SEM, SVM>& seam_mesh,
                                 VertexUVMap uvm)
{
    using Seam_mesh = CGAL::Seam_mesh<Mesh, SEM, SVM>;
    using Vertex = typename boost::graph_traits<Seam_mesh>::vertex_descriptor;
    using VPM = std::map<Vertex, bool>;

    VPM vpm;
    _impl::embed_circle_packing_metric(
        mesh,
        vrm,
        ewm,
        seam_mesh,
        uvm,
        boost::associative_property_map<VPM>(vpm));
}

template<typename Mesh,
         typename VertexRadiusMap,
         typename EdgeWeightMap,
         typename VertexCurvatureMap>
RicciFlowSolverStatus ricci_flow(Mesh& mesh,
                                 VertexRadiusMap vrm,
                                 EdgeWeightMap ewm,
                                 VertexCurvatureMap vcm,
                                 const RicciFlowSolverSettings& settings)
{
    std::cout << "==========Ricci flow==========" << std::endl
              << "solver type: ";
    if (settings.type == RicciFlowSolverType::GradientDescent) {
        std::cout << "gradient descent" << std::endl;
    }
    else {
        std::cout << "Newton" << std::endl;
    }
    std::cout << "step: " << settings.step << std::endl
              << "eps: " << settings.eps << std::endl
              << "max iters: " << settings.max_iters << std::endl
              << "==============================" << std::endl;

    if (!_impl::check_gauss_bonnet(mesh, vcm)) {
        std::cerr << "Error: target curvatures are not admissible."
                  << std::endl;
        return RicciFlowSolverStatus::InvalidInput;
    }

    using Scalar = typename boost::property_traits<VertexRadiusMap>::value_type;
    using SpMat = Eigen::SparseMatrix<Scalar>;
    using Vec = Eigen::Matrix<Scalar, Eigen::Dynamic, 1>;
    auto vimap = get(boost::vertex_index, mesh);
    auto eimap = get(boost::edge_index, mesh);
    std::vector<Scalar> lengths(num_edges(mesh));
    auto type = settings.type;

    Eigen::SimplicialLDLT<SpMat> solver;
    if (settings.type == RicciFlowSolverType::Newton) {
        // prefactor the solver using graph Laplacian
        auto L = graph_laplacian_matrix(mesh);
        solver.analyzePattern(L);
    }

    for (auto iter = 0; iter < settings.max_iters; ++iter) {
        // update edge lengths from current vertex radii and edge weights
        for (auto e : edges(mesh)) {
            lengths[get(eimap, e)] =
                _impl::elen_from_circle_packing_metric(mesh, e, vrm, ewm);
        }

        // compute the gradients based on solver type
        Scalar kdiff_max = 0;
        Vec grads = Vec::Zero(num_vertices(mesh));
        if (type == RicciFlowSolverType::Newton) {
            // FIXME: Newton is not working
            assert(type == RicciFlowSolverType::GradientDescent);
            // SpMat H;
            // _impl::hessian(mesh, vrm, ewm, vimap, eimap, lengths, H);
            // solver.factorize(H);
            // Vec b(num_vertices(mesh));
            // for (auto v : vertices(mesh)) {
            //     auto kdiff =
            //         get(vcm, v) - _impl::gaussian(mesh, v, lengths, eimap);
            //     b(get(vimap, v)) = kdiff;
            //     kdiff_max = std::max(std::abs(kdiff), kdiff_max);
            // }
            // grads = solver.solve(b);
        }
        else { // GradientDescent
            for (auto v : vertices(mesh)) {
                auto kdiff =
                    get(vcm, v) - _impl::gaussian(mesh, v, lengths, eimap);
                grads(get(vimap, v)) = kdiff * settings.step;
                kdiff_max = std::max(std::abs(kdiff), kdiff_max);
            }
        }

        // optimize the Ricci energy
        for (auto v : vertices(mesh)) {
            auto u = std::log(get(vrm, v)); // convert radius to scaling factor
            u += grads(get(vimap, v));      // Ricci flow
            put(vrm, v, std::exp(u));       // convert back to vertex radius
        }

        // stopping criterion
        if (settings.verbose) {
            std::cout << "iter: " << iter << ", error: " << kdiff_max
                      << std::endl;
        }
        if (kdiff_max < settings.eps) {
            std::cout << "Optimization done." << std::endl;
            return RicciFlowSolverStatus::Optimal;
        }
    }
    std::cout << "Max iterations reached." << std::endl;
    return RicciFlowSolverStatus::Suboptimal;
}

template<typename Mesh, typename SEM, typename SVM>
Ricci_flow_parameterizer3<Mesh, SEM, SVM>::Ricci_flow_parameterizer3(
    const Mesh& mesh)
    : _underlying_mesh(mesh), _vrpm(_vrm), _ewpm(_ewm), _vcpm(_vcm)
{
    circle_packing_metric(_underlying_mesh, _vrpm, _ewpm);
    for (auto v : vertices(mesh)) {
        put(_vcpm, v, static_cast<Scalar>(0));
    }
}

template<typename Mesh, typename SEM, typename SVM>
void Ricci_flow_parameterizer3<Mesh, SEM, SVM>::set_solver_settings(
    const RicciFlowSolverSettings& settings)
{
    _settings = settings;
}

template<typename Mesh, typename SEM, typename SVM>
void Ricci_flow_parameterizer3<Mesh, SEM, SVM>::add_cone(
    typename boost::graph_traits<Mesh>::vertex_descriptor v,
    Scalar k)
{
    put(_vcpm, v, k * boost::math::constants::pi<Scalar>());
}

template<typename Mesh, typename SEM, typename SVM>
template<typename VertexUVMap,
         typename VertexIndexMap,
         typename VertexParameterizedMap>
CGAL::Surface_mesh_parameterization::Error_code
Ricci_flow_parameterizer3<Mesh, SEM, SVM>::parameterize(
    TriangleMesh& mesh,
    halfedge_descriptor,
    VertexUVMap uvmap,
    VertexIndexMap,
    VertexParameterizedMap vpmap)
{
    auto status = ricci_flow(_underlying_mesh, _vrpm, _ewpm, _vcpm, _settings);
    _impl::embed_circle_packing_metric(
        _underlying_mesh, _vrpm, _ewpm, mesh, uvmap, vpmap);
    if (status == RicciFlowSolverStatus::InvalidInput) {
        return CGAL::Surface_mesh_parameterization::ERROR_WRONG_PARAMETER;
    }
    else {
        return CGAL::Surface_mesh_parameterization::OK;
    }
}

} // namespace Euclid
