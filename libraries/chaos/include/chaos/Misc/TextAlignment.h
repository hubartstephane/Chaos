namespace chaos
{
#ifdef CHAOS_FORWARD_DECLARATION

	enum class TextAlignment;

#elif !defined CHAOS_TEMPLATE_IMPLEMENTATION

	enum class TextAlignment : int
	{
		Left = 0,
		Right = 1,
		Center = 2,
		Justify = 3
	};

	CHAOS_DECLARE_ENUM_METHOD(TextAlignment, CHAOS_API);

#endif

}; // namespace chaos