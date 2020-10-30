#ifdef CHAOS_FORWARD_DECLARATION

namespace chaos
{
	namespace StreamTools
	{
		class Indent;

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

		/** a class that deserves to insert indents into a output stream */
		class Indent
		{
		public:

			/** constructor */
			Indent(int in_value = 0) :
				value(in_value) {}
			/** friend function to add indents */
			friend std::ostream & operator << (std::ostream & stream, Indent const & stream_indent)
			{
				for (int i = 0; i < stream_indent.value; ++i)
					stream << "  ";
				return stream;
			}

		protected:

			/** number of indent to insert */
			int value = 0;
		};

	}; // namespace StreamTools

}; // namespace chaos

#endif // CHAOS_FORWARD_DECLARATION

