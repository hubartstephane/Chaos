namespace chaos
{
#ifdef CHAOS_FORWARD_DECLARATION

	template<typename T, typename... PARAMS>
	inline constexpr bool IsAnyOf_v = (std::is_same_v<std::remove_cvref_t<PARAMS>, std::remove_cvref_t<T>> || ...);

	template<typename T, typename... PARAMS>
	inline constexpr bool InheritFromAny_v = (std::is_base_of_v<std::remove_cvref_t<PARAMS>, std::remove_cvref_t<T>> || ...);

	#define CHAOS_GENERATE_IS_ANY_OF_CONCEPT(concept_name, ...)\
	template<typename T>\
	concept concept_name = chaos::IsAnyOf_v<T __VA_OPT__(,) __VA_ARGS__>;

	#define CHAOS_GENERATE_INHERIT_FROM_ANY_CONCEPT(concept_name, ...)\
	template<typename T>\
	concept concept_name = chaos::InheritFromAny_v<T __VA_OPT__(,) __VA_ARGS__>;

#endif

}; // namespace chaos

