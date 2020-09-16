#define DisableWarnings                                                        \
  _Pragma("GCC diagnostic push") _Pragma("GCC diagnostic ignored \"-Wall\"")   \
      _Pragma("GCC diagnostic ignored \"-Wunused-parameter\"")

#define EnableWarnings _Pragma("GCC diagnostic pop")
#define UNUSED(x) (void)(x)