FILE(GLOB Eigen_AutoDiff_SRCS "*.h")

INSTALL(FILES
  ${Eigen_AutoDiff_SRCS}
  DESTINATION ${INCLUDE_INSTALL_DIR}/unsupported/StormEigen/src/AutoDiff COMPONENT Devel
  )
