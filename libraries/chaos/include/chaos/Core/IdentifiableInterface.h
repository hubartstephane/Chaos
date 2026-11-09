namespace chaos
{
#ifdef CHAOS_FORWARD_DECLARATION

	class IdentifiableInterface;

#elif !defined CHAOS_TEMPLATE_IMPLEMENTATION
	
	/**
	 * IdentifiableInterface: this is a class that has a unique id
	 */

	class CHAOS_API IdentifiableInterface : public virtual Interface
	{
	public:

		/** constructor */
		IdentifiableInterface();

		/** gets the unique ID */
		uint64_t GetUniqueID() const { return unique_id;}

	protected:

		/** gets the unique ID */
		uint64_t unique_id = 0;
	};

#endif

}; // namespace chaos