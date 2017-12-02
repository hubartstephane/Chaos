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
#include <chaos/SpriteManager.h>

// ======================================================================================

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

// ======================================================================================

class SpriteLayer
{
public:

	void Draw(chaos::GLProgramVariableProviderChain & uniform_provider);

public:

	boost::intrusive_ptr<chaos::SpriteManager> sprite_manager;

	int layer;
};

void SpriteLayer::Draw(chaos::GLProgramVariableProviderChain & uniform_provider)
{


}


// ======================================================================================

class Game : public chaos::ReferencedObject
{
public:

	void Tick(double delta_time);

	bool Initialize(boost::filesystem::path const & path);

	void Finalize();

	void Display(glm::ivec2 size);

protected:

	bool LoadObjectDefinition(boost::filesystem::path const & path);

	bool LoadObjectDefinition(nlohmann::json const & json_entry);

	bool GenerateAtlas(boost::filesystem::path const & path);

	bool GenerateSpriteGPUProgram(boost::filesystem::path const & path);

	bool GenerateSpriteLayers();

	SpriteLayer * FindSpriteLayer(int layer);



protected:

	std::vector<SpriteLayer> sprite_layers;

	chaos::BitmapAtlas::TextureArrayAtlas texture_atlas;

	std::vector<ObjectDefinition> object_definitions;

	boost::intrusive_ptr<chaos::GLProgram> sprite_program;
};

// ======================================================================================

void Game::Tick(double delta_time)
{

}

bool Game::Initialize(boost::filesystem::path const & path)
{
	boost::filesystem::path object_path = path / "objects";

	if (!LoadObjectDefinition(object_path / "objects.json"))
		return false;
	if (!GenerateAtlas(object_path))
		return false;
	if (!GenerateSpriteGPUProgram(path))
		return false;
	if (!GenerateSpriteLayers())
		return false;

	return true;
}

void Game::Finalize()
{
	sprite_layers.clear();
	object_definitions.clear();
	texture_atlas.Clear();

	sprite_program = nullptr;
}

SpriteLayer * Game::FindSpriteLayer(int layer)
{
	for (size_t i = 0 ; i < sprite_layers.size(); ++i)
		if (sprite_layers[i].layer == layer)
			return &sprite_layers[i];
	return nullptr;
}

bool Game::GenerateSpriteLayers()
{
	chaos::SpriteManagerInitParams sprite_params;
	sprite_params.atlas = &texture_atlas;
	sprite_params.program = sprite_program;

	for (size_t i = 0 ; i < object_definitions.size() ; ++i)
	{
		int object_layer = object_definitions[i].layer;

		SpriteLayer * sprite_layer = FindSpriteLayer(object_layer);
		if (sprite_layer == nullptr)
		{		
			boost::intrusive_ptr<chaos::SpriteManager> sprite_manager = new chaos::SpriteManager();
			if (sprite_manager == nullptr)
				return false;			
			if (!sprite_manager->Initialize(sprite_params))
				return false;	

			SpriteLayer sl;
			sl.layer = object_layer;
			sl.sprite_manager = sprite_manager;

			sprite_layers.push_back(std::move(sl));
			
			sprite_layer = &sprite_layers.back();
		} 
	}

	// sort the layers
	std::sort(sprite_layers.begin(), sprite_layers.end(), [](SpriteLayer const & obj1, SpriteLayer const & obj2){	
		return (obj1.layer < obj2.layer);			
	});

	return true;
}

bool Game::GenerateSpriteGPUProgram(boost::filesystem::path const & path)
{
	chaos::GLProgramLoader loader;

	loader.AddShaderSource(GL_VERTEX_SHADER, (path / "sprite_vertex_shader.txt").string().c_str());
	loader.AddShaderSource(GL_FRAGMENT_SHADER, (path / "sprite_pixel_shader.txt").string().c_str());

	sprite_program = loader.GenerateProgramObject();
	if (sprite_program == nullptr)
		return false;

	return true;
}

bool Game::GenerateAtlas(boost::filesystem::path const & path)
{
	// Fill Atlas generation Input
	int ATLAS_SIZE = 1024;
	int ATLAS_PADDING = 10;
	chaos::BitmapAtlas::AtlasGeneratorParams params = chaos::BitmapAtlas::AtlasGeneratorParams(ATLAS_SIZE, ATLAS_SIZE, ATLAS_PADDING, chaos::PixelFormatMergeParams());
	
	chaos::BitmapAtlas::AtlasInput input;

	chaos::BitmapAtlas::BitmapSetInput * bitmap_set = input.AddBitmapSet("sprites");
	if (bitmap_set == nullptr)
		return false;

	for (ObjectDefinition const & def : object_definitions)
	{
		boost::filesystem::path image_path = chaos::BoostTools::FindAbsolutePath(path, def.bitmap_path); // make the image path relative to resource path
		if (!bitmap_set->AddBitmapFile(image_path, nullptr, def.id))
			return false;
	}

	// generate STD Atlas
	chaos::BitmapAtlas::Atlas          atlas;
	chaos::BitmapAtlas::AtlasGenerator generator;
	if (!generator.ComputeResult(input, atlas, params))
		return false;

	// Display debug Atlas
#if 0
	boost::filesystem::path dst_p;
	if (chaos::FileTools::CreateTemporaryDirectory("TestMergedAtlas", dst_p))
	{
		atlas.SaveAtlas(dst_p / "LudumAtlas");
		chaos::WinTools::ShowFile(dst_p.string().c_str());
	}
#endif

	// generate texture Atlas
	if (!texture_atlas.LoadFromBitmapAtlas(atlas))
		return false;

	return true;
}

bool Game::LoadObjectDefinition(boost::filesystem::path const & path)
{
	chaos::Buffer<char> buf = chaos::FileTools::LoadFile(path, true);
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

void Game::Display(glm::ivec2 size)
{	
	chaos::GLProgramVariableProviderChain uniform_provider;

	for (int i = sprite_layers.size() - 1 ; i >= 0; --i)
		sprite_layers[i].Draw(uniform_provider);



#if 0
	chaos::GLProgramVariableProviderChain uniform_provider;
	uniform_provider.AddVariableValue("local_to_cam", local_to_cam);

	sprite_manager.Display(&uniform_provider);
#endif
}















// ======================================================================================


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

		glEnable(GL_DEPTH_TEST);
		glDisable(GL_CULL_FACE);   // when viewer is inside the cube


		game->Display(size);


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
		if (!game->Initialize(resources_path))
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


