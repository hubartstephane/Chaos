namespace chaos
{
#ifdef CHAOS_FORWARD_DECLARATION

#define CHAOS_JSON_ATTRIBUTE(config, x) chaos::JSONTools::GetAttribute(config, #x, x)

	namespace JSONTools {};

#elif !defined CHAOS_TEMPLATE_IMPLEMENTATION

	// =================
	// Some meta methods
	// =================

	CHAOS_GENERATE_CHECK_METHOD_AND_FUNCTION(SerializeFromJSON);
	CHAOS_GENERATE_CHECK_METHOD_AND_FUNCTION(SerializeIntoJSON);
	CHAOS_GENERATE_CHECK_METHOD_AND_FUNCTION(GetClass);

	template<typename T>
	concept JSONSource = requires(T t)
	{
		requires
			std::is_convertible_v<T, JSONReadConfiguration> ||
			std::is_convertible_v<T, JSONWriteConfiguration> ||
			std::is_convertible_v<T, nlohmann::json const*>;
	};

	template<typename T>
	concept HasGetClass = requires(T t)
	{
		{t.GetClass()} -> std::convertible_to<Class const *>;
	};

	template<typename T>
	concept HasDoLoadFromJSON = requires(T t)
	{
		{DoLoadFromJSON(meta::FakeInstance<JSONReadConfiguration>(), t)} -> std::convertible_to<bool>;
	};

	template<typename T>
	concept HasDoSaveIntoJSON = requires(T const & t)
	{
		{DoSaveIntoJSON(meta::FakeInstance<nlohmann::json *>(), t)} -> std::convertible_to<bool>;
	};

	// ====================
	// Standalone functions
	// ====================

	/** loading a bool (because we try to read an int as a fallback) */
	template<JSONSource SRC_TYPE>
	bool LoadFromJSON(SRC_TYPE src, bool& dst);
	/** serialize a path from json */
	template<JSONSource SRC_TYPE>
	bool LoadFromJSON(SRC_TYPE src, boost::filesystem::path& dst);

	/** an utility function to create an object from a json object */
	template<typename T, JSONSource SRC_TYPE>
	T* LoadFromJSONCreateObject(SRC_TYPE src);

	/** default template loading (catch exceptions) */
	template<typename T, JSONSource SRC_TYPE>
	bool LoadFromJSON(SRC_TYPE src, T& dst);
	/** template for unique_ptr */
	template<typename T, JSONSource SRC_TYPE, typename DELETER>
	bool LoadFromJSON(SRC_TYPE src, std::unique_ptr<T, DELETER>& dst);
	/** template for shared_ptr */
	template<typename T, JSONSource SRC_TYPE>
	bool LoadFromJSON(SRC_TYPE src, shared_ptr<T>& dst);
	/** loading specialization for vector */
	template<typename T, JSONSource SRC_TYPE>
	bool LoadFromJSON(SRC_TYPE src, std::vector<T>& dst);
	/** template for optional */
	template<typename T, JSONSource SRC_TYPE>
	bool LoadFromJSON(SRC_TYPE src, std::optional<T>& dst);

	/** serialize a path into json */
	CHAOS_API bool SaveIntoJSON(nlohmann::json * json, boost::filesystem::path const& src);
	/** basic types */
	template<typename T>
	bool SaveIntoJSON(nlohmann::json * json, T const& src);
	/** template for unique_ptr */
	template<typename T, typename DELETER>
	bool SaveIntoJSON(nlohmann::json * json, std::unique_ptr<T, DELETER> const& src);
	/** template for shared_ptr */
	template<typename T>
	bool SaveIntoJSON(nlohmann::json * json, shared_ptr<T> const& src);
	/** specialization for vector */
	template<typename T>
	bool SaveIntoJSON(nlohmann::json * json, std::vector<T> const& src);
	/** specialization for optional */
	template<typename T>
	bool SaveIntoJSON(nlohmann::json* json, std::optional<T> const& src);

	/** ensure pointers is not null and the node is a json object */
	CHAOS_API bool PrepareSaveObjectIntoJSON(nlohmann::json* json);
	/** ensure pointers is not null and the node is a json array */
	CHAOS_API bool PrepareSaveArrayIntoJSON(nlohmann::json* json);

	// =================
	// JSONTools
	// =================

	namespace JSONTools
	{
		/** parsing a JSON file (catch exceptions) */
		CHAOS_API bool Parse(char const* buffer, nlohmann::json& result);
		/** parsing a JSON file from a buffer (load any dependant files) */
		CHAOS_API bool ParseRecursive(char const* buffer, boost::filesystem::path const& config_path, nlohmann::json& result, LoadFileFlag flag = LoadFileFlag::NONE);
		/** Load a JSON file in a recursive whay */
		CHAOS_API bool LoadJSONFile(FilePathParam const& path, nlohmann::json& result, LoadFileFlag flag = LoadFileFlag::NONE);
		/** create a temporary directory to hold the configuration (returns the path of the directory where the file is) */
		CHAOS_API boost::filesystem::path DumpConfigFile(nlohmann::json const * json, char const* filename = "myconfig.json");
		/** save the json into a file */
		CHAOS_API bool SaveJSONToFile(nlohmann::json const * json, FilePathParam const& path);

		/** set an attribute in a json structure */
		template<typename T>
		bool SetAttribute(nlohmann::json * json, std::string_view path, T const& src);
		/** set an attribute in a json array */
		template<typename T>
		bool SetElement(nlohmann::json * json, size_t index, T const& src);

		/** reading an attribute from a JSON structure */
		template<typename T, JSONSource SRC_TYPE>
		bool GetAttribute(SRC_TYPE src, std::string_view path, T& result);
		/** reading an attribute from a JSON array */
		template<typename T, JSONSource SRC_TYPE>
		bool GetElement(SRC_TYPE src, size_t index, T& result);
		/** reading an attribute with default value */
		template<typename T, JSONSource SRC_TYPE, typename Y>
		bool GetAttribute(SRC_TYPE src, std::string_view path, T& result, Y const & default_value);
		/** reading an attribute with default value */
		template<typename T, JSONSource SRC_TYPE, typename Y>
		bool GetAttribute(SRC_TYPE src, std::string_view path, T& result, Y && default_value);
		/** reading an attribute with default value */
		template<typename T, JSONSource SRC_TYPE, typename Y>
		bool GetElement(SRC_TYPE src, size_t index, T& result, Y const &default_value);
		/** reading an attribute with default value */
		template<typename T, JSONSource SRC_TYPE, typename Y>
		bool GetElement(SRC_TYPE src, size_t index, T& result, Y && default_value);

		/** getting a node by path */
		CHAOS_API nlohmann::json* GetAttributeNode(nlohmann::json * json, std::string_view path);
		/** getting a node by path */
		CHAOS_API nlohmann::json const* GetAttributeNode(nlohmann::json const * json, std::string_view path);
		/** getting or creating a node by path */
		CHAOS_API nlohmann::json* GetOrCreateAttributeNode(nlohmann::json * json, std::string_view path);
		/** getting a node by index */
		CHAOS_API nlohmann::json* GetElementNode(nlohmann::json * json, size_t index);
		/** getting a node by inde */
		CHAOS_API nlohmann::json const* GetElementNode(nlohmann::json const * json, size_t index);

		/** getting an object node by path */
		CHAOS_API nlohmann::json* GetAttributeObjectNode(nlohmann::json * json, std::string_view path);
		/** getting an object node by path */
		CHAOS_API nlohmann::json const* GetAttributeObjectNode(nlohmann::json const * json, std::string_view path);
		/** getting or creating an object node by path */
		CHAOS_API nlohmann::json* GetOrCreateAttributeObjectNode(nlohmann::json * json, std::string_view path);
		/** getting an object node by index */
		CHAOS_API nlohmann::json* GetElementObjectNode(nlohmann::json * json, size_t index);
		/** getting an object node by inde */
		CHAOS_API nlohmann::json const* GetElementObjectNode(nlohmann::json const * json, size_t index);

		/** getting an array node by path */
		CHAOS_API nlohmann::json* GetElementArrayNode(nlohmann::json * json, std::string_view path);
		/** getting an array node by path */
		CHAOS_API nlohmann::json const* GetElementArrayNode(nlohmann::json const * json, std::string_view path);
		/** getting or creating an array node by path */
		CHAOS_API nlohmann::json* GetOrCreateAttributeArrayNode(nlohmann::json * json, std::string_view path);
		/** getting an array node by index */
		CHAOS_API nlohmann::json* GetElementArrayNode(nlohmann::json * json, size_t index);
		/** getting an array node by index */
		CHAOS_API nlohmann::json const* GetElementArrayNode(nlohmann::json const * json, size_t index);

		/** getting a structure node by path */
		CHAOS_API nlohmann::json* GetAttributeStructureNode(nlohmann::json * json, std::string_view path);
		/** getting a structure node by path */
		CHAOS_API nlohmann::json const* GetAttributeStructureNode(nlohmann::json const * json, std::string_view path);
		/** getting a structure node by index */
		CHAOS_API nlohmann::json* GetElementStructureNode(nlohmann::json * json, size_t index);
		/** getting a structure node by index */
		CHAOS_API nlohmann::json const* GetElementStructureNode(nlohmann::json const * json, size_t index);

		/** iterate over the sources of a json node (its only its self) */
		template<typename FUNC>
		decltype(auto) ForEachSource(nlohmann::json const* json, FUNC const& func);

	}; // namespace JSONTools

#else

	namespace JSONTools
	{
		template<typename FUNC>
		decltype(auto) ForEachSource(nlohmann::json const * json, FUNC const& func)
		{
			using L = meta::LambdaInfo<FUNC, nlohmann::json const *>;

			if constexpr (L::convertible_to_bool)
			{
				if (json != nullptr)
					if (decltype(auto) result = func(json))
						return result;
				return typename L::result_type{};
			}
			else
			{
				if (json != nullptr)
					func(json);
			}
		}

		template<typename T, JSONSource SRC_TYPE>
		bool GetAttribute(SRC_TYPE src, std::string_view path, T& result)
		{
			if (SRC_TYPE node = GetAttributeNode(src, path))
				return LoadFromJSON(node, result);
			return false;
		}

		template<typename T, JSONSource SRC_TYPE>
		bool GetElement(SRC_TYPE src, size_t index, T& result)
		{
			if (SRC_TYPE node = GetElementNode(src, index))
				return LoadFromJSON(node, result);
			return false;
		}

		template<typename T, JSONSource SRC_TYPE, typename Y>
		bool GetAttribute(SRC_TYPE src, std::string_view path, T& result, Y const& default_value)
		{
			if (GetAttribute(src, path, result))
				return true;
			result = default_value;
			return false;
		}

		template<typename T, JSONSource SRC_TYPE, typename Y>
		bool GetAttribute(SRC_TYPE src, std::string_view path, T& result, Y&& default_value)
		{
			if (GetAttribute(src, path, result))
				return true;
			result = std::move(default_value);
			return false;
		}

		template<typename T, JSONSource SRC_TYPE, typename Y>
		bool GetElement(SRC_TYPE src, size_t index, T& result, Y const& default_value)
		{
			if (GetElement(src, index, result))
				return true;
			result = default_value;
			return false;
		}

		template<typename T, JSONSource SRC_TYPE, typename Y>
		bool GetElement(SRC_TYPE src, size_t index, T& result, Y&& default_value)
		{
			if (GetElement(src, index, result))
				return true;
			result = std::move(default_value);
			return false;
		}

		template<typename T>
		bool SetAttribute(nlohmann::json* json, std::string_view path, T const& src)
		{
			if (json == nullptr)
				return false;
			if (nlohmann::json* node = GetOrCreateAttributeNode(json, path))
			{
				if (!node->is_object())
					*node = nlohmann::json::object();
				return SaveIntoJSON(node, src);
			}
			return false;
		}

		template<typename T>
		bool SetElement(nlohmann::json* json, size_t index, T const& src)
		{
			if (!PrepareSaveArrayIntoJSON(json))
				return false;
			json->operator [](index) = nlohmann::json();
			return SaveIntoJSON(&json->operator [](index), src);
		}

	}; // namespace JSONTools

	template<JSONSource SRC_TYPE>
	bool LoadFromJSON(SRC_TYPE src, bool& dst)
	{
		return JSONTools::ForEachSource(src, [&dst](nlohmann::json const * json)
		{
			// for bool type
			try
			{
				dst = json->get<bool>();
				return true;
			}
			catch (...)
			{
			}
			// for int type (+ conversion to bool)
			try
			{
				dst = (json->get<int>() != 0);
				return true;
			}
			catch (...)
			{
			}
			return false;
		});
	}

	template<JSONSource SRC_TYPE>
	bool LoadFromJSON(SRC_TYPE src, boost::filesystem::path& dst)
	{
		std::string result;
		if (!LoadFromJSON(src, result))
			return false;
		dst = result;
		return true;
	}

	template<typename T, JSONSource SRC_TYPE>
	bool LoadFromJSON(SRC_TYPE src, T& dst)
	{
		// target is an enum
		if constexpr (std::is_enum_v<T>)
		{
			std::string encoded_str;
			if (!LoadFromJSON(src, encoded_str))
				return false;
			if (!StringToEnum(encoded_str.c_str(), dst))
				return false;
			return true;
		}
		// target is an object with SerializeFromJSON implementation
		else if constexpr (check_method_SerializeFromJSON_v<T, nlohmann::json const*>)
		{
			// check for strict class equality between target and JSON data
			if constexpr (HasGetClass<T>)
			{
				// check classname only if provided
				std::string classname;
				if (JSONTools::GetAttribute(src, "classname", classname))
				{
					SubClassOf<T> subclass = ClassManager::GetDefaultInstance()->FindClass(classname.c_str());
					if (!subclass.IsValid())
						return false;
				}
			}
			return dst.SerializeFromJSON(src);
		}
		// fallback: target is a native type
		// for native types
		else if constexpr (HasDoLoadFromJSON<T>)
		{
			return DoLoadFromJSON(src, dst);
		}
		else
		{
			return JSONTools::ForEachSource(src, [&dst](nlohmann::json const* json)
			{
				try
				{
					dst = json->get<T>(); // may throw an exception
					return true;
				}
				catch (...)
				{
				}
				return false;
			});
		}

		return false;
	}

	template<typename T, JSONSource SRC_TYPE>
	T* LoadFromJSONCreateObject(SRC_TYPE src)
	{
		std::string classname;
		if (JSONTools::GetAttribute(src, "classname", classname))
		{
			Class const* json_class = ClassManager::GetDefaultInstance()->FindClass(classname.c_str());
			if (json_class != nullptr)
			{
				Class const* dst_class = ClassManager::GetDefaultInstance()->FindCPPClass<T>();
				if (dst_class != nullptr)
					if (json_class->InheritsFrom(dst_class, true) == InheritanceType::YES) // accept equal
						return (T*)json_class->CreateInstance();
			}
		}
		return new T;
	}

	template<typename T, JSONSource SRC_TYPE, typename DELETER>
	bool LoadFromJSON(SRC_TYPE src, std::unique_ptr<T, DELETER>& dst)
	{
		std::unique_ptr<T, DELETER> other(LoadFromJSONCreateObject<T>(src)); // force to use another smart pointer and swap due to lake of copy
		if (other == nullptr)
			return false;
		if (!LoadFromJSON(src, *other))
			return false;
		std::swap(dst, other);
		return true;
	}

	template<typename T, JSONSource SRC_TYPE>
	bool LoadFromJSON(SRC_TYPE src, shared_ptr<T>& dst)
	{
		shared_ptr<T> other = LoadFromJSONCreateObject<T>(src);
		if (other == nullptr)
			return false;
		if (!LoadFromJSON(src, *other))
			return false;
		std::swap(dst, other);
		return true;
	}

	template<typename T, JSONSource SRC_TYPE>
	bool LoadFromJSON(SRC_TYPE src, std::vector<T>& dst)
	{
		dst.clear();
		return JSONTools::ForEachSource(src, [&dst](nlohmann::json const * json)
		{
			// input is an array
			if (json->is_array())
			{
				for (auto const& json_element : *json)
				{
					T element;
					if (LoadFromJSON(&json_element, element))
						dst.push_back(std::move(element));
				}
				return true;
			}
			// considere input as an array of a single element
			T element;
			if (!LoadFromJSON(json, element))
				return false;
			dst.push_back(std::move(element));
			return true;
		});
	}

	template<typename T, JSONSource SRC_TYPE>
	bool LoadFromJSON(SRC_TYPE src, std::optional<T>& dst)
	{
		T other;
		if (LoadFromJSON(src, other))
		{
			dst = std::move(other);
			return true;
		}
		dst.reset();
		return false;
	}

	template<typename T>
	bool SaveIntoJSON(nlohmann::json * json, T const& src)
	{
		// early exit
		if (json == nullptr)
			return false;

		// target is an enum
		if constexpr (std::is_enum_v<T>)
		{
			char buffer[256];
			if (char const* encoded_str = EnumToString(src, buffer, 256))
				return SaveIntoJSON(json, encoded_str);
			return false;
		}
		// class has its own implementation
		else if constexpr (check_method_SerializeIntoJSON_v<T const, nlohmann::json*>)
		{
			// we need a json object to store a C++ object
			if (!json->is_object())
				*json = nlohmann::json::object(); // create the JSON object if not already set

			// get the class of the C++ object
			Class const* src_class = nullptr;
			if constexpr (check_method_GetClass_v<T const>)
				src_class = src.GetClass();
			if (src_class == nullptr || !src_class->IsDeclared())
				src_class = ClassManager::GetDefaultInstance()->FindCPPClass<T>();
			// write the class into the json object
			if (src_class != nullptr && src_class->IsDeclared())
				JSONTools::SetAttribute(json, "classname", src_class->GetClassName());
			// save into JSON
			return src.SerializeIntoJSON(json);
		}
		else if constexpr (HasDoSaveIntoJSON<T>)
		{
			return DoSaveIntoJSON(json, src);
		}
		else
		{
			try
			{
				*json = src;
				return true;
			}
			catch (...)
			{
			}
			return false;
		}
	}

	template<typename T, typename DELETER>
	bool SaveIntoJSON(nlohmann::json * json, std::unique_ptr<T, DELETER> const& src)
	{
		if (src == nullptr)
			return true;
		return SaveIntoJSON(json, *src);
	}

	template<typename T>
	bool SaveIntoJSON(nlohmann::json * json, shared_ptr<T> const& src)
	{
		if (src == nullptr)
			return true;
		return SaveIntoJSON(json, *src);
	}

	template<typename T>
	bool SaveIntoJSON(nlohmann::json * json, std::vector<T> const& src)
	{
		if (json == nullptr)
			return false;
		*json = nlohmann::json::array();
		for (auto const& element : src)
		{
			nlohmann::json j;
			if (SaveIntoJSON(&j, element))
				json->push_back(std::move(j));
		}
		return true;
	}

	template<typename T>
	bool SaveIntoJSON(nlohmann::json* json, std::optional<T> const& src)
	{
		if (!src.has_value())
			return false;
		return SaveIntoJSON(json, src.value());
	}

#endif

}; // namespace chaos