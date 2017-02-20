FILE(GLOB Eigen_SparseCore_SRCS "*.h")

INSTALL(FILES 
  ${Eigen_SparseCore_SRCS}
  DESTINATION ${INCLUDE_INSTALL_DIR}/StormEigen/src/SparseCore COMPONENT Devel
  )
