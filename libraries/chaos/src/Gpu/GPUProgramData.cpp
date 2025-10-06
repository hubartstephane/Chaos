#include "chaos/ChaosPCH.h"
#include "chaos/ChaosInternals.h"

namespace chaos
{
	bool GPUUniformInfo::SetUniform(GPUTexture const* texture) const
	{
		if (uniform_setter == nullptr)
		{
			uniform_setter = GPUUniformSetter::GetUniformSetterForType(type);
			if (uniform_setter == nullptr)
				return false;
		}
		return uniform_setter->SetUniform(*this, texture);
	}

	void GPUProgramData::Clear()
	{
		uniforms.clear();
		attributes.clear();
	}

	void GPUProgramData::BindUniforms(GPUProgramProviderInterface const * provider) const
	{
		for (GPUUniformInfo const& uniform : uniforms)
			provider->BindUniform(uniform);
	}

	GPUUniformInfo const * GPUProgramData::FindUniform(char const * name) const
	{
		assert(name != nullptr);
		for (GPUUniformInfo const & uniform : uniforms)
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

				GPUAttributeInfo attribute;
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

				GPUUniformInfo uniform;
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

			#if 0

				glGetActiveUniformsiv(program, 1, &i, GL_UNIFORM_BLOCK_INDEX, &blockIndex);

			  // GL_MAX_UNIFORM_BUFFER_BINDINGS

				glUniformBlockBinding(program, i, 0);

				char blockName[256];
				GLsizei length;
				glGetActiveUniformBlockName(program, i, sizeof(blockName), &length, blockName);

				GLint blockSize;
				glGetActiveUniformBlockiv(program, i, GL_UNIFORM_BLOCK_DATA_SIZE, &blockSize);

				GLint bindingPoint;
				glGetActiveUniformBlockiv(program, i, GL_UNIFORM_BLOCK_BINDING, &bindingPoint);

				GLint uniformCount;
				glGetActiveUniformBlockiv(program, i, GL_UNIFORM_BLOCK_ACTIVE_UNIFORMS, &uniformCount);

				std::vector<GLint> indices(uniformCount);
				glGetActiveUniformBlockiv(program, i, GL_UNIFORM_BLOCK_ACTIVE_UNIFORM_INDICES, indices.data());

				char uniformName[256];
				glGetActiveUniformName(program, indices[j], sizeof(uniformName), &length, uniformName);

				glGetActiveUniformsiv(program, uniformCount, uniformIndices, GL_UNIFORM_OFFSET, uniformOffsets);

				GLuint uniform_index = (GLuint)i;
				GLint blockIndex;
				glGetActiveUniformsiv(program, i, &uniform_index, GL_UNIFORM_BLOCK_INDEX, &blockIndex);

				// GLuint blockIndex = glGetUniformBlockIndex(program, "SceneData");
				// glUniformBlockBinding(program, blockIndex, 0);

				glBindBufferBase(GL_UNIFORM_BUFFER, 1, ubo);
				void glBindBufferRange(GLenum target, GLuint index, GLuint buffer, GLintptr offset, GLsizeiptr size);

				#endif

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
			GPUAttributeInfo const & data = attributes[i];
			GLLog::Message("Attribute[%02d]  name = [%s]   array_size = [%d]   location = [%d]   type = [%s]", i, data.name.c_str(), data.array_size, data.location, GLTools::GLenumToString(data.type));
		}

		for (size_t i = 0; i < uniforms.size(); ++i)
		{
			GPUUniformInfo const & data = uniforms[i];
			GLLog::Message("Uniform[%02d]    name = [%s]   array_size = [%d]   location = [%d]   type = [%s]", i, data.name.c_str(), data.array_size, data.location, GLTools::GLenumToString(data.type));
		}
	}

	GLint GPUProgramData::GetLocation(VertexAttributeSemantic semantic, int semantic_index) const
	{
		for (auto const & attrib : attributes)
			if (attrib.semantic == semantic && attrib.semantic_index == semantic_index)
				return attrib.location;
		return -1;
	}

	void GPUProgramData::BindAttributes(GLuint vertex_array, GPUVertexDeclaration const & declaration, GPUProgramProviderInterface const * attribute_provider) const
	{
		size_t vertex_size = declaration.GetVertexSize();

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
						GLLog::Message("BindAttributes [%s] failure. Semantic [%d]. SemanticIndex [%d]. Location [%d]", attrib.name.c_str(), attrib.semantic, attrib.semantic_index, attrib.location);
				glDisableVertexArrayAttrib(vertex_array, location);
				continue;
			}

			// bind the attribute
			//int    entry_size = entry->GetEntrySize();
			size_t count = entry->GetComponentCount();
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
			else
				assert(0);

			glEnableVertexArrayAttrib(vertex_array, attrib.location);
		}
	}

}; // namespace chaos
