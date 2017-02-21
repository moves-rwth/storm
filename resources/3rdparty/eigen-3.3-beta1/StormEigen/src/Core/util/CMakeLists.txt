FILE(GLOB Eigen_Core_util_SRCS "*.h")

INSTALL(FILES 
  ${Eigen_Core_util_SRCS}
  DESTINATION ${INCLUDE_INSTALL_DIR}/StormEigen/src/Core/util COMPONENT Devel
  )
