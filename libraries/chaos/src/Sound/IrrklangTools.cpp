#include "chaos/ChaosPCH.h"
#include "chaos/ChaosInternals.h"

namespace irrklang
{
	/** boost/chaos smart pointer adapter */
	void IntrusiveAddSharedReference(IVirtualRefCounted* obj)
	{
		obj->grab();
	}

	/** boost/chaos smart pointer adapter */
	void IntrusiveSubSharedReference(IVirtualRefCounted* obj)
	{
		obj->drop();
	}

	/** boost/chaos smart pointer adapter */
	void IntrusiveAddSharedReference(IRefCounted* obj)
	{
		obj->grab();
	}

	/** boost/chaos smart pointer adapter */
	void IntrusiveSubSharedReference(IRefCounted* obj)
	{
		obj->drop();
	}

}; // namespace irrklang