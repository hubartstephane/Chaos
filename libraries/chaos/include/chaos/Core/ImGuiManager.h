namespace chaos
{
#ifdef CHAOS_FORWARD_DECLARATION

	enum class ImGuiStyle;

	class ImGuiManager;

#elif !defined CHAOS_TEMPLATE_IMPLEMENTATION

	enum class ImGuiStyle
	{
		Dark,
		Light,
		Classic
	};
	CHAOS_DECLARE_ENUM_METHOD(ImGuiStyle, CHAOS_API);

	/** a Manager to handle ImGui preferences */
	class ImGuiManager : public ResourceManager, public ConfigurableInterface
	{
	public:

		/** initialize one windows' imgui context */
		virtual void InitializeImGuiContext(Window* window) const;

	protected:

		/** override */
		virtual bool OnReadConfigurableProperties(JSONReadConfiguration config, ReadConfigurablePropertiesContext context) override;
		/** override */
		virtual bool DoStartManager() override;

	protected:

		/** the style of imgui */
		ImGuiStyle style = ImGuiStyle::Classic;
	};

#endif

}; // namespace chaos