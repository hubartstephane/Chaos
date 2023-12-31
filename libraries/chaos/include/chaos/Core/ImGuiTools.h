namespace chaos
{

#ifdef CHAOS_FORWARD_DECLARATION

	enum class DrawImGuiVariableFlags;

#elif !defined CHAOS_TEMPLATE_IMPLEMENTATION

	enum class DrawImGuiVariableFlags : int
	{
		None = 0,
		ReadOnly = 1
	};

	CHAOS_DECLARE_ENUM_FLAG_METHOD(DrawImGuiVariableFlags);

	// XXX: In order to display variable, the system is looking
	//       -whether a function named DrawImGuiVariableImpl(...) exists
	//       -whether a method   named DrawImGuiVariable(...) exists
	//      both have to be NON CONST. 
	//      The system will be able to work with CONST data, but the controls inside will be disabled
	//      (thoses functions should so not modified data by themselves)

	namespace ImGuiTools
	{
		/** check whether there is a function named DrawImGuiVariableImpl */
		template<typename T>
		concept HasDrawImGuiVariableImplFunction = requires(std::remove_const<T>::type & t)
		{
			{DrawImGuiVariableImpl(t, chaos::DrawImGuiVariableFlags::None)};
		};

		/** check whether there is a method named DrawImGuiVariable */
		template<typename T>
		concept HasDrawImGuiVariableMethod = requires(std::remove_const<T>::type & t)
		{
			{t.DrawImGuiVariable(chaos::DrawImGuiVariableFlags::None)};
		};

		template<typename T>
		concept CanDrawImGuiVariable = requires()
		{
			requires
				HasDrawImGuiVariableImplFunction<T> ||
				HasDrawImGuiVariableMethod<T>;
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

	/** a template to display const variables */
	template<typename T>
	void DrawImGuiVariable(T & value, DrawImGuiVariableFlags flags = DrawImGuiVariableFlags::None)
	{
		if ((flags & DrawImGuiVariableFlags::ReadOnly) != DrawImGuiVariableFlags::None)
		{
			ImGui::BeginDisabled();
		}

		if constexpr (ImGuiTools::HasDrawImGuiVariableMethod<T>)
		{
			value.DrawImGuiVariable(flags);
		}
		else if constexpr (ImGuiTools::HasDrawImGuiVariableImplFunction<T>)
		{
			DrawImGuiVariableImpl(value, flags);
		}
		else
		{
			assert(0);
		}

		if ((flags & DrawImGuiVariableFlags::ReadOnly) != DrawImGuiVariableFlags::None)
		{
			ImGui::EndDisabled();
		}
	}

	template<typename T>
	void DrawImGuiVariable(T const& value, DrawImGuiVariableFlags flags = DrawImGuiVariableFlags::None)
	{
		ImGui::BeginDisabled(); // while we are about to call some mutable function/method on a const data, we prevent UI operations to make changes

		flags |= DrawImGuiVariableFlags::ReadOnly; // ensure flag is coherent

		if constexpr (ImGuiTools::HasDrawImGuiVariableMethod<T>)
		{
			((T&)value).DrawImGuiVariable(flags); // XXX: call a MUTABLE method from a CONST reference !
		}
		else if constexpr (ImGuiTools::HasDrawImGuiVariableImplFunction<T>)
		{
			DrawImGuiVariableImpl((T&)value, flags); // XXX: call a MUTABLE function from a CONST reference !			
		}
		else
		{
			assert(0);
		}

		ImGui::EndDisabled();
	}

#endif

}; // namespace chaos