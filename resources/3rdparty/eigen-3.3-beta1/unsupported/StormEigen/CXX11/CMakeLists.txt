set(Eigen_CXX11_HEADERS Core Tensor TensorSymmetry)

install(FILES
  ${Eigen_CXX11_HEADERS}
  DESTINATION ${INCLUDE_INSTALL_DIR}/unsupported/StormEigen/CXX11 COMPONENT Devel
  )

add_subdirectory(src)
