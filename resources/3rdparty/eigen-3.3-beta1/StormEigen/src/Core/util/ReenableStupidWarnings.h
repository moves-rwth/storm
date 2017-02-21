#ifdef STORMEIGEN_WARNINGS_DISABLED
#undef STORMEIGEN_WARNINGS_DISABLED

#ifndef STORMEIGEN_PERMANENTLY_DISABLE_STUPID_WARNINGS
  #ifdef _MSC_VER
    #pragma warning( pop )
  #elif defined __INTEL_COMPILER
    #pragma warning pop
  #elif defined __clang__
    #pragma clang diagnostic pop
  #endif
#endif

#endif // STORMEIGEN_WARNINGS_DISABLED
