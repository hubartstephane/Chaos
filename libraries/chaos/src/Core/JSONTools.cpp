#include <chaos/Chaos.h>

#if 0

#include <chaos/JSONTools.h>
#include <chaos/JSONRecursiveLoader.h>
#include <chaos/FileTools.h>
#include <chaos/Application.h>

#endif



namespace chaos
{
	// =================
	// EXTERNAL FUNCTION
	// =================

	bool LoadFromJSON(nlohmann::json const & entry, bool & dst)
	{
		try
		{
			dst = entry.get<bool>(); // for bool type
			return true;
		}
		catch (...)
		{
			try
			{
				dst = (entry.get<int>() != 0); // for int type (+ conversion to bool)
				return true;
			}
			catch (...)
			{
			}
		}
		return false;
	}

	template<typename T>
	static T* LoadFromJSONCreateObject(nlohmann::json const& entry)
	{
		if (entry.is_object())
		{
			std::string classname;
			if (JSONTools::GetAttribute(entry, "classname", classname))
			{
				Class const* json_class = Class::FindClass(classname.c_str());
				if (json_class != nullptr)
				{
					Class const* dst_class = Class::FindClass<T>();
					if (dst_class != nullptr)
						if (json_class->InheritsFrom(dst_class, true) == InheritanceType::YES) // accept equal
							return (T*)json_class->CreateInstance();
				}
			}
		}
		return new T;
	}

	template<typename T, typename DELETER>
	bool LoadFromJSON(nlohmann::json const& entry, std::unique_ptr<T, DELETER>& dst)
	{
		std::unique_ptr<T, DELETER> other(LoadFromJSONCreateObject<T>(entry)); // force to use another smart pointer and swap due to lake of copy 
		if (other == nullptr)
			return false;
		if (!LoadFromJSON(entry, *other))
			return false;
		std::swap(dst, other);
		return true;
	}

	template<typename T>
	bool LoadFromJSON(nlohmann::json const& entry, shared_ptr<T>& dst)
	{
		shared_ptr<T> other = LoadFromJSONCreateObject<T>(entry);
		if (other == nullptr)
			return false;
		if (!LoadFromJSON(entry, *other))
			return false;
		std::swap(dst, other);
		return true;
	}

	template<typename T>
	bool LoadFromJSON(nlohmann::json const& entry, std::vector<T>& dst)
	{
		dst.clear();
		// input is an array		
		if (entry.is_array())
		{
			for (auto const& json_entry : entry)
			{
				T element;
				if (LoadFromJSON(json_entry, element))
					dst.push_back(std::move(element));
			}
			return true;
		}
		// considere input as an array of a single element
		T element;
		if (!LoadFromJSON(entry, element))
			return false;
		dst.push_back(std::move(element));
		return true;
	}

	template<typename T>
	bool LoadFromJSON(nlohmann::json const& entry, T& dst)
	{
		if constexpr (std::is_pointer_v<T>)
		{
			using T2 = std::remove_pointer_t<T>;

			// object instanciation
			T2* other = LoadFromJSONCreateObject<T2>(entry); // XXX : use classname to generate the object
			if (other == nullptr)
				return false;
			if (!LoadFromJSON(entry, *other)) // XXX: in that case, there is a useless check for classname again, not a big deal
			{
				delete(other);
				return false;
			}
			dst = other;
			return true;
		}
		// class has its own implementation
		else if constexpr (check_method_SerializeFromJSON_v<T, nlohmann::json const&>)
		{
			// check whether data is an object
			if (!entry.is_object())
				return false;
			// check for strict class equality between target and JSON data
			if constexpr (check_method_GetClass_v<T const>)
			{
				// shu48
				// peut etre qu on devrait accepter les entrées ou classname n est pas specifiée ?


				Class const* src_class = dst.GetClass();
				if (src_class == nullptr || !src_class->IsDeclared())
					return false;

				std::string classname;
				JSONTools::GetAttribute(entry, "classname", classname);

				Class const* json_class = Class::FindClass(classname.c_str());
				if (json_class == nullptr || !json_class->IsDeclared() || json_class != src_class)
					return false;
			}
			return dst.SerializeFromJSON(entry);
		}
		// for native types
		else
		{
			try
			{
				dst = entry.get<T>(); // may throw an exception
				return true;
			}
			catch (...)
			{
			}
			return false;
		}
	}

	template<typename T, typename DELETER>
	bool SaveIntoJSON(nlohmann::json& entry, std::unique_ptr<T, DELETER> const& src)
	{
		if (src == nullptr)
			return true;
		return SaveIntoJSON(entry, *src);
	}

	template<typename T>
	bool SaveIntoJSON(nlohmann::json& entry, shared_ptr<T> const& src)
	{
		if (src == nullptr)
			return true;
		return SaveIntoJSON(entry, *src);
	}

