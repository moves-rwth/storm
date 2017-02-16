FILE(GLOB Eigen_Core_arch_Default_SRCS "*.h")

INSTALL(FILES
  ${Eigen_Core_arch_Default_SRCS}
  DESTINATION ${INCLUDE_INSTALL_DIR}/StormEigen/src/Core/arch/Default COMPONENT Devel
)
