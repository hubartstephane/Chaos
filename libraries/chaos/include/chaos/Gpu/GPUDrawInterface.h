namespace chaos
{
#ifdef CHAOS_FORWARD_DECLARATION

	template<typename VERTEX_TYPE>
	class GPUDrawInterface;

#elif !defined CHAOS_TEMPLATE_IMPLEMENTATION

	/**
	 * GPUDrawInterface: this is a primitive that have its own GPUMesh embedded for directed drawing
	 */

	template<typename VERTEX_TYPE>
	class GPUDrawInterface : public GPUDeviceResourceInterface, public GPUPrimitiveOutput<VERTEX_TYPE>
	{
	public:

		/** default/minimum number of vertices allocation (same value than GPUPrimitiveOutput's) */
		static constexpr size_t MIN_VERTEX_ALLOCATION = 100;

		/** constructor */
		GPUDrawInterface(GPUDevice * in_gpu_device, ObjectRequest in_render_material_request, size_t in_vertex_requirement_evaluation = MIN_VERTEX_ALLOCATION):
			GPUDeviceResourceInterface(in_gpu_device),
			GPUPrimitiveOutput<VERTEX_TYPE>(new GPUMesh(in_gpu_device), GetVertexDeclaration(), in_render_material_request, in_vertex_requirement_evaluation)
		{
		}
		/** constructor */
		GPUDrawInterface(GPUDevice * in_gpu_device, GPURenderMaterial* in_render_material, size_t in_vertex_requirement_evaluation = MIN_VERTEX_ALLOCATION):
			GPUDeviceResourceInterface(in_gpu_device),
			GPUPrimitiveOutput<VERTEX_TYPE>(new GPUMesh(in_gpu_device), GetVertexDeclaration(), (in_render_material != nullptr) ? in_render_material : DefaultScreenSpaceProgram::GetMaterial(), in_vertex_requirement_evaluation)
		{
		}

		/** destructor */
		virtual ~GPUDrawInterface()
		{
			Clear();
		}

		/** flush and display the pending content */
		int Display(GPURenderContext* render_context, GPUProgramProviderInterface const * uniform_provider, GPURenderParams const& render_params)
		{
			this->Flush();
			int result = this->mesh->Display(render_context, uniform_provider, render_params);
			this->mesh->Clear();
			return result;
		}

		/** extract the mesh for external purpose */
		GPUMesh* GetDynamicMesh()
		{
			GPUMesh * result = new GPUMesh(GetGPUDevice());

			this->Flush();
			swap(*result, *this->mesh);

			return result;
		}

		/** clear the dynamic mesh */
		void Clear()
		{
			this->Flush(); // to unmap pending mapped buffer
			this->mesh->Clear();
		}

	protected:

		/** gets the shared GPUVertexDeclaration */
		static GPUVertexDeclaration* GetVertexDeclaration()
		{
			static shared_ptr<GPUVertexDeclaration> result;
			if (result == nullptr)
			{
				result = new GPUVertexDeclaration;
				GetTypedVertexDeclaration(result.get(), boost::mpl::identity<VERTEX_TYPE>());
			}
			return result.get();
		}
	};

#endif

}; // namespace chaos