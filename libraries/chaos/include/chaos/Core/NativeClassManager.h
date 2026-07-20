namespace chaos
{
#ifdef CHAOS_FORWARD_DECLARATION

	class NativeClassManager;

#elif !defined CHAOS_TEMPLATE_IMPLEMENTATION

	/**
	 * NativeClassManager: a manager dedicated to C++ native classes (should be fulfilled at startup)
	 */

	class CHAOS_API NativeClassManager : public ClassManager, public Singleton<NativeClassManager>
	{
	public:

		/** register some native class */
		template<typename CPP_TYPE>
		ClassRegistrationResult<CPP_TYPE> RegisterNativeClass(char const* name)
		{
			Class<CPP_TYPE>* result = Class<CPP_TYPE>::GetMutableNativeClassInstance();
			if (result->IsFullyInitialized()) // class has already be registered ?
			{
				assert(StringTools::Stricmp(result->name, name) == 0); // ensure class is registered with same name
				return { result };
			}

			result->name = name;
			result->class_size = sizeof(CPP_TYPE);
			result->info = &typeid(CPP_TYPE);
			result->manager = this;
			if constexpr (HasSuperType<CPP_TYPE>)
				result->parent_class = Class<SuperClass_t<CPP_TYPE>>::GetNativeClassInstance();

			classes.push_back(result);
			return { result };
		}
	};

#endif

}; // namespace chaos