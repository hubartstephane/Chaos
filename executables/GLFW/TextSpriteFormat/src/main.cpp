#include "chaos/Chaos.h"

// --------------------------------------------------------------------

class WindowOpenGLTest : public chaos::Window
{
	CHAOS_DECLARE_OBJECT_CLASS(WindowOpenGLTest, chaos::Window);

protected:

	virtual bool OnDraw(chaos::GPURenderContext * render_context, chaos::GPUProgramProviderInterface const * uniform_provider, chaos::WindowDrawParams const& draw_params) override
	{
		glm::vec4 clear_color(0.0f, 0.0f, 0.7f, 0.0f);
		glClearBufferfv(GL_COLOR, 0, (GLfloat*)&clear_color);

		float far_plane = 1000.0f;
		glClearBufferfi(GL_DEPTH_STENCIL, 0, far_plane, 0);

		glEnable(GL_DEPTH_TEST);
		glEnable(GL_CULL_FACE);   // when viewer is inside the cube

		// XXX : the scaling is used to avoid the near plane clipping
		chaos::box3 b(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(1.0f, 1.0f, 1.0f));

		static float FOV = 60.0f;
		glm::mat4 projection = glm::perspectiveFov(FOV * (float)M_PI / 180.0f, float(draw_params.viewport.size.x), float(draw_params.viewport.size.y), 1.0f, far_plane);
		glm::mat4 world_to_camera = fps_view_controller.GlobalToLocal();
		glm::mat4 local_to_world = glm::translate(b.position) * glm::scale(b.half_size);


		class MyProvider : public chaos::GPUProgramProviderBase
		{
		protected:

			virtual bool DoProcessAction(chaos::GPUProgramProviderExecutionData const& execution_data) const override
			{
				if (auto lock = execution_data.CanDeduce("local_to_cam"))
				{
					glm::mat4 translate_mat;
					glm::mat4 scale_mat;

					if (execution_data.GetValue<glm::mat4>("translate_mat", translate_mat))
						if (execution_data.GetValue<glm::mat4>("scale_mat", scale_mat))
							return execution_data.Process(translate_mat * scale_mat);
				}
				return false;
			}
		};

		float w = (float)draw_params.viewport.size.x;
		float h = (float)draw_params.viewport.size.y;
		glm::vec3 scale = glm::vec3(2.0f / w, 2.0f / h, 1.0f);
		glm::vec3 tr = glm::vec3(-1.0f, -1.0f, 0.0f);

		chaos::shared_ptr<MyProvider> dynamic_provider = new MyProvider;
		chaos::GPUProgramProviderChain main_uniform_provider(uniform_provider);
		main_uniform_provider.AddVariable("translate_mat", glm::translate(tr));
		main_uniform_provider.AddVariable("scale_mat", glm::scale(scale));
		main_uniform_provider.AddVariable("toto", glm::vec2(5.0f, 6.0f));
		main_uniform_provider.AddProvider(dynamic_provider.get());

		chaos::GPURenderParams render_params;
		particle_manager->Display(render_context, &main_uniform_provider, render_params);

		return true;
	}

	virtual void Finalize() override
	{
		particle_manager = nullptr;
		atlas = nullptr;
		material = nullptr;
		particles_allocation = nullptr;
		chaos::Window::Finalize();
	}

	bool LoadAtlas(boost::filesystem::path const & resources_path)
	{
		atlas = new chaos::GPUAtlas;
		if (atlas == nullptr)
			return false;
		return atlas->LoadAtlas(resources_path / "Atlas" / "MyAtlas.json");
	}

