namespace chaos
{
#ifdef CHAOS_FORWARD_DECLARATION

	/** check whether one type is base for the other (remove extra qualifiers) */
	template<typename BASE, typename DERIVED>
	using IsBaseOfExt = std::is_base_of<std::remove_cvref_t<BASE>, std::remove_cvref_t<DERIVED>>;
	template<typename BASE, typename DERIVED>
	inline constexpr bool IsBaseOfExt_v = IsBaseOfExt<BASE, DERIVED>::value;

	/** check whether one type inherit from another type ina a list (remove extra qualifiers) */
	template<typename T, typename... PARAMS>
	inline constexpr bool InheritFromAny_v = (IsBaseOfExt_v<PARAMS, T> || ...);

	/** generate a concept to check whether a type inherits from an element a list */
	#define CHAOS_GENERATE_INHERIT_FROM_ANY_CONCEPT(concept_name, ...)\
	template<typename T>\
	concept concept_name = chaos::InheritFromAny_v<T __VA_OPT__(,) __VA_ARGS__>;

#endif

}; // namespace chaos

