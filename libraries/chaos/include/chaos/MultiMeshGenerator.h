#pragma once

#include <chaos/StandardHeaders.h>
#include <chaos/GPUSimpleMeshGenerator.h>

namespace chaos
{
	/**
	* MultiMeshGenerator : this class is used to generator multiple mesh in a row, shared vertex and index buffer
	*/

	class MultiMeshGenerator
	{
	public:

		/** constructor */
		MultiMeshGenerator();
		/** destructor */
		virtual ~MultiMeshGenerator();

		/** the insertion method */
		void AddGenerator(GPUSimpleMeshGenerator * generator, shared_ptr<GPUSimpleMesh> & target_ptr);
		/** clean all generators */
		void Clean();
		/** generate all meshes */
		bool GenerateMeshes() const;

	protected:

		/** the registered element to generate */
		std::vector<std::pair<shared_ptr<GPUSimpleMeshGenerator>, shared_ptr<GPUSimpleMesh> *>> generators;
	};


}; // namespace chaos

