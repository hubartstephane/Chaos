#ifdef CHAOS_FORWARD_DECLARATION

// Class mapping is a meta programming feature that is used to map anything into another one
//
//   MyMapping_t<MyClassA> ---> C
//   MyMapping_t<MyClassB> ---> D
// 
//   MyMapping_t<MyEnum::A> ---> C
//   MyMapping_t<MyEnum::B> ---> D
// 
//   MyMapping_t<7> ---> C
//   MyMapping_t<8> ---> D
// 
// You first need to declare the mapping itself
//
//     CHAOS_GENERATE_CLASS_MAPPING_DECLARATION(MyMapping, XXX)
// 
//        or 
// 
//     CHAOS_GENERATE_CLASS_MAPPING_DECLARATION(MyMapping, XXX, default_class)
// 
// where XXX, is what you want to map with (it can be 'typename', 'MyEnum', 'int' ...)
//
// Then the specializations
//
//     CHAOS_GENERATE_CLASS_MAPPING_SPECIALIZATION(MyMapping, MyClassA, C)
//     CHAOS_GENERATE_CLASS_MAPPING_SPECIALIZATION(MyMapping, MyClassB, D)
// 
//     CHAOS_GENERATE_CLASS_MAPPING_SPECIALIZATION(MyMapping, MyEnum::A, C)
//     CHAOS_GENERATE_CLASS_MAPPING_SPECIALIZATION(MyMapping, MyEnum::B, D)
// 
//     CHAOS_GENERATE_CLASS_MAPPING_SPECIALIZATION(MyMapping, 7, C)
//     CHAOS_GENERATE_CLASS_MAPPING_SPECIALIZATION(MyMapping, 8, D)
//
// You can then access the type with the following code
//
//     using type = MyMapping_t<A>;
// 
// During specialization, you re not forced to use the same type as return value (float/int here)

#define CHAOS_GENERATE_CLASS_MAPPING_DECLARATION(mapping_name, template_param_type, ...)\
template<template_param_type T>\
struct mapping_name BOOST_PP_IIF(\
	BOOST_PP_IS_EMPTY(__VA_ARGS__),\
	CHAOS_GENERATE_CLASS_MAPPING_DECLARATION_NODEFAULT,\
	CHAOS_GENERATE_CLASS_MAPPING_DECLARATION_WITHDEFAULT\
)(__VA_ARGS__);\
template<template_param_type T>\
using mapping_name##_t = mapping_name<T>::type;

#define CHAOS_GENERATE_CLASS_MAPPING_DECLARATION_NODEFAULT(...)
#define CHAOS_GENERATE_CLASS_MAPPING_DECLARATION_WITHDEFAULT(default_value, ...) : public boost::mpl::identity<default_value> {}

#define CHAOS_GENERATE_CLASS_MAPPING_SPECIALIZATION(mapping_name, template_param_specialization, dst_class)\
template<> struct mapping_name<template_param_specialization> : public boost::mpl::identity<dst_class>{};

#endif