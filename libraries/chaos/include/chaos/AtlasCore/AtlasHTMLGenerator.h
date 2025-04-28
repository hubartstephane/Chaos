namespace chaos
{
#ifdef CHAOS_FORWARD_DECLARATION

	class AtlasHTMLOutputParams;
	class AtlasHTMLGenerator;

#elif !defined CHAOS_TEMPLATE_IMPLEMENTATION

	/**
	* AtlasHTMLOutputParams : settings to output an atlas into an HTML file
	*/

	class CHAOS_API AtlasHTMLOutputParams
	{
	public:

		/** show the HTML header */
		bool  show_header = true;
		/** show the atlas header */
		bool  show_atlas_header = true;
		/** show the textures */
		bool  show_textures = true;
		/** show the texture names */
		bool  show_textures_names = true;
		/** should the document refresh itself ("http-equiv", "refresh") every 2 seconds */
		bool  auto_refresh = true;
		/** a scale factor to be applied for the rendering of the textures (-1 for auto factor dependent on the atlas size) */
		float texture_scale = -1.0f;
	};

	/**
	* AtlasHTMLGenerator : use to generate HTML from an atlas
	*/

	class CHAOS_API AtlasHTMLGenerator
	{
	public:

		/** create an XML document and output debug information */
		tinyxml2::XMLDocument* OutputToHTMLDocument(Atlas const& atlas, AtlasHTMLOutputParams params = {}) const;
		/** create an XML document and output debug information */
		void OutputToHTMLDocument(Atlas const& atlas, tinyxml2::XMLDocument* doc, AtlasHTMLOutputParams params = {}) const;
		/** output the atlas trees in HTML format */
		bool OutputToHTMLFile(Atlas const& atlas, FilePathParam const& path, AtlasHTMLOutputParams params = {}) const;

	protected:

		/** utility methods to iterate over BitmapSets or AtlasFontInfos and display their entries informations into HTML */
		template<typename T>
		void OutputElementsToHTMLDocument(char const* folder_path, std::vector<T> const& elements, XMLTools& html, tinyxml2::XMLElement* TABLE, tinyxml2::XMLElement*& TR, int bitmap_index, int& count) const;
		/** utility methods to iterate over elements of a folders and display their entries informations into HTML */
		void OutputElementsToHTMLDocument(char const* folder_path, AtlasFolderInfo const* folder_info, XMLTools& html, tinyxml2::XMLElement* TABLE, tinyxml2::XMLElement*& TR, int bitmap_index, int& count) const;

		/** utility methods to iterate over BitmapSets or AtlasFontInfos and display the texture rectangle into HTML */
		template<typename T>
		void OutputBitmapsToHTMLDocument(std::vector<T> const& elements, XMLTools& html, tinyxml2::XMLElement* SVG, int bitmap_index, float scale) const;
		/** utility method to recursively draw the bitmaps and characters from folders */
		void OutputBitmapsToHTMLDocument(AtlasFolderInfo const* folder_info, XMLTools& html, tinyxml2::XMLElement* SVG, int bitmap_index, float scale) const;

		/** utility methods to iterate over BitmapSets or AtlasFontInfos and display the texture filename into HTML */
		template<typename T>
		void OutputBitmapFilenamesToHTMLDocument(std::vector<T> const& elements, XMLTools& html, tinyxml2::XMLElement* SVG, int bitmap_index, float scale) const;
		/** utility method to recursively display bitmaps and characters filenames */
		void OutputBitmapFilenamesToHTMLDocument(AtlasFolderInfo const* folder_info, XMLTools& html, tinyxml2::XMLElement* SVG, int bitmap_index, float scale) const;

		/** get a string with the general information */
		std::string GetGeneralInformationString(Atlas const& atlas) const;
		/** get a string with the surface occupation of one atlas */
		std::string GetAtlasSpaceOccupationString(Atlas const& atlas, int bitmap_index) const;

		/** get the string for information about one element */
		template<typename T>
		std::string GetElementInfoString(T const& element) const;

		/** display information */
		void DoOutputElementInfo(AtlasFolderInfo const& folder_info, std::ostream& stream, int indent = 0) const;
		/** display information about one named element */
		void DoOutputElementInfo(NamedInterface const& info, std::ostream& stream, int indent = 0) const;
		/** display information about one bitmap layout */
		void DoOutputElementInfo(AtlasBitmapLayout const& info, std::ostream& stream, int indent = 0) const;
		/** display information about one character layout */
		void DoOutputElementInfo(AtlasCharacterLayout const& info, std::ostream& stream, int indent = 0) const;
		/** display information about one bitmap info */
		void DoOutputElementInfo(AtlasBitmapInfo const& info, std::ostream& stream, int indent = 0) const;
		/** display information about one character info */
		void DoOutputElementInfo(AtlasCharacterInfo const& info, std::ostream& stream, int indent = 0) const;
		/** display information about one font info */
		void DoOutputElementInfo(AtlasFontInfo const& info, std::ostream& stream, int indent = 0) const;

	protected:

		/** identation value */
		static constexpr size_t INDENT_VALUE = 2;
	};

#endif


}; // namespace chaos