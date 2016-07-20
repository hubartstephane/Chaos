#pragma once

#include <chaos/StandardHeaders.h>

namespace chaos
{

/**
 * Simply a base class
 */
class EmptyClass {};


/**
 * Simply a base class with a virtual destructor
 */
class EmptyClassWithVirtualDestructor
{
public:

  virtual ~EmptyClassWithVirtualDestructor(){}
};

/**
 * A meta function to select one of the previous class according to a boolean
 */
template<bool VIRTUAL_DESTRUCTOR>
struct BaseEmptyClass : public boost::mpl::if_c<VIRTUAL_DESTRUCTOR, EmptyClassWithVirtualDestructor, EmptyClass>{};

}; // namespace chaos
