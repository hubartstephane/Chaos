#include "chaos/Chaos.h"

std::vector<chaos::PixelFormat> pixel_formats =
{
	chaos::PixelFormat::Gray,
	chaos::PixelFormat::GrayFloat,
	chaos::PixelFormat::BGR,
	chaos::PixelFormat::BGRA,
	chaos::PixelFormat::RGBFloat,
	chaos::PixelFormat::RGBAFloat
};

class WindowOpenGLTest : public chaos::Window
{
	CHAOS_DECLARE_OBJECT_CLASS(WindowOpenGLTest, chaos::Window);

protected:

	virtual bool OnKeyEventImpl(chaos::KeyEvent const & event) override
	{
		if (event.IsKeyReleased(chaos::KeyboardButton::KP_ADD))
		{
			ChangeSkyBox(skybox_index + 1);
			return true;
		}
		else if (event.IsKeyReleased(chaos::KeyboardButton::KP_SUBTRACT))
		{
			ChangeSkyBox(skybox_index - 1);
			return true;
		}
		return chaos::Window::OnKeyEventImpl(event);
	}

	void ChangeSkyBox(int index)
	{







		chaos::shared_ptr<chaos::GPUTexture> new_texture = GenerateSkyBox(index);
		if (new_texture != nullptr)
		{
			skybox_index = index;
			texture = new_texture;
			debug_display.Clear();

			chaos::PixelFormat pf = pixel_formats[index];

			char const * component_type = (pf.component_type == chaos::PixelComponentType::UNSIGNED_CHAR) ? "unsigned char" : "float";

			debug_display.AddLine(chaos::StringTools::Printf("format : index = [%d] component = [%d] type = [%s]", index, pf.component_count, component_type).c_str());
		}
	}

	chaos::shared_ptr<chaos::GPUTexture> GenerateSkyBox(int index)
	{
		if (index < 0 || index >= pixel_formats.size())
			return nullptr;

		chaos::PixelFormat pixel_format = pixel_formats[index];
		if (!pixel_format.IsValid())
			return nullptr;

		chaos::PixelFormatMergeParams merge_params;
		merge_params.pixel_format = pixel_format;

#if 0

		// let OpenGL do the conversion
		return chaos::GPUTextureLoader().GenTextureObject(&skybox, merge_params);

#else

		// do the conversion ourselves
		chaos::SkyBoxImages single_skybox = skybox.ToSingleImage(true, glm::vec4(0.0f, 0.0f, 0.0f, 1.0f), merge_params);

		if (!single_skybox.IsEmpty())
			return chaos::GPUTextureLoader().GenTextureObject(&single_skybox);

#endif

		return nullptr;
	}

	virtual bool OnDraw(chaos::GPURenderer * renderer, chaos::GPUProgramProviderInterface const * uniform_provider, chaos::WindowDrawParams const& draw_params) override
	{
		glm::vec4 clear_color(0.0f, 0.0f, 0.0f, 0.0f);
		glClearBufferfv(GL_COLOR, 0, (GLfloat*)&clear_color);

		float far_plane = 1000.0f;
		glClearBufferfi(GL_DEPTH_STENCIL, 0, far_plane, 0);

		glEnable(GL_DEPTH_TEST);
		glDisable(GL_CULL_FACE);   // when viewer is inside the cube

		// XXX : the scaling is used to avoid the near plane clipping
		static float FOV =  60.0f;
		glm::mat4 projection_matrix      = glm::perspectiveFov(FOV * (float)M_PI / 180.0f, 2.0f * float(draw_params.viewport.size.x), float(draw_params.viewport.size.y), 1.0f, far_plane);
		glm::mat4 local_to_world_matrix  = glm::scale(glm::vec3(10.0f, 10.0f, 10.0f));
		glm::mat4 world_to_camera_matrix = fps_view_controller.GlobalToLocal();

		chaos::GPUProgramProviderChain main_uniform_provider(uniform_provider);
		main_uniform_provider.AddVariable("projection",      projection_matrix);
		main_uniform_provider.AddVariable("local_to_world",  local_to_world_matrix);
		main_uniform_provider.AddVariable("world_to_camera", world_to_camera_matrix);
		main_uniform_provider.AddTexture("material", texture);

		chaos::GPURenderParams render_params;
		mesh->DisplayWithProgram(program.get(), renderer, &main_uniform_provider, render_params);

		debug_display.Display(renderer, int(draw_params.viewport.size.x), int(draw_params.viewport.size.y));

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
		chaos::Window::Finalize();
	}

	bool LoadSkyboxBitmaps(boost::filesystem::path const & resources_path)
	{
		// load the images
		chaos::FileTools::WithDirectoryContent(resources_path / "images", [this](boost::filesystem::path const &p)
		{
			FIBITMAP * bitmap = chaos::ImageTools::LoadImageFromFile(p);
			if (bitmap != nullptr)
				skybox_bitmaps.push_back(bitmap);
			return false; // don't stop
		});

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

			void * color = (desc.pixel_format.component_type == chaos::PixelComponentType::UNSIGNED_CHAR) ? (void*)&c1[0] : (void*)&c2; // select a color for background

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
			skybox.SetImage((chaos::SkyBoxImageType)i, skybox_bitmaps[i], false);
		}
		return true;
	}

	virtual bool InitializeFromConfiguration(nlohmann::json const * config) override
	{
		if (!chaos::Window::InitializeFromConfiguration(config))
			return false;

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

		chaos::GPUProgramGenerator program_generator;
		program_generator.AddShaderSourceFile(chaos::ShaderType::FRAGMENT, resources_path / "pixel_shader_cube.txt");
		program_generator.AddShaderSourceFile(chaos::ShaderType::VERTEX,   resources_path / "vertex_shader.txt");

		program = program_generator.GenProgramObject();
		if (program == nullptr)
			return false;

		chaos::box3 b = chaos::box3(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(1.0f, 1.0f, 1.0f));

		mesh = chaos::GPUBoxMeshGenerator(b).GenerateMesh();
		if (mesh == nullptr)
			return false;

		return true;
	}

	virtual bool DoTick(float delta_time) override
	{
		fps_view_controller.Tick(glfw_window, delta_time);

		debug_display.Tick(delta_time);

		return true; // refresh
	}

	virtual bool OnMouseButtonImpl(int button, int action, int modifier) override
	{
		if (button == 1 && action == GLFW_RELEASE)
		{
			debug_display.AddLine("HelloWorld");
			return true;
		}
		return false;
	}

protected:

	std::vector<FIBITMAP*> skybox_bitmaps;

	chaos::SkyBoxImages skybox;

	chaos::shared_ptr<chaos::GPUProgram>  program;
	chaos::shared_ptr<chaos::GPUMesh> mesh;
	chaos::shared_ptr<chaos::GPUTexture>    texture;

	chaos::FPSViewController fps_view_controller;

	chaos::GLDebugOnScreenDisplay debug_display;

	int skybox_index = 0;
};

int main(int argc, char ** argv, char ** env)
{
	return chaos::RunWindowApplication<WindowOpenGLTest>(argc, argv, env);
}