	template<typename T>
	bool SaveIntoJSON(nlohmann::json& entry, std::vector<T> const& src)
	{
		entry = nlohmann::json::array();
		for (auto const& element : src)
		{
			nlohmann::json j;
			if (SaveIntoJSON(j, element))
				entry.push_back(std::move(j));
		}
		return true;
	}
	template<typename T>
	bool SaveIntoJSON(nlohmann::json& entry, T const& src) // copy for basic types
	{
		// check for pointer
		if constexpr (std::is_pointer_v<T>)
		{
			if (src == nullptr)
				return true;
			return SaveIntoJSON(entry, *src);
		}
		// class has its own implementation
		else if constexpr (std::is_class_v<T> && !std::is_same_v<T, std::string>) // string is to be handled in the native json way
		{
			if constexpr (check_method_SerializeIntoJSON_v<T const, nlohmann::json&>)
			{
				// we need a json object to store a C++ object
				if (!entry.is_object())
				{
					if (!entry.is_null())
						return false;
					entry = nlohmann::json::object(); // create the JSON object if not already set
				}

				// get the class of the C++ object
				Class const* src_class = nullptr;
				if constexpr (check_method_GetClass_v<T const>)
					src_class = src.GetClass();
				if (src_class == nullptr || !src_class->IsDeclared())
					src_class = Class::FindClass<T>();
				// write the class into the json object
				if (src_class != nullptr && src_class->IsDeclared())
					JSONTools::SetAttribute(entry, "classname", src_class->GetClassName());

				// save into JSON
				return src.SerializeIntoJSON(entry);
			}
			else
			{
				return false; // do not know how to save the object (we should never come here if the object had a Save method or if there was a standalone Save function)
			}
		}
		// for native types
		else
		{
			try
			{
				entry = src;
				return true;
			}
			catch (...)
			{
			}
			return false;
		}
	}

	template<typename T, typename ENCODE_TABLE>
	bool SaveEnumIntoJSON(nlohmann::json& json_entry, ENCODE_TABLE const& encode_table, T const& src)
	{
		std::string encoded_str;
		if (!EnumTools::EnumToString(src, encode_table, encoded_str))
			return false;
		return SaveIntoJSON(json_entry, encoded_str);
	}

	template<typename T, typename ENCODE_TABLE>
	bool LoadEnumFromJSON(nlohmann::json const& json_entry, ENCODE_TABLE const& encode_table, T& dst)
	{
		std::string encoded_str;
		if (!LoadFromJSON(json_entry, encoded_str))
			return false;
		if (!EnumTools::StringToEnum(encoded_str.c_str(), encode_table, dst))
			return false;
		return true;
	}











	// =================
	// JSONTools
	// =================

	namespace JSONTools
	{
		template<typename T>
		bool SetAttribute(nlohmann::json& entry, char const* name, T const& src)
		{
			assert(name != nullptr);
			if (entry.is_null())
				entry = nlohmann::json::object();
			else if (!entry.is_object())
				return false;
			entry[name] = nlohmann::json();
			return SaveIntoJSON(entry[name], src);
		}

		template<typename T>
		bool SetAttributeByIndex(nlohmann::json& entry, size_t index, T const& src)
		{
			if (entry.is_null())
				entry = nlohmann::json::array();
			else if (!entry.is_array())
				return false;
			entry[index] = nlohmann::json();
			return SaveIntoJSON(entry[index], src);
		}

		template<typename T, typename ENCODE_TABLE>
		bool SetEnumAttribute(nlohmann::json& entry, char const* name, ENCODE_TABLE const& encode_table, T const& src)
		{
			std::string encoded_src;
			if (!EnumTools::EnumToString(src, encode_table, encoded_src))
				return false;
			return SetAttribute(entry, name, encoded_src);
		}

		template<typename T, typename ENCODE_TABLE>
		bool SetEnumAttributeByIndex(nlohmann::json& entry, size_t index, ENCODE_TABLE const& encode_table, T const& src)
		{
			std::string encoded_src;
			if (!EnumTools::EnumToString(src, encode_table, encoded_src))
				return false;
			return SetAttributeByIndex(entry, index, encoded_src);
		}

		template<typename T>
		bool GetAttribute(nlohmann::json const& entry, char const* name, T& result)
		{
			assert(name != nullptr);
			if (!entry.is_object())
				return false;
			nlohmann::json::const_iterator it = entry.find(name);
			if (it == entry.end())
				return false;
			return LoadFromJSON(*it, result);
		}

		template<typename T>
		bool GetAttributeByIndex(nlohmann::json const& entry, size_t index, T& result)
		{
			if (!entry.is_array() || index >= entry.size())
				return false;
			return LoadFromJSON(entry[index], result);
		}

