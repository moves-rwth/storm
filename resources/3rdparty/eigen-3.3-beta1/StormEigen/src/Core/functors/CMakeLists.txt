FILE(GLOB Eigen_Core_Functor_SRCS "*.h")

INSTALL(FILES
  ${Eigen_Core_Functor_SRCS}
  DESTINATION ${INCLUDE_INSTALL_DIR}/StormEigen/src/Core/functors COMPONENT Devel
  )
