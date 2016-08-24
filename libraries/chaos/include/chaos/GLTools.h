#pragma once

#include <chaos/StandardHeaders.h>
#include <chaos/ImageTools.h>
#include <chaos/SkyBoxTools.h>
#include <chaos/Buffer.h>
#include <chaos/VertexDeclaration.h>
#include <chaos/GLProgram.h>
#include <chaos/Texture.h>
#include <chaos/VertexArray.h>
#include <chaos/VertexBuffer.h>
#include <chaos/IndexBuffer.h>

namespace chaos
{

  /**
  * GenTextureResult : result for gen texture methods
  **/

class GenTextureResult
{
public:

  /** the id of the texture */
  GLuint             texture_id;
  /** the GL handler for the texture */
  TextureDescription texture_description;
};

/**
 * GenTextureParameters : some parameters to load textures
 **/

class GenTextureParameters
{
public:

  /** constructor */
  GenTextureParameters():
    wrap_s(GL_REPEAT),
    wrap_t(GL_REPEAT),
    wrap_r(GL_REPEAT),
    mag_filter(GL_LINEAR),
    min_filter(GL_LINEAR_MIPMAP_LINEAR),
    level(0),
    border(0),
    build_mipmaps(true),
    rectangle_texture(false){}
  
  /** parameters for glTexParameteri(...) */
  GLenum wrap_s;
  /** parameters for glTexParameteri(...) */
  GLenum wrap_t;
  /** parameters for glTexParameteri(...) */
  GLenum wrap_r;
  /** parameters for glTexParameteri(...) */
  GLenum mag_filter;
  /** parameters for glTexParameteri(...) */
  GLenum min_filter;
  /** parameters for glTexImageXX(...) */
  GLint  level;
  /** parameters for glTexImageXX(...) */
  GLint  border;
  /** enable build mipmaps */
  bool build_mipmaps;
  /** enable the texture to be used has rectangular instead of GL_TEXTURE_1D or GL_TEXTURE_2D */
  bool rectangle_texture;
};


/**
 * GLTools : used to have some generic functions for OpenGL
 **/

class GLTools
{
public:

  /** display generic information on the OpenGL driver */
  static void DisplayGenericInformation();
  /** a utility function to display some diagnostic */
  static void DisplayProgramDiagnostic(GLuint program);
  /** a utility function to check some status for a program and display error logs */
  static GLint CheckProgramStatus(GLuint program, GLenum status, char const * format);
  /** transform a type enumeration into a string */
  static char const * GLenumToString(GLenum type);
  /** Initialize the Debug message handler */
  static void SetDebugMessageHandler();

  /** Generate a 1D/2D/rectangle texture from an file */
  static GenTextureResult GenTexture(char const * filename, GenTextureParameters const & parameters = GenTextureParameters());
  /** Generate a 1D/2D/rectangle texture from an image */
  static GenTextureResult GenTexture(ImageDescription const & image, GenTextureParameters const & parameters = GenTextureParameters());
  /** Generate a 1D/2D/rectangle texture from an image */
  static GenTextureResult GenTexture(FIBITMAP const * image, GenTextureParameters const & parameters = GenTextureParameters());
  /** Generate a cube texture from a skybox */
  static GenTextureResult GenTexture(SkyBoxImages const * skybox, GenTextureParameters const & parameters = GenTextureParameters());


  /** Generate a 1D/2D/rectangle texture from an file */
  static boost::intrusive_ptr<Texture> GenTextureObject(char const * filename, GenTextureParameters const & parameters = GenTextureParameters());
  /** Generate a 1D/2D/rectangle texture from an image */
  static boost::intrusive_ptr<Texture> GenTextureObject(ImageDescription const & image, GenTextureParameters const & parameters = GenTextureParameters());
  /** Generate a 1D/2D/rectangle texture from an image */
  static boost::intrusive_ptr<Texture> GenTextureObject(FIBITMAP const * image, GenTextureParameters const & parameters = GenTextureParameters());
  /** Generate a cube texture from a skybox */
  static boost::intrusive_ptr<Texture> GenTextureObject(SkyBoxImages const * skybox, GenTextureParameters const & parameters = GenTextureParameters());

  /** Get Format/Internal Format pair from BPP */
  static std::pair<GLenum, GLenum> GetTextureFormatsFromBPP(int bpp);
  /** map an index and vertex buffer at the same time, all mapped or none mapped */
  static bool MapBuffers(GLuint vertex_buffer, GLuint index_buffer, size_t vb_size, size_t ib_size, std::pair<char*, GLuint*> & result);
  /** generate multiple arrays/buffer with a single call */
  static bool GenerateVertexAndIndexBuffers(GLuint * vertex_array, GLuint * vertex_buffer, GLuint * index_buffer);
  /** generate multiple arrays/buffer with a single call */
  static bool GenerateVertexAndIndexBuffersObject(boost::intrusive_ptr<VertexArray> * vertex_array, boost::intrusive_ptr<VertexBuffer> * vertex_buffer, boost::intrusive_ptr<IndexBuffer> * index_buffer);
  /** free multiple arrays/buffer with a single call */
  static void FreeVertexAndIndexBuffers(GLuint * vertex_array, GLuint * vertex_buffer, GLuint * index_buffer);

  /** utility function to compute target (GL_TEXTURE_1D, GL_TEXTURE_2D, GL_TEXTURE_RECTANGLE) from dimension */
  static GLenum GetTextureTargetFromSize(int width, int height, bool rectangle_texture);

  /** transform a program into a binary chunk (the binary_format is encoded in the buffer) */
  static Buffer<char> GetProgramBinary(GLuint program);
  /** create a program from its binary format */
  static GLuint GetProgramFromBinary(Buffer<char> const & buffer);
  /** create a program from its binary format */
  static boost::intrusive_ptr<GLProgram> GetProgramObjectFromBinary(Buffer<char> const & buffer);

