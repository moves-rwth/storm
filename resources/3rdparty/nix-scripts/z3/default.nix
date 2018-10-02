{ stdenv, fetchFromGitHub, python3 }:

stdenv.mkDerivation rec {
  name = "z3-${version}";
  version = "4.6.0";

  src = fetchFromGitHub {
    owner = "Z3Prover";
    repo = "z3";
    rev = "z3-${version}";
    sha256 = "1cgwlmjdbf4rsv2rriqi2sdpz9qxihxrcpm6a4s37ijy437xg78l";
  };

  buildInputs = [ python3 ];
  phases = "unpackPhase buildPhase installPhase fixupPhase";
  preBuild = ''
    python3 scripts/mk_make.py --prefix=$out
    cd build
  '';
}
