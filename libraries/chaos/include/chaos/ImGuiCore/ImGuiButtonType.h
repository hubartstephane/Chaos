namespace chaos
{
#ifdef CHAOS_FORWARD_DECLARATION

	enum class ImGuiButtonType;

#elif !defined CHAOS_TEMPLATE_IMPLEMENTATION

	/** a bitfield to hold all known button types */
	enum class ImGuiButtonType
	{
		None               = 0,
		Yes                = (1 << 0),
		No                 = (1 << 1),
		YesNo              = Yes | No,
		Cancel             = (1 << 2),
		Validate           = (1 << 3),
		Apply              = (1 << 4),
		Close              = (1 << 5),
		ValidateClose      = Validate | Close,
		ValidateApplyClose = Validate | Apply | Close,
		ValidationButtons  = Yes | Validate | Apply
	};

	CHAOS_DECLARE_ENUM_BITMASK_METHOD(ImGuiButtonType, CHAOS_API);

#endif

}; // namespace chaos