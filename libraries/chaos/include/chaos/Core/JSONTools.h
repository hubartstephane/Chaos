#ifdef CHAOS_FORWARD_DECLARATION

// =================
// Some macros for enum json reading
// =================

namespace JSONTools 
{


};

#define CHAOS_IMPLEMENT_ENUMJSON_METHOD(enum_type, table_name)\
bool LoadFromJSON(nlohmann::json const& json_entry, enum_type& dst)\
{\
	return LoadEnumFromJSON(json_entry, table_name, dst);\
}\
bool SaveIntoJSON(nlohmann::json& json_entry, enum_type const& src)\
{\
	return SaveEnumIntoJSON(json_entry, table_name, src);\
}\

#define CHAOS_JSON_ATTRIBUTE(config, x) chaos::JSONTools::GetAttribute(config, #x, x)

#else 

namespace chaos
{
	// =================
	// EXTERNAL FUNCTION
	// =================

	CHAOS_GENERATE_CHECK_METHOD_AND_FUNCTION(SerializeFromJSON);
	CHAOS_GENERATE_CHECK_METHOD_AND_FUNCTION(SerializeIntoJSON);
	CHAOS_GENERATE_CHECK_METHOD_AND_FUNCTION(GetClass);

	/** an utility function to create an object from a json object */
	template<typename T>
	static T* LoadFromJSONCreateObject(nlohmann::json const& entry);

	/** loading a bool (because we try to read an int as a fallback) */
	bool LoadFromJSON(nlohmann::json const& entry, bool& dst);

	/** template for unique_ptr */
	template<typename T, typename DELETER>
	bool LoadFromJSON(nlohmann::json const& entry, std::unique_ptr<T, DELETER>& dst);
	/** template for shared_ptr */
	template<typename T>
	bool LoadFromJSON(nlohmann::json const& entry, shared_ptr<T>& dst);

	/** loading specialization for vector */
	template<typename T>
	bool LoadFromJSON(nlohmann::json const& entry, std::vector<T>& dst);
	/** default template loading (catch exceptions) */
	template<typename T>
	bool LoadFromJSON(nlohmann::json const& entry, T& dst);

	/** template for unique_ptr */
	template<typename T, typename DELETER>
	bool SaveIntoJSON(nlohmann::json& entry, std::unique_ptr<T, DELETER> const& src);
	/** template for shared_ptr */
	template<typename T>
	bool SaveIntoJSON(nlohmann::json& entry, shared_ptr<T> const& src);

	/** specialization for vector */
	template<typename T>
	bool SaveIntoJSON(nlohmann::json& entry, std::vector<T> const& src);

	template<typename T>
	bool SaveIntoJSON(nlohmann::json& entry, T const& src);

	/** enumeration method */
	template<typename T, typename ENCODE_TABLE>	
	bool SaveEnumIntoJSON(nlohmann::json& json_entry, ENCODE_TABLE const& encode_table, T const& src);

	template<typename T, typename ENCODE_TABLE>
	bool LoadEnumFromJSON(nlohmann::json const& json_entry, ENCODE_TABLE const& encode_table, T& dst);

	// =================
	// JSONTools
	// =================

	namespace JSONTools
	{
		/** parsing a JSON file (catch exceptions) */
		bool Parse(char const * buffer, nlohmann::json & result);
		/** parsing a JSON file from a buffer (load any dependant files) */
		bool ParseRecursive(char const * buffer, boost::filesystem::path const & config_path, nlohmann::json & result);
		/** Load a JSON file in a recursive whay */
		bool LoadJSONFile(FilePathParam const & path, nlohmann::json & result, bool recursive = false);
		/** create a temporary directory to hold the configuration (returns the path of the directory where the file is) */				
		boost::filesystem::path DumpConfigFile(nlohmann::json const & json, char const * filename = "myconfig.json");

		/** get a sub object from an object */
		nlohmann::json * GetStructure(nlohmann::json & entry, char const * name);
		/** get a sub object from an object */
		nlohmann::json const * GetStructure(nlohmann::json const & entry, char const * name);

		/** get a sub object from an object */
		nlohmann::json * GetStructureByIndex(nlohmann::json & entry, size_t index);
		/** get a sub object from an object */
		nlohmann::json const * GetStructureByIndex(nlohmann::json const & entry, size_t index);

		/** set an attribute in a json structure */
		template<typename T>
		bool SetAttribute(nlohmann::json& entry, char const* name, T const& src);

		/** set an attribute in a json array */
		template<typename T>
		bool SetAttributeByIndex(nlohmann::json& entry, size_t index, T const& src);

		/** set an attribute in a json structure with a lookup table */
		template<typename T, typename ENCODE_TABLE>
		bool SetEnumAttribute(nlohmann::json& entry, char const* name, ENCODE_TABLE const& encode_table, T const& src);

		/** set an attribute in a json structure with a lookup table */
		template<typename T, typename ENCODE_TABLE>
		bool SetEnumAttributeByIndex(nlohmann::json& entry, size_t index, ENCODE_TABLE const& encode_table, T const& src);

		/** reading an attribute from a JSON structure */
		template<typename T>
		bool GetAttribute(nlohmann::json const& entry, char const* name, T& result);

		/** reading an attribute from a JSON array */
		template<typename T>
		bool GetAttributeByIndex(nlohmann::json const& entry, size_t index, T& result);

		/** reading an attribute with default value */
		template<typename T, typename Y>
		bool GetAttribute(nlohmann::json const& entry, char const* name, T& result, Y default_value);

		/** reading an attribute with default value */
		template<typename T, typename Y>
		bool GetAttributeByIndex(nlohmann::json const& entry, size_t index, T& result, Y default_value);

		/** reading an attribute and make a lookup on an encoding table */
		template<typename T, typename ENCODE_TABLE>
		bool GetEnumAttribute(nlohmann::json const& entry, char const* name, ENCODE_TABLE const& encode_table, T& result);

		/** reading an attribute and make a lookup on an encoding table */
		template<typename T, typename ENCODE_TABLE>
		bool GetEnumAttributeByIndex(nlohmann::json const& entry, size_t index, ENCODE_TABLE const& encode_table, T& result);

		/** reading an attribute and make a lookup on an encoding table with a default value */
		template<typename T, typename ENCODE_TABLE, typename Y>
		bool GetEnumAttribute(nlohmann::json const& entry, char const* name, ENCODE_TABLE const& encode_table, T& result, Y default_value);

		/** reading an attribute and make a lookup on an encoding table with a default value */
		template<typename T, typename ENCODE_TABLE, typename Y>
		bool GetEnumAttributeByIndex(nlohmann::json const& entry, size_t index, ENCODE_TABLE const& encode_table, T& result, Y default_value);

	}; // namespace JSONTools

}; // namespace chaos

#endif // CHAOS_FORWARD_DECLARATION


