#ifndef FLIF_COMPILER_SPECIFIC_HPP
#define FLIF_COMPILER_SPECIFIC_HPP

#ifdef _MSC_VER
#define ATTRIBUTE_HOT
#else
#define ATTRIBUTE_HOT __attribute__ ((hot))
#endif

#endif
