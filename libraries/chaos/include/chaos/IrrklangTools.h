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

namespace chaos
{
	namespace IrrklangTools
	{
		/** convert a glm vector into an irrklang vector */
		irrklang::vec3df ToIrrklangVector(glm::vec3 const & src);


	}; // namespace IrrklangTools

}; // namespace chaos