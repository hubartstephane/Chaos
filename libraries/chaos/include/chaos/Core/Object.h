#ifdef CHAOS_FORWARD_DECLARATION

/**
 * Macros for declaring classes in object derivated classes
 */

// It's important that last line is:    static inline .... DeclareCPPClass ... (no comma)
// so that we can use the short class name syntax
//    CHAOS_DECLARE_OBJECT_CLASS(child_class, parent_class)("short_name");
//
// That's why the class member is not private (because users will have to redefines the privacy)
//

#define CHAOS_DECLARE_OBJECT_CLASS(CLASS, PARENT_CLASS)\
public:\
using Super = PARENT_CLASS;\
static chaos::SubClassOf<CLASS> GetStaticClass(){ return CLASS##_class;}\
virtual chaos::Class const * GetClass() const { return CLASS##_class; }\
static inline chaos::Class const * CLASS##_class = DeclareObjectClass<CLASS, PARENT_CLASS>(#CLASS)

namespace chaos
{
	class Object;

}; // namespace chaos

#elif !defined CHAOS_TEMPLATE_IMPLEMENTATION

namespace chaos
{
	/**
	* Object: base class for most objects
	*/

	class CHAOS_API Object : public ReferenceCountedInterface
	{
		friend class SharedPointerPolicy;
		friend class WeakPointerPolicy;

	protected:

		/** declare the class in the default C++ ClassManager (must be declared before the CHAOS_DECLARE_OBJECT_CLASS usage) */
		template<typename CLASS_TYPE, typename PARENT_CLASS_TYPE = EmptyClass>
		static ClassRegistration DeclareObjectClass(std::string name);

	public:

		static chaos::SubClassOf<Object> GetStaticClass() { return Object_class; }
		virtual chaos::Class const* GetClass() const { return Object_class; }
		static inline Class const* Object_class = DeclareObjectClass<Object>("Object");
	};

}; // namespace chaos

#else // defined CHAOS_TEMPLATE_IMPLEMENTATION

namespace chaos
{
	template<typename CLASS_TYPE, typename PARENT_CLASS_TYPE>
	ClassRegistration Object::DeclareObjectClass(std::string name)
	{
		return chaos::ClassManager::GetDefaultInstance()->DeclareCPPClass<CLASS_TYPE, PARENT_CLASS_TYPE>(std::move(name));
	}
}; // namespace chaos

#endif