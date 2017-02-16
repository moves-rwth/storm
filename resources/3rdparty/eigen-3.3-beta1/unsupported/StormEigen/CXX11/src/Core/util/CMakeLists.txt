FILE(GLOB Eigen_CXX11_Core_util_SRCS "*.h")

INSTALL(FILES
  ${Eigen_CXX11_Core_util_SRCS}
  DESTINATION ${INCLUDE_INSTALL_DIR}/unsupported/StormEigen/CXX11/src/Core/util COMPONENT Devel
  )
