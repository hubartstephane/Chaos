#include <chaos/StandardHeaders.h> 
#include <chaos/FileTools.h> 
#include <chaos/LogTools.h> 
#include <chaos/GLTools.h> 
#include <chaos/GLTextureTools.h>
#include <chaos/MyGLFWGamepadManager.h> 
#include <chaos/MyGLFWWindow.h> 
#include <chaos/WinTools.h> 
#include <chaos/GLProgramLoader.h>
#include <chaos/Application.h>
#include <chaos/SimpleMeshGenerator.h>
#include <chaos/GLDebugOnScreenDisplay.h>
#include <chaos/SimpleMesh.h>
#include <chaos/GLProgramData.h>
#include <chaos/GLProgram.h>
#include <chaos/Texture.h>
#include <chaos/VertexDeclaration.h>
#include <chaos/GLProgramVariableProvider.h>
#include <chaos/SoundManager.h>
#include <json.hpp>
#include <chaos/BoostTools.h>
#include <chaos/BitmapAtlas.h>
#include <chaos/BitmapAtlasGenerator.h>
#include <chaos/TextureArrayAtlas.h>


class ObjectDefinition
{

public:

	bool LoadFromJSON(nlohmann::json const & json_entry);

public:

	int id = 0;
	int layer = 0;
	float scale = 1.0f;
	boost::filesystem::path bitmap_path;


};

bool ObjectDefinition::LoadFromJSON(nlohmann::json const & json_entry)
{
	id = json_entry.value("id", 0);
	layer = json_entry.value("layer", 0);
	scale = json_entry.value("scale", 1.0f);
	bitmap_path = json_entry.value("bitmap", "");

	return true;
}


class Game : public chaos::ReferencedObject
{
public:

	void Tick(double delta_time);

	bool Initialize(boost::filesystem::path const & obj_def_path);

	void Finalize();

protected:

	bool LoadObjectDefinition(boost::filesystem::path const & obj_def_path);

	bool LoadObjectDefinition(nlohmann::json const & json_entry);

	bool GenerateAtlas(boost::filesystem::path const & obj_def_path);

protected:

	chaos::BitmapAtlas::TextureArrayAtlas texture_atlas;

	std::vector<ObjectDefinition> object_definitions;
};

void Game::Tick(double delta_time)
{

}

bool Game::Initialize(boost::filesystem::path const & obj_def_path)
{
	if (!LoadObjectDefinition(obj_def_path))
		return false;
	if (!GenerateAtlas(obj_def_path))
		return false;




	return true;
}

void Game::Finalize()
{
	object_definitions.clear();


}

bool Game::GenerateAtlas(boost::filesystem::path const & obj_def_path)
{
	int ATLAS_SIZE = 1024;
	int ATLAS_PADDING = 10;
	chaos::BitmapAtlas::AtlasGeneratorParams params = chaos::BitmapAtlas::AtlasGeneratorParams(ATLAS_SIZE, ATLAS_SIZE, ATLAS_PADDING, chaos::PixelFormatMergeParams());
	
	chaos::BitmapAtlas::AtlasInput input;

	chaos::BitmapAtlas::BitmapSetInput * bitmap_set = input.AddBitmapSet("sprites");
	if (bitmap_set == nullptr)
		return false;

	for (ObjectDefinition const & def : object_definitions)
	{
		boost::filesystem::path image_path = chaos::BoostTools::FindAbsolutePath(obj_def_path, def.bitmap_path); // make the image path relative to resource path
		if (!bitmap_set->AddBitmapFile(image_path, nullptr, def.id))
			return false;
	}

	chaos::BitmapAtlas::Atlas          atlas;
	chaos::BitmapAtlas::AtlasGenerator generator;
	if (!generator.ComputeResult(input, atlas, params))
		return false;

#if 1

	// the tests
	boost::filesystem::path dst_p;
	if (chaos::FileTools::CreateTemporaryDirectory("TestMergedAtlas", dst_p))
	{
		atlas.SaveAtlas(dst_p / "LudumAtlas");
		chaos::WinTools::ShowFile(dst_p.string().c_str());
	}

#endif

	texture_atlas.LoadFromBitmapAtlas(atlas);

	return true;
}

bool Game::LoadObjectDefinition(boost::filesystem::path const & obj_def_path)
{
	chaos::Buffer<char> buf = chaos::FileTools::LoadFile(obj_def_path, true);
	if (buf == nullptr)
		return false;

	try
	{
		nlohmann::json json_entry = nlohmann::json::parse(buf.data);
		return LoadObjectDefinition(json_entry);
	}
	catch(...)
	{

	}
	return false;
}

bool Game::LoadObjectDefinition(nlohmann::json const & json_entry)
{
	nlohmann::json objects = json_entry["objects"];

	for (auto const & obj : objects)
	{
		ObjectDefinition def;
		if (!def.LoadFromJSON(obj))
			continue;
		object_definitions.push_back(std::move(def));
	}
	return true;
}

















class MyGLFWWindowOpenGLTest1 : public chaos::MyGLFW::Window
{

protected:

	virtual void OnKeyEvent(int key, int scan_code, int action, int modifier) override
	{
		if (key == GLFW_KEY_KP_ADD && action == GLFW_RELEASE)
		{

		}
		else if (key == GLFW_KEY_KP_SUBTRACT && action == GLFW_RELEASE)
		{

		}
	}



