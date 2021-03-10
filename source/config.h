#ifndef LIB_CONFIG_H
#define LIB_CONFIG_H

#ifdef BEGIN_NAMESPACE_LIB
#undef BEGIN_NAMESPACE_LIB
#endif
#ifdef END_NAMESPACE_LIB
#undef END_NAMESPACE_LIB
#endif

#define BEGIN_NAMESPACE_LIB namespace lib {
#define END_NAMESPACE_LIB };

#endif//LIB_CONFIG_H
