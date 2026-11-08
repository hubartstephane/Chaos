#ifdef CHAOS_FORWARD_DECLARATION

namespace chaos
{
	class ShareableInterface;

}; // namespace chaos

#elif !defined CHAOS_TEMPLATE_IMPLEMENTATION

namespace chaos
{

	/**
	* ShareableInterface reference count external methods
	*/

	/** utility method for shared_ptr */
	CHAOS_API void IntrusiveAddSharedReference(ShareableInterface* obj);
	/** utility method for shared_ptr */
	CHAOS_API void IntrusiveSubSharedReference(ShareableInterface* obj);

	/**
	* ShareableInterface is a base class that have a reference count (shared and weak)
	*/

	// XXX : due to memory allocation management
	//       it is important that ShareableInterface is the very first object in hierarchy chain
	//
	//       the destructor operator calls   free(p) : p must point to the beginning of the allocated buffer
	//
	//
	//   case 1 : BAD !!!
	//
	//
	//                  +--- this (from the point of view of ShareableInterface)
	//                  v
	//   +------------+-----------------------+
	//   |            | ShareableInterface    |
	//   +------------+-----------------------+
	//   ^
	//   +-- allocated buffer for the whole class
	//
	//   operator delete(this) <==> free(this)   ===> we call free with a BAD pointer
	//
	//
	//   case 2 : GOOD !!!
	//
	//   +--- this (from the point of view of ShareableInterface)
	//   v
	//   +---------------------+------------+
	//   | ShareableInterface  |            |
	//   +---------------------+------------+
	//   ^
	//   +-- allocated buffer for the whole class
	//
	//   operator delete(this) <==> free(this)   ===> we call free with a GOOD pointer

	class CHAOS_API ShareableInterface
	{
		friend class SharedPointerPolicy;
		friend class WeakPointerPolicy;
		friend void IntrusiveAddSharedReference(ShareableInterface* obj);
		friend void IntrusiveSubSharedReference(ShareableInterface* obj);

	public:

		/** constructor */
		ShareableInterface();
		/** destructor */
		virtual ~ShareableInterface();

	protected:

		/** adding a shared reference */
		virtual void AddReference();
		/** removing a shared reference */
		virtual void SubReference();
		/** called whenever there are no more reference on the interface */
		virtual void OnLastReferenceLost();

	protected:

		/** count shared reference */
		std::atomic<int> shared_ref_count;
		/** a reference to the weak structure */
		mutable WeakPointerData* weak_ptr_data = nullptr;
	};

}; // namespace chaos

#endif