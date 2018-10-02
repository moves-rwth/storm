{ stdenv, fetchurl, file
, gmp
, reentrantBuild ? true
}:

let
  version = "5.5.1";
  name = "mathsat-${version}";
  genUrl = reentrant: "http://mathsat.fbk.eu/download.php?file=${name}-linux-x86_64${reentrant}.tar.gz";
  srcAttrs = if reentrantBuild then {
    url = genUrl "-reentrant";
    sha256 = "10ng53nvxyyvml3gbzl87vj3c75fgb14zdlakwasz7zczn7hm978";
  } else {
    url = genUrl "";
    sha256 = "0jnbiaq27hzdzavkr3sdh2ym0bc3ykamacj8k08pvyf7vil2hkdz";
  };

in stdenv.mkDerivation rec {
  inherit name version;

  src = fetchurl srcAttrs;

  nativeBuildInputs = [ gmp ];

  libPath = stdenv.lib.makeLibraryPath [ stdenv.cc.cc stdenv.cc.libc stdenv.glibc gmp ];
  phases = "unpackPhase installPhase fixupPhase";

  installPhase = ''
    mkdir -p $out/{bin,lib,include}
    patchelf --set-rpath "$libPath" lib/libmathsat.so
    cp bin/* $out/bin
    cp lib/* $out/lib
    cp -r include/* $out/include
  '';

  meta = with stdenv.lib; {
    description = "Satisfiability modulo theories (SMT) solver";
    homepage = http://mathsat.fbk.eu;
    license = {
      fullName = "Unfree, redistributable for non-commercial applications";
      free = false;
    };
    maintainer = [ maintainers.spacefrogg ];
    platforms = platforms.linux;
  };
}
