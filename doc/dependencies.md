# Dependencies

## Included Dependencies:
- Carl 1.0
- CUDD  3.0.0
 	CUDD is included in the StoRM Sources under /resources/3rdparty/cudd-2.5.0 and builds automatically alongside StoRM.
 	Its Sourced where heavily modified as to incorporate newer Versions of Boost, changes in C++ (TR1 to C++11) and
 	to remove components only available under UNIX.
- Eigen 3.3 beta1
 	Eigen is included in the StoRM Sources under /resources/3rdparty/eigen and builds automatically alongside StoRM.


- GTest  1.7.0
	GTest is included in the StoRM Sources under /resources/3rdparty/gtest-1.7.0 and builds automatically alongside StoRM
- GMM >= 4.2
	GMM is included in the StoRM Sources under /resources/3rdparty/gmm-4.2 and builds automatically alongside StoRM.

## Optional:
- Gurobi >= 5.6.2
	Specify the path to the gurobi root dir using -DGUROBI_ROOT=/your/path/to/gurobi
- Z3 >= 4.3.2
	Specify the path to the z3 root dir using -DZ3_ROOT=/your/path/to/z3
- MathSAT >= 5.2.11
	Specify the path to the mathsat root dir using -DMSAT_ROOT=/your/path/to/mathsat
- MPIR >= 2.7.0
	MSVC only and only if linked with MathSAT
	Specify the path to the gmp-include directory -DGMP_INCLUDE_DIR=/your/path/to/mathsat
	Specify the path to the mpir.lib directory -DGMP_MPIR_LIBRARY=/your/path/to/mpir.lib
	Specify the path to the mpirxx.lib directory -DGMP_MPIRXX_LIBRARY=/your/path/to/mpirxx.lib
- GMP
	clang and gcc only
- CUDA Toolkit >= 6.5
	Specify the path to the cuda toolkit root dir using -DCUDA_ROOT=/your/path/to/cuda
- CUSP >= 0.4.0
	Only of built with CUDA Toolkit
	CUSP is included in the StoRM Sources as a git-submodule unter /resources/3rdparty/cusplibrary

