// ==========================================================================================
// Chaos inclusions
// ==========================================================================================

// warning 4251: stl objects should be declared with dllexport to be usable.
//               if you compile a dll with a given version of STL/compiler... and you use it with another, some object like string may become broken
//               (the size of a string may even change)
//               here, we use same compiler, same STL version to build DLL and executable that use the DLL. We can disable the warning
#ifdef _MSC_VER
#  pragma warning(push)
#  pragma warning(disable: 4251)
#endif

// first step, just forward declarations
#define CHAOS_FORWARD_DECLARATION
#include "chaos/ChaosFiles.h"
#undef CHAOS_FORWARD_DECLARATION

// second step, full declarations
#include "chaos/ChaosFiles.h"

// last step, template implementation
#define CHAOS_TEMPLATE_IMPLEMENTATION
#include "chaos/ChaosFiles.h"
#undef CHAOS_TEMPLATE_IMPLEMENTATION

#ifdef _MSC_VER
#  pragma warning(pop)
#endif