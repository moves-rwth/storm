FILE(GLOB Eigen_Core_arch_AVX_SRCS "*.h")

INSTALL(FILES
  ${Eigen_Core_arch_AVX_SRCS}
  DESTINATION ${INCLUDE_INSTALL_DIR}/StormEigen/src/Core/arch/AVX COMPONENT Devel
)
