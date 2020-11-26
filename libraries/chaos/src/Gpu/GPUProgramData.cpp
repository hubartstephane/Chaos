#include <chaos/Chaos.h>

namespace chaos
{
	template<typename T1, typename T2>
	static bool SetUniformVectorImplHelper(GLUniformInfo const & uniform, T2 const & value)
	{
		int arity = GLTools::GetTypedEnumVectorArity<T1>(uniform.type);
		if (arity < 1 || arity > 4)
			return false;

		if (arity == 1)
			GLTools::SetUniform(uniform.location, RecastVector<glm::tvec1<T1>>(GLMTools::ConvertIntoVector(value))); // when the arity is 1, we force the usage of vec1 because it is simpler than a scalar value
		else if (arity == 2)
			GLTools::SetUniform(uniform.location, RecastVector<glm::tvec2<T1>>(GLMTools::ConvertIntoVector(value)));
		else if (arity == 3)
			GLTools::SetUniform(uniform.location, RecastVector<glm::tvec3<T1>>(GLMTools::ConvertIntoVector(value)));
		else if (arity == 4)
			GLTools::SetUniform(uniform.location, RecastVector<glm::tvec4<T1>>(GLMTools::ConvertIntoVector(value)));

		return true;
	}

	template<typename T>
	static bool SetUniformVectorImpl(GLUniformInfo const & uniform, T const & value)
	{
		// try a conversion/set uniform for each basic types
		if (SetUniformVectorImplHelper<GLfloat>(uniform, value))
			return true;
		if (SetUniformVectorImplHelper<GLdouble>(uniform, value))
			return true;
		if (SetUniformVectorImplHelper<GLint>(uniform, value))
			return true;
		if (SetUniformVectorImplHelper<GLuint>(uniform, value))
			return true;
		return false;
	}

	template<typename MATRIX_TYPE, typename T>
	static bool SetUniformMatrixImplHelper(GLUniformInfo const & uniform, GLenum matrix_type, T const & value)
	{
		if (uniform.type != matrix_type)
			return false;

		MATRIX_TYPE mat(value);
		GLTools::SetUniform(uniform.location, mat);

		return true;
	}

	template<typename T>
	static bool SetUniformMatrixImpl(GLUniformInfo const & uniform, T const & value)
	{
		if (SetUniformMatrixImplHelper<glm::mat2>(uniform, GL_FLOAT_MAT2, value))
			return true;
		if (SetUniformMatrixImplHelper<glm::mat3>(uniform, GL_FLOAT_MAT3, value))
			return true;
		if (SetUniformMatrixImplHelper<glm::mat4>(uniform, GL_FLOAT_MAT4, value))
			return true;
		if (SetUniformMatrixImplHelper<glm::mat2x3>(uniform, GL_FLOAT_MAT2x3, value))
			return true;
		if (SetUniformMatrixImplHelper<glm::mat2x4>(uniform, GL_FLOAT_MAT2x4, value))
			return true;
		if (SetUniformMatrixImplHelper<glm::mat2x3>(uniform, GL_FLOAT_MAT3x2, value))
			return true;
		if (SetUniformMatrixImplHelper<glm::mat2x4>(uniform, GL_FLOAT_MAT3x4, value))
			return true;
		if (SetUniformMatrixImplHelper<glm::mat2x3>(uniform, GL_FLOAT_MAT4x2, value))
			return true;
		if (SetUniformMatrixImplHelper<glm::mat2x4>(uniform, GL_FLOAT_MAT4x3, value))
			return true;

		if (SetUniformMatrixImplHelper<glm::dmat2>(uniform, GL_DOUBLE_MAT2, value))
			return true;
		if (SetUniformMatrixImplHelper<glm::dmat3>(uniform, GL_DOUBLE_MAT3, value))
			return true;
		if (SetUniformMatrixImplHelper<glm::dmat4>(uniform, GL_DOUBLE_MAT4, value))
			return true;
		if (SetUniformMatrixImplHelper<glm::dmat2x3>(uniform, GL_DOUBLE_MAT2x3, value))
			return true;
		if (SetUniformMatrixImplHelper<glm::dmat2x4>(uniform, GL_DOUBLE_MAT2x4, value))
			return true;
		if (SetUniformMatrixImplHelper<glm::dmat2x3>(uniform, GL_DOUBLE_MAT3x2, value))
			return true;
		if (SetUniformMatrixImplHelper<glm::dmat2x4>(uniform, GL_DOUBLE_MAT3x4, value))
			return true;
		if (SetUniformMatrixImplHelper<glm::dmat2x3>(uniform, GL_DOUBLE_MAT4x2, value))
			return true;
		if (SetUniformMatrixImplHelper<glm::dmat2x4>(uniform, GL_DOUBLE_MAT4x3, value))
			return true;

		return false;
	}

