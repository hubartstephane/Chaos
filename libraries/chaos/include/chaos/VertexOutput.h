#pragma once

#include <chaos/StandardHeaders.h>

#include <chaos/GPUClasses.h>

namespace chaos
{
    // ==================================================
    // VertexOutputBase
    // ==================================================

    class VertexOutputBase
    {
    public:

        /** default constructor */
        VertexOutputBase() = default;
        /** copy constructor */
        VertexOutputBase(VertexOutputBase const & src) = default;
        /** constructor for dynamic GPU buffer allocation */
        VertexOutputBase(ParticleLayerBase * in_particle_layer) :
            particle_layer(in_particle_layer)
        {
            assert(in_particle_layer != nullptr);
        }

        /** gets current buffer */
        void* GetBuffer() const
        {
            return buffer;
        }
        /** gets the buffer size */
        size_t GetBufferSize() const
        {
            return buffer_size;
        }
        /** gets the particle size */
        size_t GetParticleSize() const
        {
            return particle_size;
        }
        /** gets the current position */
        size_t GetPosition() const
        {
            return position;
        }
        /** gets the max vertex index */
        size_t GetMaxVertexIndex() const
        {
            return max_vertex_index;
        }
        /** the particle layer */
        class ParticleLayerBase* GetParticleLayer() const
        {
            return particle_layer;
        }

    protected:

        /** the particle layer in use (to store primitives to render) */
        ParticleLayerBase* particle_layer = nullptr;

        /** the buffer where we write buffer */
        void* buffer = nullptr;
        /** the full buffer size */
        size_t buffer_size = 0;
        /** the particle size */
        size_t particle_size = 0;
        /** the current cursor for considered particle */
        size_t position = 0;
        /** the maximum vertex index implied for the current particle */
        size_t max_vertex_index = std::numeric_limits<size_t>::max();
    };

    // ==================================================
    // VertexOutput
    // ==================================================

    template<typename VERTEX_TYPE>
    class VertexOutput : public VertexOutputBase
    {
    public:

        /** default constructor */
        VertexOutput() = default;
        /** copy constructor */
        VertexOutput(VertexOutput const& src) = default;
        /** downcast constructor */
        template<typename OTHER_VERTEX_TYPE>
        VertexOutput(VertexOutput<OTHER_VERTEX_TYPE> const& src, std::enable_if_t<std::is_base_of_v<VERTEX_TYPE, OTHER_VERTEX_TYPE>, int> = 0) :
            buffer(src.GetBuffer()),
            buffer_size(src.GetBufferSize()),
            particle_size(src.GetParticleSize()),
            position(src.GetPosition()),
            max_vertex_index(src.GetMaxVertexIndex()),
            particle_Layer(src.GetParticleLayer()){}

        /** constructor for dynamic GPU buffer allocation */
        VertexOutput(ParticleLayerBase * in_particle_layer) :
            VertexOutputBase(in_particle_layer){}

        VERTEX_TYPE& operator [](size_t index)
        {
            // update the max vertex index used for this particle
            max_vertex_index = (max_vertex_index == std::numeric_limits<size_t>::max()) ?
                index : std::max(max_vertex_index, index);

            // the index is out of bound
            if (particle_size * (position + index + 1) > buffer_size) // +1 to be sure the END of the vertex is in the reserve buffer
            {


            }


            // return the particle
            return *((VERTEX_TYPE*)(((char*)buffer) + particle_size * (position + index)));
        }

        void NextParticle()
        {
            // no particle inserted
            if (max_vertex_index == std::numeric_limits<size_t>::max())
                return;
            // change the current writing position in the whole buffer
            position += max_vertex_index;
            // no particles inserted yet
            max_vertex_index = std::numeric_limits<size_t>::max();
        }

    };


}; // namespace chaos

