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
#include <chaos/SkyBoxTools.h>
#include <chaos/GLDebugOnScreenDisplay.h>
#include <chaos/FPSViewInputController.h>
#include <chaos/SimpleMesh.h>
#include <chaos/GLProgramData.h>
#include <chaos/GLProgram.h>
#include <chaos/Texture.h>
#include <chaos/VertexDeclaration.h>
#include <chaos/GLProgramVariableProvider.h>
#include <chaos/SoundManager.h>



























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

		glUseProgram(program->GetResourceID());

#if 0

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

		fps_view_controller.Tick(glfw_window, delta_time);

		debug_display.Tick(delta_time);

		return true; // refresh
	}

	virtual void OnMouseButton(int button, int action, int modifier) override
	{
		if (button == 1 && action == GLFW_RELEASE)
			debug_display.AddLine("HelloWorld");
	}

protected:

	boost::intrusive_ptr<chaos::SoundManager> sound_manager;

	boost::intrusive_ptr<chaos::GLProgram>  program;
	boost::intrusive_ptr<chaos::SimpleMesh> mesh;
	boost::intrusive_ptr<chaos::Texture>    texture;

	chaos::FPSViewInputController fps_view_controller;

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