	bool GLUniformInfo::SetUniform(glm::mat2x3 const & value) const
	{
		return SetUniformMatrixImpl(*this, value);
	}
	bool GLUniformInfo::SetUniform(glm::mat2x4 const & value) const
	{
		return SetUniformMatrixImpl(*this, value);
	}
	bool GLUniformInfo::SetUniform(glm::mat3x2 const & value) const
	{
		return SetUniformMatrixImpl(*this, value);
	}
	bool GLUniformInfo::SetUniform(glm::mat3x4 const & value) const
	{
		return SetUniformMatrixImpl(*this, value);
	}
	bool GLUniformInfo::SetUniform(glm::mat4x2 const & value) const
	{
		return SetUniformMatrixImpl(*this, value);
	}
	bool GLUniformInfo::SetUniform(glm::mat4x3 const & value) const
	{
		return SetUniformMatrixImpl(*this, value);
	}
	bool GLUniformInfo::SetUniform(glm::mat2 const & value) const
	{
		return SetUniformMatrixImpl(*this, value);
	}
	bool GLUniformInfo::SetUniform(glm::mat3 const & value) const
	{
		return SetUniformMatrixImpl(*this, value);
	}
	bool GLUniformInfo::SetUniform(glm::mat4 const & value) const
	{
		return SetUniformMatrixImpl(*this, value);
	}

	bool GLUniformInfo::SetUniform(glm::dmat2x3 const & value) const
	{
		return SetUniformMatrixImpl(*this, value);
	}
	bool GLUniformInfo::SetUniform(glm::dmat2x4 const & value) const
	{
		return SetUniformMatrixImpl(*this, value);
	}
	bool GLUniformInfo::SetUniform(glm::dmat3x2 const & value) const
	{
		return SetUniformMatrixImpl(*this, value);
	}
	bool GLUniformInfo::SetUniform(glm::dmat3x4 const & value) const
	{
		return SetUniformMatrixImpl(*this, value);
	}
	bool GLUniformInfo::SetUniform(glm::dmat4x2 const & value) const
	{
		return SetUniformMatrixImpl(*this, value);
	}
	bool GLUniformInfo::SetUniform(glm::dmat4x3 const & value) const
	{
		return SetUniformMatrixImpl(*this, value);
	}
	bool GLUniformInfo::SetUniform(glm::dmat2 const & value) const
	{
		return SetUniformMatrixImpl(*this, value);
	}
	bool GLUniformInfo::SetUniform(glm::dmat3 const & value) const
	{
		return SetUniformMatrixImpl(*this, value);
	}
	bool GLUniformInfo::SetUniform(glm::dmat4 const & value) const
	{
		return SetUniformMatrixImpl(*this, value);
	}

	bool GLUniformInfo::SetUniform(glm::tvec1<GLfloat> const & value) const
	{
		return SetUniformVectorImpl(*this, value);
	}

	bool GLUniformInfo::SetUniform(glm::tvec2<GLfloat> const & value) const
	{
		return SetUniformVectorImpl(*this, value);
	}

	bool GLUniformInfo::SetUniform(glm::tvec3<GLfloat> const & value) const
	{
		return SetUniformVectorImpl(*this, value);
	}

	bool GLUniformInfo::SetUniform(glm::tvec4<GLfloat> const & value) const
	{
		return SetUniformVectorImpl(*this, value);
	}

	bool GLUniformInfo::SetUniform(glm::tvec1<GLdouble> const & value) const
	{
		return SetUniformVectorImpl(*this, value);
	}

	bool GLUniformInfo::SetUniform(glm::tvec2<GLdouble> const & value) const
	{
		return SetUniformVectorImpl(*this, value);
	}

	bool GLUniformInfo::SetUniform(glm::tvec3<GLdouble> const & value) const
	{
		return SetUniformVectorImpl(*this, value);
	}

	bool GLUniformInfo::SetUniform(glm::tvec4<GLdouble> const & value) const
	{
		return SetUniformVectorImpl(*this, value);
	}

