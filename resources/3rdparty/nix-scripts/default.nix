# This file defines a nixpkgs overlay. To use it, do one of the following:
# 1. Link this file to ~/.config/nixpkgs/overlays/<some-name>
# 2. Add the full path to the environment variable NIX_PATH, i.e.:
#    NIX_PATH=${NIX_PATH}:nixpkgs-overlays=$PWD/default.nix
# See also https://nixos.org/nixpkgs/manual/#chap-overlays
#
# To build storm from the current branch call either of:
# nix-build '<nixpkgs>' -A stormChecker
# nix-build '<nixpkgs>' -A stormCheckerFull

self: super:
with self;
with self.lib;
let
  callPackage = super.lib.callPackageWith self;
  _self = {
    z3 = callPackage ./z3 { };
    stormChecker = callPackage ./storm-checker { ltoSupport = false; tbbSupport = false; mathsatSupport = false; z3Support = false; };
    stormCheckerFull = callPackage ./storm-checker { ltoSupport = true; tbbSupport = true; mathsatSupport = true; z3Support = true; };
    carl = callPackage ./carl { };
    googletest = callPackage ./googletest { };
    l3pp = callPackage ./l3pp { };
    mathsat = callPackage ./mathsat { };
  };
in _self