		template<typename T, typename Y>
		bool GetAttribute(nlohmann::json const& entry, char const* name, T& result, Y default_value)
		{
			if (GetAttribute(entry, name, result))
				return true;
			result = default_value;
			return false;
		}

		template<typename T, typename Y>
		bool GetAttributeByIndex(nlohmann::json const& entry, size_t index, T& result, Y default_value)
		{
			if (GetAttributeByIndex(entry, index, result))
				return true;
			result = default_value;
			return false;
		}

		template<typename T, typename ENCODE_TABLE>
		bool GetEnumAttribute(nlohmann::json const& entry, char const* name, ENCODE_TABLE const& encode_table, T& result)
		{
			std::string str_result;
			if (!GetAttribute(entry, name, str_result))
				return false;
			return EnumTools::StringToEnum(str_result.c_str(), encode_table, result);
		}

		template<typename T, typename ENCODE_TABLE>
		bool GetEnumAttributeByIndex(nlohmann::json const& entry, size_t index, ENCODE_TABLE const& encode_table, T& result)
		{
			std::string str_result;
			if (!GetAttributeByIndex(entry, index, str_result))
				return false;
			return EnumTools::StringToEnum(str_result.c_str(), encode_table, result);
		}

		template<typename T, typename ENCODE_TABLE, typename Y>
		bool GetEnumAttribute(nlohmann::json const& entry, char const* name, ENCODE_TABLE const& encode_table, T& result, Y default_value)
		{
			if (GetEnumAttribute(entry, name, encode_table, result))
				return true;
			result = default_value;
			return false;
		}

		template<typename T, typename ENCODE_TABLE, typename Y>
		bool GetEnumAttributeByIndex(nlohmann::json const& entry, size_t index, ENCODE_TABLE const& encode_table, T& result, Y default_value)
		{
			if (GetEnumAttributeByIndex(entry, index, encode_table, result))
				return true;
			result = default_value;
			return false;
		}

		nlohmann::json* GetStructure(nlohmann::json& entry, char const* name)
		{
			if (entry.is_object())
			{
				auto it = entry.find(name);
				if (it != entry.end() && it->is_structured())
					return &*it;
			}
			return nullptr;
		}

		nlohmann::json const* GetStructure(nlohmann::json const& entry, char const* name)
		{
			if (entry.is_object())
			{
				auto it = entry.find(name);
				if (it != entry.end() && it->is_structured())
					return &*it;
			}
			return nullptr;
		}

		nlohmann::json* GetStructureByIndex(nlohmann::json& entry, size_t index)
		{
			if (entry.is_array() && index < entry.size())
			{
				nlohmann::json& result = entry[index];
				if (result.is_structured())
					return &result;
			}
			return nullptr;
		}

		nlohmann::json const* GetStructureByIndex(nlohmann::json const& entry, size_t index)
		{
			if (entry.is_array() && index < (size_t)entry.size())
			{
				nlohmann::json const& result = entry[index];
				if (result.is_structured())
					return &result;
			}
			return nullptr;
		}

		bool Parse(char const* buffer, nlohmann::json& result)
		{
			assert(buffer != nullptr);
			try
			{
				result = nlohmann::json::parse(buffer);
				return true;
			}
			catch (std::exception & error)
			{
				char const* error_string = error.what();
				return false;
			}
			catch (...)
			{
			}
			return false;
		}

		bool ParseRecursive(char const* buffer, boost::filesystem::path const& config_path, nlohmann::json& result)
		{
			assert(buffer != nullptr);
			JSONRecursiveLoader loader;
			return loader.ParseJSONFile(buffer, config_path, result);
		}

		bool LoadJSONFile(FilePathParam const& path, nlohmann::json& result, bool recursive)
		{
			if (!recursive)
			{
				Buffer<char> buffer = FileTools::LoadFile(path, true);
				if (buffer != nullptr)
					return Parse(buffer.data, result);
				return false; // loading failure
			}
			else
			{
				JSONRecursiveLoader loader;
				return loader.LoadJSONFile(path, result);
			}
		}

		boost::filesystem::path DumpConfigFile(nlohmann::json const& json, char const* filename)
		{
			boost::filesystem::path result;
			if (filename == nullptr)
				return result;

			Application* application = Application::GetInstance();
			if (application != nullptr)
			{
				result = application->CreateUserLocalTempDirectory();
			}
			else
			{
				FileTools::CreateTemporaryDirectory("MyTempDirectory_%d", result);
			}

			if (!result.empty())
			{
				boost::filesystem::path path = result / filename;
				std::ofstream stream(path.string().c_str());
				stream << json.dump(4);
			}

			return result;
		}

	}; // namespace JSONTools

}; // namespace chaos
