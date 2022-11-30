namespace chaos
{
#ifdef CHAOS_FORWARD_DECLARATION

	class Padding;

#elif !defined CHAOS_TEMPLATE_IMPLEMENTATION

	class CHAOS_API Padding
	{
	public:

		/** default constructor */
		Padding() = default;
		/** copy constructor */
		Padding(Padding const & src) = default;
		/** initialization constructor */
		Padding(int padding):
			left(padding),
			right(padding),
			top(padding),
			bottom(padding)
		{}
		/** initialization constructor */
		Padding(int l, int r, int t, int b):
			left(l),
			right(r),
			top(t),
			bottom(b)
		{}

	public:

		/** the left padding */
		int left = 0;
		/** the right padding */
		int right = 0;
		/** the top padding */
		int top = 0;
		/** the bottom padding */
		int bottom = 0;
	};

#endif

}; // namespace chaos