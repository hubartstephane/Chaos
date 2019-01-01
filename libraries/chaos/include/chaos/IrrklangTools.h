#pragma once

#include <chaos/StandardHeaders.h>
#include <chaos/SmartPointers.h>

/** boost smart pointer adapter */
extern void intrusive_ptr_add_ref(irrklang::IVirtualRefCounted * obj, chaos::SharedPointerPolicy policy = chaos::SharedPointerPolicy());
/** boost smart pointer adapter */
extern void intrusive_ptr_release(irrklang::IVirtualRefCounted * obj, chaos::SharedPointerPolicy policy = chaos::SharedPointerPolicy());
/** boost smart pointer adapter */
extern void intrusive_ptr_add_ref(irrklang::IRefCounted * obj, chaos::SharedPointerPolicy policy = chaos::SharedPointerPolicy());
/** boost smart pointer adapter */
extern void intrusive_ptr_release(irrklang::IRefCounted * obj, chaos::SharedPointerPolicy policy = chaos::SharedPointerPolicy());

namespace chaos
{
	namespace IrrklangTools
	{
		/** convert a glm vector into an irrklang vector */
		irrklang::vec3df ToIrrklangVector(glm::vec3 const & src);

	}; // namespace IrrklangTools

}; // namespace chaos