FILE(GLOB Eigen_Core_arch_CUDA_SRCS "*.h")

INSTALL(FILES
  ${Eigen_Core_arch_CUDA_SRCS}
  DESTINATION ${INCLUDE_INSTALL_DIR}/StormEigen/src/Core/arch/CUDA COMPONENT Devel
)
