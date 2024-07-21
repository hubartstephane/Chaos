#include "chaos/ChaosPCH.h"
#include "chaos/ChaosInternals.h"

/** boost/chaos smart pointer adapter */
void intrusive_ptr_add_ref(irrklang::IVirtualRefCounted * obj)
{
	obj->grab();
}

/** boost/chaos smart pointer adapter */
void intrusive_ptr_release(irrklang::IVirtualRefCounted * obj)
{
	obj->drop();
}

/** boost/chaos smart pointer adapter */
void intrusive_ptr_add_ref(irrklang::IRefCounted * obj)
{
	obj->grab();
}

/** boost/chaos smart pointer adapter */
void intrusive_ptr_release(irrklang::IRefCounted * obj)
{
	obj->drop();
}


