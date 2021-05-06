
#ifdef CHAOS_FORWARD_DECLARATION

namespace chaos
{
	class GPUMultiMeshGenerator;

}; // namespace chaos

#elif defined CHAOS_TEMPLATE_IMPLEMENTATION


#else 

namespace chaos
{

	/**
	* GPUMultiMeshGenerator : this class is used to generator multiple mesh in a row, shared vertex and index buffer
	*/

	class GPUMultiMeshGenerator
	{
	public:

		/** constructor */
		GPUMultiMeshGenerator();
		/** destructor */
		virtual ~GPUMultiMeshGenerator();

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

#endif // CHAOS_FORWARD_DECLARATION



