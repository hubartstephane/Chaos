#ifdef CHAOS_FORWARD_DECLARATION

namespace chaos
{
	class SupervisableInterface;

}; // namespace chaos

#elif !defined CHAOS_TEMPLATE_IMPLEMENTATION

namespace chaos
{
	class CHAOS_API SupervisableInterface : public virtual Interface
	{
		friend class ManagedPointerPolicy;

	public:

		/** constructor */
		SupervisableInterface();
		/** destructor */
		virtual ~SupervisableInterface();

	protected:

		/** override */
		virtual void SubReference() override;

		/** add a supervised reference */
		virtual void AddSuperviserReference();
		/** remove a managed reference */
		virtual void SubSuperviserReference();
		/** when last SHARED reference is lost */
		virtual void OnRemoveFromSuperviser();

	protected:

		/** count superviser reference */
		std::atomic<int> supervised_ref_count;
	};

}; // namespace chaos

#endif