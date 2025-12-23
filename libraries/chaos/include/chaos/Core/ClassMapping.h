#ifdef CHAOS_FORWARD_DECLARATION

// Class mapping is a meta programming feature that is used to map on class into another one
//
//   MyMapping<A> ---> C
//   MyMapping<B> ---> D
// 
// You first need to declare the mapping itself
//
//     CHAOS_GENERATE_DECLARE_CLASS_MAPPING(MyMapping)
//
// Then the specializations
//
//     CHAOS_GENERATE_SPECIALIZE_CLASS_MAPPING(MyMapping, A, C)
//     CHAOS_GENERATE_SPECIALIZE_CLASS_MAPPING(MyMapping, B, D)
//

template <template<typename> class T>
struct is_strict_mapping : public boost::mpl::bool_<false> {};
template <template<typename> class T>
constexpr bool is_strict_mapping_v = is_strict_mapping<T>::value;

#define CHAOS_GENERATE_DECLARE_CLASS_MAPPING(mapping_name, strict_mapping)\
template<typename T>\
struct mapping_name;\
template<typename T>\
using mapping_name##_t = mapping_name<T>::type;\
template<>\
struct is_strict_mapping<mapping_name> : public boost::mpl::bool_<strict_mapping> {};

#define CHAOS_GENERATE_SPECIALIZE_CLASS_MAPPING(mapping_name, from_class, to_class)\
template<> struct mapping_name<from_class> : public boost::mpl::identity<to_class>{};

#endif