#pragma once

#if 0

#include <chaos/StandardHeaders.h>
#include <chaos/GLProgramData.h>
#include <chaos/GLTools.h>
#include <chaos/Texture.h>

namespace chaos
{
#if 0
	class GLUniformProviderVisitor
	{
	public:

		virtual void Provide(GLUniformInfo const & info, glm::vec1 const & value)
		{
		
		}

		virtual void Provide(GLUniformInfo const & info, glm::vec2 const & value)
		{

		}	

		...
	};

	template<typename T>
	class GLUniformProviderVisitor_Get
	{
	public:

		GLUniformProviderVisitor_Get(T * in_result)

		virtual void Provide(GLUniformInfo const & info, glm::vec1 const & value)
		{
			if (info.type == GL_VEC1)
			{
				result = ConvertVector<T>()
			
			}
			return true;

		

		}

		virtual void Provide(GLUniformInfo const & info, glm::vec2 const & value)
		{

		}	

		...
	};





	class GLUniformProviderBase
	{
	public:

		virtual bool Provide(GLUniformInfo const & info, GLUniformProviderVisitor & visitor){return false;}
	};

	class GLUniformProviderList : public GLUniformProviderBase
	{
	
	public:

		virtual bool Provide(GLUniformInfo const & info, GLUniformProviderVisitor & visitor)
		{
			size_t count = children.size();
			for (size_t i = 0 ; i < count ; ++i)
				if (children[i]->Provide(info, visitor))
					return true;
			return false;				
		}

	protected:

		std::vector<std::unique_ptr<GLUniformProviderBase>> children;
	};

	template<typename T>
	class GLUniformProviderValue : public GLUniformProviderBase
	{
	public:
	
		virtual bool Provide(GLUniformInfo const & info, GLUniformProviderVisitor & visitor)
		{
			if (info.name == name)
			{
				visitor.Provide(info, value);
				return true;
			}
			return false;		
		}
	
	protected:

		std::string name;

		T value;
	};

	class GLUniformProvider_Test1 : public GLUniformProviderBase
	{
	public:

		virtual bool Provide(GLUniformInfo const & info, GLUniformProviderVisitor & visitor)
		{
			if (info.name == "myvalue")
			{
				glm::vec3 tmp;
				visitor.Provide(info, tmp);			
				return true;
			}
			return false;		
		}		
	};

	class GLUniformProvider_Test1 : public GLUniformProviderBase
	{
	public:

		virtual bool Provide(GLUniformInfo const & info, GLUniformProviderVisitor & visitor)
		{
			if (info.name == "local_to_camera")
			{
				glm::mat4 local_to_world;
				glm::mat4 world_to_cam;




			}
		}
	};













	/** GLProgramUniformProviderVisitor : some action that will be done on uniform */
	class GLProgramUniformProviderVisitor
	{
	public:

		/** constructor */
		GLProgramUniformProviderVisitor(class GLProgramUniformProvider const * in_provider) : 
			provider(in_provider){ assert(provider != nullptr); }		
		/** the destructor */
		virtual ~GLProgramUniformProviderVisitor() = default;
		/** the method of interest */
		virtual void ProcessUniform(GLUniformInfo const & info){}

	protected:

		/** the provider */
		class GLProgramUniformProvider const * provider;
	};

	/** GLProgramUniformProviderVisitor : some action that will be done on uniform */
	class GLProgramUniformProviderVisitor_SetUniform : public GLProgramUniformProviderVisitor
	{
	public:

		/** constructor */
		GLProgramUniformProviderVisitor_SetUniform(class GLProgramUniformProvider const * in_provider) : 
			GLProgramUniformProviderVisitor(in_provider){}
		/** the method of interest */
		virtual void ProcessUniform(GLUniformInfo const & info);	
	};


	/** GLProgramUniformProviderVisitor : some action that will be done on uniform */
	class GLProgramUniformProviderVisitor_Test : public GLProgramUniformProviderVisitor
	{
	public:

		/** constructor */
		GLProgramUniformProviderVisitor_Test(class GLProgramUniformProvider const * in_provider) : 
			GLProgramUniformProviderVisitor(in_provider){}
		/** the method of interest */
		virtual void ProcessUniform(GLUniformInfo const & info);
	};


	class GLUniformProviderEntry
	{
	
	
	}



	class GLUniformFunc
	{
	public:

		void Process(GLUniformInfo const & info)
		{
			if (info.type == GL_FLOAT2)
				return ProcessFloat2(info);
		}

	protected:

		virtual void ProcessFloat2(GLUniformInfo const & info)
		{
			glm::vec2 result;
			
			
		}

	
	
	};
















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

	template<typename T>
	class GLProgramUniformProviderVisitor_GetValue 
	{
	
	};






	class UniformProviderVisitor
	{
	public:

		virtual void Visit(){};


	};


	/**
	* GLProgramUniformProvider : used to fill GLProgram binding for uniforms
	*/

	class GLProgramUniformProvider
	{

	public:

		/** constructor */
		GLProgramUniformProvider(GLProgramUniformProvider * in_previous_provider = nullptr):
			previous_provider(in_previous_provider) {}
		/** destructor */
		virtual ~GLProgramUniformProvider() = default;

		/** the main method */
		bool BindUniform(GLUniformInfo const & uniform) const;
		/** getting a value */
		template<typename T>
		bool GetValue(char const * name, T * return_value)
		{
			bool result = false;
			if (GLProgramUniformProviderVisitor_GetValue<T> visitor(this, return_value, &result))
				return result;
			return false;		
		}
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
		virtual bool ProcessUniform(GLProgramUniformProviderVisitor & visitor, GLUniformInfo const & uniform) const;
		/** protected implementation of uniform binding */
		virtual bool ProcessUniformImpl(GLProgramUniformProviderVisitor & visitor, GLUniformInfo const & uniform) const;

	protected:

		/** responsability chain for providers */
		GLProgramUniformProvider * previous_provider;
		/** the uniforms texture to be set */
		std::map<std::string, boost::intrusive_ptr<Texture>> texture_map;
		/** the uniforms to be set */
		std::map<std::string, std::unique_ptr<GLUniformProviderEntryBase>> uniform_map;
	};

#endif

	// -------------------------------------------------------------------

	class GLUniformProviderBase
	{
	public:

		virtual bool Provide(GLUniformInfo const & info, GLUniformProviderVisitor & visitor){return false;}
	};





}; // namespace chaos

#endif