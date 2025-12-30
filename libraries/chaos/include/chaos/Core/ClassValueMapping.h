#ifdef CHAOS_FORWARD_DECLARATION

// Class mapping is a meta programming feature that is used to map on class into a value
//
//   MyMapping_v<A> ---> float 666.0f
//   MyMapping_v<B> ---> int 666
// 
// You first need to declare the mapping itself
//
//     CHAOS_PP_GENERATE_DECLARE_CLASS_VALUE_MAPPING(MyMapping)
// 
//        or 
// 
//     CHAOS_PP_GENERATE_DECLARE_CLASS_VALUE_MAPPING(MyMapping, default_value_type, default_value)
//
// Then the specializations
//
//     CHAOS_PP_GENERATE_SPECIALIZE_CLASS_VALUE_MAPPING(MyMapping, A, float, 666.0f)
//     CHAOS_PP_GENERATE_SPECIALIZE_CLASS_VALUE_MAPPING(MyMapping, B, int, 666)
//
// You can then access the value with the following code
//
//     auto value = MyMapping_v<A>;

#define CHAOS_PP_GENERATE_DECLARE_CLASS_VALUE_MAPPING(mapping_name, ...)\
template<typename T>\
struct mapping_name BOOST_PP_IIF(\
	BOOST_PP_OR(\
		BOOST_PP_IS_EMPTY(__VA_ARGS__),\
		BOOST_PP_LESS(\
			BOOST_PP_VARIADIC_SIZE(__VA_ARGS__),\
			2\
		)\
	),\
	CHAOS_PP_GENERATE_DECLARE_CLASS_VALUE_MAPPING_NODEFAULT, \
	CHAOS_PP_GENERATE_DECLARE_CLASS_VALUE_MAPPING_WITHDEFAULT\
)(__VA_ARGS__); \
template<typename T>\
static auto constexpr mapping_name##_v = mapping_name<T>::value;

#define CHAOS_PP_GENERATE_DECLARE_CLASS_VALUE_MAPPING_NODEFAULT(...)
#define CHAOS_PP_GENERATE_DECLARE_CLASS_VALUE_MAPPING_WITHDEFAULT(value_type, default_value, ...) { static constexpr value_type value = default_value; }

#define CHAOS_PP_GENERATE_SPECIALIZE_CLASS_VALUE_MAPPING(mapping_name, target_class, value_type, target_value)\
template<> struct mapping_name<target_class> { static constexpr value_type value = target_value; };

#endif