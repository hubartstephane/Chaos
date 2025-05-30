namespace chaos
{
#ifdef CHAOS_FORWARD_DECLARATION

	class GPUMeshElement;
	class GPUMesh;

#elif !defined CHAOS_TEMPLATE_IMPLEMENTATION

	/**
	* GPUMeshElement: a renderable element
	*/

	class CHAOS_API GPUMeshElement
	{

	public:

		/** constructor */
		GPUMeshElement() = default;
		/** copy constructor */
		GPUMeshElement(GPUMeshElement const& src);

	public:

		/** the vertex declaration for this element */
		shared_ptr<GPUVertexDeclaration> vertex_declaration;
		/** the material to use for this rendering */
		shared_ptr<GPURenderMaterial> render_material;
		/** the vertex buffer */
		shared_ptr<GPUBuffer> vertex_buffer;
		/** the index buffer */
		shared_ptr<GPUBuffer> index_buffer;
		/** the primitive to render */
		std::vector<GPUDrawPrimitive> primitives;
		/** the vertex buffer offset */
		GLintptr vertex_buffer_offset = 0;
	};

	/**
	* GPUMesh: a mesh to be dynamicaly constructed
	*/

	class CHAOS_API GPUMesh : public GPURenderable, public GPUDeviceResourceInterface
	{
	public:

		template<typename VERTEX_TYPE>
		friend class GPUDrawInterface;

		/** constructor */
		GPUMesh(GPUDevice * in_gpu_device);

		/** returns whether there is nothing to render */
		bool IsEmpty() const { return (elements.size() == 0); }
		/** remove all elements of the rendering */
		void Clear();
		/** gets the number of mesh element */
		size_t GetMeshElementCount() const { return elements.size(); }
		/** access any mesh element */
		GPUMeshElement& GetMeshElement(size_t index) { return elements[index]; }
		/** access any mesh element */
		GPUMeshElement const& GetMeshElement(size_t index) const { return elements[index]; }
		/** add an rendering element */
		GPUMeshElement& AddMeshElement(GPUVertexDeclaration* vertex_declaration, GPUBuffer * vertex_buffer, GPUBuffer * index_buffer);

		/** setting the bounding box of the mesh */
		void SetBoundingBox(std::optional<box3> const & in_bounding_box);
		/** get the bounding box of the mesh */
		std::optional<box3> const& GetBoundingBox() const;

		/** swapping two meshes */
		friend void swap(GPUMesh& src1, GPUMesh& src2)
		{
			std::swap(src1.elements, src2.elements);
			std::swap(src1.bounding_box, src2.bounding_box);
		}

		/** override the material for the mesh and display */
		int DisplayWithMaterial(GPURenderMaterial const* material, GPURenderContext* render_context, GPUProgramProviderInterface const * uniform_provider, GPURenderParams const& render_params);
		/** override the program for the mesh and display */
		int DisplayWithProgram(GPUProgram const* program, GPURenderContext* render_context, GPUProgramProviderInterface const * uniform_provider, GPURenderParams const& render_params);

	protected:

		/** override */
		virtual int DoDisplay(GPURenderContext* render_context, GPUProgramProviderInterface const * uniform_provider, GPURenderParams const& render_params);

	protected:

		/** the element to render */
		std::vector<GPUMeshElement> elements;
		/** the bounding box */
		std::optional<box3> bounding_box;
	};

#endif

}; // namespace chaos