	bool InitializeParticleManager(boost::filesystem::path const & resources_path)
	{
		// create the material
		material = chaos::DefaultParticleProgram::GetMaterial();
		if (material == nullptr)
			return false;
		// create the particle manager
		particle_manager = new chaos::ParticleManager;
		if (particle_manager == nullptr)
			return false;
		particle_manager->SetTextureAtlas(atlas.get());
		// create the layer
		int render_order = 0;
		int layer_id = 0;
		chaos::ParticleLayerBase * layer = particle_manager->AddLayer<chaos::ParticleDefaultLayerTrait>(render_order, layer_id, material.get());
		if (layer == nullptr)
			return false;

		// parse the text
		chaos::ParticleTextGenerator::Generator generator(*atlas);
		generator.AddColor("red", glm::vec4(1.0f, 0.0f, 0.0f, 1.0f));
		generator.AddColor("blue", glm::vec4(0.0f, 0.0f, 1.0f, 1.0f));
		generator.AddColor("green", glm::vec4(0.0f, 1.0f, 0.0f, 1.0f));

		chaos::AtlasFolderInfo const * folder = atlas->GetFolderInfo("folder_input1");
		if (folder != nullptr)
		{
			chaos::AtlasBitmapInfo const * bitmap = folder->GetBitmapInfo("xboxControllerButtonA");
			if (bitmap != nullptr)
				generator.AddBitmap("BUTTON", bitmap);
		}

		generator.AddFontInfo("C1", atlas->GetAtlasFontInfo("font_info1"));
		generator.AddFontInfo("C2", atlas->GetAtlasFontInfo("font_info2"));

		chaos::ParticleTextGenerator::GeneratorParams generator_params;
		generator_params.line_height = 50;
		generator_params.font_info_name = "font_info1"; // the default character set
		generator_params.position = glm::vec2(0.0f, 0.0f);
		generator_params.hotpoint = chaos::Hotpoint::BOTTOM_LEFT;
		generator_params.character_spacing = 0.0f;
		generator_params.line_spacing = 0.0f;
		generator_params.bitmap_padding.x = 1.0f * 30.0f;
		generator_params.bitmap_padding.y = 5.0f;
		generator_params.alignment = chaos::TextAlignment::LEFT;
		generator_params.justify_space_factor = 8.0f;
		generator_params.max_text_width = 200.0f;
		generator_params.word_wrap = true;

		//std::string message = "Hi [RED Ben]\nHere is a first example\nof my own stuff with [button][button][button] embedded";

		std::string message = "HELLO [GREEN Hi [RED] [C2] Ben [BLUE] truc] bidule";

		chaos::ParticleTextGenerator::GeneratorResult generator_result;
		generator.Generate(message.c_str(), generator_result, generator_params);

		// create a text allocation
		particles_allocation = chaos::ParticleTextGenerator::CreateTextAllocation(layer, generator_result);

		return true;
	}

	virtual bool OnInitialize(chaos::JSONReadConfiguration config) override
	{
		if (!chaos::Window::OnInitialize(config))
			return false;

		chaos::Application * application = chaos::Application::GetInstance();
		if (application == nullptr)
			return false;

		// compute resource path
		boost::filesystem::path resources_path = application->GetResourcesPath();

		// initialize the atlas
		if (!LoadAtlas(resources_path))
			return false;

		// initialize the sprite manager
		if (!InitializeParticleManager(resources_path))
			return false;

		// place camera
		fps_view_controller.fps_view.position.y = 0.0f;
		fps_view_controller.fps_view.position.z = 10.0f;

		return true;
	}

	virtual bool DoTick(float delta_time) override
	{
		particle_manager->Tick(delta_time);
		return true; // refresh
	}

	virtual bool TraverseInputEventReceiverHierarchy(chaos::TraverseInputEventReceiverFunc event_func) override
	{
		if (fps_view_controller.TraverseInputEventReceiverHierarchy(event_func))
			return true;
		return chaos::Window::TraverseInputEventReceiverHierarchy(event_func);
	}

protected:

	// the particle manager
	chaos::shared_ptr<chaos::ParticleManager> particle_manager;
	/** the texture atlas */
	chaos::shared_ptr<chaos::GPUAtlas> atlas;
	/** the material */
	chaos::shared_ptr<chaos::GPURenderMaterial> material;
	/** allocation */
	chaos::shared_ptr<chaos::ParticleAllocationBase> particles_allocation;

	// the camera
	chaos::FPSViewController fps_view_controller;
};








int main(int argc, char ** argv, char ** env)
{
	return chaos::RunSimpleWindowApplication<WindowOpenGLTest>(argc, argv, env);
}