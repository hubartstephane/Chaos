#pragma once

#include <chaos/StandardHeaders.h>

namespace chaos
{

/** generate meta tags functions */
BOOST_MPL_HAS_XXX_TRAIT_NAMED_DEF(has_nocopy_tag, nocopy_tag, true)
BOOST_MPL_HAS_XXX_TRAIT_NAMED_DEF(has_vdestroy_tag, vdestroy_tag, true)

/** tag classes generation */
#define CHAOS_GENERATE_TAG_CLASS(name)  struct name { class name##_tag {}; }
CHAOS_GENERATE_TAG_CLASS(nocopy);
CHAOS_GENERATE_TAG_CLASS(vdestroy);

/** Simply a base class */
class EmptyClass {};

/** utility class to add a virtual destructor to any base class */
template<typename BASE_CLASS = EmptyClass>
class add_vdestroy : public BASE_CLASS
{
public:

  /** simply a virtual destructor */
  virtual ~add_vdestroy() {}
};

/** an utility class to conditionnally add a virtual destructor to any base class */
template<typename COND, typename BASE_CLASS = EmptyClass>
using cond_add_vdestroy = boost::mpl::eval_if <
  COND,
	add_vdestroy<BASE_CLASS>,
  BASE_CLASS>;

/** RemoveCopy : an utility class to suppress copy construction/operator */
template<typename BASE_CLASS = EmptyClass>
class remove_copy : public BASE_CLASS
{
public:

  /** keep default constructor */
	remove_copy() {}
  /** delete copy constructor */
	remove_copy(remove_copy const &) = delete;
  /** delete copy operator */
	remove_copy & operator = (remove_copy const &) = delete;
};

/** an utility class to conditionnally suppress copy construction/operator */
template<typename COND, typename BASE_CLASS = EmptyClass>
using cond_remove_copy = boost::mpl::eval_if <
	COND,
	remove_copy<BASE_CLASS>,
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
