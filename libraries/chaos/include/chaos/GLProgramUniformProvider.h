#pragma once

#include <chaos/StandardHeaders.h>
#include <chaos/GLProgramData.h>
#include <chaos/GLTools.h>
#include <chaos/Texture.h>

namespace chaos
{

	/** GLUniformProviderEntryBase : hold data to be send as uniform */
	class GLUniformProviderEntryBase
	{
	public:
		/** destructor */
		virtual ~GLUniformProviderEntryBase() = default;
		/** set uniform */
		virtual bool BindUniform(GLUniformInfo const & uniform) = 0;
	};

	/** GLUniformProviderEntryValue : a typed specialization of GLUniformProviderEntryBase */
	template<typename T>
	class GLUniformProviderEntryValue : public GLUniformProviderEntryBase
	{
	public:
		/** the constructor */
		GLUniformProviderEntryValue(T const & in_value) : value(in_value) {}
		/** setting the uniform */
		virtual bool BindUniform(GLUniformInfo const & uniform) override { return uniform.SetUniform(value); }
		/** the value to be set */
		T value;
	};

	class UniformProviderVisitor
	{
	public:

		virtual void Visit(){};


	};


	/**
	* GLProgramUniformProviderChain : used to fill GLProgram binding for uniforms
	*/

	class GLProgramUniformProviderChain
	{

	public:

		/** constructor */
		GLProgramUniformProviderChain(GLProgramUniformProviderChain * in_previous_provider = nullptr):
			previous_provider(in_previous_provider) {}
		/** destructor */
		virtual ~GLProgramUniformProviderChain() = default;
		/** the main method */
		bool BindUniform(GLUniformInfo const & uniform) const;
		/** remove all uniforms for binding */
		void Clear()
		{
			uniform_map.clear();
		}
		/** register a uniform value */
		template<typename T>
		void AddUniformValue(char const * name, T const & value)
		{
			AddUniform(name, new GLUniformProviderEntryValue<T>(value));
		}
		/** register a uniform */
		void AddUniform(char const * name, GLUniformProviderEntryBase * uniform)
		{
			uniform_map[name] = std::move(std::unique_ptr<GLUniformProviderEntryBase>(uniform));		
		}
		/** register a texture as uniform */
		void AddTexture(char const * name, boost::intrusive_ptr<class Texture> const & texture)
		{
			texture_map[name] = texture;
		}

	protected:

		/** protected implementation of uniform binding */
		virtual bool BindUniformImpl(GLUniformInfo const & uniform) const;

	protected:

		/** responsability chain for providers */
		GLProgramUniformProviderChain * previous_provider;
		/** the uniforms texture to be set */
		std::map<std::string, boost::intrusive_ptr<Texture>> texture_map;
		/** the uniforms to be set */
		std::map<std::string, std::unique_ptr<GLUniformProviderEntryBase>> uniform_map;
	};

}; // namespace chaos
