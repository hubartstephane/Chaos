namespace chaos
{
#ifdef CHAOS_FORWARD_DECLARATION

	/** implementation of DrawImGui for a bool */
	CHAOS_API void DrawImGui(bool& value);
	/** implementation of DrawImGui for a int */
	CHAOS_API void DrawImGui(int& value);
	/** implementation of DrawImGui for a float */
	CHAOS_API void DrawImGui(float& value);
	/** implementation of DrawImGui for a double */
	CHAOS_API void DrawImGui(double& value);

	namespace ImGuiTools
	{
		/** concept to know whether DrawImGui can be called on a variable */
		template<typename T>
		concept HasDrawImGuiFunction = requires (T & t)
		{
			{DrawImGui(t)};
		};

		/** ImGUI use 32 bits integers (or strings) for ID's. when using pointers as key, a dedicated function is necessary while pointers are 64 bits */
		CHAOS_API void PushID(void* ptr);

	}; // namespace ImGuiTools


#elif !defined CHAOS_TEMPLATE_IMPLEMENTATION


#endif

}; // namespace chaos