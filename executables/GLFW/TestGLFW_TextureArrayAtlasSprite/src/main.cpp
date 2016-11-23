#include <chaos/StandardHeaders.h> 
#include <chaos/FileTools.h> 
#include <chaos/LogTools.h> 
#include <chaos/GLTools.h> 
#include <chaos/StringTools.h> 
#include <chaos/MyGLFWGamepadManager.h> 
#include <chaos/MyGLFWWindow.h> 
#include <chaos/WinTools.h> 
#include <chaos/GLProgramLoader.h>
#include <chaos/Application.h>
#include <chaos/SimpleMeshGenerator.h>
#include <chaos/SkyBoxTools.h>
#include <chaos/GLDebugOnScreenDisplay.h>
#include <chaos/MyGLFWFpsCamera.h>
#include <chaos/SimpleMesh.h>
#include <chaos/MultiMeshGenerator.h>
#include <chaos/GLProgramData.h>
#include <chaos/GLProgram.h>
#include <chaos/VertexDeclaration.h>
#include <chaos/GLTextureTools.h>
#include <chaos/TextureArrayAtlas.h>
#include <chaos/MathTools.h>

// --------------------------------------------------------------------

class TextParseElement
{
public:

  static const int CHANGE_COLOR         = 0;
  static const int CHANGE_CHARACTER_SET = 1;

  /** the type of the element in the stack */
  int type{CHANGE_COLOR};
  /** the color to use */
  glm::vec3 color;
  /** the character set selected */
  chaos::BitmapAtlas::CharacterSet * character_set{nullptr};
};

class ParseTextParams
{
public:

  /** the size to use for the characters */
  float character_height{32.0f};
  /** the text limits */
  float max_text_width{0.0f};
  /** the text limits */
  float max_text_height{0.0f};
  /** the color to use for the parsing */
  glm::vec3 default_color{1.0f, 1.0f, 1.0f};
  /** tab size */
  int tab_size{2};
};

class TextParserData
{



  /** the stack used for parsing */
  std::vector<TextParseElement> parse_stack;
};

class TextParser
{
public:

  /** default constructor */
  TextParser() = default;
  /** constructor with atlas initialization */
  TextParser(chaos::BitmapAtlas::Atlas const & in_atlas) : atlas(&in_atlas) {}

  /** add a named color in the parser */
  bool AddColor(char const * name, glm::vec3 const & color);
  /** add a named bitmap in the parser */
  bool AddBitmap(char const * name, char const * bitmap_set_name, char const * bitmap_name);
  /** add a named bitmap in the parser */
  bool AddBitmap(char const * name, chaos::BitmapAtlas::BitmapEntry const * entry);

  /** add a named character set in the parser */
  bool AddCharacterSet(char const * name, char const * font_name);
  /** add a named character set in the parser */
  bool AddCharacterSet(char const * name, chaos::BitmapAtlas::CharacterSet const * character_set);

  /** the method to parse a text */
  void ParseText(char const * text, ParseTextParams const & params = ParseTextParams());

protected:

  /** utility method to emit characters */
  void EmitCharacters(char const c, int count, TextParserData & data);

public:

  /** the colors to use, indexed by a joker name */
  std::map<std::string, glm::vec3> colors;
  /** the bitmaps to use, indexed by a joker name */
  std::map<std::string, chaos::BitmapAtlas::BitmapEntry const *> bitmaps;
  /** the character_set to use, indexed by a joker name */
  std::map<std::string, chaos::BitmapAtlas::CharacterSet const *> character_sets;
  /** the atlas where to find entries */
  chaos::BitmapAtlas::Atlas const * atlas{nullptr};
};



bool TextParser::AddColor(char const * name, glm::vec3 const & color)
{
  assert(name != nullptr);

  if (colors.find(name) != colors.end())
    return false;
  colors.insert(std::make_pair(name, color));
  return true;
}

bool TextParser::AddCharacterSet(char const * name, char const * font_name)
{
  assert(name      != nullptr);
  assert(font_name != nullptr);

  if (atlas == nullptr)
    return false;

  chaos::BitmapAtlas::CharacterSet const * character_set = atlas->GetCharacterSet(font_name);
  if (character_set == nullptr)
    return false;
  return AddCharacterSet(name, character_set);
}

