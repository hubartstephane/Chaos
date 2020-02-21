#pragma once

#include <chaos/StandardHeaders.h>
#include <chaos/GeometryFramework.h>


namespace chaos
{

	/**
	* BitmapFontTextMeshAlign : alignment of generated texts
	*/

	enum class BitmapFontTextMeshAlign : int
	{
		/** some bit field to generate alignment */
		TOP_FLAG = (1 << 0),
		/** some bit field to generate alignment */
		BOTTOM_FLAG = (1 << 1),
		/** some bit field to generate alignment */
		LEFT_FLAG = (1 << 2),
		/** some bit field to generate alignment */
		RIGHT_FLAG = (1 << 3),
		/** some bit field to generate alignment */
		MIDDLE_VERT_FLAG = (1 << 4),
		/** some bit field to generate alignment */
		MIDDLE_HORIZ_FLAG = (1 << 5),

		/** some alignment */
		ALIGN_CENTER = MIDDLE_VERT_FLAG | MIDDLE_HORIZ_FLAG,
		/** some alignment */
		ALIGN_TOPLEFT = TOP_FLAG | LEFT_FLAG,
		/** some alignment */
		ALIGN_TOPRIGHT = TOP_FLAG | RIGHT_FLAG,
		/** some alignment */
		ALIGN_BOTTOMLEFT = BOTTOM_FLAG | LEFT_FLAG,
		/** some alignment */
		ALIGN_BOTTOMRIGHT = BOTTOM_FLAG | RIGHT_FLAG
	};

	/**
	* BitmapFontTextMeshBuilder : this class is used to generate quads to represents characters for a text using a bitmap font
	*/

	class BitmapFontTextMeshBuilder
	{
	public:

		/** Structure used as input for the builder */
		class Params
		{
		public:

			/** constructor */
			Params(char const * characters = nullptr) :
				font_characters(characters)
			{}

			/** the characters contained inside the font bitmap, in order */
			char const * font_characters = nullptr;
			/** the number of characters on a line of the bitmap */
			int          font_characters_per_line = 0;
			/** the number of lines of characters in the bitmap */
			int          font_characters_line_count = 0;
			/** the size of a tab in 'SPACE' equivalent */
			int          tab_size = 2;
			/** the alignment for the vertices */
			BitmapFontTextMeshAlign alignment_flags = BitmapFontTextMeshAlign::ALIGN_TOPLEFT;
			/** maximum number of characters on a line */
			int          line_limit = 0;

			/** the spot position */
			glm::vec2 position = glm::vec2(0.0f, 0.0f);
			/** in the result buffer, the size of each quad */
			glm::vec2 character_size = glm::vec2(1.0f, 1.0f);
			/** a space between characters or lines */
			glm::vec2 spacing = glm::vec2(0.0f, 0.0f);
			/** some texture ratio to crop each character */
			glm::vec2 crop_texture = glm::vec2(0.0f, 0.0f);
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
