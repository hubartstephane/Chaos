#pragma once

#include <chaos/StandardHeaders.h>

/** boost smart pointer adapter */
void intrusive_ptr_add_ref(irrklang::IVirtualRefCounted * obj)
{
	obj->grab();
}

/** boost smart pointer adapter */
void intrusive_ptr_release(irrklang::IVirtualRefCounted * obj)
{
	obj->drop();
}

/** boost smart pointer adapter */
void intrusive_ptr_add_ref(irrklang::IRefCounted * obj)
{
	obj->grab();
}

/** boost smart pointer adapter */
void intrusive_ptr_release(irrklang::IRefCounted * obj)
{
	obj->drop();
}

namespace chaos
{



}; // namespace chaos
