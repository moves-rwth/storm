{ stdenv, fetchFromGitHub, writeText, autoconf, automake, cmake
, boost, carl, cln, doxygen, gmp, ginac, glpk, hwloc, l3pp, xercesc
, ltoSupport ? true
, mathsatSupport ? false, mathsat
, tbbSupport ? false, tbb
, z3Support ? true, z3
}:

let
  l3ppCmakeSed = writeText "l3pp-sed" ''
8,27d
28i\
set(l3pp_INCLUDE "${l3pp}/include/")
30d
  '';
  inherit (stdenv.lib) optional singleton;
  genCmakeOption = bool: name:
    singleton "-D${name}=${if bool then "on" else "off"}";

in stdenv.mkDerivation {
  name = "storm-git";

  src = ../../../../.;
  # Exchange with expression below to build a specific github revision:
  # src = fetchFromGitHub {
  #   owner = "moves-rwth";
  #   repo = "storm";
  #   rev = "8332abab58f0c672561f5bbebd585a159852d8cc";
  #   sha256 = "02ixywhfkxr8xlcizqbysb1yinsjzl6rc0cjlsg8dz8w2r3m6qix";
  # };

  buildInputs = [ boost carl cln doxygen gmp ginac glpk hwloc l3pp xercesc ]
    ++ optional tbbSupport tbb
    ++ optional z3Support z3;

  nativeBuildInputs = [ autoconf automake cmake ];

  cmakeFlags =  genCmakeOption tbbSupport "STORM_USE_INTELTBB"
    ++ genCmakeOption ltoSupport "STORM_USE_LTO"
    ++ optional mathsatSupport "-DMSAT_ROOT=${mathsat}" ;

  postPatch = ''
    sed -f ${l3ppCmakeSed} -i resources/3rdparty/CMakeLists.txt
  '';

  meta = with stdenv.lib; {
    description = "Probabilistic Model Checker";
    homepage = http://www.stormchecker.org;
    license = licenses.gpl3;
    maintainer = [ maintainers.spacefrogg ];
    platforms = platforms.all;
  };
}
