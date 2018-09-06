#include <chaos/StandardHeaders.h> 
#include <chaos/FileTools.h> 
#include <chaos/LogTools.h> 
#include <chaos/GLTools.h> 
#include <chaos/StringTools.h> 
#include <chaos/MyGLFWGamepadManager.h> 
#include <chaos/MyGLFWSingleWindowApplication.h> 
#include <chaos/MyGLFWWindow.h> 
#include <chaos/WinTools.h> 
#include <chaos/GPUProgramGenerator.h>
#include <chaos/Application.h>
#include <chaos/SimpleMeshGenerator.h>
#include <chaos/SkyBoxTools.h>
#include <chaos/GLDebugOnScreenDisplay.h>
#include <chaos/FPSViewInputController.h>
#include <chaos/SimpleMesh.h>
#include <chaos/MultiMeshGenerator.h>
#include <chaos/GPUProgramData.h>
#include <chaos/GPUProgram.h>
#include <chaos/GPUVertexDeclaration.h>
#include <chaos/GLTextureTools.h>
#include <chaos/TextureArrayAtlas.h>
#include <chaos/GPUProgramProvider.h>

class MyGLFWWindowOpenGLTest1 : public chaos::MyGLFW::Window
{

protected:

	chaos::BitmapAtlas::BitmapInfo const * FindBitmapRecursive(chaos::BitmapAtlas::FolderInfo const * folder, size_t & count)
	{
		// recursive first (because bitmaps are in a sub folder in this example)
		size_t child_folder_count = folder->folders.size();
		for (size_t i = 0; i < child_folder_count; ++i)
		{
			chaos::BitmapAtlas::BitmapInfo const * result = FindBitmapRecursive(folder->folders[i].get(), count);
			if (result != nullptr)
				return result;
		}
		// bitmaps
		size_t bitmap_count = folder->bitmaps.size();
		if (count < bitmap_count)
			return &folder->bitmaps[count];
		else
			count = count - bitmap_count;
		// fonts
		size_t font_count = folder->fonts.size();
		for (size_t i = 0; i < font_count; ++i)
		{
			chaos::BitmapAtlas::FontInfo const & font_info = folder->fonts[i];

			size_t character_count = font_info.elements.size();
			if (count < character_count)
				return &font_info.elements[count];
			else
				count = count - character_count;
		}
		return nullptr;
	}

  chaos::BitmapAtlas::BitmapInfo const * ClampBitmapIndexAndGetEntry()
  {
    chaos::BitmapAtlas::BitmapInfo const * result = nullptr;

    if (atlas.GetBitmapCount() != 0)
    {
      if (bitmap_index < 0)
        bitmap_index = 0;

      size_t count = bitmap_index;
			result = FindBitmapRecursive(atlas.GetRootFolder(), count);
			if (result == nullptr)
			{
				bitmap_index = 0;
				count = bitmap_index;
				result = FindBitmapRecursive(atlas.GetRootFolder(), count);
			}
    }
    return result;
  }


  virtual bool OnDraw(glm::ivec2 size) override
  {
    glm::vec4 clear_color(0.0f, 0.0f, 0.7f, 0.0f);
    glClearBufferfv(GL_COLOR, 0, (GLfloat*)&clear_color);

    float far_plane = 1000.0f;
    glClearBufferfi(GL_DEPTH_STENCIL, 0, far_plane, 0);

    chaos::BitmapAtlas::BitmapInfo const * info = ClampBitmapIndexAndGetEntry();
    if (info == nullptr)
      return true;

    glViewport(0, 0, size.x, size.y);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);   // when viewer is inside the cube

    // XXX : the scaling is used to avoid the near plane clipping
    chaos::box3 b(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(1.0f, 1.0f, 1.0f));

    static float FOV = 60.0f;
    glm::mat4 projection      = glm::perspectiveFov(FOV * (float)M_PI / 180.0f, (float)size.x, (float)size.y, 1.0f, far_plane);
    glm::mat4 world_to_camera = fps_view_controller.GlobalToLocal();
    glm::mat4 local_to_world  = glm::translate(b.position) * glm::scale(b.half_size);

    chaos::GPUProgramProvider uniform_provider;

    uniform_provider.AddVariableValue("projection",      projection);
    uniform_provider.AddVariableValue("world_to_camera", world_to_camera);
    uniform_provider.AddVariableValue("local_to_world",  local_to_world);
    uniform_provider.AddVariableValue("texture_slice", (float)info->bitmap_index);

    uniform_provider.AddVariableTexture("material", atlas.GetTexture());

    glm::vec2 atlas_dimension = atlas.GetAtlasDimension();

