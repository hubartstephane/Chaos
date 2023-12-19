#include "chaos/ChaosPCH.h"
#include "chaos/ChaosInternals.h"

namespace chaos
{
	void DrawImGui(std::string& value)
	{
		ImGuiTools::PushID(&value);
		ImGuiTools::InputText("", value);
		ImGui::PopID();
	}

	void DrawImGui(bool& value)
	{
		ImGuiTools::PushID(&value);
		ImGui::Checkbox("", &value);
		ImGui::PopID();
	}

	void DrawImGui(int& value)
	{
		ImGuiTools::PushID(&value);
		ImGui::InputInt("", &value);
		ImGui::PopID();
	}

	void DrawImGui(float& value)
	{
		ImGuiTools::PushID(&value);
		ImGui::InputFloat("", &value);
		ImGui::PopID();
	}

	void DrawImGui(double& value)
	{
		ImGuiTools::PushID(&value);
		ImGui::InputDouble("", &value);
		ImGui::PopID();
	}

	namespace ImGuiTools
	{
		void PushID(void* ptr) // native ImGui provides PushID(char *) and PushID(void *). Using this ImGuiTools method prevent ambiguity. If a char * pointer was given as parameter, the content
		{                      // would not be hashed, only the pointer
			ImGui::PushID(ptr);
		}

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

	}; // namespace ImGuiTools


}; // namespace chaos