namespace chaos
{
#ifdef CHAOS_FORWARD_DECLARATION

	class GLDebugOnScreenDisplay;

#elif !defined CHAOS_TEMPLATE_IMPLEMENTATION

	/**
	* GLDebugOnScreenDisplay : used to display some text on screen with timeout from a bitmap font (a texture)
	**/

	class CHAOS_API GLDebugOnScreenDisplay
	{
	public:

		/** class that is used to for initialization */
		class Params
		{
		public:

			/** the path of the bitmap texture */
			boost::filesystem::path texture_path;
			/** the characters contained inside the font bitmap, in order */
			std::string font_characters;
			/** the number of characters on a line of the bitmap */
			int         font_characters_per_line = 0;
			/** the number of lines of the bitmap */
			int         font_characters_line_count = 0;
			/** character size in pixel */
			int         character_width = 20;
			/** some space to add (or remove between each characters) in pixels */
			glm::ivec2  spacing = glm::ivec2(0, 0);
			/** some texels to remove in texture font grid to have pixels of interest */
			glm::ivec2  crop_texture = glm::ivec2(0, 0);
		};

		/** display the debug text */
		void Display(GPURenderContext* render_context, int width, int height) const;
		/** tick all lines (returns true whether there is a change) */
		bool Tick(float delta_time);
		/** Add a line in the log (character size and position are in pixels) */
		void AddLine(char const* line, float duration = -1);
		/** clear all the lines */
		void Clear();

		/** Initialize the displayer */
		bool Initialize(GLDebugOnScreenDisplay::Params const& params);
		/** Finalize */
		void Finalize();

		/** get the characters in the font */
		std::string const& GetFontCharacters() const { return mesh_builder_params.font_characters; }

	protected:

		/** internal initialization method */
		bool DoInitialize(GLDebugOnScreenDisplay::Params const& params);
		/** internal method to build the vertex buffer */
		bool BuildVertexBuffer(int width, int height) const;

	protected:

		/** the lines of the log (string + lifetime */
		std::vector<std::pair<std::string, float>> lines;
		/** indicates whether the buffer as to be updated */
		mutable bool rebuild_required = true;
		/** size of the screen (useful to know when lines have to change) */
		mutable int screen_width = -1;

		/** the program to run */
		shared_ptr<GPUProgram> program;
		/** the texture */
		shared_ptr<GPUTexture> texture;
		/** the vertex array */
		shared_ptr<GPUVertexArrayCache> vertex_array_cache;
		/** the vertex buffer */
		shared_ptr<GPUBuffer> vertex_buffer;
		/** the declaration of the vertex buffer */
		GPUVertexDeclaration declaration;

		/** number of element to draw */
		mutable size_t draw_count = 0;

		/** a copy of the parameters */
		Params mesh_builder_params;

		/** the source for the vertex shader */
		static char const* vertex_shader_source;
		/** the source for the pixel shader */
		static char const* fragment_shader_source;
	};

#endif

}; // namespace chaos