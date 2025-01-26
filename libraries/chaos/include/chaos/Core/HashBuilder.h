namespace chaos
{
#ifdef CHAOS_FORWARD_DECLARATION

	class HashBuilder;

#elif !defined CHAOS_TEMPLATE_IMPLEMENTATION
	
	/**
	 * HashBuilder: utility class to build a hash from other ID
	 */

	class HashBuilder
	{
	public:

		// https://en.wikipedia.org/wiki/Xorshift
		// https://stackoverflow.com/questions/4948780/magic-number-in-boosthash-combine
		//
		// the value 0x9e3779b97f4a7c15 is some kind of magic value to combine different values
		// there are several algorithm for that, but this value seems good

		/** insert a new value to the build */
		uint64_t Add(uint64_t src)
		{
			result = hasher(result) ^ (hasher(src) + 0x9e3779b97f4a7c15 + (result << 6) + (result >> 2)); // combine current result with incomming data
			return result;
		}

		/** get current result */
		uint64_t GetResult() const
		{
			return result;
		}

	protected:

		/** get the hasher object */
		std::hash<uint64_t> hasher;
		/** current result */
		uint64_t result = 0;
	};

#endif

}; // namespace chaos