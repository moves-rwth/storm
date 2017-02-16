FILE(GLOB Eigen_misc_SRCS "*.h")

INSTALL(FILES
  ${Eigen_misc_SRCS}
  DESTINATION ${INCLUDE_INSTALL_DIR}/StormEigen/src/misc COMPONENT Devel
  )
