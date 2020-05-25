#pragma once

#include <chaos/StandardHeaders.h>
#include <chaos/SmartPointers.h>
#include <chaos/Class.h>

namespace chaos
{

	/**
	 * Macros for declaring classes in object derivated classes
	 */

#define CHAOS_OBJECT_DECLARE_CLASS1(classname)\
private:\
static inline chaos::Class const * classname##_class = chaos::ClassTools::DeclareClass<classname>(#classname);\
public:\
static chaos::Class const * GetStaticClass(){ return classname##_class;}\
virtual chaos::Class const * GetClass() { return classname##_class; }

#define CHAOS_OBJECT_DECLARE_CLASS2(classname, parent_classname)\
private:\
static inline chaos::Class const * classname##_class = chaos::ClassTools::DeclareClass<classname, parent_classname>(#classname);\
public:\
static chaos::Class const * GetStaticClass(){ return classname##_class;}\
virtual chaos::Class const * GetClass() { return classname##_class; }

	/**
	* Object is a base class that have a reference count (shared and weak)
	*/

	// XXX : due to memory allocation management (destruction is manually called with ->~destructor())
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

		CHAOS_OBJECT_DECLARE_CLASS1(Object);

	public:

		/** constructor */
		Object();
		/** destructor */
		virtual ~Object() = default;

	public:

		/** adding a shared reference */
		virtual void AddReference(SharedPointerPolicy policy);
		/** adding a weak reference */
		virtual void AddReference(WeakPointerPolicy policy);
		/** removing a shared reference */
		virtual void SubReference(SharedPointerPolicy policy);
		/** removing a weak reference */
		virtual void SubReference(WeakPointerPolicy policy);

	protected:

		/** called whenever there are no more reference on the object */
		virtual void OnLastReferenceLost();

	protected:

		/** count shared reference */
		boost::atomic<int> shared_count;
		/** count weak reference */
		boost::atomic<int> weak_count;
		/** whenever the content has been destroyed */
		bool shared_destroyed = false;
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
		virtual void AddReference(WeakPointerPolicy policy) override { }
		virtual void AddReference(SharedPointerPolicy policy) override { }
		virtual void SubReference(WeakPointerPolicy policy) override { }
		virtual void SubReference(SharedPointerPolicy policy) override { }		
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
	 * XXX : theses functions are out of chaos scope, else while shared_ptr is in chaos, it searches for chaos::intrusive_ptr_add function in prioriy
	 *       and if it was finding Object reference functions inside chaos scope, it will fail with IrrklangTools functions
	 */

	 /** utility method for shared_ptr */
template<typename POLICY = chaos::SharedPointerPolicy>
void intrusive_ptr_add_ref(chaos::Object * obj, POLICY policy = POLICY()) // to work with boost::intrusive_ptr<>
{
	obj->AddReference(policy);
}

/** utility method for shared_ptr */
template<typename POLICY = chaos::SharedPointerPolicy>
void intrusive_ptr_release(chaos::Object * obj, POLICY policy = POLICY()) // to work with boost::intrusive_ptr<>
{
	obj->SubReference(policy);
}

