#pragma once

#include <chaos/StandardHeaders.h>

#include <chaos/GPUClasses.h>

namespace chaos
{

    /**
     * PrimitiveBase : base object for writing GPU primitives into memory (GPU mapped memory for the usage) 
     */

    template<size_t VERTICES_COUNT = 0>
    class PrimitiveBase
    {
    public:

        /** gets the buffer for this primitive */
        void* GetBuffer() const { return buffer; }
        /** gets the size of one vertice of this primitive */
        size_t GetVertexSize() const { return vertex_size; }
        /** gets the number of vertices for this primitive */
        size_t GetVerticesCount() const { return vertices_count; }

    protected:

        /** the buffer where we write buffer */
        void* buffer = nullptr;
        /** the size of a vertex for this primitive */
        size_t vertex_size = 0;
        /** the number of vertices for this primitive */
        size_t vertices_count = VERTICES_COUNT;
    };

    /**
      * TypedPrimitiveBase : base object for writing GPU primitives into memory (vertex type is known)
      */

    template<typename VERTEX_TYPE, size_t VERTICES_COUNT = 0>
    class TypedPrimitiveBase : public PrimitiveBase<VERTICES_COUNT>
    {
    public:

        using vertex_type = VERTEX_TYPE;

        /** base constructor */
        TypedPrimitiveBase() = default;
        /** copy constructor */
        TypedPrimitiveBase(TypedPrimitiveBase const & src) = default;
        /** downcast constructor */
        template<typename OTHER_VERTEX_TYPE>
        TypedPrimitiveBase(TypedPrimitiveBase<OTHER_VERTEX_TYPE> const& src, std::enable_if_t<std::is_base_of_v<VERTEX_TYPE, OTHER_VERTEX_TYPE>, int> = 0) :
            TypedPrimitiveBase(src.GetBuffer(), src.GetVertexSize(), src.GetVerticesCount()) {}
        /** constructor */
        TypedPrimitiveBase(void * in_buffer, size_t in_vertex_size, size_t in_vertices_count) :
            buffer(in_buffer),
            vertex_size(in_vertex_size),
            vertices_count(in_vertices_count)
        {
            assert(in_buffer != nullptr);
            assert(vertex_size > 0);
            // xxx : do not test for vertices_count. At this point 0, is a valid value for Fans and Strips
        }

        /** accessor */
        vertex_type & operator [](size_t index)
        {
            assert(index < vertices_count);
            return *((vertex_type*)(((char*)buffer) + vertex_size * index));
        }

        /** const accessor */
        vertex_type const & operator [](size_t index) const
        {
            assert(index < vertices_count);
            return *((vertex_type const*)(((char const*)buffer) + vertex_size * index));
        }
    };

    /**
      * The usual existing primitives
      */

    template<typename VERTEX_TYPE> using TrianglePrimitive = TypedPrimitiveBase<VERTEX_TYPE, 3>;
    template<typename VERTEX_TYPE> using TrianglePairPrimitive = TypedPrimitiveBase<VERTEX_TYPE, 6>;
    template<typename VERTEX_TYPE> using QuadPrimitive = TypedPrimitiveBase<VERTEX_TYPE, 4>;
    template<typename VERTEX_TYPE> using TriangleFanPrimitive = TypedPrimitiveBase<VERTEX_TYPE, 0>;   // 0 for non-fixed vertices count
    template<typename VERTEX_TYPE> using TriangleStripPrimitive = TypedPrimitiveBase<VERTEX_TYPE, 0>;








    template<size_t VERTICES_COUNT = 0>
    class PrimitiveOutputBase
    {
    public:

        /** constructor */
        PrimitiveOutputBase(ParticleLayerBase* in_particle_layer, GPURenderer* in_renderer) :
            particle_layer(in_particle_layer),
            renderer(in_renderer)
        {
          //  assert(in_particle_layer != nullptr);
           // assert(in_renderer != nullptr);
        }

    protected:

        /** the particle layer in use (to store primitives to render) */
        ParticleLayerBase* particle_layer = nullptr;
        /** the renderer used fence requests */
        GPURenderer* renderer = nullptr;
    };







    template<typename VERTEX_TYPE, size_t VERTICES_COUNT = 0>
    class TypedPrimitiveOutputBase : public PrimitiveOutputBase<VERTICES_COUNT>
    {
    public:

        using vertex_type = VERTEX_TYPE;

        using primitive_type = TypedPrimitiveBase<VERTEX_TYPE, VERTICES_COUNT>;

        /** constructor */
        TypedPrimitiveOutputBase(ParticleLayerBase* in_particle_layer, GPURenderer* in_renderer) :
            PrimitiveOutputBase(in_particle_layer, in_renderer) {}

        primitive_type AddPrimitive()
        {
            primitive_type result;



            return result;
        }





    };

    /**
      * The usual existing primitives output's
      */

    template<typename VERTEX_TYPE> using TriangleOutput = TypedPrimitiveOutputBase<VERTEX_TYPE, 3>;
    template<typename VERTEX_TYPE> using TrianglePairOutput = TypedPrimitiveOutputBase<VERTEX_TYPE, 6>;
    template<typename VERTEX_TYPE> using QuadOutput = TypedPrimitiveOutputBase<VERTEX_TYPE, 4>;
    template<typename VERTEX_TYPE> using TriangleFanOutput = TypedPrimitiveOutputBase<VERTEX_TYPE, 0>;   // 0 for non-fixed vertices count
    template<typename VERTEX_TYPE> using TriangleStripOutput = TypedPrimitiveOutputBase<VERTEX_TYPE, 0>;


#if 0


























    // ==================================================
    // PrimitiveOutputInterface
    // ==================================================

    class PrimitiveOutputInterface
    {
        /** constructor */
        PrimitiveOutputInterface(ParticleLayerBase* in_particle_layer, GPURenderer* in_renderer) :
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
    };



    TriangleOutput;
    TrianglePairOutput;
    QuadOutput;
    TriangleFanOutput;
    TriangleStripOutput;


    TrianglePrimitive;
    TrianglePairPrimitive;
    QuadPrimitive;
    TriangleFanPrimitive;
    TriangleStripPrimitive;


    template<typename VERTEX_TYPE>
    class Primitive
    {

        /**/

        /** the particle size */
        size_t vertex_size = 0;
    };






#endif

    // ==========================================================================




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