  /** an utility function to 'compute' offset in vertex buffer (simple cast in facts) */
  static inline void * GetOffset(size_t s)
  {
    return ((char *)nullptr) + s;
  }

  /** settings uniforms */
  static void SetUniform(GLint location, glm::mat4  const & value){ glUniformMatrix4fv(location, 1, false, (GLfloat const *)&value);}
  static void SetUniform(GLint location, glm::dmat4 const & value){ glUniformMatrix4dv(location, 1, false, (GLdouble const *)&value);}

  static void SetUniform(GLint location, glm::tvec2<GLfloat> const & value){ glUniform2fv(location, 1, (GLfloat const *)&value);}
  static void SetUniform(GLint location, glm::tvec3<GLfloat> const & value){ glUniform3fv(location, 1, (GLfloat const *)&value);}
  static void SetUniform(GLint location, glm::tvec4<GLfloat> const & value){ glUniform4fv(location, 1, (GLfloat const *)&value);}

  static void SetUniform(GLint location, glm::tvec2<GLdouble> const & value){ glUniform2dv(location, 1, (GLdouble const *)&value);}
  static void SetUniform(GLint location, glm::tvec3<GLdouble> const & value){ glUniform3dv(location, 1, (GLdouble const *)&value);}
  static void SetUniform(GLint location, glm::tvec4<GLdouble> const & value){ glUniform4dv(location, 1, (GLdouble const *)&value);}

  static void SetUniform(GLint location, glm::tvec2<GLint> const & value){ glUniform2iv(location, 1, (GLint const *)&value);}
  static void SetUniform(GLint location, glm::tvec3<GLint> const & value){ glUniform3iv(location, 1, (GLint const *)&value);}
  static void SetUniform(GLint location, glm::tvec4<GLint> const & value){ glUniform4iv(location, 1, (GLint const *)&value);}

  static void SetUniform(GLint location, glm::tvec2<GLuint> const & value){ glUniform2uiv(location, 1, (GLuint const *)&value);}
  static void SetUniform(GLint location, glm::tvec3<GLuint> const & value){ glUniform3uiv(location, 1, (GLuint const *)&value);}
  static void SetUniform(GLint location, glm::tvec4<GLuint> const & value){ glUniform4uiv(location, 1, (GLuint const *)&value);}

  static void SetUniform(GLint location, GLfloat  value){ glUniform1f (location, value);}
  static void SetUniform(GLint location, GLdouble value){ glUniform1d (location, value);}
  static void SetUniform(GLint location, GLint    value){ glUniform1i (location, value);}
  static void SetUniform(GLint location, GLuint   value){ glUniform1ui(location, value);}

  /** settings default attribute values */
  static void SetVertexAttrib(GLint location, GLfloat value){ glVertexAttrib1f(location, value);}
  static void SetVertexAttrib(GLint location, glm::tvec2<GLfloat> const & value){ glVertexAttrib2fv(location, (GLfloat const *)&value);}
  static void SetVertexAttrib(GLint location, glm::tvec3<GLfloat> const & value){ glVertexAttrib3fv(location, (GLfloat const *)&value);}
  static void SetVertexAttrib(GLint location, glm::tvec4<GLfloat> const & value){ glVertexAttrib4fv(location, (GLfloat const *)&value);}

  static void SetVertexAttrib(GLint location, GLdouble value){ glVertexAttrib1d(location, value);}
  static void SetVertexAttrib(GLint location, glm::tvec2<GLdouble> const & value){ glVertexAttrib2dv(location, (GLdouble const *)&value);}
  static void SetVertexAttrib(GLint location, glm::tvec3<GLdouble> const & value){ glVertexAttrib3dv(location, (GLdouble const *)&value);}
  static void SetVertexAttrib(GLint location, glm::tvec4<GLdouble> const & value){ glVertexAttrib4dv(location, (GLdouble const *)&value);}

  static void SetVertexAttrib(GLint location, GLshort value){ glVertexAttrib1s(location, value);}
  static void SetVertexAttrib(GLint location, glm::tvec2<GLshort> const & value){ glVertexAttrib2sv(location, (GLshort const *)&value);}
  static void SetVertexAttrib(GLint location, glm::tvec3<GLshort> const & value){ glVertexAttrib3sv(location, (GLshort const *)&value);}
  static void SetVertexAttrib(GLint location, glm::tvec4<GLshort> const & value){ glVertexAttrib4sv(location, (GLshort const *)&value);}

  // in OpenGL, for some types (GLint, GLbyte, GLshort, GLushort), there are only definition for vec4 */
  static void SetVertexAttrib(GLint location, glm::tvec4<GLint>    const & value){ glVertexAttrib4iv (location, (GLint const *)&value);}
  static void SetVertexAttrib(GLint location, glm::tvec4<GLbyte>   const & value){ glVertexAttrib4bv (location, (GLbyte const *)&value);}
  static void SetVertexAttrib(GLint location, glm::tvec4<GLubyte>  const & value){ glVertexAttrib4ubv(location, (GLubyte const *)&value);}
  static void SetVertexAttrib(GLint location, glm::tvec4<GLushort> const & value){ glVertexAttrib4usv(location, (GLushort const *)&value);}

protected:

  /** utility function for texture loading */
  static void GenTextureApplyParameters(GLenum target, GLint texture_id, GenTextureParameters const & parameters);
  /** an handler for debug messages */
  static void WINAPI DebugMessageHandler(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, const GLchar * msg, const void * user_data);
};

}; // namespace chaos
