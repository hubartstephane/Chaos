namespace chaos
{
	/** a namespace for stream related functions */
	namespace StreamTools
	{
#ifdef CHAOS_FORWARD_DECLARATION

		class Whitespaces;

#elif !defined CHAOS_TEMPLATE_IMPLEMENTATION

		/** a class that deserves to insert whitespaces into a output stream */
		class CHAOS_API Whitespaces
		{
		public:

			/** constructor */
			Whitespaces(size_t in_count = 0) :
				count(in_count) {}
			/** constructor */
			Whitespaces(Whitespaces const& src) = default;
			/** friend function to add characters */
			friend std::ostream& operator << (std::ostream& stream, Whitespaces const& whitespaces)
			{
				for (size_t i = 0; i < whitespaces.count; ++i)
					stream << " ";
				return stream;
			}

		protected:

			/** number of characters */
			size_t count = 0;
		};

#endif

	}; // namespace StreamTools

}; // namespace chaos