	bool GLUniformInfo::SetUniform(glm::tvec1<GLint> const & value) const
	{
		return SetUniformVectorImpl(*this, value);
	}

	bool GLUniformInfo::SetUniform(glm::tvec2<GLint> const & value) const
	{
		return SetUniformVectorImpl(*this, value);
	}

	bool GLUniformInfo::SetUniform(glm::tvec3<GLint> const & value) const
	{
		return SetUniformVectorImpl(*this, value);
	}

	bool GLUniformInfo::SetUniform(glm::tvec4<GLint> const & value) const
	{
		return SetUniformVectorImpl(*this, value);
	}

	bool GLUniformInfo::SetUniform(glm::tvec1<GLuint> const & value) const
	{
		return SetUniformVectorImpl(*this, value);
	}

	bool GLUniformInfo::SetUniform(glm::tvec2<GLuint> const & value) const
	{
		return SetUniformVectorImpl(*this, value);
	}

	bool GLUniformInfo::SetUniform(glm::tvec3<GLuint> const & value) const
	{
		return SetUniformVectorImpl(*this, value);
	}

	bool GLUniformInfo::SetUniform(glm::tvec4<GLuint> const & value) const
	{
		return SetUniformVectorImpl(*this, value);
	}

	bool GLUniformInfo::SetUniform(GLfloat value) const
	{
		return SetUniformVectorImpl(*this, value);
	}

	bool GLUniformInfo::SetUniform(GLdouble value) const
	{
		return SetUniformVectorImpl(*this, value);
	}

	bool GLUniformInfo::SetUniform(GLint value) const
	{
		return SetUniformVectorImpl(*this, value);
	}

	bool GLUniformInfo::SetUniform(GLuint value) const
	{
		return SetUniformVectorImpl(*this, value);
	}

	bool GLUniformInfo::SetUniform(GPUTexture const * texture) const
	{
		if (!GLTools::IsSamplerType(type))
			return false;

		glBindTextureUnit(sampler_index, texture->GetResourceID()); // shuxxx missing unbind     glGetTextureHandleARB / glMakeTextureHandleResidentARB / glProgramUniformHandleui64ARB !!!!
		glUniform1i(location, sampler_index);

		return true;
	}

	void GPUProgramData::Clear()
	{
		uniforms.clear();
		attributes.clear();
	}

	void GPUProgramData::BindUniforms(GPUProgramProviderBase const * provider) const
	{
		if (provider != nullptr)
			BindUniforms(&provider, 1);
	}

	void GPUProgramData::BindUniforms(GPUProgramProviderBase const * const * providers, int count) const
	{
		if (providers == nullptr)
			return;
		for (GLUniformInfo const & uniform : uniforms)
		{
			for (int i = 0; i < count; ++i)
			{
				GPUProgramProviderBase const * provider = providers[i];
				if (provider != nullptr && provider->BindUniform(uniform))
					break;
			}
		}
	}

	GLUniformInfo * GPUProgramData::FindUniform(char const * name)
	{
		assert(name != nullptr);
		for (GLUniformInfo & uniform : uniforms)
			if (uniform.name == name)
				return &uniform;
		return nullptr;
	}

	GLUniformInfo const * GPUProgramData::FindUniform(char const * name) const
	{
		assert(name != nullptr);
		for (GLUniformInfo const & uniform : uniforms)
			if (uniform.name == name)
				return &uniform;
		return nullptr;
	}

	std::string GPUProgramData::ExtractVariableName(char const * name, bool & is_array)
	{
		char const * n = strchr(name, '[');
		if (n == nullptr)
		{
			is_array = false;
			return name;
		}
		else
		{
			is_array = true;
			return std::string(name, n - name);
		}
	}

