#include <chaos/IrrklangTools.h>

/** boost smart pointer adapter */
void intrusive_ptr_add_ref(irrklang::IVirtualRefCounted * obj, chaos::SharedPointerPolicy policy)
{
	obj->grab();
}

/** boost smart pointer adapter */
void intrusive_ptr_release(irrklang::IVirtualRefCounted * obj, chaos::SharedPointerPolicy policy)
{
	obj->drop();
}

/** boost smart pointer adapter */
void intrusive_ptr_add_ref(irrklang::IRefCounted * obj, chaos::SharedPointerPolicy policy)
{
	obj->grab();
}

/** boost smart pointer adapter */
void intrusive_ptr_release(irrklang::IRefCounted * obj, chaos::SharedPointerPolicy policy)
{
	obj->drop();
}

namespace chaos
{
	namespace IrrklangTools
	{

		irrklang::vec3df ToIrrklangVector(glm::vec3 const & src)
		{
			irrklang::vec3df result;
			result.X = (float)src.x;
			result.Y = (float)src.y;
			result.Z = (float)src.z;
			return result;
		}

	}; // namespace IrrklangTools

}; // namespace chaos


