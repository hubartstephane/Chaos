namespace chaos
{

#ifdef CHAOS_FORWARD_DECLARATION

	enum class DrawImGuiVariableFlags;

#elif !defined CHAOS_TEMPLATE_IMPLEMENTATION

	enum class DrawImGuiVariableFlags : int
	{
		None = 0,
		Const = 1
	};

	CHAOS_DECLARE_ENUM_FLAG_METHOD(DrawImGuiVariableFlags);

	namespace ImGuiTools
	{
		/** concept to know whether there is a function named DrawImGuiVariable that can be called on a variable */
		template<typename T>
		concept HasDrawImGuiVariableImplFunction = requires (T & t)
		{
			{DrawImGuiVariableImpl(t, DrawImGuiVariableFlags::None)};
		};

		/** concept to know whether the variable has a method called DrawImGuiVariable */
		template<typename T>
		concept HasDrawImGuiVariableMethod = requires (T & t)
		{
			{t.DrawImGuiVariable(DrawImGuiVariableFlags::None)};
		};

		/** concept to know whether there is a way, one way or another to call DrawImGuiVariable on the variable */
		template<typename T>
		concept CanDrawImGuiVariable = requires (T & t)
		{
			requires HasDrawImGuiVariableImplFunction<T> || HasDrawImGuiVariableMethod<T>;
		};

		/** ImGUI use 32 bits integers (or strings) for ID's. when using pointers as key, a dedicated function is necessary while pointers are 64 bits */
		CHAOS_API void PushID(void* ptr);

		/** implementaion coming from imgui_stdlib.cpp */
		CHAOS_API bool InputText(const char* label, std::string& str, ImGuiInputTextFlags flags = 0, ImGuiInputTextCallback callback = nullptr, void* user_data = nullptr);

	}; // namespace ImGuiTools

	/** implementation of DrawImGui for a string */
	CHAOS_API void DrawImGuiVariableImpl(std::string& value, DrawImGuiVariableFlags flags = DrawImGuiVariableFlags::None);
	/** implementation of DrawImGui for a bool */
	CHAOS_API void DrawImGuiVariableImpl(bool& value, DrawImGuiVariableFlags flags = DrawImGuiVariableFlags::None);
	/** implementation of DrawImGui for a int */
	CHAOS_API void DrawImGuiVariableImpl(int& value, DrawImGuiVariableFlags flags = DrawImGuiVariableFlags::None);
	/** implementation of DrawImGui for a float */
	CHAOS_API void DrawImGuiVariableImpl(float& value, DrawImGuiVariableFlags flags = DrawImGuiVariableFlags::None);
	/** implementation of DrawImGui for a double */
	CHAOS_API void DrawImGuiVariableImpl(double& value, DrawImGuiVariableFlags flags = DrawImGuiVariableFlags::None);

	/** internal method to call proper function */
	namespace details
	{
		template<typename T>
		void CallDrawImGuiVariable(T& value, DrawImGuiVariableFlags flags)
		{
			if constexpr (ImGuiTools::HasDrawImGuiVariableMethod<T>)
				value.DrawImGuiVariable(flags); 
			else if constexpr (ImGuiTools::HasDrawImGuiVariableImplFunction<T>)
				DrawImGuiVariableImpl(value, flags);
			else
				assert(0);
		}

	}; // namespace details

	/** a template to display const variables */
	template<typename T>
	void DrawImGuiVariable(T & value, DrawImGuiVariableFlags flags = DrawImGuiVariableFlags::None)
	{
		if ((flags & DrawImGuiVariableFlags::Const) != DrawImGuiVariableFlags::None)
		{
			ImGui::BeginDisabled();
			details::CallDrawImGuiVariable(value, flags);
			ImGui::EndDisabled();
		}
		else
		{
			details::CallDrawImGuiVariable(value, flags);
		}
	}

	template<typename T>
	void DrawImGuiVariable(T const& value, DrawImGuiVariableFlags flags = DrawImGuiVariableFlags::None)
	{
		ImGui::BeginDisabled();
		details::CallDrawImGuiVariable(*(T*)&value, flags | DrawImGuiVariableFlags::Const); // ensure the const flag is properly set
		ImGui::EndDisabled();
	}

#endif

}; // namespace chaos