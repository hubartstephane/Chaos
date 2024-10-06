namespace chaos
{
#ifdef CHAOS_FORWARD_DECLARATION

    class GPUMeshElement;
    class GPUMesh;

#elif !defined CHAOS_TEMPLATE_IMPLEMENTATION

    /**
      * GPUMeshElement : a renderable element
      */

    class CHAOS_API GPUMeshElement
    {
        friend class GPUMesh;

    public:

        /** constructor */
        GPUMeshElement() = default;
        /** copy constructor */
        GPUMeshElement(GPUMeshElement const& src);
        /** destructor */
        ~GPUMeshElement();

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
     * GPUMesh : a mesh to be dynamicly constructed
     */

    class CHAOS_API GPUMesh : public GPURenderable
    {
    public:

        template<typename VERTEX_TYPE>
        friend class DrawInterface;

        /** returns whether there is nothing to render */
        bool IsEmpty() const { return (elements.size() == 0); }
        /** remove all elements of the rendering (may give GPUBuffers back to a GPUBufferPool) */
        void Clear(GPUBufferPool* buffer_pool);
        /** gets the number of mesh element */
        size_t GetMeshElementCount() const { return elements.size(); }
        /** access any mesh element */
        GPUMeshElement& GetMeshElement(size_t index) { return elements[index]; }
        /** access any mesh element */
        GPUMeshElement const& GetMeshElement(size_t index) const { return elements[index]; }
        /** add an rendering element */
        GPUMeshElement& AddMeshElement(GPUBuffer * vertex_buffer, GPUBuffer * index_buffer);

        /** change the vertex array cache */
        void SetVertexArrayCache(GPUVertexArrayCache* in_vertex_array_cache);

        friend void swap(GPUMesh& src1, GPUMesh& src2)
        {
            std::swap(src1.elements, src2.elements);
            std::swap(src1.last_rendered_fence, src2.last_rendered_fence);
            std::swap(src1.vertex_array_cache, src2.vertex_array_cache);
        }

        /** override the material for the mesh and display */
        int DisplayWithMaterial(GPURenderMaterial const* material, GPURenderer* renderer, GPUProgramProviderInterface const * uniform_provider, GPURenderParams const& render_params);
        /** override the program for the mesh and display */
        int DisplayWithProgram(GPUProgram const* program, GPURenderer* renderer, GPUProgramProviderInterface const * uniform_provider, GPURenderParams const& render_params);

    protected:

        /** override */
        virtual int DoDisplay(GPURenderer* renderer, GPUProgramProviderInterface const * uniform_provider, GPURenderParams const& render_params);

    protected:

        /** the element to render */
        std::vector<GPUMeshElement> elements;
        /** the last time the dynamic mesh was rendered with current elements */
        shared_ptr<GPUFence> last_rendered_fence;
        /** the vertex array cache */
        shared_ptr<GPUVertexArrayCache> vertex_array_cache;
    };

#endif

}; // namespace chaos