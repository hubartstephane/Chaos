#include <chaos/StandardHeaders.h> 
#include <chaos/FileTools.h> 
#include <chaos/LogTools.h> 
#include <chaos/GLTools.h> 
#include <chaos/GLTextureTools.h>
#include <chaos/MyGLFWGamepadManager.h> 
#include <chaos/MyGLFWWindow.h> 
#include <chaos/WinTools.h> 
#include <chaos/GLProgramLoader.h>
#include <chaos/GLProgramData.h>
#include <chaos/Application.h>
#include <chaos/SimpleMeshGenerator.h>
#include <chaos/SkyBoxTools.h>
#include <chaos/GeometryFramework.h>
#include <chaos/GLProgram.h>
#include <chaos/Texture.h>
#include <chaos/GLProgramVariableProvider.h>

class MyGLFWWindowOpenGLTest1 : public chaos::MyGLFW::Window
{

protected:

	virtual void OnKeyEvent(int key, int scan_code, int action, int modifier) override
	{
		if (key == GLFW_KEY_KP_ADD && action == GLFW_RELEASE)
		{
			ChangeTexture(texture_index + 1);
		}
		else if (key == GLFW_KEY_KP_SUBTRACT && action == GLFW_RELEASE)
		{
			ChangeTexture(texture_index - 1);
		}
	}

	void ChangeTexture(int index)
	{
		boost::intrusive_ptr<chaos::Texture> new_texture = GenerateTexture(index);
		if (new_texture != nullptr)
		{
			texture_index = index;
			texture = new_texture;
		}
	}

	boost::intrusive_ptr<chaos::Texture> GenerateTexture(int index)
	{
		boost::intrusive_ptr<chaos::Texture> result;

    int texture_index  = index / 4;
    int texture_offset = index % 4;


		if (texture_index < 0 || texture_index >= (int)texture_paths.size())
			return result;

#if 1

    FIBITMAP * image = chaos::ImageTools::LoadImageFromFile(texture_paths[texture_index].string().c_str());
    if (image != nullptr)
    {
      int k = 5 * texture_offset + texture_offset;

      chaos::ImageDescription desc = chaos::ImageTools::GetImageDescription(image);

      chaos::ImageDescription sub_desc = desc.GetSubImageDescription(k, k, desc.width - 2 * k, desc.height - 2 * k);

      result = chaos::GLTextureTools::GenTextureObject(sub_desc);

      FreeImage_Unload(image);
    }

#else
		result = chaos::GLTextureTools::GenTextureObject(texture_paths[index].string().c_str());
#endif

		return result;
	}

	virtual bool OnDraw(glm::ivec2 size) override
	{
		glm::vec4 clear_color(0.0f, 0.0f, 0.0f, 0.0f);
		glClearBufferfv(GL_COLOR, 0, (GLfloat*)&clear_color);

		float far_plane = 1000.0f;
		glClearBufferfi(GL_DEPTH_STENCIL, 0, far_plane, 0);

		glViewport(0, 0, size.x, size.y);
		glEnable(GL_DEPTH_TEST);
		glEnable(GL_CULL_FACE);

		glUseProgram(program->GetResourceID());

		chaos::GLProgramData const & program_data = program->GetProgramData();

		chaos::GLProgramVariableProviderChain uniform_provider;
		uniform_provider.AddVariableTexture("material", texture);
		program_data.BindUniforms(&uniform_provider);

		mesh->Render(program_data, nullptr, 0, 0);

		return true;
	}

	virtual void Finalize() override
	{
		program = nullptr;
		mesh    = nullptr;
		texture = nullptr;
	}

	virtual bool Initialize(nlohmann::json configuration) override
	{   
		chaos::Application * application = chaos::Application::GetInstance();
		if (application == nullptr)
			return false;

		boost::filesystem::path resources_path       = application->GetResourcesPath();
		boost::filesystem::path fragment_shader_path = resources_path / "pixel_shader.txt";
		boost::filesystem::path vertex_shader_path   = resources_path / "vertex_shader.txt";

		boost::filesystem::path image_path = resources_path / "Images";


		boost::filesystem::directory_iterator end;
		for (boost::filesystem::directory_iterator it(image_path); it != end; ++it)
			texture_paths.push_back(it->path());

		texture = GenerateTexture(texture_index);
		if (texture == nullptr)
			return false;

		chaos::GLProgramLoader loader;
		loader.AddShaderSourceFile(GL_FRAGMENT_SHADER, fragment_shader_path);
		loader.AddShaderSourceFile(GL_VERTEX_SHADER,   vertex_shader_path);

		program = loader.GenerateProgramObject();
		if (program == nullptr)
			return false;

		// create the mesh
		chaos::box2 b = chaos::box2(glm::vec2(0.0f, 0.0f), glm::vec2(1.0f, 1.0f));

		mesh = chaos::QuadMeshGenerator(b).GenerateMesh();
		if (mesh == nullptr)
			return false;

		return true;
	}

	virtual void TweakSingleWindowApplicationHints(chaos::MyGLFW::WindowHints & hints, GLFWmonitor * monitor, bool pseudo_fullscreen) const override
	{
		chaos::MyGLFW::Window::TweakSingleWindowApplicationHints(hints, monitor, pseudo_fullscreen);
		hints.toplevel  = 0;
		hints.decorated = 1;
	}

protected:

	std::vector<boost::filesystem::path> texture_paths;

	boost::intrusive_ptr<chaos::GLProgram>  program;
	boost::intrusive_ptr<chaos::SimpleMesh> mesh;
	boost::intrusive_ptr<chaos::Texture>    texture;

	int mipmap_level{ 0 };
	int texture_index{ 0 };
};



int _tmain(int argc, char ** argv, char ** env)
{
	chaos::Application::Initialize<chaos::Application>(argc, argv, env);

	chaos::WinTools::AllocConsoleAndRedirectStdOutput();

	chaos::MyGLFW::SingleWindowApplicationParams params;
	params.monitor       = nullptr;
	params.width         = 500;
	params.height        = 500;
	params.monitor_index = 0;
	chaos::MyGLFW::Window::RunSingleWindowApplication<MyGLFWWindowOpenGLTest1>(params);

	chaos::Application::Finalize();

	return 0;
}