bool TextParser::AddCharacterSet(char const * name, chaos::BitmapAtlas::CharacterSet const * character_set)
{
  assert(name          != nullptr);
  assert(character_set != nullptr);

  if (character_sets.find(name) != character_sets.end()) // name already in used
    return false;
  character_sets.insert(std::make_pair(name, character_set));
  return true;
}

bool TextParser::AddBitmap(char const * name, char const * bitmap_set_name, char const * bitmap_name)
{
  assert(name            != nullptr);
  assert(bitmap_set_name != nullptr);
  assert(bitmap_name     != nullptr);

  if (atlas == nullptr)
    return false;

  chaos::BitmapAtlas::BitmapSet const * bitmap_set = atlas->GetBitmapSet(bitmap_set_name);
  if (bitmap_set == nullptr)
    return false;
  chaos::BitmapAtlas::BitmapEntry const * entry = bitmap_set->GetEntry(bitmap_name);
  if (entry == nullptr)
    return false;
  return AddBitmap(name, entry);
}

bool TextParser::AddBitmap(char const * name, chaos::BitmapAtlas::BitmapEntry const * entry)
{
  assert(name  != nullptr);
  assert(entry != nullptr);

  if (bitmaps.find(name) != bitmaps.end()) // name already in used
    return false;
  bitmaps.insert(std::make_pair(name, entry));
  return true;
}

void TextParser::EmitCharacters(char const c, int count, TextParserData & data)
{


}

void TextParser::ParseText(char const * text, ParseTextParams const & params)
{
  assert(text != nullptr);

  // clear the parsing stack and initialize it with default params
  TextParserData parse_data;

  TextParseElement element;
  element.type  = TextParseElement::CHANGE_COLOR;
  element.color = params.default_color;
  parse_data.parse_stack.push_back(element);

  int tab_size = params.tab_size;
  if (tab_size < 1)
    tab_size = 1;



  float x = 0;
  float y = 0;
  for (int i = 0 ; text[i] != 0 ; ++i)
  {
    if (text[i] == '\r') // ignore chariot return (UNIX/WINDOWS differences)
      continue;

    if (text[i] == '\n') // next line
    {

      continue;
    }

    if (text[i] == '\t') // tab
    {
      EmitCharacters(' ', tab_size, parse_data);
      continue;
    }
  
    if (text[i] == '/')
    {
      if (text[i + 1] == '/' || text[i + 1] == '[' || text[i + 1] == ']')
      {
        EmitCharacters(text[i + 1], 1, parse_data); // Emit the character and additionnaly skip the extra character
        ++i;
      }
      continue; // if character after '/' does correspond to nothing, skip it
    }

    if (text[i] == '[')
    {


      continue;
    }

    if (text[i] == ']') // parse stack should be decreased (keep the very first element)
    {
      if (parse_stack.size() > 1)
        parse_stack.pop_back();
      continue;
    }
  
    EmitCharacters(text[i], 1, parse_data); // this is not a special character  
  }
  
  // clear the stack 
  parse_stack.clear();
}

// -----------------------------------------------

class SpriteVertex
{
public:

  glm::vec2 position;
  glm::vec3 texcoord;


};

class SpriteManager
{
public:

  /** theses are the offset are the ones to apply to position, if position correspond to a given border */

  static int const HOTPOINT_LEFT    = 1;
  static int const HOTPOINT_RIGHT   = 2;
  static int const HOTPOINT_HMIDDLE = 0;

  static int const HOTPOINT_BOTTOM  = 4;
  static int const HOTPOINT_TOP     = 8;
  static int const HOTPOINT_VMIDDLE = 0;

  static int const HOTPOINT_BOTTOM_LEFT  = HOTPOINT_BOTTOM  | HOTPOINT_LEFT;
  static int const HOTPOINT_TOP_LEFT     = HOTPOINT_TOP     | HOTPOINT_LEFT;
  static int const HOTPOINT_BOTTOM_RIGHT = HOTPOINT_BOTTOM  | HOTPOINT_RIGHT;
  static int const HOTPOINT_TOP_RIGHT    = HOTPOINT_TOP     | HOTPOINT_RIGHT;
  static int const HOTPOINT_CENTER       = HOTPOINT_VMIDDLE | HOTPOINT_HMIDDLE;

