#include <chaos/StandardHeaders.h> 
#include <chaos/FileTools.h> 
#include <chaos/LogTools.h> 
#include <chaos/GLTools.h> 
#include <chaos/GLTextureTools.h>
#include <chaos/GPUTextureLoader.h>
#include <chaos/MyGLFWGamepadManager.h> 
#include <chaos/MyGLFWSingleWindowApplication.h> 
#include <chaos/MyGLFWWindow.h> 
#include <chaos/WinTools.h> 
#include <chaos/GPUProgramGenerator.h>
#include <chaos/GPUProgramData.h>
#include <chaos/Application.h>
#include <chaos/SimpleMeshGenerator.h>
#include <chaos/SkyBoxTools.h>
#include <chaos/GeometryFramework.h>
#include <chaos/GPUProgram.h>
#include <chaos/GPUTexture.h>
#include <chaos/GPUProgramProvider.h>

class MyGLFWWindowOpenGLTest1 : public chaos::MyGLFW::Window
{

protected:

	virtual bool OnKeyEvent(int key, int scan_code, int action, int modifier) override
	{
		if (key == GLFW_KEY_KP_ADD && action == GLFW_RELEASE)
		{
			ChangeTexture(texture_index + 1);
			return true;
		}
		else if (key == GLFW_KEY_KP_SUBTRACT && action == GLFW_RELEASE)
		{
			ChangeTexture(texture_index - 1);
			return true;
		}
		return chaos::MyGLFW::Window::OnKeyEvent(key, scan_code, action, modifier);
	}

	void ChangeTexture(int index)
	{
		chaos::shared_ptr<chaos::GPUTexture> new_texture = GenerateTexture(index);
		if (new_texture != nullptr)
		{
			texture_index = index;
			texture = new_texture;
		}
	}

	chaos::shared_ptr<chaos::GPUTexture> GenerateTexture(int index)
	{
		chaos::shared_ptr<chaos::GPUTexture> result;

    int texture_index  = index / 4;
    int texture_offset = index % 4;


		if (texture_index < 0 || texture_index >= (int)texture_paths.size())
			return result;

#if 1

    FIBITMAP * image = chaos::ImageTools::LoadImageFromFile(texture_paths[texture_index]);
    if (image != nullptr)
    {
      int k = 5 * texture_offset + texture_offset;

      chaos::ImageDescription desc = chaos::ImageTools::GetImageDescription(image);

      chaos::ImageDescription sub_desc = desc.GetSubImageDescription(k, k, desc.width - 2 * k, desc.height - 2 * k);

      result = chaos::GPUTextureLoader().GenTextureObject(sub_desc);

      FreeImage_Unload(image);
    }

#else
		result = chaos::GPUTextureLoader().GenTextureObject(texture_paths[index]);
#endif

		return result;
	}

	virtual bool OnDraw(chaos::GPURenderer * renderer, chaos::box2 const & viewport, glm::ivec2 window_size) override
	{
		glm::vec4 clear_color(0.0f, 0.0f, 0.0f, 0.0f);
		glClearBufferfv(GL_COLOR, 0, (GLfloat*)&clear_color);

		float far_plane = 1000.0f;
		glClearBufferfi(GL_DEPTH_STENCIL, 0, far_plane, 0);

		glEnable(GL_DEPTH_TEST);
		glEnable(GL_CULL_FACE);

		chaos::GPUProgramProvider uniform_provider;
		uniform_provider.AddVariableTexture("material", texture);

		chaos::RenderParams render_params;
		mesh->Render(renderer, program.get(), &uniform_provider, render_params);

		return true;
	}

	virtual void Finalize() override
	{
		program = nullptr;
		mesh    = nullptr;
		texture = nullptr;
	}

	virtual bool InitializeFromConfiguration(nlohmann::json const & config, boost::filesystem::path const & config_path) override
	{   
		chaos::Application * application = chaos::Application::GetInstance();
		if (application == nullptr)
			return false;

		boost::filesystem::path resources_path       = application->GetResourcesPath();
		boost::filesystem::path fragment_shader_path = resources_path / "pixel_shader.txt";
		boost::filesystem::path vertex_shader_path   = resources_path / "vertex_shader.txt";

		boost::filesystem::path image_path = resources_path / "Images";


		boost::filesystem::directory_iterator end;
		for (boost::filesystem::directory_iterator it = chaos::FileTools::GetDirectoryIterator(image_path); it != end; ++it)
			texture_paths.push_back(it->path());

		texture = GenerateTexture(texture_index);
		if (texture == nullptr)
			return false;

		chaos::GPUProgramGenerator program_generator;
		program_generator.AddShaderSourceFile(GL_FRAGMENT_SHADER, fragment_shader_path);
		program_generator.AddShaderSourceFile(GL_VERTEX_SHADER,   vertex_shader_path);

		program = program_generator.GenProgramObject();
		if (program == nullptr)
			return false;

		// create the mesh
		chaos::box2 b = chaos::box2(glm::vec2(0.0f, 0.0f), glm::vec2(1.0f, 1.0f));

		mesh = chaos::QuadMeshGenerator(b).GenerateMesh();
		if (mesh == nullptr)
			return false;

		return true;
	}

	virtual void TweakHints(chaos::MyGLFW::WindowHints & hints, GLFWmonitor * monitor, bool pseudo_fullscreen) const override
	{
		chaos::MyGLFW::Window::TweakHints(hints, monitor, pseudo_fullscreen);
		hints.toplevel  = 0;
		hints.decorated = 1;
	}

protected:

	std::vector<boost::filesystem::path> texture_paths;

	chaos::shared_ptr<chaos::GPUProgram>  program;
	chaos::shared_ptr<chaos::SimpleMesh> mesh;
	chaos::shared_ptr<chaos::GPUTexture>    texture;

	int mipmap_level = 0;
	int texture_index = 0;
};



int CHAOS_MAIN(int argc, char ** argv, char ** env)
{
  chaos::MyGLFW::SingleWindowApplicationParams params;
  params.monitor = nullptr;
  params.width = 500;
  params.height = 500;
  params.monitor_index = 0;
  chaos::MyGLFW::RunWindowApplication<MyGLFWWindowOpenGLTest1>(argc, argv, env, params);

	return 0;
}


