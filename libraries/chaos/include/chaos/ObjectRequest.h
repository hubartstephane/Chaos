#pragma once

#include <chaos/StandardHeaders.h>
#include <chaos/MetaProgramming.h>
#include <chaos/Object.h>
#include <chaos/StringTools.h>
#include <chaos/Tag.h>

namespace chaos
{
	/** a utility class for parameter passing */
	enum class ObjectRequestType
	{
		/** accept anything */
		EMPTY = 0,
		/** search by string */
		STRING = 1,
		/** search by tag */
		TAG = 2
	};

    /** a utility class for parameter passing */
    class ObjectRequest
    {
    public:

		/** constructor */
		ObjectRequest() = default;
		/** constructor */
		ObjectRequest(ObjectRequest const & src) = default;
        /** constructor */
        ObjectRequest(char const* in_name) :
            name(in_name), request_type(ObjectRequestType::STRING)
        {            
        }
		/** constructor */
		ObjectRequest(std::string const & in_name) :
			name(in_name.c_str()), request_type(ObjectRequestType::STRING)
		{			
		}
        /** constructor */
        ObjectRequest(TagType in_tag) :
            tag(in_tag), request_type(ObjectRequestType::TAG)
        {
        }

		/** test whether the object name/tag match */
		template<typename T>
		bool Match(T const& object) const // use template to use NamedObjectWrapper as well as NamedObject
		{
			if (request_type == ObjectRequestType::EMPTY)
				return true;
			else if (request_type == ObjectRequestType::STRING)
				return (StringTools::Stricmp(object.GetName(), name) == 0);
			else if (request_type == ObjectRequestType::TAG)
				return (object.GetTag() == tag);
			return false; // should never happen
		}

		/** search element in a vector */
		template<typename P>
		auto FindObject(std::vector<P>& elements) const -> decltype(meta::get_raw_pointer(elements[0]))
		{
			// search in the list
			size_t count = elements.size();
			for (size_t i = 0; i < count; ++i)
			{
				auto e = meta::get_raw_pointer(elements[i]);
				if (Match(*e))
					return e;
			}
			return nullptr;
		}
		/** search element in a vector */
		template<typename P>
		auto FindObject(std::vector<P> const& elements) const -> decltype(meta::get_raw_pointer(elements[0]))
		{
			// search in the list
			size_t count = elements.size();
			for (size_t i = 0; i < count; ++i)
			{
				auto e = meta::get_raw_pointer(elements[i]);
				if (Match(*e))
					return e;
			}
			return nullptr;
		}

	public:

        /** the name for the request */
        char const* name = nullptr;
        /** the tag for the request */
        TagType tag = 0;
        /** the kind of request of interrest */
		ObjectRequestType request_type = ObjectRequestType::EMPTY;
    };

}; // namespace chaos
