#ifdef CHAOS_FORWARD_DECLARATION

namespace chaos
{
	namespace StreamTools
	{
		class Whitespaces;

	}; // namespace StreamTools

}; // namespace chaos

#else 

namespace chaos
{
	/**
	* StreamTools : a namespace for stream related functions
	*/

	namespace StreamTools
	{
		/** a class that deserves to insert whitespaces into a output stream */
		class Whitespaces
		{
		public:

			/** constructor */
			Whitespaces(int in_value = 0) :
				value(in_value) {}
			/** friend function to add indents */
			friend std::ostream & operator << (std::ostream & stream, Whitespaces const & stream_whitespaces)
			{
				for (int i = 0; i < stream_whitespaces.value; ++i)
					stream << " ";
				return stream;
			}

		protected:

			/** number of indent to insert */
			int value = 0;
		};

	}; // namespace StreamTools

}; // namespace chaos

#endif // CHAOS_FORWARD_DECLARATION

