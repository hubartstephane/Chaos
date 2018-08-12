#pragma once

#include <chaos/StandardHeaders.h>
#include <chaos/GPUProgramData.h>
#include <chaos/ReferencedObject.h>
#include <chaos/GLTools.h>
#include <chaos/GPUTexture.h>
#include <chaos/GLMTools.h>
#include <chaos/GPUProgramAction.h>

namespace chaos
{

	/**
	* GPUProgramProviderBase : a base class for filling uniforms or attributes in a program. The purpose is to take responsability to start an ACTION
	*/

	class GPUProgramProviderBase : public ReferencedObject
	{
		friend class GPUProgramProvider; // WTF : GPUProgramProvider could not call DoProcessAction(...) an another instance without that !!
		friend class GPUProgramProviderChain;
		friend class GPUProgramRenderMaterialProvider;

	public:

		/** the main method : returns try whether tha action has been handled (even if failed) */
		bool ProcessAction(char const * name, GPUProgramAction & action) const
		{
			return DoProcessAction(name, action, this);
		}

		/** utility function that deserve to set uniform */
		bool BindUniform(GLUniformInfo const & uniform) const
		{
			return ProcessAction(uniform.name.c_str(), GPUProgramSetUniformAction(uniform));
		}

		/** utility function that deserve to set attribute */
		bool BindAttribute(GLAttributeInfo const & attribute) const
		{
			return ProcessAction(attribute.name.c_str(), GPUProgramSetAttributeAction(attribute));
		}

		/** get a value for the uniform / attribute */
		template<typename T>
		bool GetValue(char const * name, T & result) const
		{
			return ProcessAction(name, GPUProgramGetValueAction<T>(result));
		}

	protected:

		/** the main method : returns true whether that action has been successfully handled */
		virtual bool DoProcessAction(char const * name, GPUProgramAction & action, GPUProgramProviderBase const * top_provider) const { return false; }
	};

	/**
	* GPUProgramProviderValue : used to fill GPUProgram binding for uniforms / attribute with simple values
	*/

	template<typename T>
	class GPUProgramProviderValue : public GPUProgramProviderBase
	{
	public:

		/** constructor */
		GPUProgramProviderValue(char const * in_name, T const & in_value) :
			handled_name(in_name), value(in_value) {}

	protected:

		/** the main method */
		virtual bool DoProcessAction(char const * name, GPUProgramAction & action, GPUProgramProviderBase const * top_provider) const override
		{
			if (handled_name != name)
				return false;
			return action.Process(name, value);
		}

	protected:

		/** the name of the uniform handled */
		std::string handled_name;
		/** the value of the uniform */
		T value;
	};

	/**
	* GPUProgramProviderTexture : used to fill GPUProgram binding for a texture
	*/

	class GPUProgramProviderTexture : public GPUProgramProviderBase
	{
	public:

		/** constructor */
		GPUProgramProviderTexture(char const * in_name, boost::intrusive_ptr<GPUTexture> in_value) :
			handled_name(in_name), value(in_value) {}

	protected:

		/** the main method */
		virtual bool DoProcessAction(char const * name, GPUProgramAction & action, GPUProgramProviderBase const * top_provider) const override;

	protected:

		/** the name of the uniform handled */
		std::string handled_name;
		/** the value of the uniform */
		boost::intrusive_ptr<GPUTexture> value;
	};

	/**
	* GPUProgramProvider : used to fill GPUProgram binding for multiple uniforms / uniforms
	*/

	class GPUProgramProvider : public GPUProgramProviderBase
	{
		friend class GPUProgramRenderMaterialProvider;

	public:

		/** register a uniform value */
		template<typename T>
		void AddVariableValue(char const * name, T const & value)
		{
			AddVariableProvider(new GPUProgramProviderValue<T>(name, value));
		}
		/** register a uniform texture */
		void AddVariableTexture(char const * name, boost::intrusive_ptr<class GPUTexture> texture)
		{
			AddVariableProvider(new GPUProgramProviderTexture(name, texture));
		}
		/** register a generic uniform */
		virtual void AddVariableProvider(GPUProgramProviderBase * provider);
		/** remove all uniforms for binding */
		virtual void Clear();

	protected:

		/** the main method */
		virtual bool DoProcessAction(char const * name, GPUProgramAction & action, GPUProgramProviderBase const * top_provider) const override;

	protected:

		/** the uniforms to be set */
		std::vector<boost::intrusive_ptr<GPUProgramProviderBase>> children_providers;
	};

	/**
	* GPUProgramProviderChain : an utility class used to enrich an existing provider with additionnal data
	*                           the reference count of this object is disabled. It deserves to be used on the stack
	*/

	class GPUProgramProviderChain : public DisableLastReferenceLost<GPUProgramProvider>
	{
	public:

		/** constructor */
		GPUProgramProviderChain(GPUProgramProviderBase const * in_other_provider) :
			other_provider(in_other_provider)
		{}

	protected:

		/** apply the actions */
		virtual bool DoProcessAction(char const * name, GPUProgramAction & action, GPUProgramProviderBase const * top_provider) const override;

	protected:

		/** another provider (use a non intrusive reference !!!) */
		GPUProgramProviderBase const * other_provider = nullptr;
	};

}; // namespace chaos
