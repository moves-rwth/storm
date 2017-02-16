FILE(GLOB Eigen_MatrixFunctions_SRCS "*.h")

INSTALL(FILES
  ${Eigen_MatrixFunctions_SRCS}
  DESTINATION ${INCLUDE_INSTALL_DIR}/unsupported/StormEigen/src/MatrixFunctions COMPONENT Devel
  )
