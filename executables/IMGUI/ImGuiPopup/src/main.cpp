
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



enum class ImGuiGenericPopupButton
{
	None     = 0,
	Close    = (1 << 0),
	Yes      = (1 << 1),
	No       = (1 << 2),
	YesNo    = Yes | No,
	Cancel   = (1 << 3),
	Apply    = (1 << 4),
	Validate = (1 << 5)
};

CHAOS_DECLARE_ENUM_BITMASK_METHOD(ImGuiGenericPopupButton, CHAOS_API);

CHAOS_IMPLEMENT_ENUM_BITMASK_METHOD(ImGuiGenericPopupButton, nullptr, CHAOS_API);


class ImGuiGenericPopupObjectBase : public chaos::ImGuiObject
{
public:

	ImGuiGenericPopupObjectBase(std::string in_message, ImGuiGenericPopupButton in_buttons) :
		message(std::move(in_message)),
		buttons(in_buttons)
	{
		if (HasAnyFlags(in_buttons, ImGuiGenericPopupButton::Close))
			SetImGuiObjectFlags(GetImGuiObjectFlags() | chaos::ImGuiObjectFlags::CloseWithCross);
	}

	virtual void OnDrawImGuiContent(chaos::Window* window) override
	{
		if (!chaos::StringTools::IsEmpty(message))
			ImGui::Text("%s", message.c_str());

		DisplayContent();

		DisplayButtons();
	}

protected:

	virtual void DisplayContent()
	{
	}

	virtual void CallNotification(ImGuiGenericPopupButton button_type)
	{
	}

	void DisplayButtons()
	{
		auto AddButton = [this](ImGuiGenericPopupButton button_type, char const * button_title, bool request_closing)
		{
			if (HasAnyFlags(buttons, button_type))
			{
				if (ImGui::Button(button_title))
				{
					CallNotification(button_type);
					if (request_closing)
						//ImGui::CloseCurrentPopup();
						RequestClosing();
				};
				ImGui::SameLine();
			}
		};

		AddButton(ImGuiGenericPopupButton::Yes, "Yes", true);
		AddButton(ImGuiGenericPopupButton::No, "No", true);
		AddButton(ImGuiGenericPopupButton::Validate, "Validate", true);
		AddButton(ImGuiGenericPopupButton::Cancel, "Cancel", true);		
		AddButton(ImGuiGenericPopupButton::Apply, "Apply", false);
		AddButton(ImGuiGenericPopupButton::Close, "Close", true);
	}

protected:

	std::string message;

	ImGuiGenericPopupButton buttons;
};


template<typename T>
class ImGuiGenericPopupObjectTypedBase : public ImGuiGenericPopupObjectBase
{
public:

	ImGuiGenericPopupObjectTypedBase(
		std::string in_message,
		T in_value = {},
		std::function<void(ImGuiGenericPopupButton, T const&)> in_notification_function = {},
		ImGuiGenericPopupButton in_buttons = ImGuiGenericPopupButton::Close
	) :
		ImGuiGenericPopupObjectBase(std::move(in_message), in_buttons),
		value(std::move(in_value)),
		notification_function(std::move(in_notification_function)){}

protected:

	virtual void CallNotification(ImGuiGenericPopupButton button_type) override
	{
		if (notification_function)
			notification_function(button_type, value);
	}

	virtual void DisplayContent() override
	{
		//chaos::ImGuiTools::DrawImGuiVariable(value);
	}

protected:

	T value;

	std::function<void(ImGuiGenericPopupButton, T const &)> notification_function;
};

template<>
class ImGuiGenericPopupObjectTypedBase<void> : public ImGuiGenericPopupObjectBase
{

public:

	ImGuiGenericPopupObjectTypedBase(
		std::string in_message, 
		std::function<void(ImGuiGenericPopupButton)> in_notification_function = {},
		ImGuiGenericPopupButton in_buttons = ImGuiGenericPopupButton::Close
	):
		ImGuiGenericPopupObjectBase(std::move(in_message), in_buttons),
		notification_function(std::move(in_notification_function))
	{
	}

protected:

	virtual void CallNotification(ImGuiGenericPopupButton button_type)
	{
		if (notification_function)
			notification_function(button_type);
	}

protected:

	std::function<void(ImGuiGenericPopupButton)> notification_function;
};


template<typename T>
class ImGuiGenericPopupObject : public ImGuiGenericPopupObjectTypedBase<T>
{
public:

	using ImGuiGenericPopupObjectTypedBase<T>::ImGuiGenericPopupObjectTypedBase;

	

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

				AddNewImGuiPopupModal<ImGuiGenericPopupObject<int>>("hello", "hello_world", 6, [](ImGuiGenericPopupButton button_type, int const & value)
				{
						button_type = button_type;

					
				},
					ImGuiGenericPopupButton::YesNo | ImGuiGenericPopupButton::Close);


#if 0

				AddNewImGuiPopupModal<chaos::ImGuiYesNoObject>("hello", "do you really want ?", [this](bool validate)
				{
					if (validate)
						AddNewImGuiPopupModal<chaos::ImGuiMessageObject>("Hello", "Hello World true");
					else
						AddNewImGuiPopupModal<chaos::ImGuiMessageObject>("Hello", "Hello World false");
				});
#endif




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

	ImGuiMessagePopup message_popup;
};

int main(int argc, char ** argv, char ** env)
{
	return chaos::RunApplication<MyApplicationTest>(argc, argv, env, WindowOpenGLTest::GetStaticClass());
}

