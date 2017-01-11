FILE(GLOB Eigen_Skyline_SRCS "*.h")

INSTALL(FILES
  ${Eigen_Skyline_SRCS}
  DESTINATION ${INCLUDE_INSTALL_DIR}/unsupported/StormEigen/src/Skyline COMPONENT Devel
  )
