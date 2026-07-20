namespace chaos
{

#ifdef CHAOS_FORWARD_DECLARATION

// Macros for declaring classes in object derivated classes
//
// It's important that last line is:    static inline .... DeclareNativeClass ... (no comma)
// so that we can use the short class name syntax
//    CHAOS_DECLARE_OBJECT_CLASS(child_class, parent_class).ShortName("short_name");
//
// That's why the class member is not private (because users will have to redefines the privacy)

#define CHAOS_DECLARE_OBJECT_CLASS_METHODS(CLASS)\
static Class<CLASS> const * GetStaticClass(){ return CLASS##_class;}\
virtual Class<CLASS> const * GetClass() const { return CLASS##_class; }\
static inline Class<CLASS> const * CLASS##_class = RegisterNativeClass<CLASS>(#CLASS)

#define CHAOS_DECLARE_OBJECT_CLASS(CLASS, PARENT_CLASS)\
public:\
using Super = PARENT_CLASS;\
CHAOS_DECLARE_OBJECT_CLASS_METHODS(CLASS)

	class Object;

#elif !defined CHAOS_TEMPLATE_IMPLEMENTATION

	/**
	* Object: base class for most objects
	*/

	class CHAOS_API Object : public ReferenceCountedInterface
	{
		friend class SharedPointerPolicy;
		friend class WeakPointerPolicy;

	protected:

		/** declare the class in the default C++ ClassManager (must be declared before the CHAOS_DECLARE_OBJECT_CLASS usage) */
		template<typename CLASS_TYPE>
		static ClassRegistrationResult<CLASS_TYPE> RegisterNativeClass(char const * name);

	public:

		CHAOS_DECLARE_OBJECT_CLASS_METHODS(Object);
	};

#else // defined CHAOS_TEMPLATE_IMPLEMENTATION

	template<typename CPP_TYPE>
	ClassRegistrationResult<CPP_TYPE> Object::RegisterNativeClass(char const* name)
	{
		return NativeClassManager::GetInstance()->RegisterNativeClass<CPP_TYPE>(name);
	}

#endif

}; // namespace chaos