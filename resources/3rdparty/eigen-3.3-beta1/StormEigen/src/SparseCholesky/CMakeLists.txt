FILE(GLOB Eigen_SparseCholesky_SRCS "*.h")

INSTALL(FILES
  ${Eigen_SparseCholesky_SRCS}
  DESTINATION ${INCLUDE_INSTALL_DIR}/StormEigen/src/SparseCholesky COMPONENT Devel
  )