  /** given a hotpoint, a sprite size, and the hotpoint_type, returns the BottomLeft hotpoint position */
  static glm::vec2 GetBottomLeftHotpointPosition(glm::vec2 const & hotpoint, glm::vec2 const & size, int hotpoint_type);
  /** given a hotpoint, a sprite size, and the hotpoint_type, returns the any other hotpoint_type position */
  static glm::vec2 GetHotpointPosition(glm::vec2 const & position, glm::vec2 const & size, int initial_hotpoint_type, int final_hotpoint_type);


  void Initialize();

  void AddSprite(chaos::BitmapAtlas::BitmapEntry * entry, glm::vec2 const & position, glm::vec2 const & size, glm::vec2 const & handler);

protected:

  /** the vertex array */
  boost::intrusive_ptr<chaos::VertexArray> vertex_array;
  /** the vertex buffer */
  boost::intrusive_ptr<chaos::VertexBuffer> vertex_buffer;
  /** the declaration of the vertex buffer */
  chaos::VertexDeclaration declaration;
  /** the texture atlas */
  chaos::BitmapAtlas::TextureArrayAtlas atlas;
  /** the sprites */
  std::vector<SpriteVertex> sprites;
};

void SpriteManager::Initialize()
{

}

glm::vec2 SpriteManager::GetHotpointPosition(glm::vec2 const & hotpoint, glm::vec2 const & size, int initial_hotpoint_type, int final_hotpoint_type)
{
  if (initial_hotpoint_type == final_hotpoint_type)
    return hotpoint;

  static float const offset_factor[] = { -0.5f, 0.0f, -1.0f, 0.0f };

  int initial_h_part = (initial_hotpoint_type >> 0) & 3;
  int initial_v_part = (initial_hotpoint_type >> 2) & 3;

  int final_h_part = (final_hotpoint_type >> 0) & 3;
  int final_v_part = (final_hotpoint_type >> 2) & 3;

  assert(initial_h_part != 3); // not both LEFT and RIGHT, nor TOP and BOTTOM at the same time
  assert(initial_v_part != 3);
  assert(final_h_part   != 3); // not both LEFT and RIGHT, nor TOP and BOTTOM at the same time
  assert(final_v_part   != 3);

  glm::vec2 result = hotpoint;
  result.x += size.x * (offset_factor[initial_h_part] - offset_factor[final_h_part]);
  result.y += size.y * (offset_factor[initial_v_part] - offset_factor[final_v_part]);
  return result;

}

glm::vec2 SpriteManager::GetBottomLeftHotpointPosition(glm::vec2 const & hotpoint, glm::vec2 const & size, int hotpoint_type)
{
  static float const offset_factor[] = { -0.5f, 0.0f, -1.0f, 0.0f };

  int h_part = (hotpoint_type >> 0) & 3;
  int v_part = (hotpoint_type >> 2) & 3;

  assert(h_part != 3); // not both LEFT and RIGHT, nor TOP and BOTTOM at the same time
  assert(v_part != 3);

  glm::vec2 result = hotpoint;
  result.x += size.x * offset_factor[h_part];
  result.y += size.y * offset_factor[v_part];
  return result;
}



