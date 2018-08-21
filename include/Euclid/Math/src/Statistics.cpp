namespace Euclid
{

template<typename DerivedA, typename DerivedB>
void covariance_matrix(const Eigen::MatrixBase<DerivedA>& vars,
                       Eigen::MatrixBase<DerivedB>& cov_mat)
{
    if (vars.cols() == 1) { cov_mat.derived().setZero(1, 1); }
    else if (vars.cols() > 1) {
        auto centered = (vars.rowwise() - vars.colwise().mean()).eval();
        cov_mat = (centered.adjoint() * centered) / (vars.cols() - 1);
    }
}

} // namespace Euclid
