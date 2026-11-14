#ifdef CHAOS_FORWARD_DECLARATION

namespace chaos
{
	class SupervisableInterface;

}; // namespace chaos

#elif !defined CHAOS_TEMPLATE_IMPLEMENTATION

namespace chaos
{

	/**
	 * SupervisableInterface reference count external methods
	 */

	/** utility method for supervised_ptr */
	CHAOS_API void IntrusiveAddSupervisedReference(SupervisableInterface* obj);
	/** utility method for supervised_ptr */
	CHAOS_API void IntrusiveSubSupervisedReference(SupervisableInterface* obj);

	/**
	 * SupervisableInterface: a class for objects that can be extract from their owner
	 */

	class CHAOS_API SupervisableInterface : public virtual Interface
	{
		friend void IntrusiveAddSupervisedReference(SupervisableInterface* obj);
		friend void IntrusiveSubSupervisedReference(SupervisableInterface* obj);

	public:

		/** constructor */
		SupervisableInterface();
		/** copy constructor */
		SupervisableInterface(SupervisableInterface const&);
		/** move constructor */
		SupervisableInterface(SupervisableInterface&&);

		/** destructor */
		virtual ~SupervisableInterface();

		/** copy operator */
		SupervisableInterface& operator = (SupervisableInterface const&);
		/** move operator */
		SupervisableInterface& operator = (SupervisableInterface&&);

	protected:

		/** override */
		virtual void SubReference() override;

		/** add a supervised reference */
		virtual void AddSuperviserReference();
		/** remove a managed reference */
		virtual void SubSuperviserReference();
		/** when last SHARED reference is lost */
		virtual void RemoveFromSuperviser() = 0;

	protected:

		/** count superviser reference */
		std::atomic<int> supervised_ref_count;
	};

}; // namespace chaos

#endif