    glm::vec2 entry_start = glm::vec2((float)info->x, (float)info->y);
    glm::vec2 entry_size  = glm::vec2((float)info->width, (float)info->height);
    glm::vec2 entry_end   = entry_start + entry_size;

		entry_start.y = 1.0f - entry_start.y; // BITMAP coordinates and OpenGL textures coordinates are inverted
		entry_end.y   = 1.0f - entry_end.y;

    uniform_provider.AddVariableValue("entry_start", entry_start / atlas_dimension);
    uniform_provider.AddVariableValue("entry_end", entry_end / atlas_dimension);

    mesh_box->Render(program_box.get(), &uniform_provider);

    debug_display.Display(size.x, size.y);

    return true;
  }

  virtual void Finalize() override
  {
    mesh_box    = nullptr;
    program_box = nullptr;
    atlas.Clear();

    debug_display.Finalize();
  }

  boost::intrusive_ptr<chaos::GPUProgram> LoadProgram(boost::filesystem::path const & resources_path, char const * ps_filename, char const * vs_filename)
  {
    chaos::GPUProgramGenerator program_generator;
    program_generator.AddShaderSourceFile(GL_FRAGMENT_SHADER, resources_path / ps_filename);
    program_generator.AddShaderSourceFile(GL_VERTEX_SHADER, resources_path / vs_filename);

    return program_generator.GenProgramObject();
  }


  bool LoadTextureArray(boost::filesystem::path const & resources_path)
  {
    return atlas.LoadAtlas(resources_path / "MyAtlas.json");
  }

  virtual bool InitializeFromConfiguration(nlohmann::json const & config, boost::filesystem::path const & config_path) override
  {
    chaos::Application * application = chaos::Application::GetInstance();
    if (application == nullptr)
      return false;

    // compute resource path
    boost::filesystem::path resources_path = application->GetResourcesPath();
    boost::filesystem::path font_path = resources_path / "font.png";

    // initialize debug font display 
    chaos::GLDebugOnScreenDisplay::Params debug_params;
    debug_params.texture_path               = font_path;
    debug_params.font_characters            = " !\"#$%&'()*+,-./0123456789:;<=>?@ABCDEFGHIJKLMNOPQRSTUVWXYZ[\\]^_`abcdefghijklmnopqrstuvwxyz{|}~";
    debug_params.font_characters_per_line   = 10;
    debug_params.font_characters_line_count = 10;
    debug_params.character_width            = 20;
    debug_params.spacing                    = glm::ivec2( 0, 0);
    debug_params.crop_texture               = glm::ivec2(15, 7);

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
    generators.AddGenerator(new chaos::CubeMeshGenerator(b), mesh_box);

    if (!generators.GenerateMeshes())
      return false;

    // place camera
    fps_view_controller.fps_controller.position.y = 0.0f;
    fps_view_controller.fps_controller.position.z = 10.0f;

    // initial display
    debug_display.AddLine("Draw a box with a texture array : \n  Use +/- to change slice.\n  Array composed of images with different size and BPP");

    return true;
  }

  virtual void TweakHints(chaos::MyGLFW::WindowHints & hints, GLFWmonitor * monitor, bool pseudo_fullscreen) const override
  {
    chaos::MyGLFW::Window::TweakHints(hints, monitor, pseudo_fullscreen);

    hints.toplevel  = 0;
    hints.decorated = 1;
  }

  virtual bool Tick(double delta_time) override
  {
    if (glfwGetKey(glfw_window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
      RequireWindowClosure();

    fps_view_controller.Tick(glfw_window, delta_time);

    debug_display.Tick(delta_time);

    return true; // refresh
  }

  virtual void OnKeyEvent(int key, int scan_code, int action, int modifier) override
  {
		if (key == GLFW_KEY_KP_ADD && action == GLFW_RELEASE)
			++bitmap_index;
		else if (key == GLFW_KEY_KP_SUBTRACT && action == GLFW_RELEASE)
		{
			if (bitmap_index > 0)
				--bitmap_index;
		}
  }

protected:

  // rendering for the box  
  boost::intrusive_ptr<chaos::SimpleMesh> mesh_box;
  boost::intrusive_ptr<chaos::GPUProgram>  program_box;

  chaos::BitmapAtlas::TextureArrayAtlas atlas;

  size_t bitmap_index = 0;

  chaos::FPSViewInputController fps_view_controller;

  chaos::GLDebugOnScreenDisplay debug_display;
};

int _tmain(int argc, char ** argv, char ** env)
{
  chaos::MyGLFW::SingleWindowApplicationParams params;
  params.monitor = nullptr;
  params.width = 1200;
  params.height = 600;
  params.monitor_index = 0;
  chaos::MyGLFW::RunWindowApplication<MyGLFWWindowOpenGLTest1>(argc, argv, env, params);

  return 0;
}


