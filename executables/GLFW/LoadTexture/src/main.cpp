#include "chaos/Chaos.h"

class WindowOpenGLTest : public chaos::Window
{
	CHAOS_DECLARE_OBJECT_CLASS(WindowOpenGLTest, chaos::Window);

protected:

	virtual bool OnKeyEventImpl(chaos::KeyEvent const & event) override
	{
		if (event.IsKeyReleased(chaos::KeyboardButton::KP_ADD))
		{
			ChangeTexture(texture_index + 1);
			return true;
		}
		else if (event.IsKeyReleased(chaos::KeyboardButton::KP_SUBTRACT))
		{
			ChangeTexture(texture_index - 1);
			return true;
		}
		return chaos::Window::OnKeyEventImpl(event);
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

	virtual bool OnDraw(chaos::GPURenderer * renderer, chaos::GPUProgramProviderInterface const * uniform_provider, chaos::WindowDrawParams const& draw_params) override
	{
		glm::vec4 clear_color(0.0f, 0.0f, 0.0f, 0.0f);
		glClearBufferfv(GL_COLOR, 0, (GLfloat*)&clear_color);

		float far_plane = 1000.0f;
		glClearBufferfi(GL_DEPTH_STENCIL, 0, far_plane, 0);

		glEnable(GL_DEPTH_TEST);
		glEnable(GL_CULL_FACE);

		chaos::GPUProgramProviderChain main_uniform_provider(uniform_provider);
		main_uniform_provider.AddTexture("material", texture);

		chaos::GPURenderParams render_params;
		mesh->DisplayWithProgram(program.get(), renderer, &main_uniform_provider, render_params);

		return true;
	}

	virtual void Finalize() override
	{
		program = nullptr;
		mesh    = nullptr;
		texture = nullptr;
		chaos::Window::Finalize();
	}

	virtual bool InitializeFromConfiguration(nlohmann::json const * config) override
	{
		if (!chaos::Window::InitializeFromConfiguration(config))
			return false;

		chaos::Application * application = chaos::Application::GetInstance();
		if (application == nullptr)
			return false;

		boost::filesystem::path resources_path       = application->GetResourcesPath();
		boost::filesystem::path fragment_shader_path = resources_path / "pixel_shader.txt";
		boost::filesystem::path vertex_shader_path   = resources_path / "vertex_shader.txt";

		boost::filesystem::path image_path = resources_path / "Images";


		chaos::FileTools::WithDirectoryContent(image_path, [this](boost::filesystem::path const& p)
		{
			texture_paths.push_back(p);
			return false; // do not stop
		});
		texture = GenerateTexture(texture_index);
		if (texture == nullptr)
			return false;

		chaos::GPUProgramGenerator program_generator;
		program_generator.AddShaderSourceFile(chaos::ShaderType::FRAGMENT, fragment_shader_path);
		program_generator.AddShaderSourceFile(chaos::ShaderType::VERTEX,   vertex_shader_path);

		program = program_generator.GenProgramObject();
		if (program == nullptr)
			return false;

		// create the mesh
		chaos::box2 b = chaos::box2(glm::vec2(0.0f, 0.0f), glm::vec2(1.0f, 1.0f));

		mesh = chaos::GPUQuadMeshGenerator(b).GenerateMesh();
		if (mesh == nullptr)
			return false;

		return true;
	}

protected:

	std::vector<boost::filesystem::path> texture_paths;

	chaos::shared_ptr<chaos::GPUProgram>  program;
	chaos::shared_ptr<chaos::GPUMesh> mesh;
	chaos::shared_ptr<chaos::GPUTexture>    texture;

	int mipmap_level = 0;
	int texture_index = 0;
};



int main(int argc, char ** argv, char ** env)
{
	chaos::WindowCreateParams create_params;
	create_params.monitor = nullptr;
	create_params.width = 500;
	create_params.height = 500;
	create_params.monitor_index = 0;

	return chaos::RunWindowApplication<WindowOpenGLTest>(argc, argv, env, create_params);
}