	virtual bool OnDraw(glm::ivec2 size) override
	{
		glm::vec4 clear_color(0.0f, 0.0f, 0.0f, 0.0f);
		glClearBufferfv(GL_COLOR, 0, (GLfloat*)&clear_color);

		float far_plane = 1000.0f;
		glClearBufferfi(GL_DEPTH_STENCIL, 0, far_plane, 0);

		chaos::GLTools::SetViewportWithAspect(size, 16.0f/9.0f);

		//glViewport(0, 0, size.x, size.y);
		glEnable(GL_DEPTH_TEST);
		glDisable(GL_CULL_FACE);   // when viewer is inside the cube
#if 0
		glUseProgram(program->GetResourceID());



		// XXX : the scaling is used to avoid the near plane clipping      
		static float FOV =  60.0f;
		glm::mat4 projection_matrix      = glm::perspectiveFov(FOV * (float)M_PI / 180.0f,(float)size.x, (float)size.y, 1.0f, far_plane);
		glm::mat4 local_to_world_matrix  = glm::scale(glm::vec3(10.0f, 10.0f, 10.0f));
		glm::mat4 world_to_camera_matrix = fps_view_controller.GlobalToLocal();

		chaos::GLProgramData const & program_data = program->GetProgramData();

		chaos::GLProgramVariableProviderChain uniform_provider;

		uniform_provider.AddVariableValue("projection",      projection_matrix);
		uniform_provider.AddVariableValue("local_to_world",  local_to_world_matrix);
		uniform_provider.AddVariableValue("world_to_camera", world_to_camera_matrix);

		uniform_provider.AddVariableTexture("material", texture);

		program_data.BindUniforms(&uniform_provider);

		mesh->Render(program_data, nullptr, 0, 0);

		debug_display.Display(size.x, size.y);    
#endif
		return true;
	}

	virtual void Finalize() override
	{
		game = nullptr;

		program = nullptr;
		mesh    = nullptr;
		texture = nullptr;

		debug_display.Finalize();

		sound_manager = nullptr;
	}

	virtual bool Initialize() override
	{   
		chaos::Application * application = chaos::Application::GetInstance();
		if (application == nullptr)
			return false;

		boost::filesystem::path resources_path = application->GetResourcesPath();
		boost::filesystem::path image_path     = resources_path / "font.png";

		chaos::GLDebugOnScreenDisplay::Params debug_params;
		debug_params.texture_path               = image_path;
		debug_params.font_characters            = " !\"#$%&'()*+,-./0123456789:;<=>?@ABCDEFGHIJKLMNOPQRSTUVWXYZ[\\]^_`abcdefghijklmnopqrstuvwxyz{|}~";
		debug_params.font_characters_per_line   = 10;
		debug_params.font_characters_line_count = 10;
		debug_params.character_width            = 20;
		debug_params.spacing                    = glm::ivec2( 0, 0);
		debug_params.crop_texture               = glm::ivec2(15, 7);

		if (!debug_display.Initialize(debug_params))
			return false;

		sound_manager = new chaos::SoundManager;
		if (sound_manager == nullptr)
			return false;

		game = new Game;
		if (game == nullptr)
			return false;
		if (!game->Initialize(resources_path / "objects" / "object_definitions.json"))
			return false;

#if 0

		chaos::GLProgramLoader loader;
		loader.AddShaderSourceFile(GL_FRAGMENT_SHADER, resources_path / "pixel_shader_cube.txt");
		loader.AddShaderSourceFile(GL_VERTEX_SHADER,   resources_path / "vertex_shader.txt");

		program = loader.GenerateProgramObject();
		if (program == nullptr)
			return false;

		chaos::box3 b = chaos::box3(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(1.0f, 1.0f, 1.0f));

		mesh = chaos::CubeMeshGenerator(b).GenerateMesh(); 
		if (mesh == nullptr)
			return false;

#endif
		return true;
	}

	virtual void TweakSingleWindowApplicationHints(chaos::MyGLFW::WindowHints & hints, GLFWmonitor * monitor, bool pseudo_fullscreen) const override
	{
		chaos::MyGLFW::Window::TweakSingleWindowApplicationHints(hints, monitor, pseudo_fullscreen);

		hints.toplevel  = 1;
		hints.decorated = 1;
	}

	virtual bool Tick(double delta_time) override
	{
		if (glfwGetKey(glfw_window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
			RequireWindowClosure();

		game->Tick(delta_time);

		debug_display.Tick(delta_time);

		return true; // refresh
	}

	virtual void OnMouseButton(int button, int action, int modifier) override
	{
		if (button == 1 && action == GLFW_RELEASE)
			debug_display.AddLine("HelloWorld");
	}

protected:

	boost::intrusive_ptr<Game> game;


	boost::intrusive_ptr<chaos::SoundManager> sound_manager;

	boost::intrusive_ptr<chaos::GLProgram>  program;
	boost::intrusive_ptr<chaos::SimpleMesh> mesh;
	boost::intrusive_ptr<chaos::Texture>    texture;

	chaos::GLDebugOnScreenDisplay debug_display;

	int skybox_index{ 0 };
};

int _tmain(int argc, char ** argv, char ** env)
{
	chaos::Application::Initialize<chaos::Application>(argc, argv, env);

	chaos::WinTools::AllocConsoleAndRedirectStdOutput();

	chaos::MyGLFW::SingleWindowApplicationParams params;
	params.monitor       = nullptr;
	params.width         = 700;
	params.height        = 700;
	params.monitor_index = 0;
	chaos::MyGLFW::Window::RunSingleWindowApplication<MyGLFWWindowOpenGLTest1>(params);

	chaos::Application::Finalize();

	return 0;
}


