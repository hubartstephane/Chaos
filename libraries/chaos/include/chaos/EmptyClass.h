#pragma once

#include <chaos/StandardHeaders.h>

namespace chaos
{

/** generate meta tags functions */
BOOST_MPL_HAS_XXX_TRAIT_NAMED_DEF(has_nocopy_tag, NoCopyTag, true)
BOOST_MPL_HAS_XXX_TRAIT_NAMED_DEF(has_virtual_destructor_tag, VirtualDestructorTag, true)

/** tag classes generation */
#define CHAOS_GENERATE_TAG_CLASS(name)  struct name { class name##Tag {}; }
CHAOS_GENERATE_TAG_CLASS(NoCopy);
CHAOS_GENERATE_TAG_CLASS(VirtualDestructor);

/** Simply a base class */
class EmptyClass {};

/** utility class to add a virtual destructor to any base class */
template<typename BASE_CLASS = EmptyClass>
class AddVirtualDestructor : public BASE_CLASS
{
public:

  /** simply a virtual destructor */
  virtual ~AddVirtualDestructor() {}
};

/** an utility class to conditionnally add a virtual destructor to any base class */
template<typename COND, typename BASE_CLASS = EmptyClass>
using ConditionnalAddVirtualDestructor = boost::mpl::eval_if <
  COND,
  AddVirtualDestructor<BASE_CLASS>,
  BASE_CLASS>;

/** RemoveCopy : an utility class to suppress copy construction/operator */
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

/** an utility class to conditionnally suppress copy construction/operator */
template<typename COND, typename BASE_CLASS = EmptyClass>
using ConditionnalRemoveCopy = boost::mpl::eval_if <
	COND,
	RemoveCopy<BASE_CLASS>,
	BASE_CLASS>;


#if 0

template<typename SEQ>
using BaseClassImpl = 
chaos::ConditionnalRemoveCopy<
	chaos::meta::has_satisfying_element<SEQ, chaos::has_nocopy<boost::mpl::_>>,
	chaos::ConditionnalAddVirtualDestructor<
	chaos::meta::has_satisfying_element<SEQ, chaos::has_virtual_destructor<boost::mpl::_>>,
	chaos::EmpyClass
	>
>
>;


template<typename ...TAGS>
using BaseClass = BaseClassImpl<boost::mpl::vector<TAGS...>>;

#endif




}; // namespace chaos
