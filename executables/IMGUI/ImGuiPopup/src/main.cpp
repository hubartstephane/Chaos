
#include "chaos/Chaos.h"


class ImGuiMessagePopup : public chaos::ImGuiPopup<int>
{
public:

	void Open(std::string in_popup_name, std::string in_message, const chaos::ImGuiWindowPlacement& in_placement = {})
	{
		if (DoOpen(std::move(in_popup_name), chaos::ImGuiPopupFlags::Modal | chaos::ImGuiPopupFlags::CloseWithEscape, in_placement))
		{
			message = std::move(in_message);
		}
	}

protected:

	virtual std::optional<int> DoProcess() override
	{
		ImGui::Text(message.c_str());
		ImGui::SliderInt("value", &value, -100, 100);
		if (ImGui::Button("Close"))
		{
			//Close();
			return value;
		}
		return {};
	}

protected:

	int value = 0;

	std::string message;
};




class MyApplicationTest : public chaos::WindowApplication
{
public:

	using chaos::WindowApplication::WindowApplication;
};





class WindowOpenGLTest : public chaos::Window
{
	CHAOS_DECLARE_OBJECT_CLASS(WindowOpenGLTest, chaos::Window);

protected:


	virtual bool OnDraw(chaos::GPURenderer* renderer, chaos::GPUProgramProviderInterface const* uniform_provider, chaos::WindowDrawParams const& draw_params) override
	{
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

				AddNewImGuiPopupModal<chaos::ImGuiContentObject<glm::vec4>>("hello", "", clear_color, [this](chaos::ImGuiButtonType  button_type, glm::vec4 const & value)
				{
					if (HasAnyFlags(button_type, chaos::ImGuiButtonType::ValidationButtons))
						clear_color = value;
				},
				chaos::ImGuiButtonType::ValidateApplyClose);


				//placement.movable = true;
				//message_popup.Open("error", "this is the error message", placement);
			}
			ImGui::End();
		}
		std::optional<int> v = message_popup.Process();
		if (v.has_value())
		{
			int k = v.value();
			k = k;
		}

		chaos::Window::OnDrawImGuiContent();
	}

protected:

	glm::vec4 clear_color = { 0.0f, 0.0f, 0.0f, 1.0f };

	ImGuiMessagePopup message_popup;
};

int main(int argc, char ** argv, char ** env)
{
	return chaos::RunApplication<MyApplicationTest>(argc, argv, env, WindowOpenGLTest::GetStaticClass());
}

