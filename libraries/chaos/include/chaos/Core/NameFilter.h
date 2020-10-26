#ifdef CHAOS_FORWARD_DECLARATION

namespace chaos
{
	class NameFilter;

}; // namespace chaos

#else 

namespace chaos
{
	// ============================================================
	// NameFilter : an object that handles enabled/disabled name lists
	// ============================================================

	class NameFilter
	{

		/** the separator to use */
		static char const name_separator = ',';

		/** JSON saving method */
		friend bool SaveIntoJSON(nlohmann::json & json_entry, NameFilter const & src);
		/** JSON loading method */
		friend bool LoadFromJSON(nlohmann::json const & json_entry, NameFilter & dst);

	public:

		/** returns true whether the filter is empty */
		bool IsEmpty() const;
	
		/** add names in the enabled list (separated with ',') */
		void AddEnabledNames(char const * names);
		/** add names in the disabled list (separated with ',') */
		void AddDisabledNames(char const * names);
		/** remove names from the enabled list (separated with ',') */
		void RemoveEnabledNames(char const * names);
		/** remove names from the disabled list (separated with ',') */
		void RemoveDisabledNames(char const * names);

		/** check whether the name passes the filter */
		bool IsNameEnabled(char const * name) const;

	public:

		/** utility method to insert names in the enabled/disabled array  (separated with ',') */
		static void AddNames(char const * names, std::vector<std::string>& target_list, char separator = name_separator);
		/** utility method to remove names from the enabled/disabled array  (separated with ',') */
		static void RemoveNames(char const * names, std::vector<std::string>& target_list, char separator = name_separator);

	protected:

		/** the list of enabled names */
		std::vector<std::string> enabled_names;
		/** the list of disabled names */
		std::vector<std::string> disabled_names;
	};

}; // namespace chaos

#endif // CHAOS_FORWARD_DECLARATION

