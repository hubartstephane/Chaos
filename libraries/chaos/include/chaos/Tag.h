#pragma once

#include <chaos/StandardHeaders.h>

// XXX : automatic TagType generation
//
//  at first, i test for an automatic tag generation with 
//
//  #define CHAOS_DECLARE_TAG(x) static chaos::TagType const x = (chaos::TagType const)&x
//
//  seemed find until i realized that each time header containing this definition was included, there was a copy of the TagType (with a different address)
//  the '#pragma once' was not preventing this issue due to several file object 'building'
//
//   => in library chaos
//   => in library death (using chaos)
//   => in the executable (using both death and chaos)
//
//  so i used a function that was creating a TagType with its variable name (and using a cache to store result)
//
//  The macro deserve to be used in declaration and so is not thread safe (the function it calls is not thread safe)

#define CHAOS_DECLARE_TAG(x) static chaos::TagType x = chaos::DeclareTag(#x)

namespace chaos
{
	/** a tag is an integer that can reference any address */
	using TagType = uintptr_t;

	/** function to generate a TagType from a name (XXX : not thread safe) */
	TagType DeclareTag(char const * name);

}; // namespace chaos
