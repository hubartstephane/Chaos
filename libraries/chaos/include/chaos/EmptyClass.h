#pragma once

#include <chaos/StandardHeaders.h>

namespace chaos
{

	/**
   * Tag methods
	 */

BOOST_MPL_HAS_XXX_TRAIT_NAMED_DEF(has_nocopy, NoCopyTag, true)
BOOST_MPL_HAS_XXX_TRAIT_NAMED_DEF(has_virtual_destructor, VirtualDestructorTag, true)

class NoCopy
{
public:

	class NoCopyTag {};
};

class VirtualDestructor
{
public:

	class VirtualDestructorTag {};
};

/**
 * Simply a base class
 */

class EmptyClass {};

/**
 * VirtualDestructor : an utility class to add a virtual destructor to any base class 
 */

template<typename BASE_CLASS = EmptyClass>
class AddVirtualDestructor : public BASE_CLASS
{
public:

  /** simply a virtual destructor */
  virtual ~AddVirtualDestructor() {}
};

/**
 * ConditionnalAddVirtualDestructor : an utility class to conditionnally add a virtual destructor to any base class 
 */

template<typename COND, typename BASE_CLASS = EmptyClass>
using ConditionnalAddVirtualDestructor = boost::mpl::eval_if <
  COND,
  AddVirtualDestructor<BASE_CLASS>,
  BASE_CLASS>;


/**
 * RemoveCopy : an utility class to suppress copy construction/operator
 */

template<typename BASE_CLASS = EmptyClass>
class RemoveCopy : public BASE_CLASS
{
public:

  /** keep default constructor */
	RemoveCopy() {}
  /** delete copy constructor */
	RemoveCopy(RemoveCopy const &) = delete;
  /** delete copy operator */
	RemoveCopy & operator = (RemoveCopy const &) = delete;
};


/**
 * ConditionnalRemoveCopy : an utility class to conditionnally suppress copy construction/operator
 */

template<typename COND, typename BASE_CLASS = EmptyClass>
using ConditionnalRemoveCopy = boost::mpl::eval_if <
	COND,
	RemoveCopy<BASE_CLASS>,
	BASE_CLASS>;





/** template */
template<bool VIRTUAL_DESTRUCTOR, bool CANCOPY>
class ClassBase {};



}; // namespace chaos
