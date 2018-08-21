namespace Euclid
{

template<typename Scalar,
         int Options,
         typename StorageIndex,
         typename UnaryFunction>
void for_each(Eigen::SparseMatrix<Scalar, Options, StorageIndex>& mat,
              UnaryFunction f)
{
    using SpMat = Eigen::SparseMatrix<Scalar, Options, StorageIndex>;
    for (int k = 0; k < mat.outerSize(); ++k) {
        for (SpMat::InnerIterator it(mat, k); it; ++it) {
            f(mat.coeffRef(it.row(), it.col()));
        }
    }
}

template<typename Scalar,
         int Options,
         typename StorageIndex,
         typename UnaryFunction>
void transform(const Eigen::SparseMatrix<Scalar, Options, StorageIndex>& mat_in,
               Eigen::SparseMatrix<Scalar, Options, StorageIndex>& mat_out,
               UnaryFunction f)
{
    mat_out = mat_in;
    Euclid::for_each(mat_out, f);
}

} // namespace Euclid
