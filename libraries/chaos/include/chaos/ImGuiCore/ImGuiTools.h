namespace chaos
{

#ifdef CHAOS_FORWARD_DECLARATION

	enum class DrawImGuiVariableFlags;

	class DrawImGuiTable;

#elif !defined CHAOS_TEMPLATE_IMPLEMENTATION

	enum class DrawImGuiVariableFlags : int
	{
		NONE = 0,
		READ_ONLY           = (1 << 0),
		COLOR               = (1 << 1),  // treat glm::vec3 and glm::vec4 has a colour
		FIXED_SIZE_ARRAY    = (1 << 2),  // prevent add/suppress element from array
		FIXED_REORDER_ARRAY = (1 << 3)   // prevent reordering element from an array

	};

	CHAOS_DECLARE_ENUM_BITMASK_METHOD(DrawImGuiVariableFlags, CHAOS_API);

	// XXX: In order to display variable, the system is looking
	//       -whether a function named DrawImGuiVariableImpl(...) exists
	//       -whether a method   named DrawImGuiVariable(...) exists
	//      both have to be NON CONST. 
	//      The system will be able to work with CONST data, but the controls inside will be disabled
	//      (thoses functions should so not modified data by themselves)

	namespace ImGuiTools
	{
		/** 
		 * DrawImGuiTable: utility class that create an imgui table with its header and closes it at destruction
		 */
		class DrawImGuiTable
		{
		public:

			/** constructor (that opens a ImGui table) */
			template<typename... COLUMN_NAME>
			DrawImGuiTable(char const* title, std::optional<int> table_flags, COLUMN_NAME... column_name)
			{
				static_assert((std::is_convertible_v<COLUMN_NAME, const char*> && ...)); // accept only char const * for column names

				int flags = table_flags.value_or(ImGuiTableFlags_Resizable | ImGuiTableFlags_RowBg);

				int column_count = sizeof...(COLUMN_NAME);
				if (ImGui::BeginTable(title, column_count, flags))
				{
					(ImGui::TableSetupColumn(column_name), ...);
					ImGui::TableHeadersRow();

					table_opened = true;
				}
			}

			/** destructor (that closes ImGui table) */
			~DrawImGuiTable()
			{
				if (table_opened)
				{
					ImGui::EndTable();
					table_opened = false;
				}
			}

			/** inserting content */
			DrawImGuiTable const & operator ()(LightweightFunction<void()> func) const
			{
				func();
				return *this;
			}

			// no copy
			DrawImGuiTable(const DrawImGuiTable&) = delete;
			DrawImGuiTable& operator=(const DrawImGuiTable&) = delete;

		protected:

			/** whether the table has sucessufully be opened */
			bool table_opened = false;
		};

		/** check whether there is a function named DrawImGuiVariableImpl */
		template<typename T>
		concept HasDrawImGuiVariableImplFunction = requires(std::remove_const<T>::type & t)
		{
			{DrawImGuiVariableImpl(t, chaos::DrawImGuiVariableFlags::NONE)};
		};

		/** check whether there is a method named DrawImGuiVariable */
		template<typename T>
		concept HasDrawImGuiVariableMethod = requires(std::remove_const<T>::type & t)
		{
			{t.DrawImGuiVariable(chaos::DrawImGuiVariableFlags::NONE)};
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

		/** create a ImGui window that fills the whole GLFW window (returns true if the window must be kept alive) */
		CHAOS_API bool FullViewportWindow(char const* title, int imgui_window_flags, LightweightFunction<void()> content_func);

	}; // namespace ImGuiTools

	/** implementation of DrawImGui for a string */
	CHAOS_API void DrawImGuiVariableImpl(std::string& value, DrawImGuiVariableFlags flags = DrawImGuiVariableFlags::NONE);
	/** implementation of DrawImGui for a bool */
	CHAOS_API void DrawImGuiVariableImpl(bool& value, DrawImGuiVariableFlags flags = DrawImGuiVariableFlags::NONE);
	/** implementation of DrawImGui for a int */
	CHAOS_API void DrawImGuiVariableImpl(int& value, DrawImGuiVariableFlags flags = DrawImGuiVariableFlags::NONE);
	/** implementation of DrawImGui for a float */
	CHAOS_API void DrawImGuiVariableImpl(float& value, DrawImGuiVariableFlags flags = DrawImGuiVariableFlags::NONE);
	/** implementation of DrawImGui for a double */
	CHAOS_API void DrawImGuiVariableImpl(double& value, DrawImGuiVariableFlags flags = DrawImGuiVariableFlags::NONE);

	namespace details
	{
		/** internal implementation of DrawImGui for enums */
		template<typename T>
		void DrawImGuiVariableEnum(T& value, DrawImGuiVariableFlags flags = DrawImGuiVariableFlags::NONE)
		{
			assert(std::is_enum_v<T> && ImGuiTools::HasEnumMetaData<T>);

			if (EnumMetaData<T> const* metadata = GetEnumMetaData(boost::mpl::identity<T>()))
			{
				if constexpr (IsEnumBitmask<T>)
				{
					char buffer[256];
					char const* preview = EnumToString(value, buffer, 256);

					// start a combo
					if (ImGui::BeginCombo("", preview, ImGuiComboFlags_None))
					{
						for (size_t i = 0; i < metadata->values.size(); ++i) // enumerate all possible values
						{
							if (MathTools::IsPowerOf2(static_cast<int>(metadata->values[i]))) // the enum may have "extra" values not corresponding to flag (NONE for example or some conjuction of other flags as BOTTOM+LEFT for example). Ignore them
							{
								bool selected = (static_cast<int>(value & metadata->values[i]) != 0);

								if (ImGui::Selectable(metadata->names[i], selected, ImGuiSelectableFlags_DontClosePopups)) // ImGuiSelectableFlags_DontClosePopups -> clicking on selection does not close combo
								{
									if (selected) // was selected -> unselect
									{
										value &= ~metadata->values[i]; // invert bit value
									}
									else // was not selected -> select
									{
										value |= metadata->values[i]; // invert bit value

										if (EnumBitmaskMetaData<T> const* bitmask_metadata = GetEnumBitmaskMetaData(boost::mpl::identity<T>()))
										{
											bitmask_metadata->ForEachIncompatibleValue(metadata->values[i], [&value](T other_value)
											{
												value &= ~other_value; // remove incompatible values
											});
										}
									}
								}
							}
						}
						ImGui::EndCombo();
					}
				}
				else
				{
					// get value corresponding index
					int index = 0;
					if (std::optional<size_t> selected_index = metadata->GetValueIndex(value))
						index = (int)selected_index.value();
					// display combo
					if (ImGui::Combo("", &index, &metadata->names[0], (int)metadata->names.size()))
					{
						value = metadata->GetValueByIndex((size_t)index);
					}
				}
			}
		}

	}; // namespace details

	/** a template to display const variables */
	template<typename T>
	void DrawImGuiVariable(T & value, DrawImGuiVariableFlags flags = DrawImGuiVariableFlags::NONE)
	{
		// disable control if necessary
		if (HasAnyFlags(flags, DrawImGuiVariableFlags::READ_ONLY))
		{
			ImGui::BeginDisabled();
		}

		ImGui::PushID((void*)&value);

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
			details::DrawImGuiVariableEnum(value, flags);
		}
		// error
		else
		{
			assert(0);
		}

		ImGui::PopID();

		// enable control back if necessary
		if (HasAnyFlags(flags, DrawImGuiVariableFlags::READ_ONLY))
		{
			ImGui::EndDisabled();
		}
	}

	template<typename T>
	void DrawImGuiVariable(T const& value, DrawImGuiVariableFlags flags = DrawImGuiVariableFlags::NONE)
	{
		flags |= DrawImGuiVariableFlags::READ_ONLY; // ensure flag is coherent

		DrawImGuiVariable(((T&)value), flags); // call CONST version !
	}

	bool WithMenuItem(char const* path, LightweightFunction<void()> func);

#endif

}; // namespace chaos