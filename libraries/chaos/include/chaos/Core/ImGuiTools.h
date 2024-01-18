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
		concept HasEnumMetaData = requires()
		{
			{GetEnumMetaData(boost::mpl::identity<typename std::remove_const<T>::type>())};
		};

		template<typename T>
		concept CanDrawImGuiVariable = requires()
		{
			requires
				HasDrawImGuiVariableImplFunction<T> ||
				HasDrawImGuiVariableMethod<T> ||
				HasEnumMetaData<T>;
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
		// disable control if necessary
		if ((flags & DrawImGuiVariableFlags::ReadOnly) != DrawImGuiVariableFlags::None)
		{
			ImGui::BeginDisabled();
		}

		// call method if exists
		if constexpr (ImGuiTools::HasDrawImGuiVariableMethod<T>)
		{
			value.DrawImGuiVariable(flags);
		}
		// call function if exists
		else if constexpr (ImGuiTools::HasDrawImGuiVariableImplFunction<T>)
		{
			DrawImGuiVariableImpl(value, flags);
		}
		// enum case
		else if constexpr (std::is_enum_v<T> && ImGuiTools::HasEnumMetaData<T>)
		{
			chaos::EnumTools::EnumMetaData<T> const& metadata = GetEnumMetaData(boost::mpl::identity<T>());
			if (metadata.IsValid())
			{
				int index = 0;
				if (std::optional<size_t> selected_index = metadata.GetValueIndex(value))
					index = (int)selected_index.value();

				if (ImGui::Combo("", &index, &metadata.names[0], (int)metadata.names.size()))
				{
					value = metadata.GetValueByIndex((size_t)index);
				}
			}
		}
		// error
		else
		{
			assert(0);
		}

		// enable control back if necessary
		if ((flags & DrawImGuiVariableFlags::ReadOnly) != DrawImGuiVariableFlags::None)
		{
			ImGui::EndDisabled();
		}
	}

	template<typename T>
	void DrawImGuiVariable(T const& value, DrawImGuiVariableFlags flags = DrawImGuiVariableFlags::None)
	{
		flags |= DrawImGuiVariableFlags::ReadOnly; // ensure flag is coherent

		DrawImGuiVariable(((T&)value), flags); // call CONST version !
	}

#endif

}; // namespace chaos