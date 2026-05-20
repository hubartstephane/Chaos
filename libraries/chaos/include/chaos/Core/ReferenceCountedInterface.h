#ifdef CHAOS_FORWARD_DECLARATION

namespace chaos
{
	class ReferenceCountedInterface;

	template<typename T>
	class DisableReferenceCount;

}; // namespace chaos

#elif !defined CHAOS_TEMPLATE_IMPLEMENTATION


/**
* ReferenceCountedInterface reference count external methods
*
* XXX : theses functions are out of chaos scope, else while shared_ptr is in chaos, it searches for intrusive_ptr_add function in prioriy
*       and if it was finding Object reference functions inside chaos scope, it will fail with IrrklangTools functions
*/

/** utility method for shared_ptr */
extern CHAOS_API void intrusive_ptr_add_ref(chaos::ReferenceCountedInterface* obj); // should work with boost::intrusive_ptr<>

/** utility method for shared_ptr */
extern CHAOS_API void intrusive_ptr_release(chaos::ReferenceCountedInterface* obj); // should work with boost::intrusive_ptr<>

namespace chaos
{
	/**
	* ReferenceCountedInterface is a base class that have a reference count (shared and weak)
	*/

	// XXX : due to memory allocation management
	//       it is important that ReferenceCountedInterface is the very first object in hierarchy chain
	//
	//       the destructor operator calls   free(p) : p must point to the beginning of the allocated buffer
	//
	//
	//   case 1 : BAD !!!
	//
	//
	//                  +--- this (from the point of view of ReferenceCountedInterface)
	//                  v
	//   +------------+------------------------------+
	//   |            | ReferenceCountedInterface    |
	//   +------------+------------------------------+
	//   ^
	//   +-- allocated buffer for the whole class
	//
	//   operator delete(this) <==> free(this)   ===> we call free with a BAD pointer
	//
	//
	//   case 2 : GOOD !!!
	//
	//   +--- this (from the point of view of ReferenceCountedInterface)
	//   v
	//   +----------------------------+------------+
	//   | ReferenceCountedInterface  |            |
	//   +----------------------------+------------+
	//   ^
	//   +-- allocated buffer for the whole class
	//
	//   operator delete(this) <==> free(this)   ===> we call free with a GOOD pointer

	class CHAOS_API ReferenceCountedInterface
	{
		friend class SharedPointerPolicy;
		friend class WeakPointerPolicy;
		friend void ::intrusive_ptr_add_ref(ReferenceCountedInterface* obj);
		friend void ::intrusive_ptr_release(ReferenceCountedInterface* obj);

	public:

		/** constructor */
		ReferenceCountedInterface();
		/** destructor */
		virtual ~ReferenceCountedInterface();

	protected:

		/** adding a shared reference */
		virtual void AddReference();
		/** removing a shared reference */
		virtual void SubReference();
		/** called whenever there are no more reference on the interface */
		virtual void OnLastReferenceLost();

	protected:

		/** count shared reference */
		std::atomic<int> shared_count;
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

}; // namespace chaos

#endif