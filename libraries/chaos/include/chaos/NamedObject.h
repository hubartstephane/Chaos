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

		/** the name of the object */
		std::string name;
		/** the tag of the object */
		TagType tag = 0;

	public:

		/** search element in a vector */
		template<typename T, typename P>
		static T * FindNamedObject(std::vector<P> & elements, TagType tag)
		{
			// maybe tag could math the position in the array
			size_t count = elements.size();
			if (tag >= 0 && tag < (TagType)count)
			{
				if (meta::get_raw_pointer(elements[tag])->tag == tag)
					return meta::get_raw_pointer(elements[tag]);
			}
			// search in the list
			for (size_t i = 0; i < count; ++i)
				if (meta::get_raw_pointer(elements[i])->tag == tag)
					return meta::get_raw_pointer(elements[i]);
			return nullptr;
		}
		/** search element in a vector */
		template<typename T, typename P>
		static T const * FindNamedObject(std::vector<P> const & elements, TagType tag)
		{
			// maybe tag could math the position in the array
			size_t count = elements.size();
			if (tag >= 0 && tag < (TagType)count)
			{
				if (meta::get_raw_pointer(elements[tag])->tag == tag)
					return meta::get_raw_pointer(elements[tag]);
			}
			// search in the list
			for (size_t i = 0; i < count; ++i)
				if (meta::get_raw_pointer(elements[i])->tag == tag)
					return meta::get_raw_pointer(elements[i]);
			return nullptr;
		}
		/** search element in a vector */
		template<typename T, typename P>
		static T * FindNamedObject(std::vector<P> & elements, char const * in_name)
		{
			// early exit
			if (in_name == nullptr)
				return nullptr;
			// search in the list
			size_t count = elements.size();
			for (size_t i = 0; i < count; ++i)
				if (meta::get_raw_pointer(elements[i])->name == in_name)
					return meta::get_raw_pointer(elements[i]);
			return nullptr;
		}
		/** search element in a vector */
		template<typename T, typename P>
		static T const * FindNamedObject(std::vector<P> const & elements, char const * in_name)
		{
			// early exit
			if (in_name == nullptr)
				return nullptr;
			// search in the list
			size_t count = elements.size();
			for (size_t i = 0; i < count; ++i)
				if (meta::get_raw_pointer(elements[i])->name == in_name)
					return meta::get_raw_pointer(elements[i]);
			return nullptr;
		}
	};
}; // namespace chaos
