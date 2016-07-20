#ifndef CHAOS_CUBERAYCASTINGBUFFER_H
#define CHAOS_CUBERAYCASTINGBUFFER_H

#include <assert.h>
#include <cstring>
#include <utility>
#include <chaos/GeometryFramework.h>

namespace chaos
{

// ==============================================================================================
// CubeRayCastingBufferPoint describes a cell (x, y) on one of the 6 faces of a cube 
// ==============================================================================================

class CubeRayCastingBufferPoint
{
public:

    /* returns the index of a data for a given face */
    inline size_t GetPointIndex(size_t resolution) const
    {
        return y * resolution + x;  
    }

#if _DEBUG

    /** returns whether the data is coherent with the resolution */
    bool IsValid(size_t resolution) const
    {
        if (face < 0 || face > 5)
            return false;
        if (x < 0 || x >= resolution)
            return false;
        if (y < 0 || y >= resolution)
            return false;
        return true;                          
    }

#endif // _DEBUG        

    /** the index of the cube face (from 0 to 5) */
    size_t face;
    /** the x position of the cell (from 0 to resolution - 1) */
    size_t x;
    /** the y position of the cell (from 0 to resolution - 1) */
    size_t y;  
};

// ==============================================================================================
// CubeRayCastingBufferBase give generic methods to now the size of a requested buffer for 
//  CubeRayCastingBuffer or to know how to index the buffer with a "point"
// ==============================================================================================

template<bool BIDIRECTIONNAL = true>
class CubeRayCastingBufferBase
{
public:

    static const bool bidirectionnal = BIDIRECTIONNAL;

    /** returns the memory requested for a given cube resolution */
    static inline size_t GetReservedCount(size_t resolution)
    {
        // One Ray enters by 1 face and leaves by another.
        // For each of theses faces, it intersects 1 cell 
        // cell per face = (resolution * resolution)
        // number of possible combinations of faces = C(2, 6)
        // 
        //  C(2, 6) = 6 * 5 / 2! = 15        (2! if not bidirectionnal)
        //
        // result = (5 * 6 / 2 ) * (resolution * resolution) * (resolution * resolution)

        resolution *= resolution;
        resolution *= resolution;  
        resolution *= bidirectionnal? 30 : 15;        
        return resolution;    
    }

    /** a block is bunch of data to describ all combinaison of x1, y1, x2, y2 (face ignored) */
    /** returns the index of the block for 2 given faces */
    static inline size_t GetBlockIndex(size_t face1, size_t face2)
    {
        assert(face1 != face2);  
        size_t result;

        if (bidirectionnal)
        {
// 
//             static size_t const offset[] = {
//                 -1,  0,  1,  2,  3,  4,
//                 5, -1,  6,  7,  8,  9,
//                 10, 11, -1, 12, 13, 14,
//                 15, 16, 17, -1, 18, 19,
//                 20, 21, 22, 23, -1, 24,
//                 25, 26, 27, 28, 29, -1};
           
            result =  5 * face1 + face2 - ((face2 > face1)? 1 : 0);      
            assert(result <  6 * 5);
        }
        else
        {
            assert(face1 < face2);
            assert(face1 <= 4);

            // face1 == 0    =>     initial offset = 0        nb_possibility = 5    
            // face1 == 1    =>     initial offset = 5        nb_possibility = 4
            // face1 == 2    =>     initial offset = 9        nb_possibility = 3
            // face1 == 3    =>     initial offset = 12       nb_possibility = 2
            // face1 == 4    =>     initial offset = 14       nb_possibility = 1

            static size_t const offset[] = {0, 5, 9, 12, 14};                              

            result = offset[face1] + face2 - face1 - 1; 
            assert(result <  6 * 5 / 2);
        }   
        assert(result >= 0);
        return result;
    }

