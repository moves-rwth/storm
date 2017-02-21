FILE(GLOB Eigen_Geometry_SRCS "*.h")

INSTALL(FILES
  ${Eigen_Geometry_SRCS}
  DESTINATION ${INCLUDE_INSTALL_DIR}/StormEigen/src/Geometry COMPONENT Devel
  )

ADD_SUBDIRECTORY(arch)
