#pragma once

#include <chaos/StandardHeaders.h>

#include <chaos/GPUClasses.h>

namespace chaos
{
    // XXX : VertexOutput is the object used to stream out vertices with operator []
    //
    //       Due to downcasting and copies of data, we want the pointers/data used is shared among the whole chain of calls
    //       That's why i use VertexOutputInterface
    //
    //       VertexOutputInterface is the class that is used to store buffers and other pointers

    // ==================================================
    // VertexOutputInterface
    // ==================================================

    class VertexOutputInterface
    {
        friend class VertexOutputBase;

    public:

        /** constructor for dynamic GPU buffer allocation */
        VertexOutputInterface(ParticleLayerBase * in_particle_layer, GPURenderer * in_renderer) :
            particle_layer(in_particle_layer),
            renderer(in_renderer)
        {
            assert(in_particle_layer != nullptr);
            assert(in_renderer != nullptr);
        }

    public:

        /** the particle layer in use (to store primitives to render) */
        ParticleLayerBase* particle_layer = nullptr;
        /** the renderer used fence requests */
        GPURenderer* renderer = nullptr;

        /** the buffer where we write buffer */
        void* buffer = nullptr;
        /** the full buffer size */
        size_t buffer_size = 0;
        /** the current cursor for considered particle */
        size_t position = 0;
        /** the maximum vertex index implied for the current particle */
        size_t max_vertex_index = std::numeric_limits<size_t>::max();
    };

    // ==================================================
    // VertexOutputBase
    // ==================================================

    class VertexOutputBase
    {
    public:

        /** constructor for dynamic GPU buffer allocation */
        VertexOutputBase(VertexOutputInterface & in_output_interface, size_t in_vertex_size) :
            output_interface(in_output_interface),
            vertex_size(in_vertex_size){}

        /** get the output interface */
        VertexOutputInterface& GetOutputInterface() const { return output_interface; }

        /** gets the vertex size */
        size_t GetVertexSize() const { return vertex_size; }
        /** gets current buffer */
        void* GetBuffer() const { return output_interface.buffer; }
        /** gets the buffer size */
        size_t GetBufferSize() const { return output_interface.buffer_size; }
        /** gets the current position */
        size_t GetPosition() const { return output_interface.position; }
        /** gets the max vertex index */
        size_t GetMaxVertexIndex() const { return output_interface.max_vertex_index; }
        /** the particle layer */
        ParticleLayerBase* GetParticleLayer() const { return output_interface.particle_layer; }
        /** the renderer */
        GPURenderer* GetRenderer() const { return output_interface.renderer; }

    protected:

        /** the interface to trigger when any data is inserted */
        VertexOutputInterface & output_interface;
        /** the particle size */
        size_t vertex_size = 0;
    };

    // ==================================================
    // VertexOutput
    // ==================================================

    template<typename VERTEX_TYPE>
    class VertexOutput : public VertexOutputBase
    {
    public:

        /** downcast constructor */
        template<typename OTHER_VERTEX_TYPE>
        VertexOutput(VertexOutput<OTHER_VERTEX_TYPE> const& src, std::enable_if_t<std::is_base_of_v<VERTEX_TYPE, OTHER_VERTEX_TYPE>, int> = 0) :
            VertexOutputBase(src.GetOutputInterface(), src.GetVertexSize()){}
        /** constructor for dynamic GPU buffer allocation */
        VertexOutput(VertexOutputInterface & in_output_interface) :
            VertexOutputBase(in_output_interface, sizeof(VERTEX_TYPE)){}


        /** accessor */
        VERTEX_TYPE& operator [](size_t index)
        {
            // update the max vertex index used for this particle
            output_interface.max_vertex_index = (output_interface.max_vertex_index == std::numeric_limits<size_t>::max()) ?
                index : std::max(output_interface.max_vertex_index, index);

            // the index is out of bound
            if (vertex_size * (output_interface.position + index + 1) > output_interface.buffer_size) // +1 to be sure the END of the vertex is in the reserve buffer
            {


            }


            // return the particle
            return *((VERTEX_TYPE*)(((char*)output_interface.buffer) + vertex_size * (output_interface.position + index)));
        }

        void NextParticle()
        {
            // no particle inserted
            if (output_interface.max_vertex_index == std::numeric_limits<size_t>::max())
                return;
            // change the current writing position in the whole buffer
            output_interface.position += max_vertex_index;
            // no particles inserted yet
            output_interface.max_vertex_index = std::numeric_limits<size_t>::max();
        }

    };


}; // namespace chaos