    /** returns the index of the buffer to access a data for any combinaison of cell/face */
    static inline size_t GetDataIndex(CubeRayCastingBufferPoint entry_point, CubeRayCastingBufferPoint exit_point, size_t resolution)
    {
        assert(entry_point.IsValid(resolution));  
        assert(exit_point.IsValid(resolution));    
        assert(exit_point.face != entry_point.face);    

        if (!bidirectionnal && entry_point.face > exit_point.face)
            std::swap(entry_point, exit_point);      

        // one block is a resolution ^ 4 block of data 
        // (correspond to the combinaison of all cells from one face to another)                                           
        size_t res_per_face    = resolution * resolution;
        size_t offset_entry    = entry_point.GetPointIndex(resolution);  
        size_t offset_exit     = exit_point.GetPointIndex(resolution);
        size_t offset_in_block = offset_entry * res_per_face + offset_exit;   

        size_t num_block    = GetBlockIndex(entry_point.face, exit_point.face);
        size_t block_size   = res_per_face * res_per_face;    

        size_t result = num_block * block_size + offset_in_block;    
        assert(result < GetReservedCount(resolution));
        return result;
    }
};

// ==============================================================================================
// CubeRayCastingBuffer is used to make a binding between 2 cells on 2 differents faces 
//   So we can index a buffer by using theses 2 points
//   If buffer is not bidirectionnal   data(p1, p2) is equal to data(p2, p1)
// ==============================================================================================

template<typename DATA_TYPE, bool BIDIRECTIONNAL = true>
class CubeRayCastingBuffer : public CubeRayCastingBufferBase<BIDIRECTIONNAL>
{
public:

    typedef DATA_TYPE type;

    /** constructor */
    CubeRayCastingBuffer() : resolution(0), buffer(NULL) {};

    /** destructor */
    ~CubeRayCastingBuffer(){ ClearBuffer();  }

    /** delete the buffer */
    void ClearBuffer(bool release_memory = true) 
    {
        if (buffer != NULL)
        {
            if (release_memory)
                delete [] buffer;

            resolution = NULL;
            buffer     = NULL;
        }
    }

    /** allocate a new buffer */
    type * Allocate(size_t in_resolution)
    {
        assert(in_resolution > 0);
        
        ClearBuffer(true);

        size_t size = GetReservedCount(in_resolution);
        buffer = new type[size];
        if (buffer != NULL)
            resolution = in_resolution;

        return buffer;
    }

    /** returns the total number of elements reserved */
    inline size_t GetBufferSize() const { return GetReservedCount(resolution); }

    /** returns the address of an element */
    inline size_t GetDataIndex(CubeRayCastingBufferPoint const & p1, CubeRayCastingBufferPoint const & p2) const
    {
        return CubeRayCastingBufferBase<bidirectionnal>::GetDataIndex(p1, p2, resolution);
    }

    /** returns the first elements of the buffer */
    inline type * const GetBuffer() const { return buffer; }
    /** returns the first elements of the buffer */
    inline type * GetBuffer() { return buffer; }

    /** access an element on a buffer */
    inline type const & operator ()(CubeRayCastingBufferPoint const & p1, CubeRayCastingBufferPoint const & p2) const
    {
        assert(p1.IsValid(resolution));
        assert(p2.IsValid(resolution));
        return buffer[GetDataIndex(p1, p2)];
    }

    /** access an element on a buffer */
    inline type & operator ()(CubeRayCastingBufferPoint const & p1, CubeRayCastingBufferPoint const & p2)
    {
        assert(p1.IsValid(resolution));
        assert(p2.IsValid(resolution));
        return buffer[GetDataIndex(p1, p2)];
    }

