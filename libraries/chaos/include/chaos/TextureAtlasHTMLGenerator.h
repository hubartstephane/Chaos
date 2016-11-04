#pragma once

#include <chaos/StandardHeaders.h>
#include <chaos/TextureAtlas.h>
#include <chaos/HTMLTools.h>

namespace chaos
{
  namespace TextureAtlasx
  {
    class AtlasHTMLOutputParams
    {
    public:

      /** show the HTML header */
      bool  show_header {true};
      /** show the atlas header */
      bool  show_atlas_header {true};
      /** show the textures */
      bool  show_textures {true};
      /** show the texture names */
      bool  show_textures_names {true};
      /** should the document refresh itself ("http-equiv", "refresh") every 2 seconds */
      bool  auto_refresh {true};
      /** a scale factor to be applied for the rendering of the textures (-1 for auto factor dependent on the atlas size) */
      float texture_scale {-1.0f};
    };

    /**
    * AtlasHTMLGenerator : use to generate HTML from an atlas
    */

    class AtlasHTMLGenerator
    {
    public:

      /** create an XML document and output debug information */
      static tinyxml2::XMLDocument * OutputToHTMLDocument(Atlas const & atlas, AtlasHTMLOutputParams params = AtlasHTMLOutputParams());
      /** create an XML document and output debug information */
      static void OutputToHTMLDocument(Atlas const & atlas, tinyxml2::XMLDocument * doc, AtlasHTMLOutputParams params = AtlasHTMLOutputParams());
      /** output the atlas trees in HTML format */
      static bool OutputToHTMLFile(Atlas const & atlas, boost::filesystem::path const & path, AtlasHTMLOutputParams params = AtlasHTMLOutputParams());
      /** output the atlas trees in HTML format */
      static bool OutputToHTMLFile(Atlas const & atlas, char const * filename, AtlasHTMLOutputParams params = AtlasHTMLOutputParams());

    protected:

      /** utility methods to iterate over BitmapSets or CharacterSets and display their entries informations into HTML */
      template<typename T>
      static void OutputElementsToHTMLDocument(std::vector<T*> const & elements, HTMLTools & html, tinyxml2::XMLElement * TABLE, tinyxml2::XMLElement * &TR, int bitmap_index, int & count);

      /** utility methods to iterate over BitmapSets or CharacterSets and display the texture rectangle into HTML */
      template<typename T>
      static void OutputBitmapsToHTMLDocument(std::vector<T*> const & elements, HTMLTools & html, tinyxml2::XMLElement * SVG, int bitmap_index, float scale);

      /** utility methods to iterate over BitmapSets or CharacterSets and display the texture filename into HTML */
      template<typename T>
      static void OutputBitmapFilenamesToHTMLDocument(std::vector<T*> const & elements, HTMLTools & html, tinyxml2::XMLElement * SVG, int bitmap_index, float scale);
    };




























  };

  // ===========================================================


  /**
   * TextureAtlasHTMLOutputParams : the parameters for HTML output of atlas 
   */

  class TextureAtlasHTMLOutputParams
  {
  public:

    /** constructor */
    TextureAtlasHTMLOutputParams():
      show_header(true), 
      show_atlas_header(true),
      show_textures(true), 
      show_textures_names(true),
      auto_refresh(true),
      texture_scale(-1.0f){}

    /** show the HTML header */
    bool  show_header;
    /** show the atlas header */
    bool  show_atlas_header;
    /** show the textures */
    bool  show_textures;
    /** show the texture names */
    bool  show_textures_names;
    /** should the document refresh itself ("http-equiv", "refresh") every 2 seconds */
    bool  auto_refresh;
    /** a scale factor to be applied for the rendering of the textures (-1 for auto factor dependent on the atlas size) */
    float texture_scale;
  };

  /**
  * TextureAtlasHTMLGenerator : use to generate HTML from an atlas
  */

  class TextureAtlasHTMLGenerator
  {
  public:

    /** create an XML document and output debug information */
    static tinyxml2::XMLDocument * OutputToHTMLDocument(TextureAtlas const & atlas, TextureAtlasHTMLOutputParams params = TextureAtlasHTMLOutputParams());
    /** create an XML document and output debug information */
    static void OutputToHTMLDocument(TextureAtlas const & atlas, tinyxml2::XMLDocument * doc, TextureAtlasHTMLOutputParams params = TextureAtlasHTMLOutputParams());
    /** output the atlas trees in HTML format */
    static bool OutputToHTMLFile(TextureAtlas const & atlas, boost::filesystem::path const & path, TextureAtlasHTMLOutputParams params = TextureAtlasHTMLOutputParams());
    /** output the atlas trees in HTML format */
    static bool OutputToHTMLFile(TextureAtlas const & atlas, char const * filename, TextureAtlasHTMLOutputParams params = TextureAtlasHTMLOutputParams());
  };
};
