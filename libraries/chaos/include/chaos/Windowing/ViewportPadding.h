namespace chaos
{
#ifdef CHAOS_FORWARD_DECLARATION

	class ViewportPadding;

#elif !defined CHAOS_TEMPLATE_IMPLEMENTATION

	class ViewportPadding
	{
	public:

		/** default constructor */
		ViewportPadding() = default;
		/** copy constructor */
		ViewportPadding(ViewportPadding const & src) = default;
		/** initialization constructor */
		ViewportPadding(int padding):
			left(padding),
			right(padding),
			top(padding),
			bottom(padding)
		{}
		/** initialization constructor */
		ViewportPadding(int l, int r, int t, int b):
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