void SpriteManager::AddSprite(chaos::BitmapAtlas::BitmapEntry * entry, glm::vec2 const & position, glm::vec2 const & size, glm::vec2 const & handler)
{
  glm::vec2 atlas_size = atlas.GetAtlasDimension();
  glm::vec2 pos_bottom_left = position + size * handler;
  glm::vec2 pos_top_right = pos_bottom_left + size;

  float tex_x1 = chaos::MathTools::CastAndDiv<float>(entry->x, atlas_size.x);
  float tex_y1 = chaos::MathTools::CastAndDiv<float>(entry->y, atlas_size.y);
  float tex_x2 = chaos::MathTools::CastAndDiv<float>(entry->x + entry->width, atlas_size.x);
  float tex_y2 = chaos::MathTools::CastAndDiv<float>(entry->y + entry->height, atlas_size.y);

  float bitmap_index = (float)entry->bitmap_index;

  SpriteVertex bl;
  bl.position = pos_bottom_left;
  bl.texcoord.x = tex_x1;
  bl.texcoord.y = tex_y1;
  bl.texcoord.z = bitmap_index;

  SpriteVertex tr;
  tr.position = pos_top_right;
  tr.texcoord.x = tex_x2;
  tr.texcoord.y = tex_y2;
  tr.texcoord.z = bitmap_index;

  SpriteVertex tl;
  tl.position.x = pos_bottom_left.x;
  tl.position.y = pos_top_right.y;
  tl.texcoord.x = tex_x1;
  tl.texcoord.y = tex_y2;
  tl.texcoord.z = bitmap_index;

  SpriteVertex br;
  br.position.x = pos_top_right.x;
  br.position.y = pos_bottom_left.y;
  br.texcoord.x = tex_x2;
  br.texcoord.y = tex_y1;
  br.texcoord.z = bitmap_index;

  sprites.push_back(bl);
  sprites.push_back(br);
  sprites.push_back(tr);

  sprites.push_back(bl);
  sprites.push_back(tr);
  sprites.push_back(tl);
}



























// --------------------------------------------------------------------

class MyGLFWWindowOpenGLTest1 : public chaos::MyGLFWWindow
{
public:

  MyGLFWWindowOpenGLTest1()
  {
    auto u = SpriteManager::HOTPOINT_BOTTOM_LEFT;


    glm::vec2 p(0.0f, 0.0f);
    glm::vec2 s(10.0f, 10.0f);

    auto p2 = SpriteManager::GetBottomLeftHotpointPosition(p, s, SpriteManager::HOTPOINT_TOP);

    p2 = p2;

    auto p3 = SpriteManager::GetHotpointPosition(p, s, SpriteManager::HOTPOINT_TOP, SpriteManager::HOTPOINT_BOTTOM_LEFT);
    p3 = p3;

    auto p4 = SpriteManager::GetHotpointPosition(p2, s, SpriteManager::HOTPOINT_BOTTOM_LEFT, SpriteManager::HOTPOINT_TOP);
    p4 = p4;
  }

protected:

  virtual bool OnDraw(int width, int height) override
  {
    glm::vec4 clear_color(0.0f, 0.0f, 0.7f, 0.0f);
    glClearBufferfv(GL_COLOR, 0, (GLfloat*)&clear_color);

    float far_plane = 1000.0f;
    glClearBufferfi(GL_DEPTH_STENCIL, 0, far_plane, 0);

    glViewport(0, 0, width, height);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);   // when viewer is inside the cube

                              // XXX : the scaling is used to avoid the near plane clipping
    chaos::box3 b(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(1.0f, 1.0f, 1.0f));

    static float FOV = 60.0f;
    glm::mat4 projection = glm::perspectiveFov(FOV * (float)M_PI / 180.0f, (float)width, (float)height, 1.0f, far_plane);
    glm::mat4 world_to_camera = fps_camera.GlobalToLocal();
    glm::mat4 local_to_world = glm::translate(b.position) * glm::scale(b.half_size);

    chaos::GLProgramData const & program_data = program_box->GetProgramData();

    glUseProgram(program_box->GetResourceID());
    program_data.SetUniform("projection", projection);
    program_data.SetUniform("world_to_camera", world_to_camera);
    program_data.SetUniform("local_to_world", local_to_world);



    mesh_box->Render(program_box->GetProgramData(), nullptr, 0, 0);

    debug_display.Display(width, height);

