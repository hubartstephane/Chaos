namespace chaos
{
#ifdef CHAOS_FORWARD_DECLARATION

	enum class TextAlignment;

#elif !defined CHAOS_TEMPLATE_IMPLEMENTATION

	enum class CHAOS_API TextAlignment : int
	{
		LEFT = 0,
		RIGHT = 1,
		CENTER = 2,
		JUSTIFY = 3
	};

	CHAOS_DECLARE_ENUM_METHOD(TextAlignment);

#endif

}; // namespace chaos