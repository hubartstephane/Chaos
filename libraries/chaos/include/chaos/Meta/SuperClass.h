namespace chaos
{
#ifdef CHAOS_FORWARD_DECLARATION

	// Two ways to express information concerning class inheritance
	//
	// Intrusive way
	// 
	//		class B : public A
	//		{
	//			using Super = A;
	//		};
	//
	// External way
	// 
	//		class B : public A { ... };		
	// 
	//		CHAOS_DECLARE_EXTERNAL_SUPER(B, A)		---> this declares some fake functions
	//

	// check whether a class has a 'Super' typename declared inside (and check whether this 'Super' is a parent class of T)
	template<typename T>
	concept HasInternalSuperType = requires
	{
		typename T::Super;

		requires IsBaseOf_v<typename T::Super, T>;
	};

	// check whether a class has a ChaosGetSuperClass function defined (and check whether this 'Super' is a parent class of T)
	template<typename T>
	concept HasExternalSuperType = requires
	{
		requires (std::is_class_v<
			decltype(ChaosGetSuperClass(std::type_identity_t<T>()))
		>);

		requires IsBaseOf_v<decltype(ChaosGetSuperClass(std::type_identity_t<T>())), T>;
	};

	// meta function to get super class 
	template<typename T>
	struct SuperClass;

	template<typename T>
	requires
	(
		HasInternalSuperType<T> && !HasExternalSuperType<T>
	)
	struct SuperClass<T>
	{
		using type = typename T::Super;
	};

	template<typename T>
	requires
	(
		!HasInternalSuperType<T>&& HasExternalSuperType<T>
	)
	struct SuperClass<T>
	{
		using type = decltype(ChaosGetSuperClass(std::type_identity_t<T>()));
	};

	template<typename T>
	requires
	(
		HasExternalSuperType<T> &&
		HasInternalSuperType<T> &&
		IsSame_v<
			typename T::Super,
			decltype(ChaosGetSuperClass(std::type_identity_t<T>()))
		>
	)
	struct SuperClass<T>
	{
		using type = typename T::Super;
	};

	template<typename T>
	requires
	(
		HasExternalSuperType<T> &&
		HasInternalSuperType<T> &&
		IsBaseStrictOf_v<
			decltype(ChaosGetSuperClass(std::type_identity_t<T>())),
			typename T::Super
		>
	)
	struct SuperClass<T>
	{
		using type = typename T::Super;
	};

	template<typename T>
	requires
	(
		HasExternalSuperType<T> &&
		HasInternalSuperType<T> &&
		IsBaseStrictOf_v<
			typename T::Super,
			decltype(ChaosGetSuperClass(std::type_identity_t<T>()))
		>
	)
	struct SuperClass<T>
	{
		using type = decltype(ChaosGetSuperClass(std::type_identity_t<T>()));
	};

	// class the meta function
	template<typename T>
	using SuperClass_t = typename SuperClass<T>::type;


	// defines an external inheritance
	#define CHAOS_DECLARE_EXTERNAL_SUPER(TARGET_CLASS, SUPER_CLASS)\
	SUPER_CLASS ChaosGetSuperClass(std::type_identity_t<TARGET_CLASS>);

#endif

}; // namespace chaos

