#include <chaos/GLDebugOnScreenDisplay.h>
#include <chaos/GeometryFramework.h>
#include <chaos/GLProgramLoader.h>
#include <chaos/BitmapFontTextMeshBuilder.h>
#include <chaos/ImageTools.h>
#include <chaos/GLTools.h>


namespace chaos
{
  // should try string literal from C++ 11
  char const * GLDebugOnScreenDisplay::vertex_shader_source = 
    "layout (location = 0) in vec2 position; \n\
    layout (location = 1) in vec2 texcoord; \n\
    uniform vec2 position_factor; \n\
    out vec2 tex_coord; \n\
    void main() \n\
    { \n\
    tex_coord = texcoord; \n\
    vec2 pos = (position * position_factor) + vec2(-1.0, 1.0); \n\
    gl_Position = vec4(pos, 0.0, 1.0); \n\
    }";
  // should try string literal from C++ 11
  char const * GLDebugOnScreenDisplay::pixel_shader_source = 
    "out vec4 output_color; \n\
    in vec2 tex_coord; \n\
    uniform sampler2D material; \n\
    void main() \n\
    { \n\
    vec4 color = texture(material, tex_coord); \n\
    float alpha = 1.0; \n\
    if ((color.r + color.b + color.a) > 2.0) \n\
    discard; \n\
    output_color = vec4(1.0, 1.0, 1.0, alpha); \n\
    }";

  GLDebugOnScreenDisplay::GLDebugOnScreenDisplay():
    rebuild_required(true),
    screen_width(-1),
    program(0),
    texture_id(0),
    vertex_array(0),
    vertex_buffer(0)
  {

  }

  bool GLDebugOnScreenDisplay::Tick(double delta_time)
  {
    int count = (int)lines.size();
    for (int i = count - 1 ; i >= 0 ; --i)
    {
      auto & l = lines[i];
      if (l.second >= 0.0f) // if lines is timed
      {
        l.second -= (float)delta_time; // decrement time
        if (l.second <= 0)     
          lines.erase(lines.begin() + i); // erase the line at this position if it is timed out
      }
    }

    rebuild_required |= (count != lines.size()); // has number of line changed ?

    return rebuild_required;
  }

  void GLDebugOnScreenDisplay::Display(int width, int height) const
  {
    if (lines.size() == 0) // do not draw nor rebuild buffer if there are no lines (maybe GPU buffer could be cleaned)
      return;

    if (rebuild_required || screen_width != width) // if screen width changes, buffer has to be reconstructed
    {
      BuildVertexBuffer(width, height);
      rebuild_required = false;
      screen_width     = width;
    }

    // context states
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glDisable(GL_DEPTH_TEST);

    // GPU-Program
    glUseProgram(program);

    // Initialize the vertex arra
    glBindVertexArray(vertex_array); 
    
    program_data.BindAttributes(vertex_array, declaration, nullptr);

    float factor_x = ((float)mesh_builder_params.character_size) * 2.0f / ((float)width);   // screen coordinates are [-1, +1]
    float factor_y = ((float)mesh_builder_params.character_size) * 2.0f / ((float)height);  // for [width, height] pixels
                                                                                            // each characters is 1.0f unit large (+0.1f for padding)                                                                                                                                                                                                         
                                                                                            // see BitmapFontTextMeshBuilder
    GLUniformInfo const * position_factor = program_data.FindUniform("position_factor");
    if (position_factor != nullptr)
      glUniform2f(position_factor->location, factor_x, factor_y);   

    // Texture
    glBindTextureUnit(0, texture_id);

    GLUniformInfo const * material = program_data.FindUniform("material");
    if (material != nullptr)
      glUniform1i(material->location, 0);   

    // The drawing   
    glDrawArrays(GL_TRIANGLES, 0, (GLsizei)draw_count);

    // restore states
    glDisable(GL_BLEND);
    glEnable(GL_DEPTH_TEST);
  }

  bool GLDebugOnScreenDisplay::BuildVertexBuffer(int width, int height) const
  {   
    chaos::BitmapFontTextMeshBuilder::Params params;
    params.font_characters          = mesh_builder_params.font_characters.c_str();
    params.font_characters_per_line = mesh_builder_params.font_characters_per_line;
    params.line_limit               = max(width / mesh_builder_params.character_size, 1);
    params.character_size           = glm::vec2(1.0f, 1.0f);
    params.spacing                  = glm::vec2(0.1f, 0.1f);

    // generate vertex buffer data
    std::vector<float> vertices;

    chaos::BitmapFontTextMeshBuilder builder;
    for (auto const & l : lines)
    {
      chaos::box2 bounding = builder.BuildBuffer(l.first.c_str(), params, vertices);
      params.position = bounding.GetCorners().first;
    }

    // fill GPU buffer
    size_t count = vertices.size();
    glNamedBufferData(vertex_buffer, count * sizeof(float), &vertices[0], GL_STATIC_DRAW);
    draw_count = count / 4; // 4 float per vertex

    GLuint binding_index = 0;
    glVertexArrayVertexBuffer(vertex_array, binding_index, vertex_buffer, 0, declaration.GetVertexSize());
    
    return true;
  }

  void GLDebugOnScreenDisplay::AddLine(char const * line, float duration)
  {
    assert(line != nullptr);
    lines.push_back(std::make_pair(line, duration));
    rebuild_required = true;
  }

  bool GLDebugOnScreenDisplay::Initialize(GLDebugOnScreenDisplay::Params const & params)
  {
    if (DoInitialize(params))
      return true;
    Finalize();
    return false;
  }

  bool GLDebugOnScreenDisplay::DoInitialize(GLDebugOnScreenDisplay::Params const & params)
  {
    // load image
    FIBITMAP * image = chaos::ImageTools::LoadImageFromFile(params.texture_path.string().c_str());
    if (image == nullptr)
      return false;

    // create texture
    texture_id = chaos::GLTools::GenTexture(image).texture_id;
    FreeImage_Unload(image);
    if (texture_id == 0)
      return false;

    // create GPU-Program
    chaos::GLProgramLoader loader;
    loader.AddShaderSource(GL_VERTEX_SHADER,   vertex_shader_source);
    loader.AddShaderSource(GL_FRAGMENT_SHADER, pixel_shader_source);

    program = loader.GenerateProgram();
    if (program == 0)
      return false;

    program_data = GLProgramData::GetData(program);

    // prepare the vertex declaration
    declaration.Push(chaos::SEMANTIC_POSITION, 0, chaos::TYPE_FLOAT2);
    declaration.Push(chaos::SEMANTIC_TEXCOORD, 0, chaos::TYPE_FLOAT2);

    // Generate Vertex Array and Buffer
    if (!GLTools::GenerateVertexAndIndexBuffers(&vertex_array, &vertex_buffer, nullptr))
      return false;

    // keep a copy of initialization params
    mesh_builder_params = params;

    return true;
  }

  void GLDebugOnScreenDisplay::Finalize()
  {
    if (program != 0)
    {
      glDeleteProgram(program);
      program = 0;
    }

    GLTools::FreeVertexAndIndexBuffers(&vertex_array, &vertex_buffer, nullptr);

    if (texture_id != 0)
    {
      glDeleteTextures(1, &texture_id);
      texture_id = 0;
    }

    declaration.Clear();
  }

  void GLDebugOnScreenDisplay::Clear()
  {
    rebuild_required |= (lines.size() > 0);
    lines.clear();
  }

}; // namespace chaos
