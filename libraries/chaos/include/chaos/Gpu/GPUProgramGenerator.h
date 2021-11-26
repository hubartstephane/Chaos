namespace chaos
{
#ifdef CHAOS_FORWARD_DECLARATION

	enum class ShaderType;
	class GPUProgramGenerator;

#elif !defined CHAOS_TEMPLATE_IMPLEMENTATION

	/**
	 * The types of possible shaders
	 */

	enum class ShaderType
	{
		ANY = GL_NONE,
		VERTEX = GL_VERTEX_SHADER,
		FRAGMENT = GL_FRAGMENT_SHADER,
		GEOMETRY = GL_GEOMETRY_SHADER,
		TESS_EVALUATION = GL_TESS_EVALUATION_SHADER,
		TESS_CONTROL = GL_TESS_CONTROL_SHADER,
		COMPUTE = GL_COMPUTE_SHADER
	};

	CHAOS_DECLARE_ENUM_METHOD(ShaderType);

	/**
	* GPUProgramGenerator : this class deserves to generate GPU programs from sources.
	*                       It is possible to use cache system and to add some definitions so we can generate multiple programs with small macro differences.
	*/

	class GPUProgramGenerator
	{
	public:

		using GeneratorSet = std::vector<shared_ptr<GPUProgramSourceGenerator>>;

		using DefinitionSet = std::map<std::string, int>;

		/** destructor */
		virtual ~GPUProgramGenerator() = default;
		/** reset the content */
		void Reset();

		/** generate a program from the sources */
		virtual GPUProgram* GenProgramObject(DefinitionSet const& definitions = {}) const;

		/** add a generator a given shader */
		bool AddSourceGenerator(ShaderType shader_type, GPUProgramSourceGenerator* generator);

		/** add a source for a given shader */
		bool AddShaderSource(ShaderType shader_type, Buffer<char> buffer);
		/** add a source for a given shader */
		bool AddShaderSource(ShaderType shader_type, char const* src);
		/** add a source file for a given shader */
		bool AddShaderSourceFile(ShaderType shader_type, FilePathParam const& path);

		/** generate a string for all definitions */
		static std::string DefinitionsToString(DefinitionSet const& definitions);

	protected:

		/** generate a shader for a set of sources and attach it to the program */
		GLuint GenerateShader(GLuint program, ShaderType shader_type, GeneratorSet const& generators, DefinitionSet const& definitions, std::string const& definitions_string) const;
		/** generate a shader for a set of sources */
		GLuint DoGenerateShader(ShaderType shader_type, GeneratorSet const& generators, DefinitionSet const& definitions, std::string const& definitions_string) const;
		/** generate a shader for a set of sources */
		GLuint DoGenerateShader(ShaderType shader_type, std::vector<char const*> const& sources) const;
		/** called just before linkage */
		virtual bool PreLinkProgram(GLuint program) const;
		/** generate a program from the sources */
		GLuint GenProgram(DefinitionSet const& definitions = DefinitionSet()) const;
		/** insert extra source (utility functions ...) for the given shader type */
		void AddFrameworkSources(ShaderType shader_type, std::vector<char const*>& sources, std::vector<Buffer<char>>& buffers) const;

	protected:

		/** the shaders */
		std::map<ShaderType, GeneratorSet> shaders;
		/** whether a render shader has been inserted (nor ANY nor COMPUTE) */
		bool has_render_shader = false;
		/** whether a compute shader has been inserted */
		bool has_compute_shader = false;
	};

#endif

}; // namespace chaos