FILE(GLOB Eigen_OrderingMethods_SRCS "*.h")

INSTALL(FILES
  ${Eigen_OrderingMethods_SRCS}
  DESTINATION ${INCLUDE_INSTALL_DIR}/StormEigen/src/OrderingMethods COMPONENT Devel
  )
