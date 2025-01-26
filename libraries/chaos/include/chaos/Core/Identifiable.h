namespace chaos
{
#ifdef CHAOS_FORWARD_DECLARATION

	class Identifiable;

#elif !defined CHAOS_TEMPLATE_IMPLEMENTATION
	
	/**
	 * Identifiable: this is a class that has a unique id
	 */

	class CHAOS_API Identifiable
	{
	public:

		/** constructor */
		Identifiable();

		/** gets the unique ID */
		uint64_t GetUniqueID() const { return unique_id;}

	protected:

		/** gets the unique ID */
		uint64_t unique_id = 0;
	};

#endif

}; // namespace chaos