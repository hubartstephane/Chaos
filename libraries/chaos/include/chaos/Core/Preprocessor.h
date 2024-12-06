#ifdef CHAOS_FORWARD_DECLARATION

// concate 4 values altogether
#define CHAOS_PP_CONCAT_4(a, b, c, d) CHAOS_PP_CONCAT_4_IMPL(a, b, c, d)
#define CHAOS_PP_CONCAT_4_IMPL(a, b, c, d) BOOST_PP_CAT(BOOST_PP_CAT(a, b), BOOST_PP_CAT(c, d))

#endif