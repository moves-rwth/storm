FILE(GLOB Eigen_SparseQR_SRCS "*.h")

INSTALL(FILES
  ${Eigen_SparseQR_SRCS}
  DESTINATION ${INCLUDE_INSTALL_DIR}/StormEigen/src/SparseQR/ COMPONENT Devel
  )
