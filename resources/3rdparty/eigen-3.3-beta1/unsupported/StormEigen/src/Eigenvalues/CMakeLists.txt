FILE(GLOB Eigen_Eigenvalues_SRCS "*.h")

INSTALL(FILES
  ${Eigen_Eigenvalues_SRCS}
  DESTINATION ${INCLUDE_INSTALL_DIR}/unsupported/StormEigen/src/Eigenvalues COMPONENT Devel
  )
