
#include "chaos/Chaos.h"

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
		chaos::ImGuiTools::FullViewportWindow("fullscreen", 0, [this]()
		{
			// first line
			if (ImGui::Button("void content popup"))
			{
				AddNewImGuiPopupModal<chaos::ImGuiContentObject<void>>("void content", "void content", chaos::Default(), chaos::ImGuiButtonType::Close);
			}

			ImGui::SameLine();
			if (ImGui::Button("content popup"))
			{
				AddNewImGuiPopupModal<chaos::ImGuiContentObject<glm::vec4>>("content popup", "content", clear_color, [this](chaos::ImGuiButtonType  button_type, glm::vec4 value)
				{
					if (HasAnyFlags(button_type, chaos::ImGuiButtonType::ValidationButtons))
						clear_color = value;
				},
				chaos::ImGuiButtonType::ValidateApplyClose);
			}

			ImGui::SameLine();
			if (ImGui::Button("reference content popup"))
			{
				AddNewImGuiPopupModal<chaos::ImGuiContentObject<glm::vec4&>>("reference content popup", "reference content popup", std::ref(clear_color), chaos::Default(), chaos::ImGuiButtonType::Close);
			}

			// second line
			if (ImGui::Button("message"))
			{
				AddNewImGuiPopupModal<chaos::ImGuiMessageObject>("message", "message");
			}
		});

		chaos::Window::OnDrawImGuiContent();
	}

protected:

	glm::vec4 clear_color = { 0.0f, 0.0f, 0.0f, 1.0f };
};

int main(int argc, char ** argv, char ** env)
{
	return chaos::RunApplication<MyApplicationTest>(argc, argv, env, WindowOpenGLTest::GetStaticClass());
}

