#include <chaos/GLTools.h>
#include <chaos/LogTools.h>
#include <chaos/DebugTools.h>
#include <chaos/MathTools.h>

namespace chaos
{

void GLTools::FreeVertexAndIndexBuffers(GLuint * vertex_array, GLuint * vertex_buffer, GLuint * index_buffer) 
{
  // release the buffers
  GLuint buffers[2] = {0, 0};

  int buffer_count = 0;
  if (vertex_buffer != nullptr && *vertex_buffer > 0)
    std::swap(buffers[buffer_count++], *vertex_buffer); // in the same movement, this reset vertex_buffer value and push it in another array

  if (index_buffer != nullptr && *index_buffer > 0)
    std::swap(buffers[buffer_count++], *index_buffer);  // in the same movement, this reset index_buffer value and push it in another array

  glDeleteBuffers(buffer_count, buffers); // single call for both index & vertex

  // release the vertex array
  if (vertex_array != nullptr)
  {
    glDeleteVertexArrays(1, vertex_array);  
    *vertex_array = 0;
  }
}

bool GLTools::MapBuffers(GLuint vertex_buffer, GLuint index_buffer, size_t vb_size, size_t ib_size, std::pair<char*, GLuint*> & result)
{
  result = std::make_pair(nullptr, nullptr);

  if (vertex_buffer != 0)
  {
    glNamedBufferData(vertex_buffer, vb_size, nullptr, GL_STATIC_DRAW);
    result.first = (char *)glMapNamedBuffer(vertex_buffer, GL_WRITE_ONLY);
    if (result.first == nullptr)
      return false;  
  }

  if (index_buffer != 0)
  {
    glNamedBufferData(index_buffer, ib_size, nullptr, GL_STATIC_DRAW);
    result.second = (GLuint *)glMapNamedBuffer(index_buffer, GL_WRITE_ONLY);  
    if (result.second == nullptr && vertex_buffer != 0)
    {    
      glUnmapNamedBuffer(vertex_buffer);
      return false;
    }
  }

  return true;
}

bool GLTools::GenerateVertexAndIndexBuffers(GLuint * vertex_array, GLuint * vertex_buffer, GLuint * index_buffer)
{
  int buffer_count = 0;

  // clean the result
  if (vertex_array != nullptr)
    *vertex_array = 0;

  if (vertex_buffer != nullptr)
  {
    *vertex_buffer = 0;
    ++buffer_count;
  }

  if (index_buffer != nullptr)
  {
    *index_buffer = 0;
    ++buffer_count;
  }

  // allocate the vertex array
  if (vertex_array != nullptr)
  {
    glCreateVertexArrays(1, vertex_array);
    if (*vertex_array <= 0)
      return false;    
  }

  // allocate the buffers
  if (buffer_count > 0)
  {
    GLuint buffers[2] = {0, 0};
    glCreateBuffers(buffer_count, buffers);

    int count = 0;
    if (vertex_buffer != nullptr)
      *vertex_buffer = buffers[count++];
    if (index_buffer != nullptr)
      *index_buffer = buffers[count++];

    if ((vertex_buffer != nullptr && *vertex_buffer <= 0) || 
        (index_buffer  != nullptr && *index_buffer  <= 0))
    {
      FreeVertexAndIndexBuffers(vertex_array, vertex_buffer, index_buffer); // ensure all success
      return false;          
    }
  }

  return true;
}

bool GLTools::GenerateVertexAndIndexBuffersObject(boost::intrusive_ptr<VertexArray> * vertex_array, boost::intrusive_ptr<VertexBuffer> * vertex_buffer, boost::intrusive_ptr<IndexBuffer> * index_buffer)
{
  GLuint va = 0;
  GLuint vb = 0;
  GLuint ib = 0;

  GLuint * va_ptr = (vertex_array  != nullptr)? &va : nullptr;
  GLuint * vb_ptr = (vertex_buffer != nullptr)? &vb : nullptr;
  GLuint * ib_ptr = (index_buffer  != nullptr)? &ib : nullptr;

  if (GenerateVertexAndIndexBuffers(va_ptr, vb_ptr, ib_ptr))
  {
    if (vertex_array != nullptr)
      *vertex_array = new VertexArray(va);
    if (vertex_buffer != nullptr)
      *vertex_buffer = new VertexBuffer(vb);
    if (index_buffer != nullptr)
      *index_buffer = new IndexBuffer(ib);

    return true;
  }
  return false;
}

void GLTools::DisplayGenericInformation()
{
  GLubyte const * renderer = glGetString(GL_RENDERER);
  if (renderer != nullptr)
    LogTools::Log("OpenGL RENDERER        : %s", renderer);

  GLubyte const * vendor = glGetString(GL_VENDOR);
  if (vendor != nullptr)
    LogTools::Log("OpenGL VENDOR          : %s", vendor);

  GLubyte const * version = glGetString(GL_VERSION);
  if (version != nullptr)
    LogTools::Log("OpenGL VERSION         : %s", version);

  GLubyte const * shading_version = glGetString(GL_SHADING_LANGUAGE_VERSION);
  if (shading_version != nullptr)
    LogTools::Log("OpenGL SHADING LANGAGE : %s", shading_version);
}

char const * GLTools::GLenumToString(GLenum type)
{
#define CHAOS_GLENUM_TO_STRING(x) if (type == x) return #x
  CHAOS_GLENUM_TO_STRING(GL_BYTE);
  CHAOS_GLENUM_TO_STRING(GL_UNSIGNED_BYTE);
  CHAOS_GLENUM_TO_STRING(GL_SHORT);
  CHAOS_GLENUM_TO_STRING(GL_UNSIGNED_SHORT);
  CHAOS_GLENUM_TO_STRING(GL_INT);
  CHAOS_GLENUM_TO_STRING(GL_UNSIGNED_INT);
  CHAOS_GLENUM_TO_STRING(GL_FLOAT);
  CHAOS_GLENUM_TO_STRING(GL_2_BYTES);
  CHAOS_GLENUM_TO_STRING(GL_3_BYTES);
  CHAOS_GLENUM_TO_STRING(GL_4_BYTES);
  CHAOS_GLENUM_TO_STRING(GL_DOUBLE);
  CHAOS_GLENUM_TO_STRING(GL_FLOAT_VEC2);
  CHAOS_GLENUM_TO_STRING(GL_FLOAT_VEC3);
  CHAOS_GLENUM_TO_STRING(GL_FLOAT_VEC4);
  CHAOS_GLENUM_TO_STRING(GL_INT_VEC2);
  CHAOS_GLENUM_TO_STRING(GL_INT_VEC3);
  CHAOS_GLENUM_TO_STRING(GL_INT_VEC4);
  CHAOS_GLENUM_TO_STRING(GL_BOOL);
  CHAOS_GLENUM_TO_STRING(GL_BOOL_VEC2);
  CHAOS_GLENUM_TO_STRING(GL_BOOL_VEC3);
  CHAOS_GLENUM_TO_STRING(GL_BOOL_VEC4);
  CHAOS_GLENUM_TO_STRING(GL_FLOAT_MAT2);
  CHAOS_GLENUM_TO_STRING(GL_FLOAT_MAT3);
  CHAOS_GLENUM_TO_STRING(GL_FLOAT_MAT4);
  CHAOS_GLENUM_TO_STRING(GL_SAMPLER_1D);
  CHAOS_GLENUM_TO_STRING(GL_SAMPLER_2D);
  CHAOS_GLENUM_TO_STRING(GL_SAMPLER_3D);
  CHAOS_GLENUM_TO_STRING(GL_SAMPLER_CUBE);
  CHAOS_GLENUM_TO_STRING(GL_SAMPLER_1D_SHADOW);
  CHAOS_GLENUM_TO_STRING(GL_SAMPLER_2D_SHADOW);
  CHAOS_GLENUM_TO_STRING(GL_SAMPLER_1D_ARRAY);
  CHAOS_GLENUM_TO_STRING(GL_SAMPLER_2D_ARRAY);
  CHAOS_GLENUM_TO_STRING(GL_SAMPLER_1D_ARRAY_SHADOW);
  CHAOS_GLENUM_TO_STRING(GL_SAMPLER_2D_ARRAY_SHADOW);
  CHAOS_GLENUM_TO_STRING(GL_SAMPLER_CUBE_SHADOW);
  CHAOS_GLENUM_TO_STRING(GL_UNSIGNED_INT_VEC2);
  CHAOS_GLENUM_TO_STRING(GL_UNSIGNED_INT_VEC3);
  CHAOS_GLENUM_TO_STRING(GL_UNSIGNED_INT_VEC4);
  CHAOS_GLENUM_TO_STRING(GL_INT_SAMPLER_1D);
  CHAOS_GLENUM_TO_STRING(GL_INT_SAMPLER_2D);
  CHAOS_GLENUM_TO_STRING(GL_INT_SAMPLER_3D);
  CHAOS_GLENUM_TO_STRING(GL_INT_SAMPLER_CUBE);
  CHAOS_GLENUM_TO_STRING(GL_INT_SAMPLER_1D_ARRAY);
  CHAOS_GLENUM_TO_STRING(GL_INT_SAMPLER_2D_ARRAY);
  CHAOS_GLENUM_TO_STRING(GL_UNSIGNED_INT_SAMPLER_1D);
  CHAOS_GLENUM_TO_STRING(GL_UNSIGNED_INT_SAMPLER_2D);
  CHAOS_GLENUM_TO_STRING(GL_UNSIGNED_INT_SAMPLER_3D);
  CHAOS_GLENUM_TO_STRING(GL_UNSIGNED_INT_SAMPLER_CUBE);
  CHAOS_GLENUM_TO_STRING(GL_UNSIGNED_INT_SAMPLER_1D_ARRAY);
  CHAOS_GLENUM_TO_STRING(GL_UNSIGNED_INT_SAMPLER_2D_ARRAY);

  CHAOS_GLENUM_TO_STRING(GL_DEBUG_SEVERITY_HIGH);
  CHAOS_GLENUM_TO_STRING(GL_DEBUG_SEVERITY_MEDIUM);
  CHAOS_GLENUM_TO_STRING(GL_DEBUG_SEVERITY_LOW);
  CHAOS_GLENUM_TO_STRING(GL_DEBUG_SEVERITY_NOTIFICATION);

  CHAOS_GLENUM_TO_STRING(GL_DEBUG_TYPE_ERROR);
  CHAOS_GLENUM_TO_STRING(GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR);
  CHAOS_GLENUM_TO_STRING(GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR);
  CHAOS_GLENUM_TO_STRING(GL_DEBUG_TYPE_PORTABILITY);
  CHAOS_GLENUM_TO_STRING(GL_DEBUG_TYPE_PERFORMANCE);
  CHAOS_GLENUM_TO_STRING(GL_DEBUG_TYPE_OTHER);
  CHAOS_GLENUM_TO_STRING(GL_DEBUG_TYPE_MARKER);
  CHAOS_GLENUM_TO_STRING(GL_DEBUG_TYPE_PUSH_GROUP);
  CHAOS_GLENUM_TO_STRING(GL_DEBUG_TYPE_POP_GROUP);

  CHAOS_GLENUM_TO_STRING(GL_DEBUG_SOURCE_API);
  CHAOS_GLENUM_TO_STRING(GL_DEBUG_SOURCE_WINDOW_SYSTEM);
  CHAOS_GLENUM_TO_STRING(GL_DEBUG_SOURCE_SHADER_COMPILER);
  CHAOS_GLENUM_TO_STRING(GL_DEBUG_SOURCE_THIRD_PARTY);
  CHAOS_GLENUM_TO_STRING(GL_DEBUG_SOURCE_APPLICATION);
  CHAOS_GLENUM_TO_STRING(GL_DEBUG_SOURCE_OTHER);

#undef CHAOS_GLENUM_TO_STRING

  return "Unknown";
}

void WINAPI GLTools::DebugMessageHandler(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, const GLchar * msg, const void * user_data)
{
  if (severity != GL_DEBUG_SEVERITY_NOTIFICATION)
  {
    char const * source_str   = GLenumToString(source);
    char const * type_str     = GLenumToString(type);
    char const * severity_str = GLenumToString(severity);

    LogTools::Log("OpenGL warning message   [%s]", msg);
    LogTools::Log("               severity  [%s]", severity_str);
    LogTools::Log("               source    [%s]", source_str);
    LogTools::Log("               type      [%s]", type_str);

    DebugTools::DisplayCallStack(std::cout);  
  }
}

void GLTools::SetDebugMessageHandler()
{
  if (glDebugMessageCallback == nullptr) // some low profile cannot use debug features
    return;

  glDebugMessageCallback(DebugMessageHandler, nullptr);

#if _DEBUG
  glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS_ARB); // this make debug events called immediately (Can BREAK in visual studio)
#endif
}

}; // namespace chaos
