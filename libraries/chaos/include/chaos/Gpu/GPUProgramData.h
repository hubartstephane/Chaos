#ifdef CHAOS_FORWARD_DECLARATION

namespace chaos
{
	class GLVariableInfo;
	class GLUniformInfo;
	class GLAttributeInfo;
	class GPUProgramData;

}; // namespace chaos

#elif defined CHAOS_TEMPLATE_IMPLEMENTATION


#else 

namespace chaos
{

	/**
	* GLVariableInfo : a base class for uniforms and attributes
	*/

	class GLVariableInfo
	{
	public:

		/** self descriptive */
		std::string name;
		/** self descriptive */
		GLint  array_size = 0;
		/** self descriptive */
		GLint  location = 0;
		/** self descriptive */
		GLenum type = GL_NONE;
	};

	/**
	* GLUniformInfo : definition of an uniform in the program
	*/

	class GLUniformInfo : public GLVariableInfo
	{
	public:

		bool SetUniform(glm::mat2x3 const & value) const;
		bool SetUniform(glm::mat2x4 const & value) const;
		bool SetUniform(glm::mat3x2 const & value) const;
		bool SetUniform(glm::mat3x4 const & value) const;
		bool SetUniform(glm::mat4x2 const & value) const;
		bool SetUniform(glm::mat4x3 const & value) const;
		bool SetUniform(glm::mat2   const & value) const;
		bool SetUniform(glm::mat3   const & value) const;
		bool SetUniform(glm::mat4   const & value) const;

		bool SetUniform(glm::dmat2x3 const & value) const;
		bool SetUniform(glm::dmat2x4 const & value) const;
		bool SetUniform(glm::dmat3x2 const & value) const;
		bool SetUniform(glm::dmat3x4 const & value) const;
		bool SetUniform(glm::dmat4x2 const & value) const;
		bool SetUniform(glm::dmat4x3 const & value) const;
		bool SetUniform(glm::dmat2   const & value) const;
		bool SetUniform(glm::dmat3   const & value) const;
		bool SetUniform(glm::dmat4   const & value) const;

		bool SetUniform(glm::tvec1<GLfloat> const & value) const;
		bool SetUniform(glm::tvec2<GLfloat> const & value) const;
		bool SetUniform(glm::tvec3<GLfloat> const & value) const;
		bool SetUniform(glm::tvec4<GLfloat> const & value) const;

		bool SetUniform(glm::tvec1<GLdouble> const & value) const;
		bool SetUniform(glm::tvec2<GLdouble> const & value) const;
		bool SetUniform(glm::tvec3<GLdouble> const & value) const;
		bool SetUniform(glm::tvec4<GLdouble> const & value) const;

		bool SetUniform(glm::tvec1<GLint> const & value) const;
		bool SetUniform(glm::tvec2<GLint> const & value) const;
		bool SetUniform(glm::tvec3<GLint> const & value) const;
		bool SetUniform(glm::tvec4<GLint> const & value) const;

		bool SetUniform(glm::tvec1<GLuint> const & value) const;
		bool SetUniform(glm::tvec2<GLuint> const & value) const;
		bool SetUniform(glm::tvec3<GLuint> const & value) const;
		bool SetUniform(glm::tvec4<GLuint> const & value) const;

		bool SetUniform(GLfloat  value) const;
		bool SetUniform(GLdouble value) const;
		bool SetUniform(GLint    value) const;
		bool SetUniform(GLuint   value) const;

		bool SetUniform(GPUTexture const * texture) const;

	public:

		/** the number of the sampler in the program */
		GLuint sampler_index = 0;
	};

	/**
	* GLAttributeInfo : definition of an attribute in the program
	*/

	class GLAttributeInfo : public GLVariableInfo
	{
	public:

		/** see GPUVertexDeclaration */
		VertexAttributeSemantic semantic = VertexAttributeSemantic::NONE;
		/** see GPUVertexDeclaration */
		int semantic_index = 0;
	};

	/**
	* GPUProgramData : used to register attributes, uniforms in a given program
	*/

	class GPUProgramData
	{
	public:

		/** generate the program data from a program */
		static GPUProgramData GetData(GLuint program);

		/** display information on program */
		void DisplayProgramDiagnostic() const;

		/** get the location for a given semantic/semantic_index */
		GLint GetLocation(VertexAttributeSemantic semantic, int semantic_index) const;

		/** bind the attributes */
		void BindAttributes(GLuint vertex_array, GPUVertexDeclaration const & declaration, class GPUProgramProviderBase const * attribute_provider = nullptr) const;

		/** find a uniform */
		GLUniformInfo * FindUniform(char const * name);
		/** find an uniform */
		GLUniformInfo const * FindUniform(char const * name) const;

		/** set an uniform by its name */
		template<typename T>
		bool SetUniform(char const * name, T const & value) const
		{
			assert(name != nullptr);
			GLUniformInfo const * uniform = FindUniform(name);
			if (uniform == nullptr)
				return false;
			GLTools::SetUniform(uniform->location, value); // beware, there is no verification of data coherence
			return true;  
		}
		/** try to bind all uniforms */
		void BindUniforms(class GPUProgramProviderBase const * provider) const;
		/** try to bind all uniforms using multiples provider */
		void BindUniforms(class GPUProgramProviderBase const * const * providers, int count) const;

		/** clear the program data object */
		void Clear();

	protected:

		/** compute the semantic and semantic index from a name */
		static std::string ExtractSemanticDataAndName(char const * attrib_name, std::pair<VertexAttributeSemantic, int> & semantic_data, bool & is_array);
		/** remove the '[' part from a variable name et returns if it is an array*/
		static std::string ExtractVariableName(char const * name, bool & is_array);

	public:

		/** the attributes used in the program */
		std::vector<GLAttributeInfo> attributes;
		/** the uniforms used in the program */
		std::vector<GLUniformInfo> uniforms;
	};

}; // namespace chaos

#endif // CHAOS_FORWARD_DECLARATION
