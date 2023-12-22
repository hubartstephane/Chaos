namespace chaos
{
#ifdef CHAOS_FORWARD_DECLARATION

	namespace ImGuiTools
	{
		/** concept to know whether DrawImGui can be called on a variable */
		template<typename T>
		concept HasDrawImGuiFunction = requires (T & t)
		{
			{DrawImGui(t)};
		};

		/** concept to know whether variable has a method called DrawImGui */
		template<typename T>
		concept HasDrawImGuiMethod = requires (T & t)
		{
			{t.DrawImGui()};
		};

		/** ImGUI use 32 bits integers (or strings) for ID's. when using pointers as key, a dedicated function is necessary while pointers are 64 bits */
		CHAOS_API void PushID(void* ptr);

		/** implementaion coming from imgui_stdlib.cpp */
		CHAOS_API bool InputText(const char* label, std::string& str, ImGuiInputTextFlags flags = 0, ImGuiInputTextCallback callback = nullptr, void* user_data = nullptr);

	}; // namespace ImGuiTools

	/** implementation of DrawImGui for a string */
	CHAOS_API void DrawImGui(std::string& value);
	/** implementation of DrawImGui for a bool */
	CHAOS_API void DrawImGui(bool& value);
	/** implementation of DrawImGui for a int */
	CHAOS_API void DrawImGui(int& value);
	/** implementation of DrawImGui for a float */
	CHAOS_API void DrawImGui(float& value);
	/** implementation of DrawImGui for a double */
	CHAOS_API void DrawImGui(double& value);

#elif !defined CHAOS_TEMPLATE_IMPLEMENTATION

#endif

}; // namespace chaos