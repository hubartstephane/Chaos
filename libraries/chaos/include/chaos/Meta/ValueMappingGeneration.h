#ifdef CHAOS_FORWARD_DECLARATION

// Class mapping is a meta programming feature that is used to map anything into a value
//
//   MyMapping_v<MyClassA> ---> float 666.0f
//   MyMapping_v<MyClassB> ---> int 666
// 
//   MyMapping_v<MyEnum::A> ---> float 666.0f
//   MyMapping_v<MyEnum::B> ---> int 666 
// 
//   MyMapping_v<7> ---> float 666.0f
//   MyMapping_v<8> ---> int 666 
// 
// You first need to declare the mapping itself
//
//     CHAOS_GENERATE_VALUE_MAPPING_DECLARATION(MyMapping, XXX)
// 
//        or 
// 
//     CHAOS_GENERATE_VALUE_MAPPING_DECLARATION(MyMapping, XXX, default_value_type, default_value)
// 
// where XXX, is what you want to map with (it can be 'typename', 'MyEnum', 'int' ...)
//
// Then the specializations
//
//     CHAOS_GENERATE_VALUE_MAPPING_SPECIALIZATION(MyMapping, MyClassA, float, 666.0f)
//     CHAOS_GENERATE_VALUE_MAPPING_SPECIALIZATION(MyMapping, MyClassB, int, 666)
// 
//     CHAOS_GENERATE_VALUE_MAPPING_SPECIALIZATION(MyMapping, MyEnum::A, float, 666.0f)
//     CHAOS_GENERATE_VALUE_MAPPING_SPECIALIZATION(MyMapping, MyEnum::B, int, 666)
// 
//     CHAOS_GENERATE_VALUE_MAPPING_SPECIALIZATION(MyMapping, 7, float, 666.0f)
//     CHAOS_GENERATE_VALUE_MAPPING_SPECIALIZATION(MyMapping, 8, int, 666)
//
// You can then access the value with the following code
//
//     auto value = MyMapping_v<A>;
//
// During specialization, you re not forced to use the same type as return value (float/int here)

#define CHAOS_GENERATE_VALUE_MAPPING_DECLARATION(mapping_name, template_param_type, ...)\
template<template_param_type T>\
struct mapping_name BOOST_PP_IIF(\
	BOOST_PP_OR(\
		BOOST_PP_IS_EMPTY(__VA_ARGS__),\
		BOOST_PP_LESS(\
			BOOST_PP_VARIADIC_SIZE(__VA_ARGS__),\
			2\
		)\
	),\
	CHAOS_GENERATE_VALUE_MAPPING_DECLARATION_NODEFAULT, \
	CHAOS_GENERATE_VALUE_MAPPING_DECLARATION_WITHDEFAULT\
)(__VA_ARGS__); \
template<template_param_type T>\
static auto constexpr mapping_name##_v = mapping_name<T>::value;

#define CHAOS_GENERATE_VALUE_MAPPING_DECLARATION_NODEFAULT(...)
#define CHAOS_GENERATE_VALUE_MAPPING_DECLARATION_WITHDEFAULT(value_type, default_value, ...) { static constexpr value_type value = default_value; }

#define CHAOS_GENERATE_VALUE_MAPPING_SPECIALIZATION(mapping_name, template_param_specialization, value_type, target_value)\
template<> struct mapping_name<template_param_specialization> { static constexpr value_type value = target_value; };

#endif