#include "chaos/Chaos.h"

class WindowOpenGLTest : public chaos::Window
{
	CHAOS_DECLARE_OBJECT_CLASS(WindowOpenGLTest, chaos::Window);

protected:

	virtual bool EnumerateKeyActions(chaos::KeyActionEnumerator & in_action_enumerator) override
	{
		if (in_action_enumerator(IsKeyPressed(chaos::KeyboardButton::KP_ADD), "Next Texture", [this]()
		{
			ChangeTexture(texture_index + 1);
		}))
		{
			return true;
		}

		if (in_action_enumerator(IsKeyPressed(chaos::KeyboardButton::KP_SUBTRACT), "Previous Texture", [this]()
		{
			ChangeTexture(texture_index - 1);
		}))
		{
			return true;
		}

		if (in_action_enumerator(IsKeyPressed(chaos::KeyboardButton::KP_ENTER), "Change Current Level", [this]()
		{
			ChangeTextureLevel();
		}))
		{
			return true;
		}

		return chaos::Window::EnumerateKeyActions(in_action_enumerator);
	}

	void ChangeTextureLevel()
	{
		if (texture != nullptr)
		{
			chaos::ImageDescription desc;

			char * buffer = chaos::GLTextureTools::GetTextureImage(texture->GetResourceID(), 0, desc);
			if (buffer != nullptr)
			{
				chaos::shared_ptr<chaos::GPUTexture> new_texture = chaos::GPUTextureLoader(GetGPUDevice()).GenTextureObject(desc.GetSubImageDescription(0, 0, desc.width - 7, desc.height - 7));
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
		chaos::FilePathParam file_path(filename);
		chaos::shared_ptr<chaos::GPUTexture> new_texture = chaos::GPUTextureLoader(GetGPUDevice()).GenTextureObject(file_path);
		if (new_texture == nullptr)
			return false;
		texture = new_texture;
		return true;
	}

	chaos::shared_ptr<chaos::GPUTexture> GenerateTexture(int index)
	{
		static constexpr int GENERATE_GRAY = 0;
		static constexpr int GENERATE_RGB  = 1;
		static constexpr int GENERATE_RGBA = 2;
		static constexpr int GENERATE_GRAY_FLOAT = 3;
		static constexpr int GENERATE_RGB_FLOAT = 4;
		static constexpr int GENERATE_RGBA_FLOAT = 5;
		static constexpr int GENERATE_BACKGROUND = 6;
		static constexpr int GENERATE_BACKGROUND_GRAY = 7;
		static constexpr int GENERATE_FLOAT_BACKGROUND = 8;

		chaos::shared_ptr<chaos::GPUTexture> result;

		static int TEXTURE_SIZE = 512;

		// test GENERATION GRAY
		if (index == GENERATE_GRAY)
		{
			result = chaos::GPUTextureLoader(GetGPUDevice()).GenTextureObject<chaos::PixelGray>(TEXTURE_SIZE, TEXTURE_SIZE, [](chaos::ImageDescription & desc)
			{
				for (int i = 0; i < desc.height; ++i)
				{
					chaos::ImagePixelAccessor<chaos::PixelGray> accessor(desc);

					chaos::PixelGray * line = &accessor(0, i);

					for (int j = 0; j < desc.width; ++j)
						line[j] = (unsigned char)i;
				}
			});
		}
		// test GENERATION RGB
		if (index == GENERATE_RGB)
		{
			result = chaos::GPUTextureLoader(GetGPUDevice()).GenTextureObject<chaos::PixelBGR>(TEXTURE_SIZE, TEXTURE_SIZE, [](chaos::ImageDescription & desc)
			{
				for (int i = 0; i < desc.height; ++i)
				{
					chaos::ImagePixelAccessor<chaos::PixelBGR> accessor(desc);

					chaos::PixelBGR * line = &accessor(0, i);

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
			result = chaos::GPUTextureLoader(GetGPUDevice()).GenTextureObject<chaos::PixelBGRA>(TEXTURE_SIZE, TEXTURE_SIZE, [](chaos::ImageDescription & desc)
			{
				for (int i = 0; i < desc.height; ++i)
				{
					chaos::ImagePixelAccessor<chaos::PixelBGRA> accessor(desc);

					chaos::PixelBGRA * line = &accessor(0, i);

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
			result = chaos::GPUTextureLoader(GetGPUDevice()).GenTextureObject<chaos::PixelGrayFloat>(TEXTURE_SIZE, TEXTURE_SIZE, [](chaos::ImageDescription & desc)
			{
				for (int i = 0; i < desc.height; ++i)
				{
					chaos::ImagePixelAccessor<chaos::PixelGrayFloat> accessor(desc);

					chaos::PixelGrayFloat * line = &accessor(0, i);

					for (int j = 0; j < desc.width; ++j)
						line[j] = chaos::MathTools::CastAndDiv<float>(i, desc.height);
				}
			});
		}

		// test GENERATION RGB FLOAT
		if (index == GENERATE_RGB_FLOAT)
		{
			result = chaos::GPUTextureLoader(GetGPUDevice()).GenTextureObject<chaos::PixelRGBFloat>(TEXTURE_SIZE, TEXTURE_SIZE, [](chaos::ImageDescription & desc)
			{
				for (int i = 0; i < desc.height; ++i)
				{
					chaos::ImagePixelAccessor<chaos::PixelRGBFloat> accessor(desc);

					chaos::PixelRGBFloat* line = &accessor(0, i);

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
			result = chaos::GPUTextureLoader(GetGPUDevice()).GenTextureObject<chaos::PixelRGBAFloat>(TEXTURE_SIZE, TEXTURE_SIZE, [](chaos::ImageDescription & desc)
			{
				for (int i = 0; i < desc.height; ++i)
				{
					chaos::ImagePixelAccessor<chaos::PixelRGBAFloat> accessor(desc);

					chaos::PixelRGBAFloat* line = &accessor(0, i);

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
			FIBITMAP * image = chaos::ImageTools::GenFreeImage(chaos::PixelFormat(chaos::PixelComponentType::UNSIGNED_CHAR, 4), TEXTURE_SIZE, TEXTURE_SIZE);
			if (image != nullptr)
			{
				chaos::ImageDescription image_description = chaos::ImageTools::GetImageDescription(image);

				chaos::ImageTools::FillImageBackground(image_description, glm::vec4(0.0f, 1.0f, 0.0f, 0.9f));

				result = chaos::GPUTextureLoader(GetGPUDevice()).GenTextureObject(image);
				FreeImage_Unload(image);
			}
		}

		if (index == GENERATE_BACKGROUND_GRAY)
		{
			FIBITMAP * image = chaos::ImageTools::GenFreeImage(chaos::PixelFormat(chaos::PixelComponentType::UNSIGNED_CHAR, 1), TEXTURE_SIZE, TEXTURE_SIZE);
			if (image != nullptr)
			{
				chaos::ImageDescription image_description = chaos::ImageTools::GetImageDescription(image);

				chaos::ImageTools::FillImageBackground(image_description, glm::vec4(1.0f, 0.0f, 0.0f, 0.9f));

				result = chaos::GPUTextureLoader(GetGPUDevice()).GenTextureObject(image);
				FreeImage_Unload(image);
			}
		}

		if (index == GENERATE_FLOAT_BACKGROUND)
		{
			FIBITMAP * image = chaos::ImageTools::GenFreeImage(chaos::PixelFormat(chaos::PixelComponentType::FLOAT, 4), TEXTURE_SIZE, TEXTURE_SIZE);
			if (image != nullptr)
			{
				chaos::ImageDescription image_description = chaos::ImageTools::GetImageDescription(image);

				chaos::ImageTools::FillImageBackground(image_description, glm::vec4(1.0f, 0.0f, 0.0f, 0.9f));

				result = chaos::GPUTextureLoader(GetGPUDevice()).GenTextureObject(image);
				FreeImage_Unload(image);
			}
		}

		return result;
	}

	virtual bool OnMouseButtonImpl(chaos::MouseButtonEvent const &mouse_button_event) override
	{
		if (mouse_button_event.IsKeyReleased(chaos::MouseButton::BUTTON_1))
		{
			chaos::TextureDescription desc = texture->GetTextureDescription();

			int max_mipmap = chaos::GLTextureTools::GetMipmapLevelCount(desc.width, desc.height);

			mipmap_level = (mipmap_level + 1) % max_mipmap;
			glTextureParameteri(texture->GetResourceID(), GL_TEXTURE_BASE_LEVEL, mipmap_level); //GL_TEXTURE_MAX_LEVEL
			return true;
		}
		return false;
	}

	virtual bool OnDraw(chaos::GPURenderContext * render_context, chaos::GPUProgramProviderInterface const * uniform_provider, chaos::WindowDrawParams const& draw_params) override
	{
		glm::vec4 clear_color(0.0f, 0.0f, 0.0f, 0.0f);
		glClearBufferfv(GL_COLOR, 0, (GLfloat*)&clear_color);

		float far_plane = 1000.0f;
		glClearBufferfi(GL_DEPTH_STENCIL, 0, far_plane, 0);

		glEnable(GL_DEPTH_TEST);
		glEnable(GL_CULL_FACE);

		chaos::GPUProgramProviderChain main_uniform_provider(uniform_provider);
		main_uniform_provider.AddTexture("material", texture);
		main_uniform_provider.AddVariable("screen_size", glm::vec2(float(draw_params.viewport.size.x), float(draw_params.viewport.size.y)));

		chaos::GPURenderParams render_params;
		mesh->DisplayWithProgram(program.get(), render_context, &main_uniform_provider, render_params);

		return true;
	}

	virtual void Finalize() override
	{
		program = nullptr;
		mesh    = nullptr;
		texture = nullptr;
		chaos::Window::Finalize();
	}

	virtual bool OnInitialize(chaos::JSONReadConfiguration config) override
	{
		if (!chaos::Window::OnInitialize(config))
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
		program_generator.AddShaderSourceFile(chaos::ShaderType::FRAGMENT, fragment_shader_path);
		program_generator.AddShaderSourceFile(chaos::ShaderType::VERTEX,   vertex_shader_path);

		program = program_generator.GenProgramObject();
		if (program == nullptr)
			return false;

		// create the mesh
		//chaos::box2 b = chaos::box2(glm::vec2(0.0f, 0.0f), glm::vec2(1.0f, 1.0f));
		//mesh = chaos::GPUQuadMeshGenerator(b).GenerateMesh(GetGPUDevice());


		chaos::sphere2 s(glm::vec2(0.0f, 0.0f), 1.0f);
		mesh = chaos::GPUCircleMeshGenerator(s, glm::mat4x4(1.0f), 30).GenerateMesh(GetGPUDevice());
		if (mesh == nullptr)
			return false;

		return true;
	}

protected:

	chaos::shared_ptr<chaos::GPUProgram>  program;
	chaos::shared_ptr<chaos::GPUMesh> mesh;
	chaos::shared_ptr<chaos::GPUTexture>    texture;

	int mipmap_level = 0;
	int texture_index = 0;
};



int main(int argc, char ** argv, char ** env)
{
	return chaos::RunSimpleWindowApplication<WindowOpenGLTest>(argc, argv, env);
}