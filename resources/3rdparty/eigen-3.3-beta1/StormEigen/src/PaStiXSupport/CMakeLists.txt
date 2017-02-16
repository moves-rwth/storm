FILE(GLOB Eigen_PastixSupport_SRCS "*.h")

INSTALL(FILES 
  ${Eigen_PastixSupport_SRCS}
  DESTINATION ${INCLUDE_INSTALL_DIR}/StormEigen/src/PaStiXSupport COMPONENT Devel
  )
