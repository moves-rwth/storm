{ stdenv, fetchFromGitHub, autoconf, pkgconfig, cmake
, cln, ginac, gmp, boost, eigen3_3, python3, googletest }:

let
  gtest-cmake = ./gtest.cmake;

in stdenv.mkDerivation rec {
  name = "carl-${version}";
  version = "18.06";

  buildInputs = [ cln ginac gmp boost python3 googletest ];

  nativeBuildInputs = [ autoconf pkgconfig cmake ];

  propagatedBuildInputs = [ eigen3_3 ];

  src = fetchFromGitHub {
    owner = "smtrat";
    repo = "carl";
    rev = version;
    sha256 = "0lb4pbs3bwpi4z4bnh5113s9c4fzq7c8iwa0952j2jrhxf4kcb8q";
  };

  enableParallelBuilding = true;

  cmakeFlags = [
    "-DEXPORT_TO_CMAKE=off"
    "-DUSE_CLN_NUMBERS=on"
    "-DTHREAD_SAFE=on"
    "-DUSE_GINAC=on"
    "-DGINAC_FOUND=on"
    "-DGINAC_INCLUDE_DIR=${ginac}/include/ginac"
    "-DGINAC_LIBRARY=${ginac}/lib/libginac.so"
    "-DGTEST_FOUND=on"
    "-DGTEST_VERSION=${googletest.version}"
    "-DGTEST_MAIN_LIBRARY=${googletest}/lib/libgtest_main.a"
    "-DGTEST_LIBRARY=${googletest}/lib/libgtest.a"
  ];

  postPatch = ''
    cp ${gtest-cmake} resources/gtest.cmake
    substituteInPlace resources/gtest.cmake --subst-var-by googletest ${googletest}
    sed -e '/print_resource_info("GTest"/i include(resources/gtest.cmake)' -i resources/resources.cmake
  '';

  meta = with stdenv.lib; {
    description = "Computer ARithmetic and Logic library";
    homepage = http://smtrat.github.io/carl;
    mainainers = [ maintainers.spacefrogg ];
    platforms = platforms.all;
  };
}