    /** an enumerator */
    template<typename FUNC_TYPE>
    FUNC_TYPE ForEachCombinaison(FUNC_TYPE func) // function must returns an integral result for early exit 
    {
        if (bidirectionnal)
        {
            for (size_t f1 = 0 ; f1 < 6 ; ++f1)
            {
                for (size_t f2 = 0 ; f2 < 6 ; ++f2)
                {
                    if (f1 == f2)
                        continue;

                    for (size_t y1 = 0 ; y1 < resolution ; ++y1)
                    {
                        for (size_t x1 = 0 ; x1 < resolution ; ++x1)
                        {
                            CubeRayCastingBufferPoint p1;
                            p1.face   = f1;
                            p1.x = x1;                
                            p1.y = y1;

                            for (size_t y2 = 0 ; y2 < resolution ; ++y2)
                            {
                                for (size_t x2 = 0 ; x2 < resolution ; ++x2)
                                {
                                    CubeRayCastingBufferPoint p2;
                                    p2.face   = f2;
                                    p2.x = x2;                
                                    p2.y = y2; 

                                    if (!func(*this, p1, p2))
                                        return func;
                                }      
                            }      
                        }      
                    }
                }  
            }
        }
        else
        {
            for (size_t f1 = 0 ; f1 < 5 ; ++f1)
            {
                for (size_t f2 = f1 + 1 ; f2 < 6 ; ++f2)
                {
                    for (size_t y1 = 0 ; y1 < resolution ; ++y1)
                    {
                        for (size_t x1 = 0 ; x1 < resolution ; ++x1)
                        {
                            CubeRayCastingBufferPoint p1;
                            p1.face   = f1;
                            p1.x = x1;                
                            p1.y = y1;

                            for (size_t y2 = 0 ; y2 < resolution ; ++y2)
                            {
                                for (size_t x2 = 0 ; x2 < resolution ; ++x2)
                                {
                                    CubeRayCastingBufferPoint p2;
                                    p2.face   = f2;
                                    p2.x = x2;                
                                    p2.y = y2; 

                                    if (!func(*this, p1, p2))
                                        return func;
                                }      
                            }      
                        }      
                    }
                }  
            }
        }
        return func;
    }

#if 0
    void GetIntersectingCells(float3 const & p1, float3 const & p2, CubeRayCastingBufferPoint

    /** returns the coordinates of the center of the cell. The cube's edge are supposed to be graduated [-1 , +1] */
    float3 GetCellCenter(CubeRayCastingBufferPoint const & cell) const
    {
      static float3 const direction[] = {
          float3( 1.0f,  0.0f,  0.0f),    // face 0 = POS_X
          float3(-1.0f,  0.0f,  0.0f),    // face 1 = NEG_X
          float3( 0.0f,  1.0f,  0.0f),    // face 2 = POS_Y
          float3( 0.0f, -1.0f,  0.0f),    // face 3 = NEG_Y
          float3( 0.0f,  0.0f,  1.0f),    // face 4 = POS_Z
          float3( 0.0f,  0.0f, -1.0f)     // face 5 = NEG_Z
      } 

      float size = 2.0f / (float)resolution;

      size_t face = cell.face;
      return direction[face] 
        + direction[(cell.face + 2) % 6]
        + direction[(cell.face + 4) % 6];
    }

#endif

protected:

    /** the resolution per face of the cube */
    size_t resolution;
    /** the storage */
    type * buffer;
};

// ================================================================================================

#if _DEBUG

template<typename CUBE_TYPE>
class CubeRayCastingTestFunc
{
public:

    CubeRayCastingTestFunc(int in_limit_value) : limit_value(in_limit_value), result(true){}

    bool operator () (CUBE_TYPE & cube, CubeRayCastingBufferPoint p1, CubeRayCastingBufferPoint p2)
    {
        CUBE_TYPE::type & data1 = cube(p1, p2);
        CUBE_TYPE::type & data2 = cube(p2, p1);
        if (cube.bidirectionnal && (&data1 == &data2)) // data must points different addresses
            return result = false;  
        else if (!cube.bidirectionnal && (&data1 != &data2)) // data must points same address
            return result = false;
        if (++data1 > limit_value)
            return result = false;  
        return true;
    }

    int limit_value;

    bool result;
};


// This test ensure that all cells for all faces are modified once and only once
// means there is a bijection between the buffer and the (cell1, cell2, face1, face2) set
// It ensures that for bidirectionnal cube, inverting coordinates change buffer cell address
// (reciproq for non bidirectionnal cube)
template<typename CUBE_TYPE>
bool TestCube(size_t resolution, int limit_value)
{
    CUBE_TYPE cube;
    
    int * buffer = cube.Allocate(resolution);

    if (buffer != NULL)
    {
        size_t size = cube.GetBufferSize();
        memset(buffer, 0, size * sizeof(int));

        CubeRayCastingTestFunc<CUBE_TYPE> func(limit_value);
        if (!cube.ForEachCombinaison(func).result)
            return false;

        for (size_t i = 0 ; i < size ; ++i)
            if (buffer[i] != limit_value)
                return false;     
    }

    return true;
}

void TestCube()
{
    for (size_t i = 1 ; i < 16 ; ++i)
    {
        if (TestCube< CubeRayCastingBuffer<int, false> >(i, 1) == false)
        {
            assert(0);  
        }
    }

    for (size_t i = 1 ; i < 8 ; ++i)
    {
        if (TestCube< CubeRayCastingBuffer<int, true> >(i, 1) == false)
        {
            assert(0);  
        }    
    }
} 

#endif

}; // namespace chaos

#endif // CHAOS_CUBERAYCASTINGBUFFER_H