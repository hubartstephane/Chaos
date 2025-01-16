namespace chaos
{
#ifdef CHAOS_FORWARD_DECLARATION

	class GPUVariableInfo;
	class GPUUniformInfo;
	class GPUAttributeInfo;
	class GPUProgramData;

#elif !defined CHAOS_TEMPLATE_IMPLEMENTATION

	/**
	* GPUVariableInfo : a base class for uniforms and attributes
	*/

	class CHAOS_API GPUVariableInfo
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
	* GPUUniformInfo : definition of an uniform in the program
	*/

	class CHAOS_API GPUUniformInfo : public GPUVariableInfo
	{
	public:

		/** set the uniform for any types */
		template<typename T>
		bool SetUniform(T const& value) const;
		/** set the uniform for textures */
		bool SetUniform(GPUTexture const* texture) const;

	public:

		/** the number of the sampler in the program */
		GLuint sampler_index = 0;

	protected:

		/** a pointer on the (singleton) setter */
		mutable GPUUniformSetter const * uniform_setter = nullptr;
	};

	/**
	* GPUAttributeInfo : definition of an attribute in the program
	*/

	class CHAOS_API GPUAttributeInfo : public GPUVariableInfo
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

	class CHAOS_API GPUProgramData
	{
	public:

		/** generate the program data from a program */
		static GPUProgramData GetData(GLuint program);

		/** display information on program */
		void DisplayProgramDiagnostic() const;

		/** get the location for a given semantic/semantic_index */
		GLint GetLocation(VertexAttributeSemantic semantic, int semantic_index) const;

		/** bind the attributes */
		void BindAttributes(GLuint vertex_array, GPUVertexDeclaration const& declaration, class GPUProgramProviderInterface const* attribute_provider = nullptr) const;

		/** find an uniform */
		GPUUniformInfo const* FindUniform(char const* name) const;

		/** set an uniform by its name */
		template<typename T>
		bool SetUniform(char const* name, T const& value) const
		{
			assert(name != nullptr);
			GPUUniformInfo const* uniform = FindUniform(name);
			if (uniform == nullptr)
				return false;
			return uniform->SetUniform(value);
		}
		/** try to bind all uniforms */
		void BindUniforms(class GPUProgramProviderInterface const* provider) const;

		/** clear the program data object */
		void Clear();

	protected:

		/** compute the semantic and semantic index from a name */
		static std::string ExtractSemanticDataAndName(char const* attrib_name, std::pair<VertexAttributeSemantic, int>& semantic_data, bool& is_array);
		/** remove the '[' part from a variable name et returns if it is an array*/
		static std::string ExtractVariableName(char const* name, bool& is_array);

	protected:

		/** the attributes used in the program */
		std::vector<GPUAttributeInfo> attributes;
		/** the uniforms used in the program */
		std::vector<GPUUniformInfo> uniforms;
	};

#else // CHAOS_TEMPLATE_IMPLEMENTATION

	template<typename T>
	bool GPUUniformInfo::SetUniform(T const& value) const
	{
		if (uniform_setter == nullptr)
		{
			uniform_setter = GPUUniformSetter::GetUniformSetterForType(type);
			if (uniform_setter == nullptr)
				return false;
		}
		return uniform_setter->SetUniform(*this, value);
	}

#endif

}; // namespace chaos