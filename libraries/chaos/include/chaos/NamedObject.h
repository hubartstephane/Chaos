#pragma once

#include <chaos/StandardHeaders.h>

namespace chaos
{
	/** a tag is an integer that can reference any address */
	using TagType = intptr_t;

	/** a class that describe an object that can be reference by tag and by name */
	class NamedObject
	{
	public:

		/** constructor */
		NamedObject() : tag(0){}

		/** the name of the object */
		std::string name;
		/** the tag of the object */
		TagType tag;
	};

}; // namespace chaos
