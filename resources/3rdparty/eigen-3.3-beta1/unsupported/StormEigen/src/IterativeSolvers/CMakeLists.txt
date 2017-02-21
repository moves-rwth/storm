FILE(GLOB Eigen_IterativeSolvers_SRCS "*.h")

INSTALL(FILES
  ${Eigen_IterativeSolvers_SRCS}
  DESTINATION ${INCLUDE_INSTALL_DIR}/unsupported/StormEigen/src/IterativeSolvers COMPONENT Devel
  )
