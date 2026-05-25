namespace chaos
{
#ifdef CHAOS_FORWARD_DECLARATION

	/** check whether one type is base strict for the other (remove extra qualifiers) */
	template<typename BASE, typename DERIVED>
	using IsBaseStrictOf = std::bool_constant<
		std::is_base_of_v<std::remove_cvref_t<BASE>, std::remove_cvref_t<DERIVED>> &&
		!std::is_same_v<std::remove_cvref_t<BASE>, std::remove_cvref_t<DERIVED>>
	>;
	template<typename BASE, typename DERIVED>
	inline constexpr bool IsBaseStrictOf_v = IsBaseStrictOf<BASE, DERIVED>::value;

	/** check whether one type inherit from another type ina a list (remove extra qualifiers) */
	template<typename T, typename... PARAMS>
	inline constexpr bool InheritStrictFromAny_v = (IsBaseStrictOf_v<PARAMS, T> || ...);

	/** generate a concept to check whether a type inherits from an element a list */
	#define CHAOS_GENERATE_INHERIT_STRICT_FROM_ANY_CONCEPT(concept_name, ...)\
	template<typename T>\
	concept concept_name = chaos::InheritStrictFromAny_v<T __VA_OPT__(,) __VA_ARGS__>;

#endif

}; // namespace chaos

