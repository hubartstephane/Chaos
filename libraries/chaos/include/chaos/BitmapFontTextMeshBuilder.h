#pragma once

#include <chaos/StandardHeaders.h>
#include <chaos/GeometryFramework.h>


namespace chaos
{

  /**
   * BitmapFontTextMeshBuilder : this class is used to generate quads to represents characters for a text using a bitmap font
   */

class BitmapFontTextMeshBuilder
{

public:

  /** some bit field to generate alignment */
  static int const TOP_FLAG          = (1 << 0);
  /** some bit field to generate alignment */
  static int const BOTTOM_FLAG       = (1 << 1);
  /** some bit field to generate alignment */
  static int const LEFT_FLAG         = (1 << 2);
  /** some bit field to generate alignment */
  static int const RIGHT_FLAG        = (1 << 3);
  /** some bit field to generate alignment */
  static int const MIDDLE_VERT_FLAG  = (1 << 4);
  /** some bit field to generate alignment */
  static int const MIDDLE_HORIZ_FLAG = (1 << 5);

  /** some alignment */
  static int const ALIGN_CENTER      = MIDDLE_VERT_FLAG | MIDDLE_HORIZ_FLAG;
  /** some alignment */
  static int const ALIGN_TOPLEFT     = TOP_FLAG    | LEFT_FLAG;
  /** some alignment */
  static int const ALIGN_TOPRIGHT    = TOP_FLAG    | RIGHT_FLAG;
  /** some alignment */
  static int const ALIGN_BOTTOMLEFT  = BOTTOM_FLAG | LEFT_FLAG;
  /** some alignment */
  static int const ALIGN_BOTTOMRIGHT = BOTTOM_FLAG | RIGHT_FLAG;

  /** Structure used as input for the builder */
	class Params
	{
	public:

		/** constructor */
    Params(char const * characters = nullptr) :
      font_characters(characters),
      font_characters_per_line(0),
      font_characters_line_count(0),
      tab_size(2),
      alignment_flags(ALIGN_TOPLEFT),
      line_limit(0),
      position(0.0f, 0.0f),
      character_size(1.0f, 1.0f),
      spacing(0.0f, 0.0f),
      crop_texture(0.0f, 0.0f)
    {}

		/** the characters contained inside the font bitmap, in order */
		char const * font_characters;
		/** the number of characters on a line of the bitmap */
		int          font_characters_per_line;
    /** the number of lines of characters in the bitmap */
    int          font_characters_line_count;
    /** the size of a tab in 'SPACE' equivalent */
    int          tab_size;
    /** the alignment for the vertices */
    int          alignment_flags;
    /** maximum number of characters on a line */
    int          line_limit;
	
    /** the spot position */
    glm::vec2 position;
		/** in the result buffer, the size of each quad */
		glm::vec2 character_size;
		/** a space between characters or lines */
		glm::vec2 spacing;
    /** some texture ratio to crop each character */
    glm::vec2 crop_texture;
	};

	/** the main function */
	box2 BuildBuffer(char const * msg, Params const & params, std::vector<float> & vertices) const;

protected:

	/** insert vertices components by components */
	static void InsertVertexComponent(std::vector<float> & vertices, glm::vec2 const & v, glm::vec2 const & t);
  /** apply offset to all vertices */
  static void ApplyVertexOffset(std::vector<float> & vertices, glm::vec2 const & offset, size_t start, size_t end);
};

}; // namespace chaos
