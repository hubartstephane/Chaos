#ifdef CHAOS_FORWARD_DECLARATION

/**
 * Macros for declaring classes in object derivated classes
 */

#define CHAOS_DECLARE_OBJECT_CLASS1(classname)\
private:\
static inline chaos::Class const * classname##_class = chaos::Class::DeclareClass<classname>(#classname);\
public:\
static chaos::Class const * GetStaticClass(){ return classname##_class;}\
virtual chaos::Class const * GetClass() const { return classname##_class; }

#define CHAOS_DECLARE_OBJECT_CLASS2(classname, parent_classname)\
private:\
static inline chaos::Class const * classname##_class = chaos::Class::DeclareClass<classname, parent_classname>(#classname);\
public:\
static chaos::Class const * GetStaticClass(){ return classname##_class;}\
virtual chaos::Class const * GetClass() const { return classname##_class; }

namespace chaos
{
	class Object;

	template<typename T>
	class DisableReferenceCount;

	template<typename T>
	class ReferencedObjectDataWrapper;

}; // namespace chaos

#else 

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

	class Object
	{
		friend class SharedPointerPolicy;
		friend class WeakPointerPolicy;

		CHAOS_DECLARE_OBJECT_CLASS1(Object);

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
	class DisableReferenceCount : public T
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
	class ReferencedObjectDataWrapper : public DisableReferenceCount<Object>
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
	auto MakeReferencedObjectWrapper(PARAMS... params)
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
void intrusive_ptr_add_ref(chaos::Object* obj); // should work with boost::intrusive_ptr<>

	/** utility method for shared_ptr */
void intrusive_ptr_release(chaos::Object* obj); // should work with boost::intrusive_ptr<>

#endif // CHAOS_FORWARD_DECLARATION