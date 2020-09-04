#include <chaos/StandardHeaders.h> 
#include <chaos/FileTools.h> 
#include <chaos/LogTools.h> 
#include <chaos/GLTools.h> 
#include <chaos/StringTools.h> 
#include <chaos/MyGLFWGamepadManager.h> 
#include <chaos/MyGLFWSingleWindowApplication.h> 
#include <chaos/MyGLFWWindow.h> 
#include <chaos/WinTools.h> 
#include <chaos/GPUProgramGenerator.h>
#include <chaos/Application.h>
#include <chaos/GPUSimpleMeshGenerator.h>
#include <chaos/SkyBoxTools.h>
#include <chaos/GLDebugOnScreenDisplay.h>
#include <chaos/FPSViewInputController.h>
#include <chaos/GPUSimpleMesh.h>
#include <chaos/GPUMultiMeshGenerator.h>
#include <chaos/GPUProgramData.h>
#include <chaos/GPUProgram.h>
#include <chaos/GPUVertexDeclaration.h>
#include <chaos/GPUProgramProvider.h>
#include <chaos/GPURenderer.h>
#include <chaos/GPURenderParams.h>

class MyGLFWWindowOpenGLTest1;

// ====================================================================

static constexpr int EVENT_SINGLE_TEST  = 0;
static constexpr int EVENT_RANGE_TEST   = 1;
static constexpr int EVENT_FOREVER_TEST = 2;

class MyEvent : public chaos::ClockEvent
{
public:

	MyEvent(char const * in_message, int in_type, MyGLFWWindowOpenGLTest1 * in_application) : 
		message(in_message), 
		type(in_type),
		application(in_application) {}

	~MyEvent();

	virtual chaos::ClockEventTickResult Tick(chaos::ClockEventTickData const & tick_data) override;

	virtual void OnEventRemovedFromClock() override;

protected:

	std::string message;
	int type;
	MyGLFWWindowOpenGLTest1 * application;
};

// ====================================================================

static glm::vec4 const red = glm::vec4(1.0f, 0.0f, 0.0f, 1.0f);
static glm::vec4 const green = glm::vec4(0.0f, 1.0f, 0.0f, 1.0f);
static glm::vec4 const blue = glm::vec4(0.0f, 0.0f, 1.0f, 1.0f);
static glm::vec4 const white = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);

class RenderingContext
{
public:

	chaos::GPURenderer * renderer = nullptr;

	glm::mat4 projection;
	glm::mat4 world_to_camera;
};

class PrimitiveRenderingContext
{
public:

	glm::mat4 local_to_world;
	glm::vec4 color;
};

// ====================================================================

class MyGLFWWindowOpenGLTest1 : public chaos::MyGLFW::Window
{
	friend class MyEvent;

protected:

	void DebugDisplayTips()
	{
		debug_display.Clear();
		debug_display.AddLine(chaos::StringTools::Printf("update clock [%s] with NUM1 & NUM2 : %f", clock1->GetClockName(), clock1->GetTimeScale()).c_str());
		debug_display.AddLine(chaos::StringTools::Printf("update clock [%s] with NUM4 & NUM5 : %f", clock2->GetClockName(), clock2->GetTimeScale()).c_str());
		debug_display.AddLine(chaos::StringTools::Printf("update clock [%s] with NUM7 & NUM8 : %f", clock3->GetClockName(), clock3->GetTimeScale()).c_str());
		debug_display.AddLine("Press A to generate an Event on Clock 1 : SingleTickEvent");
		debug_display.AddLine("Press Z to generate an Event on Clock 2 : RangeEvent");
		debug_display.AddLine("Press E to generate an Event on Clock 3 : Forever Event");
		debug_display.AddLine("Press T to pause");
	}

	void PrepareObjectProgram(chaos::GPUProgramProvider & uniform_provider, RenderingContext const & ctx, PrimitiveRenderingContext const & prim_ctx)
	{
		uniform_provider.AddVariableValue("projection", ctx.projection);
		uniform_provider.AddVariableValue("world_to_camera", ctx.world_to_camera);
		uniform_provider.AddVariableValue("local_to_world", prim_ctx.local_to_world);
		uniform_provider.AddVariableValue("color", prim_ctx.color);
	}

