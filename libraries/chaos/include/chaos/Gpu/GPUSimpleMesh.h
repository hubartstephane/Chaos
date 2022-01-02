namespace chaos
{






#if 0






#ifdef CHAOS_FORWARD_DECLARATION

	class GPUDynamicMesh;

#elif !defined CHAOS_TEMPLATE_IMPLEMENTATION

	/**
	* GPUDynamicMesh : how it says
	*/

	class GPUDynamicMesh : public GPUResource
	{
	public:

		/** constructor */
		GPUDynamicMesh() = default;
		/** destructor */
		~GPUDynamicMesh();

		/** render the primitive (base_instance is an offset applyed to gl_InstanceID) */
		void Render(GPURenderer* renderer, GPUProgram const* program, GPUProgramProviderBase const* uniform_provider, GPURenderParams const& render_params);
		/** render the primitive (base_instance is an offset applyed to gl_InstanceID) */
		void Render(GPURenderer* renderer, GPURenderMaterial const* material, GPUProgramProviderBase const* uniform_provider, GPURenderParams const& render_params);

		/** should bind index buffer and vertex buffer, as musch as for the vertex declaration */
		void SetVertexBufferOffset(GLintptr vertex_buffer_offset);
		/** offset the index or vertex position */
		void ShiftPrimitivesIndexAndVertexPosition(int vb_offset, int ib_offset);

		/** cleaning the object */
		virtual void Release() override;

	protected:

		/** rendering internal method */
		void DoRender(GPURenderer* renderer, GPUProgram const* program, GPURenderParams const& render_params);

	public:

		/** self descriptive */
		shared_ptr<GPUVertexDeclaration> vertex_declaration;
		/** the primitives to render */
		std::vector<GPUDrawPrimitive> primitives;

		/** self descriptive */
		shared_ptr<GPUBuffer> vertex_buffer;
		/** self descriptive */
		shared_ptr<GPUBuffer> index_buffer;
		/** in the vertex buffer (that may be shared by other simpled mesh), the offset from the begining given to it */
		GLintptr vertex_buffer_offset = 0;

	protected:

		/** the cache for all vertex arrays */
		shared_ptr<GPUVertexArrayCache> vertex_array_cache;
	};

#endif




#endif






}; // namespace chaos