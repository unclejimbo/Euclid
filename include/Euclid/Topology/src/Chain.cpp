namespace Euclid
{

template<typename Mesh>
bool is_chain(const Mesh& mesh, const VertexChain<Mesh>& chain)
{
    for (size_t i = 0; i < chain.size(); ++i) {
        auto v0 = chain[0];
        auto v1 = chain[1];
        auto h = halfedge(v0, v1, mesh);
        if (!h.second) { return false; }
    }
    return true;
}

template<typename Mesh>
bool is_loop(const Mesh& mesh, const VertexChain<Mesh>& chain)
{
    auto v0 = chain.front();
    auto v1 = chain.back();
    auto h = halfedge(v0, v1, mesh);
    return h.second && is_chain(mesh, chain);
}

} // namespace Euclid