	std::string GPUProgramData::ExtractSemanticDataAndName(char const * attrib_name, std::pair<VertexAttributeSemantic, int> & semantic_data, bool & is_array)
	{
		static std::pair<char const *, VertexAttributeSemantic> const names[] = // should be a prefix for name
		{
			{ "position",   VertexAttributeSemantic::POSITION },
			{ "color",      VertexAttributeSemantic::COLOR },
			{ "normal",     VertexAttributeSemantic::NORMAL },
			{ "binormal",   VertexAttributeSemantic::BINORMAL },
			{ "tangent",    VertexAttributeSemantic::TANGENT },
			{ "texcoord",   VertexAttributeSemantic::TEXCOORD },
			{ "boneindex",  VertexAttributeSemantic::BONEINDEX },
			{ "boneweight", VertexAttributeSemantic::BONEWEIGHT },
			{ "userdata",   VertexAttributeSemantic::USERDATA },
			{ nullptr, VertexAttributeSemantic::NONE }
		};

		is_array = false;

		// attribute with know semantic
		int i = 0;
		while (names[i].first != nullptr)
		{
			size_t l = strlen(names[i].first);

			if (_strnicmp(attrib_name, names[i].first, l) == 0) // prefix match
			{
				if (attrib_name[l] == 0 || std::isdigit(attrib_name[l])) // the prefix match
				{
					VertexAttributeSemantic semantic = names[i].second;
					int semantic_index = atoi(attrib_name + l);

					semantic_data = std::make_pair(semantic, semantic_index);
					return std::string(attrib_name, l);
				}
				else if (attrib_name[l] == '[') // this is array, no need to parse more
				{
					VertexAttributeSemantic semantic = names[i].second;
					int semantic_index = 0;

					semantic_data = std::make_pair(semantic, semantic_index);
					is_array = true;
					return std::string(attrib_name, l);
				}
			}
			++i;
		}

		// just a named attribute without any known semantic
		semantic_data = std::make_pair(VertexAttributeSemantic::NONE, -1);
		return attrib_name;
	}

	GPUProgramData GPUProgramData::GetData(GLuint program)
	{
		GPUProgramData result;

		// compute the length of a buffer to hold the longest name string 
		GLint max_attrib_length = 0;
		GLint max_uniform_length = 0;
		GLint max_block_length = 0;
		glGetProgramiv(program, GL_ACTIVE_ATTRIBUTE_MAX_LENGTH, &max_attrib_length);
		glGetProgramiv(program, GL_ACTIVE_UNIFORM_MAX_LENGTH, &max_uniform_length);
		glGetProgramiv(program, GL_ACTIVE_UNIFORM_BLOCK_MAX_NAME_LENGTH, &max_block_length);

		GLint max_length = std::max(max_attrib_length, std::max(max_uniform_length, max_block_length));

		// allocate a buffer for the names
		char buffer[256];

		char * name = (max_length + 1 < sizeof(buffer)) ?
			buffer :
			new char[max_length + 1];

		if (name != nullptr)
		{
			// read the attributes
			GLint attrib_count = 0;
			glGetProgramiv(program, GL_ACTIVE_ATTRIBUTES, &attrib_count);
			for (GLint i = 0; i < attrib_count; ++i)
			{
				GLint  written = 0;
				GLint  array_size = 0;
				GLint  location = 0;
				GLenum type = 0;
				glGetActiveAttrib(program, i, max_length, &written, &array_size, &type, name);

				location = glGetAttribLocation(program, name);
				if (location < 0)
					continue;        // some native attribute as 'gl_InstanceID' have negative location

				std::pair<VertexAttributeSemantic, int> semantic_data;

				bool is_array = false;

				GLAttributeInfo attribute;
				attribute.name = ExtractSemanticDataAndName(name, semantic_data, is_array); // try to find a semantic and an index according to the attribute name
				attribute.array_size = is_array ? array_size : -1;
				attribute.type = type;
				attribute.location = location;
				attribute.semantic = semantic_data.first;

				if (array_size == 1) // normal attribute (or an array of size 1)
				{
					attribute.semantic_index = semantic_data.second;
					result.attributes.push_back(attribute);
				}
				else // the attribute is an array
				{
					for (int i = 0; i < array_size; ++i)
					{
						attribute.location = location + i;
						attribute.semantic_index = i;
						result.attributes.push_back(attribute);    // insert an attribute for the each element of the array           
					}
				}
			}

			// read the uniforms     
			GLuint sampler_index = 0;
			GLint  uniform_count = 0;
			glGetProgramiv(program, GL_ACTIVE_UNIFORMS, &uniform_count);
			for (GLint i = 0; i < uniform_count; ++i)
			{
				GLint  written = 0;
				GLint  array_size = 0;
				GLint  location = 0;
				GLenum type = 0;
				glGetActiveUniform(program, i, max_length, &written, &array_size, &type, name);

				location = glGetUniformLocation(program, name);
				if (location < 0)
					continue;      // should be incorrect

				bool is_array = false;

				GLUniformInfo uniform;
				uniform.name = ExtractVariableName(name, is_array);
				uniform.array_size = is_array ? array_size : -1;
				uniform.type = type;
				uniform.location = location;
				if (GLTools::IsSamplerType(type))
					uniform.sampler_index = sampler_index++;

				result.uniforms.push_back(uniform);
			}

			// read the uniform blocks
			GLint uniform_block_count = 0;
			glGetProgramiv(program, GL_ACTIVE_UNIFORM_BLOCKS, &uniform_block_count);
			for (GLint i = 0; i < uniform_block_count; ++i)
			{



			}

			if (buffer != name)
				delete[] name;
		}

		return result;
	}

