#ifdef CHAOS_FORWARD_DECLARATION

namespace chaos
{
	class Interface;

}; // namespace chaos

#elif !defined CHAOS_TEMPLATE_IMPLEMENTATION

namespace chaos
{
	/**
	* Interface reference count external methods
	*/

	/** utility method for shared_ptr */
	CHAOS_API void IntrusiveAddSharedReference(Interface* obj);
	/** utility method for shared_ptr */
	CHAOS_API void IntrusiveSubSharedReference(Interface* obj);

	/**
	* Interface: a base class for all interfaces
	*/

	class CHAOS_API Interface
	{
		template<typename T> friend class SharedPointerData;
		template<typename T> friend class WeakPointerData;
		friend void IntrusiveAddSharedReference(Interface* obj);
		friend void IntrusiveSubSharedReference(Interface* obj);

	public:

		/** constructor */
		Interface();
		/** copy constructor */
		Interface(Interface const &);
		/** move constructor */
		Interface(Interface &&);

		/** destructor */
		virtual ~Interface();

		/** copy operator */
		Interface& operator = (Interface const&);
		/** move operator */
		Interface& operator = (Interface&&);

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
		mutable WeakPointerControlBlock* weak_ptr_control_block = nullptr;
	};

}; // namespace chaos

#endif