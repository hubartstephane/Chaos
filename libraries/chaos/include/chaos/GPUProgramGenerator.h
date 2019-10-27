#pragma once

#include <chaos/StandardHeaders.h>
#include <chaos/GPUClasses.h>

#include <chaos/Buffer.h>
#include <chaos/GPUProgramSourceGenerator.h>
#include <chaos/GPUProgram.h>
#include <chaos/FilePath.h>

namespace chaos
{


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
		virtual GPUProgram * GenProgramObject(DefinitionSet const & definitions = DefinitionSet()) const;

		/** add a generator a given shader */
		bool AddSourceGenerator(GLenum shader_type, GPUProgramSourceGenerator * generator);

		/** add a source for a given shader */
		bool AddShaderSource(GLenum shader_type, Buffer<char> buffer);
		/** add a source for a given shader */
		bool AddShaderSource(GLenum shader_type, char const * src);
		/** add a source file for a given shader */
		bool AddShaderSourceFile(GLenum shader_type, FilePathParam const & path);

		/** generate a string for all definitions */
		static std::string DefinitionsToString(DefinitionSet const & definitions);

	protected:

		/** returns true whenever this is a valid shader type */
		static bool IsShaderTypeValid(GLenum shader_type);
		/** generate a shader for a set of sources */
		GLuint GenerateShader(GLenum shader_type, GeneratorSet const & generators, DefinitionSet const & definitions, std::string const & definitions_string) const;
		/** generate a shader for a set of sources */
		GLuint DoGenerateShader(GLenum shader_type, std::vector<char const *> const & sources) const;
		/** called just before linkage */
		virtual bool PreLinkProgram(GLuint program) const;
		/** generate a program from the sources */
		GLuint GenProgram(DefinitionSet const & definitions = DefinitionSet()) const;

	protected:

		/** the shaders */
		std::map<GLenum, GeneratorSet> shaders;
	};

}; // namespace chaos

