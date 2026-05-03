namespace chaos
{
#ifdef CHAOS_FORWARD_DECLARATION

	class GPUMultiMeshGenerator;

#elif !defined CHAOS_TEMPLATE_IMPLEMENTATION

	/**
	* GPUMultiMeshGenerator : this class is used to generator multiple mesh in a row, shared vertex and index buffer
	*/

	class CHAOS_API GPUMultiMeshGenerator
	{
	public:

		/** the insertion method */
		void AddGenerator(GPUMeshGenerator* generator, shared_ptr<GPUMesh>& target_ptr);
		/** clean all generators */
		void Clean();
		/** generate all meshes */
		bool GenerateMeshes(GPUDevice * in_gpu_device) const;

	protected:

		/** the registered element to generate */
		std::vector<std::pair<shared_ptr<GPUMeshGenerator>, shared_ptr<GPUMesh>*>> generators;
	};

#endif

}; // namespace chaos