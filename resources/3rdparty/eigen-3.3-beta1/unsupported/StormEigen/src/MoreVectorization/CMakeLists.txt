FILE(GLOB Eigen_MoreVectorization_SRCS "*.h")

INSTALL(FILES
  ${Eigen_MoreVectorization_SRCS}
  DESTINATION ${INCLUDE_INSTALL_DIR}/unsupported/StormEigen/src/MoreVectorization COMPONENT Devel
  )
