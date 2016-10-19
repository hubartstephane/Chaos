#pragma once

#include <chaos/StandardHeaders.h>
#include <chaos/TextureAtlas.h>

namespace chaos
{

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
