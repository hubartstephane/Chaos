#pragma once

#include <chaos/StandardHeaders.h>
#include <chaos/MetaProgramming.h>

namespace chaos
{
	/** a tag is an integer that can reference any address */
	using TagType = uintptr_t;

	/** a class that describe an object that can be reference by tag and by name */
	class NamedObject
	{
	public:

		/** get the name of the object */
		char const * GetName() const { return name.c_str(); }
		/** get the Tag of the object */
		TagType GetTag() const { return tag; }

	public:

		/** the name of the object */
		std::string name;
		/** the tag of the object */
		TagType tag = 0;

	public:

		/** search element in a vector */
		template<typename P>
		static auto FindNamedObject(std::vector<P> & elements, TagType tag) -> decltype(meta::get_raw_pointer(elements[0]))
		{
			// maybe tag could math the position in the array
			size_t count = elements.size();
			if (tag >= 0 && (size_t)tag < count)
			{
				if (meta::get_raw_pointer(elements[tag])->GetTag() == tag)
					return meta::get_raw_pointer(elements[tag]);
			}
			// search in the list
			for (size_t i = 0; i < count; ++i)
				if (meta::get_raw_pointer(elements[i])->GetTag() == tag)
					return meta::get_raw_pointer(elements[i]);
			return nullptr;
		}
		/** search element in a vector */
		template<typename P>
		static auto FindNamedObject(std::vector<P> const & elements, TagType tag) -> decltype(meta::get_raw_pointer(elements[0]))
		{
			// maybe tag could math the position in the array
			size_t count = elements.size();
			if (tag >= 0 && (size_t)tag < count)
			{
				if (meta::get_raw_pointer(elements[tag])->GetTag() == tag)
					return meta::get_raw_pointer(elements[tag]);
			}
			// search in the list
			for (size_t i = 0; i < count; ++i)
				if (meta::get_raw_pointer(elements[i])->GetTag() == tag)
					return meta::get_raw_pointer(elements[i]);
			return nullptr;
		}
		/** search element in a vector */
		template<typename P>
		static auto FindNamedObject(std::vector<P> & elements, char const * in_name) -> decltype(meta::get_raw_pointer(elements[0]))
		{
			// early exit
			if (in_name == nullptr)
				return nullptr;
			// search in the list
			size_t count = elements.size();
			for (size_t i = 0; i < count; ++i)
				if (strcmp(meta::get_raw_pointer(elements[i])->GetName(), in_name) == 0)
					return meta::get_raw_pointer(elements[i]);
			return nullptr;
		}
		/** search element in a vector */
		template<typename P>
		static auto FindNamedObject(std::vector<P> const & elements, char const * in_name) -> decltype(meta::get_raw_pointer(elements[0]))
		{
			// early exit
			if (in_name == nullptr)
				return nullptr;
			// search in the list
			size_t count = elements.size();
			for (size_t i = 0; i < count; ++i)
				if (strcmp(meta::get_raw_pointer(elements[i])->GetName(), in_name) == 0)
					return meta::get_raw_pointer(elements[i]);
			return nullptr;
		}
	};

	/** NamedObjectWrapper : this is a wrapper to ba able to use NamedObject's static methods */
	template<typename T>
	class NamedObjectWrapper
	{
	public:

		/** special method to have access to NamedObject static utility functions */
		char const * GetName() const { return object->GetName(); }
		/** special method to have access to NamedObject static utility functions */
		TagType GetTag() const { return object->GetTag(); }

	public:

		/** the renderable to render */
		boost::intrusive_ptr<T> object;
	};

	/** function to serialize into JSON */
	void SaveIntoJSON(NamedObject const & info, nlohmann::json & json_entry);
	/** function to serialize from JSON */
	void LoadFromJSON(NamedObject & info, nlohmann::json const & json_entry);

}; // namespace chaos
