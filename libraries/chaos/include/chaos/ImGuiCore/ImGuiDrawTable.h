namespace chaos
{

#ifdef CHAOS_FORWARD_DECLARATION

	class DrawImGuiTable;

#elif !defined CHAOS_TEMPLATE_IMPLEMENTATION

	/**
	 * DrawImGuiTable: utility class that create an imgui table with its header and closes it at destruction
	 */

	class CHAOS_API DrawImGuiTable : public NoCopyClass
	{
	public:

		/** constructor (that opens a ImGui table) */
		template<typename... COLUMN_NAME>
		DrawImGuiTable(char const* title, std::optional<int> table_flags, COLUMN_NAME... column_names)
		{
			table_opened = ImGuiTools::OpenTable(title, table_flags, std::forward<COLUMN_NAME>(column_names)...);
		}
		/** constructor (that opens a ImGui table) */
		DrawImGuiTable(char const* title, std::optional<int> table_flags, std::span<std::string_view> column_names);
		/** destructor (that closes ImGui table) */
		~DrawImGuiTable();

		/** inserting content */
		DrawImGuiTable const& operator ()(LightweightFunction<void()> func) const;

	protected:

		/** whether the table has sucessufully be opened */
		bool table_opened = false;
	};

#endif

}; // namespace chaos