#pragma once

#include <chaos/StandardHeaders.h>

/** boost smart pointer adapter */
extern void intrusive_ptr_add_ref(irrklang::IVirtualRefCounted * obj);
/** boost smart pointer adapter */
extern void intrusive_ptr_release(irrklang::IVirtualRefCounted * obj);
/** boost smart pointer adapter */
extern void intrusive_ptr_add_ref(irrklang::IRefCounted * obj);
/** boost smart pointer adapter */
extern void intrusive_ptr_release(irrklang::IRefCounted * obj);
