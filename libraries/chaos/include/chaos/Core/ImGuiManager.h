namespace chaos
{
#ifdef CHAOS_FORWARD_DECLARATION

	class ImGuiManager;

#elif !defined CHAOS_TEMPLATE_IMPLEMENTATION

	/** a Manager to handle ImGui preferences */
	class ImGuiManager : public ResourceManager, public ConfigurableInterface
	{
	protected:

		/** override */
		virtual bool OnReadConfigurableProperties(JSONReadConfiguration config, ReadConfigurablePropertiesContext context) override;
		/** override */
		virtual bool DoStartManager() override;
	};

#endif

}; // namespace chaos