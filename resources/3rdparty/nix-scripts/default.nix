self: super:
with self;
with self.lib;
let
  callPackage = super.lib.callPackageWith self;
  _self = {
    z3 = callPackage ./z3 { };
    stormChecker = callPackage ./storm-checker { ltoSupport = false; tbbSupport = false; mathsatSupport = false; z3Support = false; };
    carl = callPackage ./carl { };
    googletest = callPackage ./googletest { };
    l3pp = callPackage ./l3pp { };
    mathsat = callPackage ./mathsat { };
  };
in _self
