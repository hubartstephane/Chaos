#include "chaos/Chaos.h"



class WindowOpenGLTest : public chaos::Window
{
	CHAOS_DECLARE_OBJECT_CLASS(WindowOpenGLTest, chaos::Window);

protected:

	virtual bool OnKeyEventImpl(chaos::KeyEvent const & key_event) override
	{
		if (key_event.IsKeyReleased(chaos::KeyboardButton::KP_ADD))
		{
			ChangeCubeMap(cubemap_index + 1);
			return true;
		}
		else if (key_event.IsKeyReleased(chaos::KeyboardButton::KP_SUBTRACT))
		{
			ChangeCubeMap(cubemap_index - 1);
			return true;
		}
		return chaos::Window::OnKeyEventImpl(key_event);
	}

	void ChangeCubeMap(int index)
	{







		chaos::shared_ptr<chaos::GPUTexture> new_texture = GenerateCubeMap(index);
		if (new_texture != nullptr)
		{
			cubemap_index = index;
			texture = new_texture;
			debug_display.Clear();

			chaos::PixelFormat pf = pixel_formats[index];

			char const * component_type = (pf.component_type == chaos::PixelComponentType::UNSIGNED_CHAR) ? "unsigned char" : "float";

			debug_display.AddLine(chaos::StringTools::Printf("format : index = [%d] component = [%d] type = [%s]", index, pf.component_count, component_type).c_str());
		}
	}

	chaos::shared_ptr<chaos::GPUTexture> GenerateCubeMap(int index)
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
		return chaos::GPUTextureLoader(GetDevice()).GenTextureObject(&cubemap, merge_params);

#else

		// do the conversion ourselves
		chaos::CubeMapImages single_cubemap = cubemap.ToSingleImage(true, glm::vec4(0.0f, 0.0f, 0.0f, 1.0f), merge_params);

		if (!single_cubemap.IsEmpty())
			return chaos::GPUTextureLoader(GetDevice()).GenTextureObject(&single_cubemap);

#endif

		return nullptr;
	}

	virtual bool OnDraw(chaos::GPURenderContext * render_context, chaos::GPUProgramProviderInterface const * uniform_provider, chaos::WindowDrawParams const& draw_params) override
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
		mesh->DisplayWithProgram(program.get(), render_context, &main_uniform_provider, render_params);

		return true;
	}

	virtual void Finalize() override
	{
		cubemap.Release();

		for (FIBITMAP * bitmap : cubemap_bitmaps)
			FreeImage_Unload(bitmap);
		cubemap_bitmaps.clear();

		program = nullptr;
		mesh    = nullptr;
		texture = nullptr;

		chaos::Window::Finalize();
	}

	bool LoadSkyboxBitmaps(boost::filesystem::path const & resources_path)
	{
		// load the images
		chaos::FileTools::WithDirectoryContent(resources_path / "images", [this](boost::filesystem::path const &p)
		{
			FIBITMAP * bitmap = chaos::ImageTools::LoadImageFromFile(p);
			if (bitmap != nullptr)
				cubemap_bitmaps.push_back(bitmap);
			return false; // don't stop
		});

		if (cubemap_bitmaps.size() == 0)
			return false;

		// search the minimum size
		int size = -1;
		for (FIBITMAP * bitmap : cubemap_bitmaps)
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

		for (FIBITMAP * & bitmap : cubemap_bitmaps)
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

		// generate the cubemap
		for (size_t i = 0; i < cubemap_bitmaps.size() ; ++i)
		{
			cubemap.SetImage((chaos::CubeMapImageType)i, cubemap_bitmaps[i], false);
		}
		return true;
	}

	virtual bool OnInitialize(chaos::JSONReadConfiguration config) override
	{
		if (!chaos::Window::OnInitialize(config))
			return false;

		chaos::Application * application = chaos::Application::GetInstance();
		if (application == nullptr)
			return false;

		boost::filesystem::path resources_path = application->GetResourcesPath();

		if (!LoadSkyboxBitmaps(resources_path))
			return false;

		debug_display.AddLine("Press +/- to change cubemap");

		texture = GenerateCubeMap(0);
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

	virtual void OnDrawImGuiContent() override
	{
		chaos::Window::OnDrawImGuiContent();

		chaos::ImGuiTools::FullViewportWindow("fullscreen", 0, [this]()
		{
			debug_display.OnDrawImGuiContent(this);
		});
	}

protected:

	std::vector<chaos::PixelFormat> pixel_formats =
	{
		chaos::PixelFormat::Gray,
		chaos::PixelFormat::GrayFloat,
		chaos::PixelFormat::BGR,
		chaos::PixelFormat::BGRA,
		chaos::PixelFormat::RGBFloat,
		chaos::PixelFormat::RGBAFloat
	};

	std::vector<FIBITMAP*> cubemap_bitmaps;

	chaos::CubeMapImages cubemap;

	chaos::shared_ptr<chaos::GPUProgram>  program;
	chaos::shared_ptr<chaos::GPUMesh> mesh;
	chaos::shared_ptr<chaos::GPUTexture>    texture;

	chaos::FPSViewController fps_view_controller;

	chaos::GLDebugOnScreenDisplay debug_display;

	int cubemap_index = 0;
};

int main(int argc, char ** argv, char ** env)
{
	return chaos::RunSimpleWindowApplication<WindowOpenGLTest>(argc, argv, env);
}