	void DrawPrimitiveImpl(RenderingContext const & ctx, chaos::GPUSimpleMesh * mesh, chaos::GPUProgram * program, glm::vec4 const & color, glm::mat4 const & local_to_world)
	{
		glm::vec4 final_color = color;

		PrimitiveRenderingContext prim_ctx;
		prim_ctx.local_to_world = local_to_world;
		prim_ctx.color = final_color;

    chaos::GPUProgramProvider uniform_provider;
		PrepareObjectProgram(uniform_provider, ctx, prim_ctx);

		chaos::GPURenderParams render_params;
		mesh->Render(ctx.renderer, program, &uniform_provider, render_params);
	}

	void GPUDrawPrimitive(RenderingContext const & ctx, chaos::box3 const & b, glm::vec4 const & color)
	{
		if (IsGeometryEmpty(b))
			return;

		glm::mat4 local_to_world = glm::translate(b.position) * glm::scale(b.half_size);

		DrawPrimitiveImpl(ctx, mesh_box.get(), program_box.get(), color, local_to_world);
	}

	void DrawGeometryObjects(RenderingContext const & ctx)
	{
		double realtime1 = clock1->GetClockTime();
		double realtime2 = clock2->GetClockTime();
		double realtime3 = clock3->GetClockTime();

		float Y1 = 5.0f * (float)std::cos(realtime1);
		float Y2 = 5.0f * (float)std::cos(realtime2);
		float Y3 = 5.0f * (float)std::cos(realtime3);

		chaos::box3 b1(glm::vec3(0.0f, Y1, 0.0f), glm::vec3(1.0f, 1.0f, 1.0f));
		chaos::box3 b2(glm::vec3(2.0f, Y2, 0.0f), glm::vec3(1.0f, 1.0f, 1.0f));
		chaos::box3 b3(glm::vec3(4.0f, Y3, 0.0f), glm::vec3(1.0f, 1.0f, 1.0f));
		chaos::box3 b4(std::make_pair(
			glm::vec3(time_line_box_position - 10.0f, 8.0f, 0.0f),
			glm::vec3(time_line_box_position + time_line_box_size - 10.0f, 10.0f, 2.0f)
		));

		GPUDrawPrimitive(ctx, b1, red);
		GPUDrawPrimitive(ctx, b2, green);
		GPUDrawPrimitive(ctx, b3, blue);
		if (time_line_box_size > 0.0f)
			GPUDrawPrimitive(ctx, b4, white);
	}

	virtual bool OnDraw(chaos::GPURenderer * renderer, chaos::box2 const & viewport, glm::ivec2 window_size) override
	{
		glm::vec4 clear_color(0.0f, 0.0f, 0.0f, 0.0f);
		glClearBufferfv(GL_COLOR, 0, (GLfloat*)&clear_color);

		float far_plane = 1000.0f;
		glClearBufferfi(GL_DEPTH_STENCIL, 0, far_plane, 0);

		glEnable(GL_DEPTH_TEST);
		glEnable(GL_CULL_FACE);   // when viewer is inside the cube

								  // XXX : the scaling is used to avoid the near plane clipping
		RenderingContext ctx;
		ctx.renderer = renderer;

		static float FOV = 60.0f;
		ctx.projection = glm::perspectiveFov(FOV * (float)M_PI / 180.0f, 2.0f * viewport.half_size.x, 2.0f * viewport.half_size.y, 1.0f, far_plane);
		ctx.world_to_camera = fps_view_controller.GlobalToLocal();

		DrawGeometryObjects(ctx);

		debug_display.Display((int)(2.0f * viewport.half_size.x), (int)(2.0f * viewport.half_size.y));

		return true;
	}

	virtual void Finalize() override
	{
		mesh_box = nullptr;
		program_box = nullptr;

		debug_display.Finalize();

		// XXX : order is important because clock1 would destroy clock2 & clock3
		//       In fact, MainClock->FindChild(clock2) would fails, causing the removing to abord
		//       pointer clock2 & clock3 would be inconsistant
		clock1->RemoveFromParent(); // => clock2 & clock3 are destroyed ...
		clock3->RemoveFromParent(); // ... but RemoveChildClock(...) does not read the content of clock3 before the clock is found
		clock2->RemoveFromParent();

		clock1 = nullptr;
		clock2 = nullptr;
		clock3 = nullptr;
		chaos::MyGLFW::Window::Finalize();
	}

	chaos::shared_ptr<chaos::GPUProgram> LoadProgram(boost::filesystem::path const & resources_path, char const * ps_filename, char const * vs_filename)
	{
		chaos::GPUProgramGenerator program_generator;
		program_generator.AddShaderSourceFile(GL_FRAGMENT_SHADER, resources_path / ps_filename);
		program_generator.AddShaderSourceFile(GL_VERTEX_SHADER, resources_path / vs_filename);

		return program_generator.GenProgramObject();
	}

