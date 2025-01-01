#include "chaos/ChaosPCH.h"
#include "chaos/ChaosInternals.h"

namespace chaos
{
	CHAOS_IMPLEMENT_ENUM_BITMASK_METHOD(DrawImGuiVariableFlags, nullptr, CHAOS_API);

	void DrawImGuiVariableImpl(std::string& value, DrawImGuiVariableFlags flags)
	{
		ImGui::PushID(&value);
		chaos::ImGuiTools::InputText("", value);
		ImGui::PopID();
	}

	void DrawImGuiVariableImpl(bool& value, DrawImGuiVariableFlags flags)
	{
		ImGui::PushID(&value);
		ImGui::Checkbox("", &value);
		ImGui::PopID();
	}

	void DrawImGuiVariableImpl(int& value, DrawImGuiVariableFlags flags)
	{
		ImGui::PushID(&value);
		if (HasAnyFlags(flags, DrawImGuiVariableFlags::READ_ONLY))
			ImGui::InputInt("", &value, 0, 0); // prevent showing the + and - buttons
		else
			ImGui::InputInt("", &value); 
		ImGui::PopID();
	}

	void DrawImGuiVariableImpl(float& value, DrawImGuiVariableFlags flags)
	{
		ImGui::PushID(&value);
		ImGui::InputFloat("", &value);
		ImGui::PopID();
	}

	void DrawImGuiVariableImpl(double& value, DrawImGuiVariableFlags flags)
	{
		ImGui::PushID(&value);
		ImGui::InputDouble("", &value);
		ImGui::PopID();
	}

	namespace ImGuiTools
	{
		// code for using ImGui with strings
		// this comes from imgui_stdlib.cpp whose functions are not really part of the compiled lib we are using
		struct InputTextCallback_UserData
		{
			std::string* Str;
			ImGuiInputTextCallback  ChainCallback;
			void* ChainCallbackUserData;
		};

		static int InputTextCallback(ImGuiInputTextCallbackData* data)
		{
			InputTextCallback_UserData* user_data = (InputTextCallback_UserData*)data->UserData;
			if (data->EventFlag == ImGuiInputTextFlags_CallbackResize)
			{
				// Resize string callback
				// If for some reason we refuse the new length (BufTextLen) and/or capacity (BufSize) we need to set them back to what we want.
				std::string* str = user_data->Str;
				IM_ASSERT(data->Buf == str->c_str());
				str->resize(data->BufTextLen);
				data->Buf = (char*)str->c_str();
			}
			else if (user_data->ChainCallback)
			{
				// Forward to user callback, if any
				data->UserData = user_data->ChainCallbackUserData;
				return user_data->ChainCallback(data);
			}
			return 0;
		}

		bool InputText(const char* label, std::string & str, ImGuiInputTextFlags flags, ImGuiInputTextCallback callback, void* user_data)
		{
			IM_ASSERT((flags & ImGuiInputTextFlags_CallbackResize) == 0);
			flags |= ImGuiInputTextFlags_CallbackResize;

			InputTextCallback_UserData cb_user_data;
			cb_user_data.Str = &str;
			cb_user_data.ChainCallback = callback;
			cb_user_data.ChainCallbackUserData = user_data;
			return ImGui::InputText(label, (char*)str.c_str(), str.capacity() + 1, flags, InputTextCallback, &cb_user_data);
		}

		void BeginMenuBar::operator()(LightweightFunction<void()> func) const
		{
			if (ImGui::BeginMenuBar())
			{
				func();
				ImGui::EndMenuBar();
			}
		}

		void BeginMainMenuBar::operator()(LightweightFunction<void()> func) const
		{
			if (ImGui::BeginMainMenuBar())
			{
				func();
				ImGui::EndMainMenuBar();
			}
		}

		bool FullscreenWindow(char const* title, int imgui_window_flags, LightweightFunction<void()> content_func)
		{
			// update flags
			imgui_window_flags |= ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoBackground | ImGuiWindowFlags_NoBringToFrontOnFocus;

			// prepare ImGui window size and position
			ImGui::SetNextWindowPos({ ImGui::GetMainViewport()->WorkPos.x, ImGui::GetMainViewport()->WorkPos.y });
			ImGui::SetNextWindowSize({ ImGui::GetMainViewport()->Size.x, ImGui::GetMainViewport()->Size.y });
			// display then window
			if (ImGui::Begin(title, nullptr, imgui_window_flags))
			{
				content_func();
				ImGui::End();
			}
			return true; // window must be kept alive
		}

	}; // namespace ImGuiTools

}; // namespace chaos