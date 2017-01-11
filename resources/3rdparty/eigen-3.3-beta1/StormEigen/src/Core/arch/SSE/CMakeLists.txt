FILE(GLOB Eigen_Core_arch_SSE_SRCS "*.h")

INSTALL(FILES
  ${Eigen_Core_arch_SSE_SRCS}
  DESTINATION ${INCLUDE_INSTALL_DIR}/StormEigen/src/Core/arch/SSE COMPONENT Devel
)