	virtual bool InitializeFromConfiguration(nlohmann::json const & config, boost::filesystem::path const & config_path) override
	{
		if (!chaos::MyGLFW::Window::InitializeFromConfiguration(config, config_path))
			return false;

		chaos::MyGLFW::SingleWindowApplication * application = chaos::Application::GetInstance();
		if (application == nullptr)
			return false;

		// compute resource path
		boost::filesystem::path resources_path = application->GetResourcesPath();
		boost::filesystem::path image_path = resources_path / "font.png";

		// initialize debug font display 
		chaos::GLDebugOnScreenDisplay::Params debug_params;
		debug_params.texture_path = image_path;
		debug_params.font_characters = " !\"#$%&'()*+,-./0123456789:;<=>?@ABCDEFGHIJKLMNOPQRSTUVWXYZ[\\]^_`abcdefghijklmnopqrstuvwxyz{|}~";
		debug_params.font_characters_per_line = 10;
		debug_params.font_characters_line_count = 10;
		debug_params.character_width = 20;
		debug_params.spacing = glm::ivec2(0, 0);
		debug_params.crop_texture = glm::ivec2(15, 7);

		if (!debug_display.Initialize(debug_params))
			return false;

		// load programs      
		program_box = LoadProgram(resources_path, "pixel_shader_box.txt", "vertex_shader_box.txt");
		if (program_box == nullptr)
			return false;

		// create a timer
		clock1 = application->GetMainClock()->CreateChildClock("clock 1");
		clock2 = clock1->CreateChildClock("clock 2");
		clock3 = clock2->CreateChildClock("clock 3");

		// create meshes
		chaos::box3    b = chaos::box3(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(1.0f, 1.0f, 1.0f));
		chaos::sphere3 s = chaos::sphere3(glm::vec3(0.0f, 0.0f, 0.0f), 1.0f);

		chaos::GPUMultiMeshGenerator generators;
		generators.AddGenerator(new chaos::GPUCubeMeshGenerator(b), mesh_box);

		if (!generators.GenerateMeshes())
			return false;

		// place camera
		fps_view_controller.fps_controller.position.y = 10.0f;
		fps_view_controller.fps_controller.position.z = 30.0f;

		// initial display
		DebugDisplayTips();

		return true;
	}

	virtual void TweakHints(chaos::MyGLFW::WindowHints & hints, GLFWmonitor * monitor, bool pseudo_fullscreen) const override
	{
		chaos::MyGLFW::Window::TweakHints(hints, monitor, pseudo_fullscreen);

		hints.toplevel = 0;
		hints.decorated = 1;
	}

	virtual bool Tick(float delta_time) override
	{
		fps_view_controller.Tick(glfw_window, delta_time);

		debug_display.Tick(delta_time);

		return true; // refresh
	}

	void UpdateClockTimeScale(chaos::Clock * clock, float offset)
	{
		if (clock != nullptr)
			clock->SetTimeScale(clock->GetTimeScale() + offset);
		DebugDisplayTips();
	}

	bool UpdateClockTimeScaleWithKeys(chaos::Clock * clock, chaos::KeyEvent const & event, int incr_key, int decr_key)
	{
		if (event.IsKeyReleased(incr_key))
		{
			UpdateClockTimeScale(clock, 0.2f);
			return true;
		}
		else if (event.IsKeyReleased(decr_key))
		{
			UpdateClockTimeScale(clock, -0.2f);
			return true;
		}
		return false;
	}

	bool GenerateEvent(chaos::Clock * clock, chaos::KeyEvent const & event, int create_key, char const * str, int type)
	{
		if (event.IsKeyReleased(create_key))
		{
			// remove previous event
			if (clock_event != nullptr)
			{
				clock_event->RemoveFromClock();
				clock_event = nullptr;			
			}

			// add new event

			chaos::ClockEventInfo event_info;

			double start_time = 0.0f; // 9 seconds is enough to forbid action

			if (type == 0)
				event_info = chaos::ClockEventInfo::SingleTickEvent(start_time, chaos::ClockEventRepetitionInfo::Repetition(1.0f, 3));
			else if (type == 1)
				event_info = chaos::ClockEventInfo::RangeEvent(start_time, 4.0f, chaos::ClockEventRepetitionInfo::Repetition(1.0f, 3));
			else
				event_info = chaos::ClockEventInfo::ForeverEvent(start_time, chaos::ClockEventRepetitionInfo::Repetition(1.0f, 3));

			clock_event = new MyEvent(str, type, this);

			if (!clock->AddPendingEvent(clock_event.get(), event_info, false))
			{
				clock_event = nullptr;
				debug_display.AddLine("FAILED to add EVENT", 1.0f);
			}

			return true;
		}
		return false;
	}

