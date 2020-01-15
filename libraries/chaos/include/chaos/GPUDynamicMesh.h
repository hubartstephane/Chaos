#pragma once

#include <chaos/StandardHeaders.h>

#include <chaos/GPUClasses.h>
#include <chaos/GPURenderable.h>
#include <chaos/GPUBufferCache.h>
#include <chaos/GPUFence.h>
#include <chaos/GPUDrawPrimitive.h>
#include <chaos/GPUVertexArrayCache.h>
#include <chaos/GPUVertexDeclaration.h>
#include <chaos/GPURenderMaterial.h>


namespace chaos
{
    /**
     * GPUDynamicMeshElement : a renderable element
     */

    class GPUDynamicMeshElement
    {
        friend class GPUDynamicMesh;

    public:

        /** constructor */
        GPUDynamicMeshElement() = default;
        /** copy constructor */
        GPUDynamicMeshElement(GPUDynamicMeshElement const& src) : // XXX : do not copy GPUVertexArrayCache
            vertex_declaration(src.vertex_declaration),
            render_material(src.render_material),
            vertex_buffer(src.vertex_buffer),
            primitives(src.primitives) {}

        /** the vertex declaration for this element */
        shared_ptr<GPUVertexDeclaration> vertex_declaration;
        /** the material to use for this rendering */
        shared_ptr<GPURenderMaterial> render_material;
        /** the vertex buffer */
        shared_ptr<GPUBuffer> vertex_buffer;
        /** the primitive to render */
        std::vector<GPUDrawPrimitive> primitives;
    };

    /**
     * GPUDynamicMesh : a mesh to be dynamicly constructed
     */

    class GPUDynamicMesh : public GPURenderable
    {

    public:

        /** returns whether there is nothing to render */
        bool IsEmpty() const { return (elements.size() == 0); }
        /** remove all elements of the rendering (may give GPUBuffers back to a GPUBufferCache) */
        void Clear(GPUBufferCache* buffer_cache);
        /** gets the number of mesh element */
        size_t GetMeshElementCount() const { return elements.size();}
        /** access any mesh element */
        GPUDynamicMeshElement& GetMeshElement(size_t index) { return elements[index]; }
        /** access any mesh element */
        GPUDynamicMeshElement const& GetMeshElement(size_t index) const { return elements[index]; }
        /** add an rendering element */
        GPUDynamicMeshElement & AddMeshElement();
        /** set the vertex array manager */
        void SetVertexArrayCache(GPUVertexArrayCache* in_vertex_array_cache) {vertex_array_cache = in_vertex_array_cache;}

    protected:

        /** override */
        virtual int DoDisplay(GPURenderer* renderer, GPUProgramProviderBase const* uniform_provider, GPURenderParams const& render_params);

    protected:

        /** the element to render */
        std::vector<GPUDynamicMeshElement> elements;
        /** the last time the dynamic mesh was rendered with current elements */
        shared_ptr<GPUFence> last_rendered_fence;
        /** the vertex array cache */
        shared_ptr<GPUVertexArrayCache> vertex_array_cache;
    };

}; // namespace chaos
