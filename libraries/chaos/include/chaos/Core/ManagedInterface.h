#ifdef CHAOS_FORWARD_DECLARATION

namespace chaos
{
	class ManagedInterface;

}; // namespace chaos

#elif !defined CHAOS_TEMPLATE_IMPLEMENTATION

namespace chaos
{
	class CHAOS_API ManagedInterface : public ReferenceCountedInterface
	{
		friend class ManagedPointerPolicy;

	public:

		/** constructor */
		ManagedInterface();
		/** destructor */
		virtual ~ManagedInterface();

	protected:

		/** override */
		virtual void SubReference() override;

		/** add a managed reference */
		virtual void AddOwnerReference();
		/** remove a managed reference */
		virtual void SubOwnerReference();
		/** when last SHARED reference is lost */
		virtual void OnRemoveFromManager();

	protected:

		/** count manager reference */
		std::atomic<int> manager_count;
	};

}; // namespace chaos

#endif