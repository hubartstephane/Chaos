#ifdef CHAOS_FORWARD_DECLARATION

namespace chaos
{
	template<typename T>
	class DisableReferenceCount;

}; // namespace chaos

#elif !defined CHAOS_TEMPLATE_IMPLEMENTATION

namespace chaos
{
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