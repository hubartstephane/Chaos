#ifdef CHAOS_FORWARD_DECLARATION

// BoolTag is a meta programming feature use to make correspondant between a class and a bool value
//
// for example:
//   MyTag<A> == true
//   MyTag<B> == false
// 
// You first need to declare the tag itself with a default value
//
//     CHAOS_DECLARE_BOOL_TAG(MyTag, false)
//
// then the specializations
//
//     CHAOS_IMPLEMENT_BOOL_TAG_SPECIALIZATION(MyTag, A, true)
//     CHAOS_IMPLEMENT_BOOL_TAG_SPECIALIZATION(MyTag, B, true)
//

/** declare the tag */
#define CHAOS_DECLARE_BOOL_TAG(tag_name, default_value)\
template<typename T>\
struct tag_name : public boost::mpl::bool_<default_value> {};\
template<typename T>\
constexpr bool tag_name##_v = tag_name<T>::value;

/** add specialization to the tag */
#define CHAOS_IMPLEMENT_BOOL_TAG_SPECIALIZATION(tag_name, T, value)\
template<>\
struct tag_name<T> : public boost::mpl::bool_<value> {};

#endif