#pragma once

#include <chaos/StandardHeaders.h>

namespace chaos
{

/** tag classes generation */
#define CHAOS_GENERATE_TAG_CLASS(name)\
	BOOST_MPL_HAS_XXX_TRAIT_NAMED_DEF(has_##name##_tag, name##_tag, true)\
	struct name { class name##_tag {}; }

CHAOS_GENERATE_TAG_CLASS(nocopy);
CHAOS_GENERATE_TAG_CLASS(vdestroy);
CHAOS_GENERATE_TAG_CLASS(logger);

/** Simply a base class */
using EmptyClass = boost::mpl::empty_base;



template<typename COND, typename FUNC, typename BASE_CLASS>
using cond_enrich_class = boost::mpl::eval_if <
	COND,
	typename boost::mpl::apply<FUNC, BASE_CLASS>::type,
	BASE_CLASS>;



/** utility class to add a virtual destructor to any base class */
template<typename BASE_CLASS = EmptyClass>
class add_vdestroy : public BASE_CLASS
{
public:

  /** simply a virtual destructor */
  virtual ~add_vdestroy() {}
};


template<typename COND, typename BASE_CLASS = EmptyClass>
using cond_add_vdestroy = cond_enrich_class<COND, add_vdestroy<boost::mpl::_1>, BASE_CLASS>;

/** utility class to add loggers at construction/destruction */
template<typename BASE_CLASS = EmptyClass>
class add_logger : public BASE_CLASS
{
public:

	/** constructor */
	add_logger(){ std::cout << "construct object : " << this << std::endl;}
	/** destructor */
	~add_logger(){ std::cout << "destruct object : " << this << std::endl;}
};

template<typename COND, typename BASE_CLASS = EmptyClass>
using cond_add_logger = cond_enrich_class<COND, add_logger<boost::mpl::_1>, BASE_CLASS>;










#if 0

/** an utility class to conditionnally add a virtual destructor to any base class */
template<typename COND, typename BASE_CLASS = EmptyClass>
using cond_add_vdestroy = boost::mpl::eval_if <
  COND,
	add_vdestroy<BASE_CLASS>,
  BASE_CLASS>;

#endif

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

/** the list of the operations */
using class_tag_operations = boost::mpl::vector<
	boost::mpl::pair<has_vdestroy_tag<boost::mpl::_1>, cond_add_vdestroy<boost::mpl::_1, boost::mpl::_2> >,
	boost::mpl::pair<has_nocopy_tag<boost::mpl::_1>, cond_remove_copy<boost::mpl::_1, boost::mpl::_2> >
>;

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