    return true;
  }

  virtual void Finalize() override
  {
    mesh_box = nullptr;
    program_box = nullptr;
    atlas.Clear();

    debug_display.Finalize();
  }

  boost::intrusive_ptr<chaos::GLProgram> LoadProgram(boost::filesystem::path const & resources_path, char const * ps_filename, char const * vs_filename)
  {
    chaos::GLProgramLoader loader;
    loader.AddShaderSourceFile(GL_FRAGMENT_SHADER, resources_path / ps_filename);
    loader.AddShaderSourceFile(GL_VERTEX_SHADER, resources_path / vs_filename);

    return loader.GenerateProgramObject();
  }


  bool LoadTextureArray(boost::filesystem::path const & resources_path)
  {
    return atlas.LoadAtlas(resources_path / "MyAtlas.json");
  }

  virtual bool Initialize() override
  {
    chaos::Application * application = chaos::Application::GetInstance();
    if (application == nullptr)
      return false;

    // compute resource path
    boost::filesystem::path resources_path = application->GetResourcesPath();
    boost::filesystem::path font_path = resources_path / "font.png";

    // initialize debug font display 
    chaos::GLDebugOnScreenDisplay::Params debug_params;
    debug_params.texture_path = font_path;
    debug_params.font_characters = " !\"#$%&'()*+,-./0123456789:;<=>?@ABCDEFGHIJKLMNOPQRSTUVWXYZ[\\]^_`abcdefghijklmnopqrstuvwxyz{|}~";
    debug_params.font_characters_per_line = 10;
    debug_params.font_characters_line_count = 10;
    debug_params.character_width = 20;
    debug_params.spacing = glm::ivec2(0, 0);
    debug_params.crop_texture = glm::ivec2(15, 7);

    if (!debug_display.Initialize(debug_params))
      return false;

    // load texture
    if (!LoadTextureArray(resources_path))
      return false;

    // load programs      
    program_box = LoadProgram(resources_path, "pixel_shader_box.txt", "vertex_shader_box.txt");
    if (program_box == nullptr)
      return false;

    // create meshes
    chaos::box3 b = chaos::box3(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(1.0f, 1.0f, 1.0f));

    chaos::MultiMeshGenerator generators;
    generators.AddGenerator(chaos::CubeMeshGenerator(b), mesh_box);

    if (!generators.GenerateMeshes())
      return false;

    // place camera
    fps_camera.fps_controller.position.y = 0.0f;
    fps_camera.fps_controller.position.z = 10.0f;

    // initial display
    debug_display.AddLine("Draw a box with a texture array : \n  Use +/- to change slice.\n  Array composed of images with different size and BPP");

    return true;
  }

  virtual void TweakSingleWindowApplicationHints(chaos::MyGLFWWindowHints & hints, GLFWmonitor * monitor, bool pseudo_fullscreen) const override
  {
    chaos::MyGLFWWindow::TweakSingleWindowApplicationHints(hints, monitor, pseudo_fullscreen);

    hints.toplevel = 0;
    hints.decorated = 1;
  }

  virtual bool Tick(double delta_time) override
  {
    if (glfwGetKey(glfw_window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
      RequireWindowClosure();

    fps_camera.Tick(glfw_window, delta_time);

    debug_display.Tick(delta_time);

    return true; // refresh
  }

  virtual void OnKeyEvent(int key, int scan_code, int action, int modifier) override
  {
    if (key == GLFW_KEY_KP_ADD && action == GLFW_RELEASE)
      ++bitmap_index;
    else if (key == GLFW_KEY_KP_SUBTRACT && action == GLFW_RELEASE)
      --bitmap_index;
  }


  virtual void OnMouseButton(int button, int action, int modifier) override
  {
    fps_camera.OnMouseButton(glfw_window, button, action, modifier);
  }

  virtual void OnMouseMove(double x, double y) override
  {
    fps_camera.OnMouseMove(glfw_window, x, y);
  }

protected:

  // rendering for the box  
  boost::intrusive_ptr<chaos::SimpleMesh> mesh_box;
  boost::intrusive_ptr<chaos::GLProgram>  program_box;

  chaos::BitmapAtlas::TextureArrayAtlas atlas;

  int bitmap_index{ 0 };

  chaos::MyGLFWFpsCamera fps_camera;

  chaos::GLDebugOnScreenDisplay debug_display;
};

int _tmain(int argc, char ** argv, char ** env)
{
  chaos::Application::Initialize<chaos::Application>(argc, argv, env);

  chaos::WinTools::AllocConsoleAndRedirectStdOutput();

  chaos::MyGLFWSingleWindowApplicationParams params;
  params.monitor = nullptr;
  params.width = 1200;
  params.height = 600;
  params.monitor_index = 0;
  chaos::MyGLFWWindow::RunSingleWindowApplication<MyGLFWWindowOpenGLTest1>(params);

  chaos::Application::Finalize();

  return 0;
}


