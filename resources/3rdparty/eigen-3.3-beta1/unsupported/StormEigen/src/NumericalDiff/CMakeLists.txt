FILE(GLOB Eigen_NumericalDiff_SRCS "*.h")

INSTALL(FILES
  ${Eigen_NumericalDiff_SRCS}
  DESTINATION ${INCLUDE_INSTALL_DIR}/unsupported/StormEigen/src/NumericalDiff COMPONENT Devel
  )
