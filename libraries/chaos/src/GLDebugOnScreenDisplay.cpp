#include <chaos/GLDebugOnScreenDisplay.h>
#include <chaos/GeometryFramework.h>
#include <chaos/GLProgramLoader.h>
#include <chaos/BitmapFontTextMeshBuilder.h>
#include <chaos/ImageTools.h>
#include <chaos/GLTools.h>
#include <chaos/GLTextureTools.h>


namespace chaos
{
	// should try string literal from C++ 11
	char const * GLDebugOnScreenDisplay::vertex_shader_source = R"SHADERCODE(
    layout (location = 0) in vec2 position;
    layout (location = 1) in vec2 texcoord;
    uniform vec2 position_factor;
    out vec2 tex_coord;
    void main()
    {
		tex_coord   = texcoord;
		vec2 pos    = (position * position_factor) + vec2(-1.0, 1.0);
		gl_Position = vec4(pos, 0.0, 1.0);
    }
	)SHADERCODE";
	// should try string literal from C++ 11
	char const * GLDebugOnScreenDisplay::pixel_shader_source = R"SHADERCODE(
    out vec4 output_color;
    in vec2 tex_coord;
    uniform sampler2D material;
    void main()
    {
		vec4 color = texture(material, tex_coord);
		float alpha = 1.0;
		if ((color.r + color.b + color.a) > 2.0)
			discard;
		output_color = vec4(1.0, 1.0, 1.0, alpha);
    }
	)SHADERCODE";

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
    glUseProgram(program->GetResourceID());

    // Initialize the vertex array
    glBindVertexArray(vertex_array->GetResourceID()); 

    GLProgramData const & program_data = program->GetProgramData();
    
    program_data.BindAttributes(vertex_array->GetResourceID(), declaration, nullptr);

    float character_width  = (float)mesh_builder_params.character_width;
    float character_height = (float)mesh_builder_params.character_width;

    float factor_x = (character_width  * 2.0f / ((float)width));   // screen coordinates are [-1, +1]
    float factor_y = (character_height * 2.0f / ((float)height));  // for [width, height] pixels
                                                                                              // each characters is 1.0f unit large (+0.1f for padding)                                                                                                                                                                                                         
                                                                                              // see BitmapFontTextMeshBuilder
    GLUniformInfo const * position_factor = program_data.FindUniform("position_factor");
    if (position_factor != nullptr)
      glUniform2f(position_factor->location, factor_x, factor_y);   

    // Texture
    glBindTextureUnit(0, texture->GetResourceID());

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
    int cpl = mesh_builder_params.font_characters_per_line;

    int csw = mesh_builder_params.character_width;
    int csh = mesh_builder_params.character_width;
    int sx  = mesh_builder_params.spacing.x;
    int sy  = mesh_builder_params.spacing.y;

    int tw = texture->GetTextureDescription().width;
    int th = texture->GetTextureDescription().height;
    int cx = mesh_builder_params.crop_texture.x;
    int cy = mesh_builder_params.crop_texture.y;

    BitmapFontTextMeshBuilder::Params params;
    params.font_characters            = mesh_builder_params.font_characters.c_str();
    params.font_characters_per_line   = cpl;
    params.font_characters_line_count = mesh_builder_params.font_characters_line_count;
    params.line_limit                 = max(width / (csw + sx), 1);
    params.character_size             = glm::vec2(1.0f, 1.0f);    
    params.spacing                    = glm::vec2( ((float)sx) / ((float)csw), ((float)sy) / ((float)csh));
    params.crop_texture               = glm::vec2( ((float)cx) / ((float)tw),  ((float)cy) / ((float)th));

    // generate vertex buffer data
    std::vector<float> vertices;

    BitmapFontTextMeshBuilder builder;
    for (auto const & l : lines)
    {
      if (l.first.size() == 0)
        continue;
      box2 bounding = builder.BuildBuffer(l.first.c_str(), params, vertices);
      if (!bounding.IsEmpty())
        params.position = bounding.GetCorners().first; // maybe some degenerated case 
    }

    // fill GPU buffer
    size_t count = vertices.size();
    glNamedBufferData(vertex_buffer->GetResourceID(), count * sizeof(float), &vertices[0], GL_STATIC_DRAW);
    draw_count = count / 4; // 4 float per vertex

    GLuint binding_index = 0;
    glVertexArrayVertexBuffer(vertex_array->GetResourceID(), binding_index, vertex_buffer->GetResourceID(), 0, declaration.GetVertexSize());
    
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
    FIBITMAP * image = ImageTools::LoadImageFromFile(params.texture_path.string().c_str());
    if (image == nullptr)
      return false;

    // create texture
    texture = GLTextureTools::GenTextureObject(image);
    FreeImage_Unload(image);
    if (texture == nullptr)
      return false;

    // create GPU-Program
    GLProgramLoader loader;
    loader.AddShaderSource(GL_VERTEX_SHADER,   vertex_shader_source);
    loader.AddShaderSource(GL_FRAGMENT_SHADER, pixel_shader_source);

    program = loader.GenerateProgramObject();
    if (program == nullptr)
      return false;

    // prepare the vertex declaration
    declaration.Push(SEMANTIC_POSITION, 0, TYPE_FLOAT2);
    declaration.Push(SEMANTIC_TEXCOORD, 0, TYPE_FLOAT2);

    // Generate Vertex Array and Buffer
    if (!GLTools::GenerateVertexAndIndexBuffersObject(&vertex_array, &vertex_buffer, nullptr))
      return false;

    // keep a copy of initialization params
    mesh_builder_params = params;

    return true;
  }

  void GLDebugOnScreenDisplay::Finalize()
  {
    program       = nullptr;
    texture       = nullptr;
    vertex_array  = nullptr;
    vertex_buffer = nullptr;

    declaration.Clear();
  }

  void GLDebugOnScreenDisplay::Clear()
  {
    rebuild_required |= (lines.size() > 0);
    lines.clear();
  }

}; // namespace chaos
