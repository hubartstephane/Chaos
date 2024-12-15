namespace chaos
{
#ifdef CHAOS_FORWARD_DECLARATION

	enum class ImGuiStyle;

	class ImGuiManager;

#elif !defined CHAOS_TEMPLATE_IMPLEMENTATION

	CHAOS_DEFINE_LOG(ImGuiManagerLog, "ImGuiManager")

	/** describe the possible styles for ImGui */
	enum class ImGuiStyle
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

		/** the path of the font */
		boost::filesystem::path path;
		/** the range of the glyph to handle */
		std::optional<std::pair<uint32_t, uint32_t>> range;
		/** the height of the glyphs */
		std::optional<float> height;

		/** the buffer containing file content of the font */
		Buffer<char> file_buffer;
	};

	CHAOS_API bool DoSaveIntoJSON(nlohmann::json* json, ImGuiFontFace const& src);
	CHAOS_API bool DoLoadFromJSON(JSONReadConfiguration config, ImGuiFontFace& dst);

	/** a Manager to handle ImGui preferences */
	class CHAOS_API ImGuiManager : public ResourceManager, public ConfigurableInterface
	{
	public:

		/** initialize one windows' imgui context */
		virtual void InitializeImGuiContext(Window* window) const;
		/** finalize one windows' imgui context */
		virtual void FinalizeImGuiContext(Window* window) const;
		/** called whenever the menu becomes enabled or disabled */
		virtual void OnImGuiMenuEnabledChanged(Window* window, bool enabled) const;

		/** construct an atlas according to the configuration */
		ImFontAtlas* BuildAtlas() const;

	protected:

		/** override */
		virtual bool OnReadConfigurableProperties(JSONReadConfiguration config, ReadConfigurablePropertiesContext context) override;
		/** override */
		virtual bool DoStartManager() override;

	protected:

		/** the style of imgui */
		ImGuiStyle style = ImGuiStyle::Classic;
		/** the font faces */
		std::vector<ImGuiFontFace> font_faces;
	};

#endif

}; // namespace chaos