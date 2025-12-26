#ifdef CHAOS_FORWARD_DECLARATION

// concate 4 values altogether
#define CHAOS_PP_CONCAT_4(a, b, c, d) CHAOS_PP_CONCAT_4_IMPL(a, b, c, d)
#define CHAOS_PP_CONCAT_4_IMPL(a, b, c, d) BOOST_PP_CAT(BOOST_PP_CAT(a, b), BOOST_PP_CAT(c, d))

// select an elem from a variadic number of arg if present or take the default value
#define CHAOS_PP_SELECT_ELEM_OR_DEFAULT(index, default_value, ...)\
	BOOST_PP_IIF(\
		BOOST_PP_GREATER_EQUAL(\
			index,\
			BOOST_PP_VARIADIC_SIZE(__VA_ARGS__)\
		),\
		CHAOS_PP_SELECT_ELEM_OR_DEFAULT_TAKE_DEFAULT,\
		CHAOS_PP_SELECT_ELEM_OR_DEFAULT_TAKE_ELEM\
	)(index, default_value __VA_OPT__(,) __VA_ARGS__)

#define CHAOS_PP_SELECT_ELEM_OR_DEFAULT_TAKE_DEFAULT(index, default_value, ...) default_value
#define CHAOS_PP_SELECT_ELEM_OR_DEFAULT_TAKE_ELEM(index, default_value, ...) BOOST_PP_VARIADIC_ELEM(index __VA_OPT__(,) __VA_ARGS__)

#endif