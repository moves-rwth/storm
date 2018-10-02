{ stdenv, fetchFromGitHub, autoconf, pkgconfig, cmake
, cln, ginac, gmp, boost, eigen3_3, python2, googletest }:

let
  gtest-cmake = ./gtest.cmake;

in stdenv.mkDerivation rec {
  name = "carl-${version}";
  version = "17.12";

  buildInputs = [ cln ginac gmp boost python2 googletest ];

  nativeBuildInputs = [ autoconf pkgconfig cmake ];

  propagatedBuildInputs = [ eigen3_3 ];

  src = fetchFromGitHub {
    owner = "smtrat";
    repo = "carl";
    rev = version;
    sha256 = "1299i0b6w4v6s2a2kci3jrpdq1lpaw4j3p34gx6gmp9g3n1yp6xq";
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
    "-DGTEST_MAIN_LIBRARY=${googletest}/lib/libgtest_main.a"
    "-DGTEST_LIBRARY=${googletest}/lib/libgtest.a"
  ];

  postPatch = ''
    cp ${gtest-cmake} resources/gtest.cmake
    substituteInPlace resources/gtest.cmake --subst-var-by googletest ${googletest}
    sed -e '/set(GTEST/i include(resources/gtest.cmake)' -i resources/resources.cmake
  '';

  meta = with stdenv.lib; {
    description = "Computer ARithmetic and Logic library";
    homepage = http://smtrat.github.io/carl;
    mainainers = [ maintainers.spacefrogg ];
    platforms = platforms.all;
  };
}
