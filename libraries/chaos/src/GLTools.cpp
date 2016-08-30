#include <chaos/GLTools.h>
#include <chaos/LogTools.h>
#include <chaos/DebugTools.h>
#include <chaos/GLProgramData.h>
#include <chaos/VertexDeclaration.h>
#include <chaos/Buffer.h>
#include <chaos/MathTools.h>


//
// Some reminders for OpenGL:
// -------------------------
//
//                       +1
//       +--------------+ 
//       |              |
//       |              |
//       |    screen    |
//       |     space    |
//       |              |
//       |              |
//       +--------------+
//     -1              
//
//
//                       1
//       +--------------+ 
//       |     top      |
//       |              |
//       |    Texture   |  The OpenGL texture is oriented in the same direction than BITMAP FILE
//       |              |
//       |              |
//       |    bottom    |
//       +--------------+
//      0       
//
// the storage in memory is reversed
//
//       +--------------+ 
//       | LINE 5       |
//       | LINE 4       |
//       | LINE 3       |
//       | LINE 2       |
//       | LINE 1       |
//       | LINE 0       |
//       +--------------+
//          

namespace chaos
{

Buffer<char> GLTools::GetProgramBinary(GLuint program)
{
  Buffer<char> result;

  GLenum binary_format = GL_NONE;

  // get the size of the binary chunk
  GLsizei binary_size = 0; 
  glGetProgramiv(program, GL_PROGRAM_BINARY_LENGTH, &binary_size);

  // allocate the buffer and get the binary (the format is a prefix in the buffer)
  if (binary_size > 0)
  {
    result = SharedBufferPolicy<char>::NewBuffer(binary_size + sizeof(GLenum));
    if (result != nullptr)
    {      
      glGetProgramBinary(program, binary_size, &binary_size, &binary_format, result.data + sizeof(GLenum));
      memcpy(result.data, &binary_format, sizeof(GLenum));
    }
  }
  return result;
}

GLuint GLTools::GetProgramFromBinary(Buffer<char> const & buffer)
{
  if (buffer.bufsize < sizeof(GLenum))
    return 0;

  GLuint result = glCreateProgram();
  if (result != 0)
  {
    GLenum binary_format = GL_NONE;
    memcpy(&binary_format, buffer.data, sizeof(GLenum));
  
    glProgramBinary(result, binary_format, buffer.data + sizeof(GLenum), (GLsizei)buffer.bufsize - sizeof(GLenum));
    if (CheckProgramStatus(result, GL_LINK_STATUS, "Program from binary failure") != GL_TRUE)
      glDeleteProgram(result);
  }
  return result;
}

boost::intrusive_ptr<GLProgram> GLTools::GetProgramObjectFromBinary(Buffer<char> const & buffer)
{
  GLuint program_id = GetProgramFromBinary(buffer);
  return (program_id == 0)? nullptr : new GLProgram(program_id);
}

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

GLint GLTools::CheckProgramStatus(GLuint program, GLenum status, char const * format)
{
  GLint result = 0;
  glGetProgramiv(program, status, &result);
  if (result == GL_FALSE)
  {
    GLint log_len = 0;
    glGetProgramiv(program, GL_INFO_LOG_LENGTH, &log_len);
    if (log_len > 0)
    {
      if (format == nullptr)
        format = "%s";

      GLchar log_buffer[2048];
      glGetProgramInfoLog(result, sizeof(log_buffer) - 1, nullptr, log_buffer);
      LogTools::Error(format, log_buffer);
    }
  }
  return result;
}

void GLTools::DisplayProgramDiagnostic(GLuint program)
{
  GLProgramData::GetData(program).DisplayProgramDiagnostic();
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

    chaos::LogTools::Log("OpenGL warning message   [%s]", msg);
    chaos::LogTools::Log("               severity  [%s]", severity_str);
    chaos::LogTools::Log("               source    [%s]", source_str);
    chaos::LogTools::Log("               type      [%s]", type_str);

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

int GLTools::GetMipmapLevelCount(int width, int height)
{
  return GetMipmapLevelCount(max(width, height));
}

int GLTools::GetMipmapLevelCount(int width)
{
  assert(width > 0);
  return MathTools::bsr(width) + 1;
}

std::pair<GLenum, GLenum> GLTools::GetTextureFormatsFromBPP(int bpp)
{
  GLenum format = GL_NONE;
  if (bpp == 8)
    format = GL_RED; // GL_LUMINANCE / GL_LUMINANCE8 deprecated in OpenGL 4.5
  else if (bpp == 24)
    format = GL_BGR;
  else if (bpp == 32)
    format = GL_BGRA;

  GLenum internal_format = GL_NONE;
  if (bpp == 8)
    internal_format = GL_R8; // GL_LUMINANCE / GL_LUMINANCE8 deprecated in OpenGL 4.5
  else if (bpp == 24)
    internal_format = GL_RGB8;
  else if (bpp == 32)
    internal_format = GL_RGBA8;

  assert(format != GL_NONE);
  assert(internal_format != GL_NONE);

  return std::make_pair(format, internal_format);
}

GenTextureResult GLTools::GenTexture(ImageDescription const & image, GenTextureParameters const & parameters)
{
  assert(image.width > 0);
  assert(image.height > 0);
  assert(image.bpp == 8 || image.bpp == 24 || image.bpp == 32);

  GLenum target = GetTextureTargetFromSize(image.width, image.height, parameters.rectangle_texture);  // compute the format

  GenTextureResult result;
  glCreateTextures(target, 1, &result.texture_id);
  if (result.texture_id > 0)
  {  
    // choose format and internal format (beware FreeImage is BGR/BGRA)
    std::pair<GLenum, GLenum> all_formats = GetTextureFormatsFromBPP(image.bpp);

    GLenum format          = all_formats.first;
    GLenum internal_format = all_formats.second;
   
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
    glPixelStorei(GL_UNPACK_ROW_LENGTH, 8 * image.pitch_size / image.bpp);

    // store the pixels
    if (target == GL_TEXTURE_1D)
    {
      int level_count = GetMipmapLevelCount(image.width);
      glTextureStorage1D(result.texture_id, level_count, internal_format, image.width);
      glTextureSubImage1D(result.texture_id, 0, 0, image.width, format, GL_UNSIGNED_BYTE, image.data);
    }
    else
    {
      int level_count = GetMipmapLevelCount(image.width, image.height);
      glTextureStorage2D(result.texture_id, level_count, internal_format, image.width, image.height);
      glTextureSubImage2D(result.texture_id, 0, 0, 0, image.width, image.height, format, GL_UNSIGNED_BYTE, image.data);
    }

    // apply parameters
    GenTextureApplyParameters(target, result.texture_id, parameters);

    result.texture_description.type            = target;
    result.texture_description.internal_format = internal_format;
    result.texture_description.width           = image.width;
    result.texture_description.height          = image.height;
    result.texture_description.depth           = 1;
  }
  return result;
}

GenTextureResult GLTools::GenTexture(FIBITMAP const * image, GenTextureParameters const & parameters)
{
  assert(image != nullptr);
  return GenTexture(ImageTools::GetImageDescription(image), parameters);
}

GenTextureResult GLTools::GenTexture(char const * filename, GenTextureParameters const & parameters)
{
  GenTextureResult result;

  FIBITMAP * image = ImageTools::LoadImageFromFile(filename);
  if (image != nullptr)
  {
    result = GenTexture(image, parameters);
    FreeImage_Unload(image);  
  }
  return result;
}

GenTextureResult GLTools::GenTexture(int width, std::function<void(ImageDescription const &, unsigned char *)> const & func, GenTextureParameters const & parameters)
{
  GenTextureResult result;

  unsigned char * buffer = new unsigned char[width];
  if (buffer != nullptr)
  {
    ImageDescription desc(buffer, width, 1, 8, 0);
    func(desc, buffer);
    result = GenTexture(desc, parameters);
    delete[](buffer);
  }
  return result;
}

GenTextureResult GLTools::GenTexture(int width, std::function<void(ImageDescription const &, PixelRGB *)> const & func, GenTextureParameters const & parameters)
{
  GenTextureResult result;

  PixelRGB * buffer = new PixelRGB[width];
  if (buffer != nullptr)
  {
    ImageDescription desc(buffer, width, 1, 24, 0);
    func(desc, buffer);
    result = GenTexture(desc, parameters);
    delete[](buffer);
  }
  return result;
}

GenTextureResult GLTools::GenTexture(int width, std::function<void(ImageDescription const &, PixelRGBA *)> const & func, GenTextureParameters const & parameters)
{
  GenTextureResult result;

  PixelRGBA * buffer = new PixelRGBA[width];
  if (buffer != nullptr)
  {
    ImageDescription desc(buffer, width, 1, 32, 0);
    func(desc, buffer);
    result = GenTexture(desc, parameters);
    delete[](buffer);
  }
  return result;
}

GenTextureResult GLTools::GenTexture(int width, int height, std::function<void(ImageDescription const &, unsigned char *)> const & func, GenTextureParameters const & parameters)
{
  GenTextureResult result;

  unsigned char * buffer = new unsigned char[width * height];
  if (buffer != nullptr)
  {
    ImageDescription desc(buffer, width, height, 8, 0);
    func(desc, buffer);
    result = GenTexture(desc, parameters);
    delete[](buffer);
  }
  return result;
}

GenTextureResult GLTools::GenTexture(int width, int height, std::function<void(ImageDescription const &, PixelRGB *)> const & func, GenTextureParameters const & parameters)
{
  GenTextureResult result;

  PixelRGB * buffer = new PixelRGB[width * height];
  if (buffer != nullptr)
  {
    ImageDescription desc(buffer, width, height, 24, 0);
    func(desc, buffer);
    result = GenTexture(desc, parameters);
    delete[](buffer);
  }
  return result;
}

GenTextureResult GLTools::GenTexture(int width, int height, std::function<void(ImageDescription const &, PixelRGBA *)> const & func, GenTextureParameters const & parameters)
{
  GenTextureResult result;

  PixelRGBA * buffer = new PixelRGBA[width * height];
  if (buffer != nullptr)
  {
    ImageDescription desc(buffer, width, height, 32, 0);
    func(desc, buffer);
    result = GenTexture(desc, parameters);
    delete[](buffer);
  }
  return result;
}




// There are lots of very uncleared referenced for faces orientation
// Most of pictures found one GoogleImage do not correspond to OpenGL but DirectX
// There are differences between OpenGL & DirectX implementation
//
// I found this one that seems to work almost fine with OpenGL 
//  - horizontal skybox OK
//  - vertical   skybox OK
//  - multiple   skybox OK
//
// Problems : the face junctions are good, but the skybox is inverted (top to down).
//            the simplest solution is to access in shader the cube map with "-direction" instead of "direction" :
//
//                vec4 color = texture(cube_texture, -direction)
//
//            => in fact inverting the Y should be enougth
//
//                vec4 color = texture(cube_texture, direction * vec3(1.0,   -1.0,    1.0))
//
// http://www.3dcpptutorials.sk/index.php?id=24
//
//        +------+
//        |  -Y  |
//        |      |
// +------+------+------+------+
// |  -X  |  +Z  |  +X  |  -Z  |
// |      |      |      |      |
// +------+------+------+------+
//        |  +Y  |
//        |      |
//        +------+
//
//  0 = +X = right
//  1 = -X = left
//  2 = +Y = bottom
//  3 = -Y = top
//  4 = +Z = front
//  5 = -Z = back
//
// Differences between comes from the fact that OpenGL & Direct have different axis
//   +Y / -Y   are to be swapped (from one implementation to the other)
//   +Z / -Z   are to be swapped 
//
// Textures for OpenGL are oriented :
//
//  ^
//  |
//  |
//  +------>
// 0, 0
//
// Textures for DirectX are oriented :
//
// 0, 0
//  +------>
//  |
//  |
//  v
//

int GLTools::GetLayerValueFromCubeMapFace(GLenum face, int level)
{
  if (face == GL_TEXTURE_CUBE_MAP_POSITIVE_X)
    return 0 + 6 * level;
  if (face == GL_TEXTURE_CUBE_MAP_NEGATIVE_X)
    return 1 + 6 * level;
  if (face == GL_TEXTURE_CUBE_MAP_POSITIVE_Y)
    return 2 + 6 * level;
  if (face == GL_TEXTURE_CUBE_MAP_NEGATIVE_Y)
    return 3 + 6 * level;
  if (face == GL_TEXTURE_CUBE_MAP_POSITIVE_Z)
    return 4 + 6 * level;
  if (face == GL_TEXTURE_CUBE_MAP_NEGATIVE_Z)
    return 5 + 6 * level;
  return -1;
}

GenTextureResult GLTools::GenTexture(SkyBoxImages const * skybox, GenTextureParameters const & parameters)
{
  assert(skybox != nullptr);
  assert(!skybox->IsEmpty());

  GLenum target = GL_TEXTURE_CUBE_MAP;

  GenTextureResult result;
  glCreateTextures(target, 1, &result.texture_id);
  if (result.texture_id > 0)
  {
    int bpp  = skybox->GetSkyBoxBPP();
    int size = skybox->GetSkyBoxSize();

    std::pair<GLenum, GLenum> all_formats = GetTextureFormatsFromBPP(bpp);

    GLenum format          = all_formats.first;
    GLenum internal_format = all_formats.second;

    GLenum targets[] = {
      GL_TEXTURE_CUBE_MAP_NEGATIVE_X, // LEFT
      GL_TEXTURE_CUBE_MAP_POSITIVE_X, // RIGHT      
      GL_TEXTURE_CUBE_MAP_NEGATIVE_Y, // BOTTOM
      GL_TEXTURE_CUBE_MAP_POSITIVE_Y, // TOP
      GL_TEXTURE_CUBE_MAP_POSITIVE_Z, // FRONT
      GL_TEXTURE_CUBE_MAP_NEGATIVE_Z  // BACK
    };

    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

    int level_count = GetMipmapLevelCount(size, size);
    glTextureStorage2D(result.texture_id, level_count, internal_format, size, size);
    
    for (int i = SkyBoxImages::IMAGE_LEFT ; i <= SkyBoxImages::IMAGE_BACK ; ++i)
    {
      ImageDescription image = skybox->GetImageFaceDescription(i);

      void const * data = image.data;
      GLint        unpack_row_length = image.pitch_size * 8 / image.bpp;
      char       * new_buffer = nullptr;

      if (skybox->IsSingleImage()) // in single image, there may be some inversion to correct with a temporary buffer
      {
        glm::ivec3 position_and_flags = skybox->GetPositionAndFlags(i);
        if (position_and_flags.z == SkyBoxImages::IMAGE_CENTRAL_SYMETRY)
        {
          new_buffer = new char[image.width * image.height * image.bpp >> 3];
          if (new_buffer != nullptr)
          {
            ImageDescription new_image = image;

            new_image.data         = new_buffer;
            new_image.pitch_size   = new_image.line_size;
            new_image.padding_size = 0;

            ImageTools::CopyPixelsWithCentralSymetry(image, new_image, 0, 0, 0, 0, image.width, image.height); // do the symmetry
            unpack_row_length = 0;
            data              = new_buffer;
          }
        }
      }
     
      glPixelStorei(GL_UNPACK_ROW_LENGTH, unpack_row_length); // do not remove this line from the loop. Maybe future implementation will accept                                                                             // image with same size but different pitch

      int depth = GetLayerValueFromCubeMapFace(targets[i], 0);
      
      glTextureSubImage3D(result.texture_id, 0, 0, 0, depth, image.width, image.height, 1, format, GL_UNSIGNED_BYTE, image.data);

      if (new_buffer != nullptr)
        delete [](new_buffer);
    }

    // this is smoother to clamp at edges
    GenTextureParameters tmp = parameters;
    tmp.wrap_s = GL_CLAMP_TO_EDGE;
    tmp.wrap_r = GL_CLAMP_TO_EDGE;
    tmp.wrap_t = GL_CLAMP_TO_EDGE;
    GenTextureApplyParameters(target, result.texture_id, tmp);

    result.texture_description.type            = target;
    result.texture_description.internal_format = internal_format;
    result.texture_description.width           = size;
    result.texture_description.height          = size;
    result.texture_description.depth           = 1;
  }
  return result;
}

boost::intrusive_ptr<Texture> GLTools::GenTextureObject(ImageDescription const & image, GenTextureParameters const & parameters)
{
  GenTextureResult result = GenTexture(image, parameters);
  if (result.texture_id > 0)
    return new Texture(result.texture_id, result.texture_description);
  return nullptr;
}

boost::intrusive_ptr<Texture> GLTools::GenTextureObject(FIBITMAP const * image, GenTextureParameters const & parameters)
{
  GenTextureResult result = GenTexture(image, parameters);
  if (result.texture_id > 0)
    return new Texture(result.texture_id, result.texture_description);
  return nullptr;
}

boost::intrusive_ptr<Texture> GLTools::GenTextureObject(SkyBoxImages const * skybox, GenTextureParameters const & parameters)
{
  GenTextureResult result = GenTexture(skybox, parameters);
  if (result.texture_id > 0)
    return new Texture(result.texture_id, result.texture_description);
  return nullptr;
}

boost::intrusive_ptr<Texture> GLTools::GenTextureObject(char const * filename, GenTextureParameters const & parameters)
{
  GenTextureResult result = GenTexture(filename, parameters);
  if (result.texture_id > 0)
    return new Texture(result.texture_id, result.texture_description);
  return nullptr;
}

boost::intrusive_ptr<Texture> GLTools::GenTextureObject(int width, std::function<void(ImageDescription const &, unsigned char *)> const & func, GenTextureParameters const & parameters)
{
  GenTextureResult result = GenTexture(width, func, parameters);
  if (result.texture_id > 0)
    return new Texture(result.texture_id, result.texture_description);
  return nullptr;
}

boost::intrusive_ptr<Texture> GLTools::GenTextureObject(int width, std::function<void(ImageDescription const &, PixelRGB *)> const & func, GenTextureParameters const & parameters)
{
  GenTextureResult result = GenTexture(width, func, parameters);
  if (result.texture_id > 0)
    return new Texture(result.texture_id, result.texture_description);
  return nullptr;
}

boost::intrusive_ptr<Texture> GLTools::GenTextureObject(int width, std::function<void(ImageDescription const &, PixelRGBA *)> const & func, GenTextureParameters const & parameters)
{
  GenTextureResult result = GenTexture(width, func, parameters);
  if (result.texture_id > 0)
    return new Texture(result.texture_id, result.texture_description);
  return nullptr;
}

boost::intrusive_ptr<Texture> GLTools::GenTextureObject(int width, int height, std::function<void(ImageDescription const &, unsigned char *)> const & func, GenTextureParameters const & parameters)
{
  GenTextureResult result = GenTexture(width, height, func, parameters);
  if (result.texture_id > 0)
    return new Texture(result.texture_id, result.texture_description);
  return nullptr;
}

boost::intrusive_ptr<Texture> GLTools::GenTextureObject(int width, int height, std::function<void(ImageDescription const &, PixelRGB *)> const & func, GenTextureParameters const & parameters)
{
  GenTextureResult result = GenTexture(width, height, func, parameters);
  if (result.texture_id > 0)
    return new Texture(result.texture_id, result.texture_description);
  return nullptr;
}

boost::intrusive_ptr<Texture> GLTools::GenTextureObject(int width, int height, std::function<void(ImageDescription const &, PixelRGBA *)> const & func, GenTextureParameters const & parameters)
{
  GenTextureResult result = GenTexture(width, height, func, parameters);
  if (result.texture_id > 0)
    return new Texture(result.texture_id, result.texture_description);
  return nullptr;
}

void GLTools::GenTextureApplyParameters(GLenum target, GLint texture_id, GenTextureParameters const & parameters)
{
  // there are to set of functions
  //   - glTexParameteri(TARGET ...)
  // and
  //   - glTextureParameteri(TEXTURE_ID ...)
  glTextureParameteri(texture_id, GL_TEXTURE_WRAP_S, parameters.wrap_s); 
  glTextureParameteri(texture_id, GL_TEXTURE_WRAP_T, parameters.wrap_t);
  glTextureParameteri(texture_id, GL_TEXTURE_WRAP_R, parameters.wrap_r);
  glTextureParameteri(texture_id, GL_TEXTURE_MAG_FILTER, parameters.mag_filter);
  glTextureParameteri(texture_id, GL_TEXTURE_MIN_FILTER, parameters.min_filter);        

  if (parameters.build_mipmaps)
    if (target != GL_TEXTURE_RECTANGLE) // not working with RECTANGLE (crash)
      glGenerateTextureMipmap(texture_id);
}

GLenum GLTools::GetTextureTargetFromSize(int width, int height, bool rectangle_texture)
{
  if (width == height) // and power of 2 ?
    return GL_TEXTURE_2D;
  else if (height == 1)
    return GL_TEXTURE_1D;
  else 
    return (rectangle_texture)? GL_TEXTURE_RECTANGLE : GL_TEXTURE_2D;
}

// GL_TEXTURE_1D, GL_TEXTURE_2D, GL_TEXTURE_3D, GL_TEXTURE_1D_ARRAY, GL_TEXTURE_2D_ARRAY, GL_TEXTURE_CUBE_MAP, or GL_TEXTURE_CUBE_MAP_ARRAY


}; // namespace chaos
