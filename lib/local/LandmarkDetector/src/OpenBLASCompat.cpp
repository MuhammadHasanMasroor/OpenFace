// Provide a weak fallback for `openblas_set_num_threads` when OpenFace is linked
// against a non-OpenBLAS BLAS implementation (e.g. system libblas).

extern "C" void openblas_set_num_threads(int num_threads) __attribute__((weak));

extern "C" void openblas_set_num_threads(int /*num_threads*/)
{
}

