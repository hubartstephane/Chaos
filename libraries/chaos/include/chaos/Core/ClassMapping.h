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
//     CHAOS_DECLARE_CLASS_MAPPING(MyMapping, XXX)
// 
//        or 
// 
//     CHAOS_DECLARE_CLASS_MAPPING(MyMapping, XXX, default_class)
// 
// where XXX, is what you want to map with (it can be 'typename', 'MyEnum', 'int' ...)
//
// Then the specializations
//
//     CHAOS_SPECIALIZE_CLASS_MAPPING(MyMapping, MyClassA, C)
//     CHAOS_SPECIALIZE_CLASS_MAPPING(MyMapping, MyClassB, D)
// 
//     CHAOS_SPECIALIZE_CLASS_MAPPING(MyMapping, MyEnum::A, C)
//     CHAOS_SPECIALIZE_CLASS_MAPPING(MyMapping, MyEnum::B, D)
// 
//     CHAOS_SPECIALIZE_CLASS_MAPPING(MyMapping, 7, C)
//     CHAOS_SPECIALIZE_CLASS_MAPPING(MyMapping, 8, D)
//
// You can then access the type with the following code
//
//     using type = MyMapping_t<A>;
// 
// During specialization, you re not forced to use the same type as return value (float/int here)

#define CHAOS_DECLARE_CLASS_MAPPING(mapping_name, template_param_type, ...)\
template<template_param_type T>\
struct mapping_name BOOST_PP_IIF(\
	BOOST_PP_IS_EMPTY(__VA_ARGS__),\
	CHAOS_DECLARE_CLASS_MAPPING_NODEFAULT,\
	CHAOS_DECLARE_CLASS_MAPPING_WITHDEFAULT\
)(__VA_ARGS__);\
template<template_param_type T>\
using mapping_name##_t = mapping_name<T>::type;

#define CHAOS_DECLARE_CLASS_MAPPING_NODEFAULT(...)
#define CHAOS_DECLARE_CLASS_MAPPING_WITHDEFAULT(default_value, ...) : public boost::mpl::identity<default_value> {}

#define CHAOS_SPECIALIZE_CLASS_MAPPING(mapping_name, template_param_specialization, dst_class)\
template<> struct mapping_name<template_param_specialization> : public boost::mpl::identity<dst_class>{};

#endif