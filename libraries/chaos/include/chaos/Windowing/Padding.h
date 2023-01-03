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
		Padding(float padding):
			left(padding),
			right(padding),
			top(padding),
			bottom(padding)
		{}
		/** initialization constructor */
		Padding(float l, float r, float t, float b):
			left(l),
			right(r),
			top(t),
			bottom(b)
		{}

	public:

		/** the left padding */
		float left = 0.0f;
		/** the right padding */
		float right = 0.0f;
		/** the top padding */
		float top = 0.0f;
		/** the bottom padding */
		float bottom = 0.0f;
	};

#endif

}; // namespace chaos