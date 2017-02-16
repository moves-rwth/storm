FILE(GLOB Eigen_MetisSupport_SRCS "*.h")

INSTALL(FILES 
  ${Eigen_MetisSupport_SRCS}
  DESTINATION ${INCLUDE_INSTALL_DIR}/StormEigen/src/MetisSupport COMPONENT Devel
  )
