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
#include <chaos/GPUSimpleMeshGenerator.h>
#include <chaos/SkyBoxTools.h>
#include <chaos/GeometryFramework.h>
#include <chaos/GPUProgram.h>
#include <chaos/GPUTexture.h>
#include <chaos/GPUProgramProvider.h>
#include <chaos/GPURenderParams.h>

class MyGLFWWindowOpenGLTest1 : public chaos::MyGLFW::Window
{

protected:

	virtual bool OnKeyEventImpl(chaos::KeyEvent const & event) override
	{
		if (event.IsKeyReleased(GLFW_KEY_KP_ADD))
		{
			ChangeTexture(texture_index + 1);
			return true;
		}
		else if (event.IsKeyReleased(GLFW_KEY_KP_SUBTRACT))
		{
			ChangeTexture(texture_index - 1);
			return true;
		}
		else if (event.IsKeyReleased(GLFW_KEY_KP_ENTER))
		{
			ChangeTextureLevel();
			return true;
		}
		return chaos::MyGLFW::Window::OnKeyEventImpl(event);
	}

	void ChangeTextureLevel()
	{
		if (texture != nullptr)
		{
			chaos::ImageDescription desc;

			char * buffer = chaos::GLTextureTools::GetTextureImage(texture->GetResourceID(), 0, desc);
			if (buffer != nullptr)
			{
				chaos::shared_ptr<chaos::GPUTexture> new_texture = chaos::GPUTextureLoader().GenTextureObject(desc.GetSubImageDescription(0, 0, desc.width - 7, desc.height - 7));
				if (new_texture != nullptr)
					texture = new_texture;
			
				delete [](buffer);
			}
		}
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

	virtual void OnDropFile(int count, char const ** paths) override
	{
		for (int i = 0; i < count; ++i)
			if (GenerateTextureFromFilename(paths[i]))
				return;
	}

	bool GenerateTextureFromFilename(char const * filename)
	{
		chaos::shared_ptr<chaos::GPUTexture> new_texture = chaos::GPUTextureLoader().GenTextureObject(filename);
		if (new_texture == nullptr)
			return false;
		texture = new_texture;
		return true;
	}

	chaos::shared_ptr<chaos::GPUTexture> GenerateTexture(int index)
	{
		static int const GENERATE_GRAY = 0;
		static int const GENERATE_RGB  = 1;
		static int const GENERATE_RGBA = 2;
		static int const GENERATE_GRAY_FLOAT = 3;
		static int const GENERATE_RGB_FLOAT = 4;
		static int const GENERATE_RGBA_FLOAT = 5;
		static int const GENERATE_BACKGROUND = 6;
		static int const GENERATE_BACKGROUND_GRAY = 7;
		static int const GENERATE_FLOAT_BACKGROUND = 8;

		chaos::shared_ptr<chaos::GPUTexture> result;

		static int TEXTURE_SIZE = 512;

		// test GENERATION GRAY
		if (index == GENERATE_GRAY)
		{
			result = chaos::GPUTextureLoader().GenTextureObject<chaos::PixelGray>(TEXTURE_SIZE, TEXTURE_SIZE, [](chaos::ImageDescription & desc)
			{
				for (int i = 0; i < desc.height; ++i)
				{
					chaos::PixelGray * line = chaos::ImageTools::GetPixelAddress<chaos::PixelGray>(desc, 0, i);

					for (int j = 0; j < desc.width; ++j)
						line[j] = (unsigned char)i;
				}
			});
		}
		// test GENERATION RGB
		if (index == GENERATE_RGB)
		{
			result = chaos::GPUTextureLoader().GenTextureObject<chaos::PixelBGR>(TEXTURE_SIZE, TEXTURE_SIZE, [](chaos::ImageDescription & desc)
			{
				for (int i = 0; i < desc.height; ++i)
				{
					chaos::PixelBGR * line = chaos::ImageTools::GetPixelAddress<chaos::PixelBGR>(desc, 0, i);

					for (int j = 0; j < desc.width; ++j)
					{
						line[j].R = (unsigned char)i;
						line[j].G = 0;
						line[j].B = 0;
					}
				}
			});
		}
		// test GENERATION RGBA
		if (index == GENERATE_RGBA)
		{
			result = chaos::GPUTextureLoader().GenTextureObject<chaos::PixelBGRA>(TEXTURE_SIZE, TEXTURE_SIZE, [](chaos::ImageDescription & desc)
			{
				for (int i = 0; i < desc.height; ++i)
				{
					chaos::PixelBGRA * line = chaos::ImageTools::GetPixelAddress<chaos::PixelBGRA>(desc, 0, i);

					for (int j = 0; j < desc.width; ++j)
					{
						line[j].R = (unsigned char)j;
						line[j].G = 0;
						line[j].B = 0;
						line[j].A = (char)127;
					}
				}
			});
		}
		// test GENERATION GRAY FLOAT
		if (index == GENERATE_GRAY_FLOAT)
		{
			result = chaos::GPUTextureLoader().GenTextureObject<chaos::PixelGrayFloat>(TEXTURE_SIZE, TEXTURE_SIZE, [](chaos::ImageDescription & desc)
			{
				for (int i = 0; i < desc.height; ++i)
				{
					chaos::PixelGrayFloat * line = chaos::ImageTools::GetPixelAddress<chaos::PixelGrayFloat>(desc, 0, i);

					for (int j = 0; j < desc.width; ++j)
						line[j] = chaos::MathTools::CastAndDiv<float>(i, desc.height);
				}
			});
		}

		// test GENERATION RGB FLOAT
		if (index == GENERATE_RGB_FLOAT)
		{
			result = chaos::GPUTextureLoader().GenTextureObject<chaos::PixelRGBFloat>(TEXTURE_SIZE, TEXTURE_SIZE, [](chaos::ImageDescription & desc)
			{
				for (int i = 0; i < desc.height; ++i)
				{
					chaos::PixelRGBFloat * line = chaos::ImageTools::GetPixelAddress<chaos::PixelRGBFloat>(desc, 0, i);

					for (int j = 0; j < desc.width; ++j)
					{
						line[j].R = chaos::MathTools::CastAndDiv<float>(j, desc.width);
						line[j].G = chaos::MathTools::CastAndDiv<float>(i, desc.height);
						line[j].B = 0.0f;
					}
				}
			});
		}

		// test GENERATION RGBA FLOAT
		if (index == GENERATE_RGBA_FLOAT)
		{
			result = chaos::GPUTextureLoader().GenTextureObject<chaos::PixelRGBAFloat>(TEXTURE_SIZE, TEXTURE_SIZE, [](chaos::ImageDescription & desc)
			{
				for (int i = 0; i < desc.height; ++i)
				{
					chaos::PixelRGBAFloat * line = chaos::ImageTools::GetPixelAddress<chaos::PixelRGBAFloat>(desc, 0, i);

					for (int j = 0; j < desc.width; ++j)
					{
						line[j].R = chaos::MathTools::CastAndDiv<float>(j, desc.width);
						line[j].G = 0.0f;
						line[j].B = chaos::MathTools::CastAndDiv<float>(i, desc.height); 
						line[j].A = 1.0f;
					}
				}
			});
		}

		// test for background
		if (index == GENERATE_BACKGROUND)
		{
			FIBITMAP * image = chaos::ImageTools::GenFreeImage(chaos::PixelFormat(chaos::PixelComponentType::TYPE_UNSIGNED_CHAR, 4), TEXTURE_SIZE, TEXTURE_SIZE);
			if (image != nullptr)
			{
				chaos::ImageDescription image_description = chaos::ImageTools::GetImageDescription(image);

				chaos::ImageTools::FillImageBackground(image_description, glm::vec4(0.0f, 1.0f, 0.0f, 0.9f));

				result = chaos::GPUTextureLoader().GenTextureObject(image);
				FreeImage_Unload(image);
			}
		}

		if (index == GENERATE_BACKGROUND_GRAY)
		{
			FIBITMAP * image = chaos::ImageTools::GenFreeImage(chaos::PixelFormat(chaos::PixelComponentType::TYPE_UNSIGNED_CHAR, 1), TEXTURE_SIZE, TEXTURE_SIZE);
			if (image != nullptr)
			{
				chaos::ImageDescription image_description = chaos::ImageTools::GetImageDescription(image);

				chaos::ImageTools::FillImageBackground(image_description, glm::vec4(1.0f, 0.0f, 0.0f, 0.9f));

				result = chaos::GPUTextureLoader().GenTextureObject(image);
				FreeImage_Unload(image);
			}
		}

		if (index == GENERATE_FLOAT_BACKGROUND)
		{
			FIBITMAP * image = chaos::ImageTools::GenFreeImage(chaos::PixelFormat(chaos::PixelComponentType::TYPE_FLOAT, 4), TEXTURE_SIZE, TEXTURE_SIZE);
			if (image != nullptr)
			{
				chaos::ImageDescription image_description = chaos::ImageTools::GetImageDescription(image);

				chaos::ImageTools::FillImageBackground(image_description, glm::vec4(1.0f, 0.0f, 0.0f, 0.9f));

				result = chaos::GPUTextureLoader().GenTextureObject(image);
				FreeImage_Unload(image);
			}
		}

		return result;
	}

	virtual bool OnMouseButtonImpl(int button, int action, int modifier) override
	{
		if (button == 0 && action == GLFW_RELEASE)
		{
			chaos::TextureDescription desc = texture->GetTextureDescription();

			int max_mipmap = chaos::GLTextureTools::GetMipmapLevelCount(desc.width, desc.height);

			mipmap_level = (mipmap_level + 1) % max_mipmap;
			glTextureParameteri(texture->GetResourceID(), GL_TEXTURE_BASE_LEVEL, mipmap_level); //GL_TEXTURE_MAX_LEVEL
			return true;
		}
		return false;
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
		uniform_provider.AddVariableValue("screen_size", glm::vec2((float)window_size.x, (float)window_size.y));
		
		chaos::GPURenderParams render_params;
		mesh->Render(renderer, program.get(), &uniform_provider, render_params);

		return true;
	}

	virtual void Finalize() override
	{
		program = nullptr;
		mesh    = nullptr;
		texture = nullptr;
		chaos::MyGLFW::Window::Finalize();
	}

	virtual bool InitializeFromConfiguration(nlohmann::json const & config, boost::filesystem::path const & config_path) override
	{   
		if (!chaos::MyGLFW::Window::InitializeFromConfiguration(config, config_path))
			return false;

		chaos::Application * application = chaos::Application::GetInstance();
		if (application == nullptr)
			return false;

		boost::filesystem::path resources_path       = application->GetResourcesPath();
		boost::filesystem::path fragment_shader_path = resources_path / "pixel_shader.txt";
		boost::filesystem::path vertex_shader_path   = resources_path / "vertex_shader.txt";


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
		//chaos::box2 b = chaos::box2(glm::vec2(0.0f, 0.0f), glm::vec2(1.0f, 1.0f));
		//mesh = chaos::GPUQuadMeshGenerator(b).GenerateMesh();


		chaos::sphere2 s(glm::vec2(0.0f, 0.0f), 1.0f);
		mesh = chaos::GPUCircleMeshGenerator(s, glm::mat4x4(), 30).GenerateMesh();
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

	chaos::shared_ptr<chaos::GPUProgram>  program;
	chaos::shared_ptr<chaos::GPUSimpleMesh> mesh;
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


