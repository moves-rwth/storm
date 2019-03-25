{ stdenv, fetchFromGitHub }:
stdenv.mkDerivation rec {
  name = "l3pp-${version}";
  version = "git";

  src = fetchFromGitHub {
    owner = "hbruintjes";
    repo = "l3pp";
    rev = "e4f8d7fe6c328849aff34d2dfd6fd592c14070d5";
    sha256 = "0bd0m4hj7iy5y9546sr7d156hmq6q7d5jys495jd26ngvibkv9hp";
  };
  phases = "unpackPhase installPhase fixupPhase";

  installPhase = ''
    mkdir -p $out/include $out/share/doc/l3pp
    cp LICENSE Readme.md $out/share/doc/l3pp
    cp -r *.h impl $out/include
  '';

  meta = with stdenv.lib; {
    description = "Lightweight Logging Library for C++";
    homepage = https://github.com/hbruintjes/l3pp;
    maintainers = [ maintainers.spacefrogg ];
    platforms = platforms.all;
  };
}
