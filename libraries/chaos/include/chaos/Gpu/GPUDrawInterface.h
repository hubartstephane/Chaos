#ifdef CHAOS_FORWARD_DECLARATION

namespace chaos
{
	template<typename VERTEX_TYPE>
	class GPUDrawInterface;

}; // namespace chaos

#else 

namespace chaos
{
	/**
	 * GPUDrawInterface : this is a primitive that have its own GPUDynamicMesh embedded for directed drawing
	 */

	template<typename VERTEX_TYPE>
	class GPUDrawInterface : public PrimitiveOutput<VERTEX_TYPE>
	{
	public:

		/** constructor */
		GPUDrawInterface(ObjectRequest render_material_request, size_t in_vertex_requirement_evaluation = PrimitiveOutputBase::MIN_VERTEX_ALLOCATION) :
			PrimitiveOutput(&dynamic_mesh, GetBufferPool(), GetVertexDeclaration(), render_material_request, in_vertex_requirement_evaluation)
		{
			dynamic_mesh.SetVertexArrayCache(GetVertexArrayCache());
		}

		/** destructor */
		virtual ~GPUDrawInterface()
		{
			dynamic_mesh.Clear(GetBufferPool());
		}

		/** flush and display the pending content */
		int Display(GPURenderer* renderer, GPUProgramProviderBase const* uniform_provider, GPURenderParams const& render_params)
		{
			Flush();
			int result = dynamic_mesh.Display(renderer, uniform_provider, render_params);
			dynamic_mesh.Clear(GetBufferPool());
			return result;
		}

		/** extract the mesh for external purpose */
		GPUDynamicMesh* ExtractMesh()
		{
			GPUDynamicMesh* result = new GPUDynamicMesh();
			if (result != nullptr)
			{
				Flush();
				swap(*result, dynamic_mesh);
				dynamic_mesh.SetVertexArrayCache(GetVertexArrayCache());
			}
			return result;
		}

	protected:

		/** gets the shared GPUBufferPool to improve GPUBuffer allocation */
		static GPUBufferPool* GetBufferPool()
		{
			static shared_ptr<GPUBufferPool> result = new GPUBufferPool();
			return result.get();
		}
		/** gets the shared GPUVertexDeclaration */
		static GPUVertexDeclaration* GetVertexDeclaration()
		{
			static shared_ptr<GPUVertexDeclaration> result;
			if (result == nullptr)
			{
				result = new GPUVertexDeclaration;
				GetTypedVertexDeclaration(result.get(), boost::mpl::identity<VertexDefault>());
			}
			return result.get();
		}
		/** gets the shared GPUVertexArrayCache */
		static GPUVertexArrayCache* GetVertexArrayCache()
		{
			static shared_ptr<GPUVertexArrayCache> result = new GPUVertexArrayCache();
			return result.get();
		}

	protected:

		/** the internal buffer */
		GPUDynamicMesh dynamic_mesh;
	};

}; // namespace chaos

#endif // CHAOS_FORWARD_DECLARATION

