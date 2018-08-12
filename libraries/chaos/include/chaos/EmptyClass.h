#pragma once

#include <chaos/StandardHeaders.h>


#include <boost/mpl/apply_wrap.hpp>

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

	/** an utility class that derives from BASE CLASS or from FUNC<BASE_CLASS>::type */
	template<typename COND, typename FUNC, typename BASE_CLASS>
	using cond_enrich_class = boost::mpl::eval_if <
		typename COND::type,
		typename boost::mpl::apply<FUNC, BASE_CLASS>::type,
		BASE_CLASS>;

	/** utility class to add a virtual destructor to any base class */
	template<typename BASE_CLASS = EmptyClass>
	class add_vdestroy : public BASE_CLASS
	{
	public:

		/** simply a virtual destructor */
		virtual ~add_vdestroy() = default;
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

	/** remove_copy : an utility class to suppress copy construction/operator */
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

	template<typename COND, typename BASE_CLASS = EmptyClass>
	using cond_remove_copy = cond_enrich_class<COND, remove_copy<boost::mpl::_1>, BASE_CLASS>;

	/** in chain conditionnal inheritance */
	namespace details
	{
		/** the list of the operations */
		using class_tag_operations = boost::mpl::vector<
			cond_add_vdestroy<has_vdestroy_tag<boost::mpl::_1>, boost::mpl::_2>,
			cond_remove_copy<has_nocopy_tag<boost::mpl::_1>, boost::mpl::_2>,
			cond_add_logger<has_logger_tag<boost::mpl::_1>, boost::mpl::_2>
		>;

		template<typename TAGS, typename BEGIN, typename END, typename BASE>
		class BaseClassIteratorImpl : public BaseClassIteratorImpl <
			TAGS,
			typename boost::mpl::next<BEGIN>::type,
			END,
			typename boost::mpl::apply<
			boost::mpl::identity<boost::mpl::_2>,
			//typename boost::mpl::deref<BEGIN>::type,
			TAGS,
			BASE
			>::type
		> {};

		template<typename TAGS, typename IT, typename BASE>
		class BaseClassIteratorImpl<TAGS, IT, IT, BASE> : public BASE {};

		template<typename TAGS = boost::mpl::vector<>>
		using BaseClassImpl = details::BaseClassIteratorImpl<
			TAGS,
			boost::mpl::begin<details::class_tag_operations>::type,
			boost::mpl::end<details::class_tag_operations>::type,
			add_logger<add_logger<>>
			//EmptyClass
		>;
	};

	template<typename ...TAGS>
	using BaseClass = details::BaseClassImpl<boost::mpl::vector<TAGS...>>;


}; // namespace chaos
