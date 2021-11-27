namespace chaos
{
#ifdef CHAOS_FORWARD_DECLARATION

	class JSONRecursiveLoader;

#elif !defined CHAOS_TEMPLATE_IMPLEMENTATION

	// Note on recursive substitution.
	//
	// -any object {"@include", "XXX"} is replaced by the content of the JSON file XXX
	//
	// -any string (excluding keys for objects)   "@@XXX" is escaped into "XXX"
	//
	// -any string (excluding keys for objects)   "@XXX" is replaced into "PATH_OF_CURRENT_SUBJSON_FILE/XXX"

	class JSONRecursiveLoader
	{
		class LoaderEntry
		{
		public:
			/** the path corresponding to this object */
			boost::filesystem::path path;
			/** the resulting node */
			nlohmann::json json;
			/** the list of all nodes wanted to be replaced by the content of this file */
			std::vector<nlohmann::json*> to_replaced_nodes;
		};

	public:

		/** entry point to parse recursively a JSON file from its path */
		bool LoadJSONFile(FilePathParam const& path, nlohmann::json& result);
		/** entry point to parse recursively a JSON file from an already loaded buffer in memory */
		bool ParseJSONFile(char const* buffer, boost::filesystem::path const& config_path, nlohmann::json& result);

	protected:

		/** internal method */
		bool FinalizeSubstitutions(nlohmann::json& result);
		/** internal method */
		void ComputeSubstitutionChain(char const* buffer, boost::filesystem::path const& config_path);
		/** internal method */
		void ComputeSubstitutionChain(FilePathParam const& path);
		/** internal method */
		void ComputeSubstitutionChainHelper(LoaderEntry* entry);
		/** internal method */
		bool DoMakeStringSubstitution(LoaderEntry* entry, std::string& result);
		/** internal method */
		void MakeStringSubstitution(LoaderEntry* entry, nlohmann::json& root);
		/** internal method */
		void DoComputeSubstitutionChain(LoaderEntry* entry, nlohmann::json& root);
		/** internal method */
		void MakeSubstitutions();
		/** internal method */
		void Clear();

		/** internal method */
		LoaderEntry* FindEntry(FilePathParam const& path);
		/** internal method */
		LoaderEntry* FindOrCreateEntry(FilePathParam const& path, bool& infinite_recursion);
		/** internal method */
		LoaderEntry* CreateEntry(FilePathParam const& path);
		/** internal method */
		LoaderEntry* CreateEntry(char const* buffer, boost::filesystem::path const& config_path);
		/** internal method */
		LoaderEntry* DoCreateEntry(nlohmann::json& new_json, boost::filesystem::path const& config_path);

	protected:

		/** all the entries implied in the recursive loading */
		std::vector<LoaderEntry*> entries;
		/** a value to detect for infinite recursion */
		std::vector<LoaderEntry*> stacked_entries;
	};

#endif

}; // namespace chaos