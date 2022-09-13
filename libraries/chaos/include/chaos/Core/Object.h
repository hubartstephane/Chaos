#ifdef CHAOS_FORWARD_DECLARATION

/**
 * Macros for declaring classes in object derivated classes
 */

// It's important that last line is:    static inline .... DeclareClass ... (no comma)
// so that we can use the short class name syntax
//    CHAOS_DECLARE_OBJECT_CLASS(child_class, parent_class)("short_name");
//
// That's why the class member is not private (because users will have to redefines the privacy)
//
#define CHAOS_DECLARE_OBJECT_CLASS(CLASS, ...)\
public:\
static chaos::Class const * GetStaticClass(){ return CLASS##_class;}\
virtual chaos::Class const * GetClass() const { return CLASS##_class; }\
static inline chaos::Class const * CLASS##_class = DeclareObjectClass<CLASS, __VA_ARGS__>(#CLASS)

namespace chaos
{
	class Object;

	template<typename T>
	class DisableReferenceCount;

	template<typename T>
	class ReferencedObjectDataWrapper;

}; // namespace chaos

#elif !defined CHAOS_TEMPLATE_IMPLEMENTATION

namespace chaos
{
	/**
	* Object is a base class that have a reference count (shared and weak)
	*/

	// XXX : due to memory allocation management
	//       it is important that Object is the very first object in hierarchy chain
	//
	//       the destructor operator calls   free(p) : p must point to the beginning of the allocated buffer
	//
	//
	//   case 1 : BAD !!!
	//
	//
	//                  +--- this (from the point of view of Object)
	//                  v
	//   +------------+----------------------+
	//   |            | Object               |
	//   +------------+----------------------+
	//   ^
	//   +-- allocated buffer for the whole class
	//
	//   operator delete(this) <==> free(this)   ===> we call free with a BAD pointer
	//
	//
	//   case 2 : GOOD !!!
	//
	//   +--- this (from the point of view of Object)
	//   v
	//   +----------------------+------------+
	//   | Object               |            |
	//   +----------------------+------------+
	//   ^
	//   +-- allocated buffer for the whole class
	//
	//   operator delete(this) <==> free(this)   ===> we call free with a GOOD pointer

	class CHAOS_API Object
	{
		friend class SharedPointerPolicy;
		friend class WeakPointerPolicy;

	protected:

		/** declare the class in the default C++ ClassManager (must be declared before the CHAOS_DECLARE_OBJECT_CLASS usage) */
		template<typename CLASS_TYPE, typename PARENT_CLASS_TYPE = EmptyClass>
		static ClassRegistration DeclareObjectClass(std::string name)
		{
			return chaos::ClassManager::GetDefaultInstance()->DeclareClass<CLASS_TYPE, PARENT_CLASS_TYPE>(std::move(name));
		}

		CHAOS_DECLARE_OBJECT_CLASS(Object);

	public:

		/** constructor */
		Object();
		/** destructor */
		virtual ~Object();

	public:

		/** adding a shared reference */
		virtual void AddReference();
		/** removing a shared reference */
		virtual void SubReference();

	protected:

		/** called whenever there are no more reference on the object */
		virtual void OnLastReferenceLost();

	protected:

		/** count shared reference */
		boost::atomic<int> shared_count;
		/** a reference to the weak structure */
		mutable WeakPointerData* weak_ptr_data = nullptr;
	};

	/**
	* DisableReferenceCount : an utility class to help using referenced object on stack
	*/

	template<typename T>
	class /*CHAOS_API*/ DisableReferenceCount : public T
	{
	public:

		/** forwarding constructor */
		using T::T;

	protected:

		/** disable all reference count functions */
		virtual void AddReference() override { }
		virtual void SubReference() override { }
		virtual void OnLastReferenceLost() override { }
	};

	/**
	* ReferencedObjectDataWrapper : a data wrapped into a referenced object => while referenced object may be dynamic_casted we can test for the data inside
	*/

	template<typename T>
	class /*CHAOS_API*/ ReferencedObjectDataWrapper : public DisableReferenceCount<Object>
	{
		using type = T;

	public:

		/** copy constructor */
		ReferencedObjectDataWrapper(ReferencedObjectDataWrapper const & src) : data(src.data){}

		/** constructor */
		template<typename ...PARAMS>
		ReferencedObjectDataWrapper(PARAMS... params) : data(params...){}

	public:

		/** the wrapped data */
		T data;
	};

	template<typename T, typename ...PARAMS>
	/*CHAOS_API*/ auto MakeReferencedObjectWrapper(PARAMS... params)
	{
		return ReferencedObjectDataWrapper<T>(params...);
	}

}; // namespace chaos

	/**
	* Object : reference count external methods
	*
	* XXX : theses functions are out of chaos scope, else while shared_ptr is in chaos, it searches for intrusive_ptr_add function in prioriy
	*       and if it was finding Object reference functions inside chaos scope, it will fail with IrrklangTools functions
	*/

	/** utility method for shared_ptr */
CHAOS_API void intrusive_ptr_add_ref(chaos::Object* obj); // should work with boost::intrusive_ptr<>

	/** utility method for shared_ptr */
CHAOS_API void intrusive_ptr_release(chaos::Object* obj); // should work with boost::intrusive_ptr<>

#endif