	void GPUProgramData::DisplayProgramDiagnostic() const
	{
		for (size_t i = 0; i < attributes.size(); ++i)
		{
			GLAttributeInfo const & data = attributes[i];
			Log::Message("Attribute[%02d]  name = [%s]   array_size = [%d]   location = [%d]   type = [%s]", i, data.name.c_str(), data.array_size, data.location, GLTools::GLenumToString(data.type));
		}

		for (size_t i = 0; i < uniforms.size(); ++i)
		{
			GLUniformInfo const & data = uniforms[i];
			Log::Message("Uniform[%02d]    name = [%s]   array_size = [%d]   location = [%d]   type = [%s]", i, data.name.c_str(), data.array_size, data.location, GLTools::GLenumToString(data.type));
		}
	}

	GLint GPUProgramData::GetLocation(VertexAttributeSemantic semantic, int semantic_index) const
	{
		for (auto const & attrib : attributes)
			if (attrib.semantic == semantic && attrib.semantic_index == semantic_index)
				return attrib.location;
		return -1;
	}

	void GPUProgramData::BindAttributes(GLuint vertex_array, GPUVertexDeclaration const & declaration, GPUProgramProviderBase const * attribute_provider) const
	{
		int vertex_size = declaration.GetVertexSize();

		for (auto const& attrib : attributes) // for each attribute of the GPU program, try to find the correct attribute in Vertex Buffer
		{
			GLint location = attrib.location;
			if (location < 0)
				continue;          // should never happen

			GPUVertexDeclarationEntry const* entry = nullptr;

			// this entry does not have a well known semantic
			if (attrib.semantic == VertexAttributeSemantic::NONE)
			{
				entry = declaration.GetEntry(attrib.name.c_str());
			}
			// try to find a known semantic
			else
			{
				// first  : use semantic_index
				// second : ignore semantic_index			
				entry = declaration.GetEntry(attrib.semantic, attrib.semantic_index);
				if (entry == nullptr)
					entry = declaration.GetEntry(attrib.semantic, -1); 
			}

			// no matching entry found : try to find a global default-attribute from the name
			if (entry == nullptr)
			{
				if (attribute_provider != nullptr)
					if (!attribute_provider->BindAttribute(attrib))
						Log::Message("BindAttributes [%s] failure. Semantic [%d]. SemanticIndex [%d]. Location [%d]", attrib.name.c_str(), attrib.semantic, attrib.semantic_index, attrib.location);
				glDisableVertexArrayAttrib(vertex_array, location);
				continue;
			}

			// bind the attribute
			int    entry_size = entry->GetEntrySize();
			int    count = entry->GetComponentCount();
			GLenum type = entry->GetComponentType();
			GLuint binding_index = 0;

			GLboolean normalized = false;
			GLuint    relative_offset = entry->offset;

			// see : https://community.khronos.org/t/vertex-shader-integer-input-broken/72878
			//
			// XXX : attribute GLSL side and C++ side may have different types
			//       that's why there are different functions
			//
			// shuxxx : I need to have a better understanding on how it works

			glVertexArrayAttribBinding(vertex_array, attrib.location, binding_index);

			if (type == GL_FLOAT)
				glVertexArrayAttribFormat(vertex_array, attrib.location, count, type, normalized, relative_offset);
			else if (type == GL_DOUBLE)
				glVertexArrayAttribLFormat(vertex_array, attrib.location, count, type, relative_offset);
			else if (type == GL_INT)
				glVertexArrayAttribIFormat(vertex_array, attrib.location, count, type, relative_offset);
			else assert(0);

			glEnableVertexArrayAttrib(vertex_array, attrib.location);
		}
	}

}; // namespace chaos
