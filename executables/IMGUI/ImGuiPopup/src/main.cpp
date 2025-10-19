
#include "chaos/Chaos.h"

class WindowOpenGLTest : public chaos::Window
{
	CHAOS_DECLARE_OBJECT_CLASS(WindowOpenGLTest, chaos::Window);

protected:


	virtual bool OnDraw(chaos::GPURenderContext* render_context, chaos::GPUProgramProviderInterface const* uniform_provider, chaos::WindowDrawParams const& draw_params) override
	{
		glClearBufferfv(GL_COLOR, 0, (GLfloat*)&clear_color);

		float far_plane = 1000.0f;
		glClearBufferfi(GL_DEPTH_STENCIL, 0, far_plane, 0);
		return true;
	}

	virtual void OnDrawImGuiContent() override
	{
		chaos::ImGuiTools::FullViewportWindow("fullscreen", 0, [this]()
		{
			// first line
			if (ImGui::Button("void content popup"))
			{
				auto * popup = AddNewImGuiPopupModal<chaos::ImGuiContentObject<void>>("void content", "void content", chaos::Default(), chaos::ImGuiButtonType::Close);

				popup->SetWindowPlacement(chaos::ImGuiWindowPlacement::GetCenterOnCursorPlacement());
			}

			ImGui::SameLine();
			if (ImGui::Button("content popup"))
			{
				auto* popup = AddNewImGuiPopupModal<chaos::ImGuiContentObject<glm::vec4>>("content popup", "content", clear_color, [this](chaos::ImGuiButtonType  button_type, glm::vec4 value)
				{
					if (HasAnyFlags(button_type, chaos::ImGuiButtonType::ValidationButtons))
						clear_color = value;
				},
				chaos::ImGuiButtonType::ValidateApplyClose);

				popup->SetWindowPlacement(chaos::ImGuiWindowPlacement::GetCenterOnScreenPlacement());
			}

			ImGui::SameLine();
			if (ImGui::Button("reference content popup"))
			{
				auto* popup = AddNewImGuiPopupModal<chaos::ImGuiContentObject<glm::vec4&>>("reference content popup", "reference content popup", std::ref(clear_color), chaos::Default(), chaos::ImGuiButtonType::Close);

				popup->SetWindowPlacement(chaos::ImGuiWindowPlacement::GetCenterOnPositionPlacement({100.0f,100.0f}));
			}	

			// second line
			if (ImGui::Button("message"))
			{
				auto* popup = AddNewImGuiPopupModal<chaos::ImGuiMessageObject>("message", "message");
			}

			ImGui::SameLine();
			if (ImGui::Button("choices"))
			{
				char const* entries[] =
				{
					"choice A", "choice B", "choice C", nullptr
				};

				auto* popup = AddNewImGuiPopup<chaos::ImGuiChoiceObject>("choices", entries, [](size_t j)
				{
				});
			}
		});

		chaos::Window::OnDrawImGuiContent();
	}

protected:

	glm::vec4 clear_color = { 0.0f, 0.0f, 0.0f, 1.0f };
};

int main(int argc, char ** argv, char ** env)
{
	return chaos::RunSimpleWindowApplication<WindowOpenGLTest>(argc, argv, env);
}

