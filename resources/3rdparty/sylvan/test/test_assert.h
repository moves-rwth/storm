#ifndef test_assert
#define test_assert(expr)         do {                                  \
 if (!(expr))                                                           \
 {                                                                      \
         fprintf(stderr,                                                \
                "file %s: line %d (%s): precondition `%s' failed.\n",   \
                __FILE__,                                               \
                __LINE__,                                               \
                __PRETTY_FUNCTION__,                                    \
                #expr);                                                 \
         return 1;                                                      \
 } } while(0)
#endif
