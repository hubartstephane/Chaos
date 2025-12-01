 namespace chaos
{
#ifdef CHAOS_FORWARD_DECLARATION

	 template<typename T>
	 class Range;

#elif !defined CHAOS_TEMPLATE_IMPLEMENTATION

	/**
	 * Range: a class that hold a minimum and a maximum
	 */

	 template<typename T>
	 class Range
	 {
	 public:

		 /** default constructor */
		 Range() = default;
		 /** copy constructor */
		 Range(Range const & src) = default;
		 /** initialization constructor */
		 Range(T const& in_min, T const& in_max):
			 min(in_min),
			 max(in_max)
		 {}

	 public:

		 /** the minimum value of the range */
		 T min = T();
		 /** the maximum value of the range */
		 T max = T();
	 };

#endif

}; // namespace chaos