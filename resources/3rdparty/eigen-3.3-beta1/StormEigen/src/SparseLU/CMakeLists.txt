FILE(GLOB Eigen_SparseLU_SRCS "*.h")

INSTALL(FILES
  ${Eigen_SparseLU_SRCS}
  DESTINATION ${INCLUDE_INSTALL_DIR}/StormEigen/src/SparseLU COMPONENT Devel
  )
