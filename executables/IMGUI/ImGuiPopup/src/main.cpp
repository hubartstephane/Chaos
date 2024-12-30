
#include "chaos/Chaos.h"


class ImGuiMessagePopup : public chaos::ImGuiPopup<void>
{
public:

	void Open(std::string in_popup_name, std::string in_message, const chaos::ImGuiWindowPlacement& in_placement = {})
	{
		if (DoOpen(std::move(in_popup_name), in_placement))
		{
			message = std::move(in_message);
		}
	}

protected:

	virtual void DoProcess() override
	{
		if (ImGui::IsKeyDown(ImGuiKey::ImGuiKey_Escape))
		{
			Close();
		}

		ImGui::Text(message.c_str());
		if (ImGui::Button("Close"))
			Close();
	}

protected:

	std::string message;
};





class MyApplicationTest : public chaos::WindowApplication
{
public:

	using chaos::WindowApplication::WindowApplication;
};



class MyImGuiPopup : public chaos::ImGuiObject
{
public:


	MyImGuiPopup(std::function<void()> in_func) :
		func(std::move(in_func))
	{
	}

	std::function<void()> func;
};


class WindowOpenGLTest : public chaos::Window
{
	CHAOS_DECLARE_OBJECT_CLASS(WindowOpenGLTest, chaos::Window);

protected:

	void AddPopup(char const * name, std::function<void()> func)
	{
		MyImGuiPopup * popup = new MyImGuiPopup(std::move(func));
		popup->SetName(name);
		AddImGuiObject(popup);
	}

	virtual bool OnDraw(chaos::GPURenderer* renderer, chaos::GPUProgramProviderInterface const* uniform_provider, chaos::WindowDrawParams const& draw_params) override
	{
		glm::vec4 clear_color(0.1f, 0.0f, 0.0f, 0.0f);
		glClearBufferfv(GL_COLOR, 0, (GLfloat*)&clear_color);

		float far_plane = 1000.0f;
		glClearBufferfi(GL_DEPTH_STENCIL, 0, far_plane, 0);
		return true;
	}

	virtual void OnDrawImGuiContent() override
	{
		if (ImGui::Begin("help", nullptr, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_AlwaysAutoResize))
		{
			if (ImGui::Button("show popup", { 200.0f , 200.0f }))
			{
				chaos::ImGuiWindowPlacement placement = chaos::ImGuiWindowPlacement::GetCenterOnScreenPlacement();




				//placement.movable = true;
				message_popup.Open("error", "this is the error message", placement);
			}
			ImGui::End();
		}
		message_popup.Process();

		chaos::Window::OnDrawImGuiContent();
	}

protected:

	ImGuiMessagePopup message_popup;
};

int main(int argc, char ** argv, char ** env)
{
	return chaos::RunApplication<MyApplicationTest>(argc, argv, env, WindowOpenGLTest::GetStaticClass());
}

