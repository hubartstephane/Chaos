#include <chaos/Chaos.h>

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

namespace chaos
{
	irrklang::vec3df ToIrrklangVector(glm::vec3 const& src)
	{
		irrklang::vec3df result;
		result.X = (float)src.x;
		result.Y = (float)src.y;
		result.Z = (float)src.z;
		return result;
	}

}; // namespace chaos


