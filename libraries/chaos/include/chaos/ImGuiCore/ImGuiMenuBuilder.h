namespace chaos
{

#ifdef CHAOS_FORWARD_DECLARATION

	class ImGuiMenuBuilder;
	class ImGuiStandardMenuBuilder;
	class ImGuiMainMenuBuilder;

#elif !defined CHAOS_TEMPLATE_IMPLEMENTATION

	/**
	 * ImGuiMenuBuilder: an utility class to help building menu
	 */

	class CHAOS_API ImGuiMenuBuilder
	{
	public:

		/** method to create a menu */
		virtual bool WithMenu(LightweightFunction<void()> func) const { return false; }
	};

	/**
	 * ImGuiStandardMenuBuilder: the normal menu builder for imgui window
	 */

	class CHAOS_API ImGuiStandardMenuBuilder : public ImGuiMenuBuilder
	{
	public:

		/** override */
		virtual bool WithMenu(LightweightFunction<void()> func) const override;
	};

	/**
	 * ImGuiMainMenuBuilder: a menu builder that build a main menu
	 */

	class CHAOS_API ImGuiMainMenuBuilder : public ImGuiMenuBuilder
	{
	public:

		/** override */
		virtual bool WithMenu(LightweightFunction<void()> func) const override;
	};

#endif

}; // namespace chaos