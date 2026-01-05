namespace chaos
{
#ifdef CHAOS_FORWARD_DECLARATION

	/** check whether 2 types are identical (remove extra qualifiers) */
	template<typename T1, typename T2>
	using IsSameExt = std::is_same<std::remove_cvref_t<T1>, std::remove_cvref_t<T2>>;
	template<typename T1, typename T2>
	inline constexpr bool IsSameExt_v = IsSameExt<T1, T2>::value;

	/** check whether one type is in a list (remove extra qualifiers) */
	template<typename T, typename... PARAMS>
	inline constexpr bool IsAnyOf_v = (IsSameExt_v<PARAMS, T> || ...);

	/** generate a concept to check whether a type is in a list */
	#define CHAOS_GENERATE_IS_ANY_OF_CONCEPT(concept_name, ...)\
	template<typename T>\
	concept concept_name = chaos::IsAnyOf_v<T __VA_OPT__(,) __VA_ARGS__>;

#endif

}; // namespace chaos