	virtual bool OnKeyEventImpl(chaos::KeyEvent const & event) override
	{
		if (event.IsKeyReleased(GLFW_KEY_T))
		{
			chaos::Clock * clock = chaos::MyGLFW::SingleWindowApplication::GetMainClockInstance();
			if (clock != nullptr)
				clock->Toggle();
			return true;
		}
		else
		{
			if (UpdateClockTimeScaleWithKeys(clock1.get(), event, GLFW_KEY_KP_1, GLFW_KEY_KP_2))
				return true;				
			if (UpdateClockTimeScaleWithKeys(clock2.get(), event, GLFW_KEY_KP_4, GLFW_KEY_KP_5))
				return true;
			if (UpdateClockTimeScaleWithKeys(clock3.get(), event, GLFW_KEY_KP_7, GLFW_KEY_KP_8))
				return true;

			if (GenerateEvent(clock1.get(), event, GLFW_KEY_A, "EVENT 1", EVENT_SINGLE_TEST))
				return true;
			if (GenerateEvent(clock2.get(), event, GLFW_KEY_Z, "EVENT 2", EVENT_RANGE_TEST))
				return true;
			if (GenerateEvent(clock3.get(), event, GLFW_KEY_E, "EVENT 3", EVENT_FOREVER_TEST))
				return true;
		}
		return chaos::MyGLFW::Window::OnKeyEventImpl(event);
	}

protected:

	// rendering for the box  
	chaos::shared_ptr<chaos::GPUSimpleMesh> mesh_box;
	chaos::shared_ptr<chaos::GPUProgram>  program_box;

	chaos::shared_ptr<chaos::Clock> clock1;
	chaos::shared_ptr<chaos::Clock> clock2;
	chaos::shared_ptr<chaos::Clock> clock3;

	chaos::shared_ptr<chaos::ClockEvent> clock_event;

	float time_line_box_size = 0.0f;
	float time_line_box_position = 0.0f;

	chaos::FPSViewInputController fps_view_controller;

	chaos::GLDebugOnScreenDisplay debug_display;
};

// ====================================================================

MyEvent::~MyEvent()
{
	application->time_line_box_position = 0.0f;
	application->time_line_box_size = 0.0f;

	application->debug_display.AddLine(chaos::StringTools::Printf("MyEvent [%s] destroyed", message.c_str()).c_str(), 1.0f);
}

void MyEvent::OnEventRemovedFromClock()
{
	if (application->clock_event == this)
		application->clock_event = nullptr;
}

chaos::ClockEventTickResult MyEvent::Tick(chaos::ClockEventTickData const & tick_data)
{
	if (type == EVENT_SINGLE_TEST)
	{
		application->time_line_box_position = (float)(tick_data.execution_range.first);
		application->time_line_box_size = 2.0f;

	}
	else if (type == EVENT_RANGE_TEST)
	{
		application->time_line_box_position = (float)(tick_data.execution_range.first);
		application->time_line_box_size = (float)(tick_data.tick_range.second - tick_data.execution_range.first);
	}
	else if (type == EVENT_FOREVER_TEST)
	{
		application->time_line_box_position = (float)(tick_data.execution_range.first);
		application->time_line_box_size = (float)(tick_data.tick_range.second - tick_data.execution_range.first);

		if (tick_data.tick_range.first - tick_data.execution_range.first > 4.0)
		{
			if (GetExecutionCount() == 2)
				return CompleteAll();
			return CompleteExecution();
		}
	}

	application->debug_display.AddLine(chaos::StringTools::Printf("MyEvent [%s] tick [%d] execution [%d]", message.c_str(), GetTickCount(), GetExecutionCount()).c_str(), 0.1f);
	return ContinueExecution(); 
}

// ====================================================================

int CHAOS_MAIN(int argc, char ** argv, char ** env)
{
  chaos::MyGLFW::SingleWindowApplicationParams params;
  params.monitor = nullptr;
  params.width = 1200;
  params.height = 500;
  params.monitor_index = 0;
  chaos::MyGLFW::RunWindowApplication<MyGLFWWindowOpenGLTest1>(argc, argv, env, params);

	return 0;
}


