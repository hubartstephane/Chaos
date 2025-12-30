#ifdef CHAOS_FORWARD_DECLARATION

// Class mapping is a meta programming feature that is used to map on class into another one
//
//   MyMapping_t<A> ---> C
//   MyMapping_t<B> ---> D
// 
// You first need to declare the mapping itself
//
//     CHAOS_PP_GENERATE_DECLARE_CLASS_MAPPING(MyMapping)
// 
//        or 
// 
//     CHAOS_PP_GENERATE_DECLARE_CLASS_MAPPING(MyMapping, default_class)
//
// Then the specializations
//
//     CHAOS_PP_GENERATE_DECLARE_CLASS_MAPPING(MyMapping, A, C)
//     CHAOS_PP_GENERATE_DECLARE_CLASS_MAPPING(MyMapping, B, D)
//
// You can then access the type with the following code
//
//     using type = MyMapping_t<A>;

#define CHAOS_PP_GENERATE_DECLARE_CLASS_MAPPING(mapping_name, ...)\
template<typename T>\
struct mapping_name BOOST_PP_IIF(\
	BOOST_PP_IS_EMPTY(__VA_ARGS__),\
	CHAOS_PP_GENERATE_DECLARE_CLASS_MAPPING_NODEFAULT,\
	CHAOS_PP_GENERATE_DECLARE_CLASS_MAPPING_WITHDEFAULT\
)(__VA_ARGS__);\
template<typename T>\
using mapping_name##_t = mapping_name<T>::type;

#define CHAOS_PP_GENERATE_DECLARE_CLASS_MAPPING_NODEFAULT(...)
#define CHAOS_PP_GENERATE_DECLARE_CLASS_MAPPING_WITHDEFAULT(default_value, ...) : public boost::mpl::identity<default_value> {}

#define CHAOS_PP_GENERATE_SPECIALIZE_CLASS_MAPPING(mapping_name, target_class, dst_class)\
template<> struct mapping_name<target_class> : public boost::mpl::identity<dst_class>{};

#endif