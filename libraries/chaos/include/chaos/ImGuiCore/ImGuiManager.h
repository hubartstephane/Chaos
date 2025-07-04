namespace chaos
{
#ifdef CHAOS_FORWARD_DECLARATION

	enum class ImGuiWindowStyle;

	class ImGuiManager;

#elif !defined CHAOS_TEMPLATE_IMPLEMENTATION

	/** describe the possible styles for ImGui */
	enum class ImGuiWindowStyle
	{
		Dark,
		Light,
		Classic
	};
	CHAOS_DECLARE_ENUM_METHOD(ImGuiStyle, CHAOS_API);

	/** describe one's font entry */
	class CHAOS_API ImGuiFontFace
	{

	public:

		/** get the range of glyphs for ImGui */
		ImWchar const* GetGlyphRange() const;

	public:

		/** the path of the font */
		boost::filesystem::path path;
		/** the range of the glyph to handle */
		std::optional<std::pair<uint32_t, uint32_t>> range;
		/** the height of the glyphs */
		std::optional<float> height;

		/** the buffer containing file content of the font */
		Buffer<char> file_buffer;

	protected:

		/** the range for imgui (it has to be kept in memory) */
		mutable ImWchar glyph_range[3] = { 0, 0, 0 };
	};

	CHAOS_API bool DoSaveIntoJSON(nlohmann::json* json, ImGuiFontFace const& src);
	CHAOS_API bool DoLoadFromJSON(JSONReadConfiguration config, ImGuiFontFace& dst);

	/** a Manager to handle ImGui preferences */
	class CHAOS_API ImGuiManager : public ResourceManager, public ConfigurationUserInterface
	{
	public:

		/** initialize one imgui context */
		virtual void InitializeImGuiContext(ImGuiContext * imgui_context) const;
		/** finalize one imgui context */
		virtual void FinalizeImGuiContext(ImGuiContext * imgui_context) const;
		/** called whenever the menu becomes enabled or disabled */
		virtual void OnImGuiMenuEnabledChanged(ImGuiContext * imgui_context, bool enabled) const;

		/** construct an atlas according to the configuration */
		ImFontAtlas* BuildFontAtlas() const;

	protected:

		/** override */
		virtual bool OnReadConfigurableProperties(JSONReadConfiguration config, ReadConfigurablePropertiesContext context) override;
		/** override */
		virtual bool DoStartManager() override;
		/** override */
		virtual bool DoStopManager() override;

		/** set mouse flags for ImGui */
		void SetImGuiContextMouseFlag(bool enabled) const;

	protected:

		/** the style of imgui */
		ImGuiWindowStyle window_style = ImGuiWindowStyle::Classic;
		/** the font faces */
		std::vector<ImGuiFontFace> font_faces;
	};

#endif

}; // namespace chaos