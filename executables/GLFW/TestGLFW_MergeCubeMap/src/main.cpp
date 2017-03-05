#include <chaos/StandardHeaders.h> 
#include <chaos/FileTools.h> 
#include <chaos/LogTools.h> 
#include <chaos/StringTools.h>
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

class MyGLFWWindowOpenGLTest1 : public chaos::MyGLFWWindow
{

protected:

	virtual void OnKeyEvent(int key, int scan_code, int action, int modifier) override
	{
		if (key == GLFW_KEY_KP_ADD && action == GLFW_RELEASE)
		{
			ChangeSkyBox(skybox_index + 1);
		}
		else if (key == GLFW_KEY_KP_SUBTRACT && action == GLFW_RELEASE)
		{
			ChangeSkyBox(skybox_index - 1);
		}
	}

	void ChangeSkyBox(int index)
	{
		boost::intrusive_ptr<chaos::Texture> new_texture = GenerateSkyBox(index);
		if (new_texture != nullptr)
		{
			skybox_index = index;
			texture = new_texture;
			debug_display.Clear();
			debug_display.AddLine(chaos::StringTools::Printf("format : %d", index).c_str());
		}
	}

	boost::intrusive_ptr<chaos::Texture> GenerateSkyBox(int index)
	{
		chaos::PixelFormat pixel_format = chaos::PixelFormat(index + 1); // 0 = UNKNOWN FORMAT
		if (!pixel_format.IsValid())
			return nullptr;

		chaos::PixelFormatMergeParams merge_params;
		merge_params.pixel_format = pixel_format;

		return chaos::GLTextureTools::GenTextureObject(&skybox, merge_params);

		
		chaos::SkyBoxImages single_skybox = skybox.ToSingleImage(true, glm::vec4(0.0f, 0.0f, 0.0f, 1.0f), merge_params);

		if (!single_skybox.IsEmpty())
			return chaos::GLTextureTools::GenTextureObject(&single_skybox);

		return nullptr;
	}

	virtual bool OnDraw(int width, int height) override
	{
		glm::vec4 clear_color(0.0f, 0.0f, 0.0f, 0.0f);
		glClearBufferfv(GL_COLOR, 0, (GLfloat*)&clear_color);

		float far_plane = 1000.0f;
		glClearBufferfi(GL_DEPTH_STENCIL, 0, far_plane, 0);

		glViewport(0, 0, width, height);
		glEnable(GL_DEPTH_TEST);
		glDisable(GL_CULL_FACE);   // when viewer is inside the cube

		glUseProgram(program->GetResourceID());

		// XXX : the scaling is used to avoid the near plane clipping      
		static float FOV =  60.0f;
		glm::mat4 projection_matrix      = glm::perspectiveFov(FOV * (float)M_PI / 180.0f,(float)width, (float)height, 1.0f, far_plane);
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

		debug_display.Display(width, height);    

		return true;
	}

	virtual void Finalize() override
	{
		skybox.Release();

		for (FIBITMAP * bitmap : skybox_bitmaps)
			FreeImage_Unload(bitmap);
		skybox_bitmaps.clear();

		program = nullptr;
		mesh    = nullptr;
		texture = nullptr;

		debug_display.Finalize();
	}

	bool LoadSkyboxBitmaps(boost::filesystem::path const & resources_path)
	{
		// load the images
		boost::filesystem::directory_iterator end;
		for (boost::filesystem::directory_iterator it(resources_path / "images"); it != end; ++it)
		{
			FIBITMAP * bitmap = chaos::ImageTools::LoadImageFromFile(it->path().string().c_str());
			if (bitmap == nullptr)
				continue;
			skybox_bitmaps.push_back(bitmap);		
		}
		if (skybox_bitmaps.size() != 6)
			return false;

		// search the minimum size
		int size = -1;
		for (FIBITMAP * bitmap : skybox_bitmaps)
		{
			chaos::ImageDescription desc = chaos::ImageTools::GetImageDescription(bitmap);
			if (size < 0 || desc.width > size)
				size = desc.width;
			if (size < 0 || desc.height > size)
				size = desc.height;		
		}
		if (size <= 0)
			return false;

		// resize the image
		unsigned char c1[4] = { 0, 0, 0, 0 };
		glm::vec4 c2 = glm::vec4(0.0f, 0.0f, 0.0f, 0.0f);

		for (FIBITMAP * & bitmap : skybox_bitmaps)
		{
			//
			// FreeImage_EnlargeCanvas : enlarge or shrink image (returns a new image)
			//
			// FreeImage_CreateView : there is an issue, it created a new image that points the same buffer => at destruction of one, the buffer is release (no ref count)
			//

			chaos::ImageDescription desc = chaos::ImageTools::GetImageDescription(bitmap);

			void * color = (desc.pixel_format.component_type == chaos::PixelFormat::TYPE_UNSIGNED_CHAR) ? (void*)&c1[0] : (void*)&c2; // select a color for background

			int dx = size - desc.width;
			int dy = size - desc.height;

			FIBITMAP * old_bitmap = bitmap;
			bitmap = FreeImage_EnlargeCanvas(bitmap, dx / 2, dy / 2, dx - dx / 2, dy - dy / 2 , color);
			if (bitmap == nullptr)
				return false;

			FreeImage_Unload(old_bitmap);
		}

		// generate the skybox
		for (size_t i = 0; i < skybox_bitmaps.size() ; ++i)
		{
			skybox.SetImage(i, skybox_bitmaps[i], false);
		}
		return true;
	}

	virtual bool Initialize() override
	{   
		chaos::Application * application = chaos::Application::GetInstance();
		if (application == nullptr)
			return false;

		boost::filesystem::path resources_path = application->GetResourcesPath();

		if (!LoadSkyboxBitmaps(resources_path))
			return false;

		boost::filesystem::path image_path  = resources_path / "font.png";

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

		debug_display.AddLine("Press +/- to change skybox");

		texture = GenerateSkyBox(0);
		if (texture == nullptr)
			return false;

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

		return true;
	}

	virtual void TweakSingleWindowApplicationHints(chaos::MyGLFWWindowHints & hints, GLFWmonitor * monitor, bool pseudo_fullscreen) const override
	{
		chaos::MyGLFWWindow::TweakSingleWindowApplicationHints(hints, monitor, pseudo_fullscreen);

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

	std::vector<FIBITMAP*> skybox_bitmaps;

	chaos::SkyBoxImages skybox;
			
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

	chaos::MyGLFWSingleWindowApplicationParams params;
	params.monitor       = nullptr;
	params.width         = 700;
	params.height        = 700;
	params.monitor_index = 0;
	chaos::MyGLFWWindow::RunSingleWindowApplication<MyGLFWWindowOpenGLTest1>(params);

	chaos::Application::Finalize();

	return 0;
}


