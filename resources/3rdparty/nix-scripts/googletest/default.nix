{ stdenv, fetchFromGitHub, cmake }:

stdenv.mkDerivation rec {
  name = "googletest-${version}";
  version = "1.8.0";

  buildInputs = [ cmake ];

  src = fetchFromGitHub {
    owner = "google";
    repo = "googletest";
    rev = "release-${version}";
    sha256 = "0bjlljmbf8glnd9qjabx73w6pd7ibv43yiyngqvmvgxsabzr8399";
  };

  meta = with stdenv.lib; {
    description = "Google's C++ test framework";
    homepage = "https://github.com/google/googletest";
    maintainers = [ maintainers.spacefrogg ];
    platforms = platforms.all;
  };
}
