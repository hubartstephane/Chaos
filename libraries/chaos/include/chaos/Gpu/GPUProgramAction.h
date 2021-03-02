#ifdef CHAOS_FORWARD_DECLARATION

namespace chaos
{
	class GPUProgramAction;
	class GPUProgramSetUniformAction;
	class GPUProgramSetAttributeAction;

	template<typename T>
	class GPUProgramGetValueAction;

}; // namespace chaos

#else 

namespace chaos
{

	/**
	* GPUProgramAction : base class action to be applyed to Providers
	*/

	class GPUProgramAction
	{
	public:

		/** indicates that the classes in this hierarchy will have a virtual destructor */
		virtual ~GPUProgramAction() = default;

		/** processing base scalar types */
		bool Process(char const * name, GLfloat value, GPUProgramProviderBase const * provider) { return Process(name, glm::tvec1<GLfloat>(value), provider); }
		bool Process(char const * name, GLdouble value, GPUProgramProviderBase const * provider) { return Process(name, glm::tvec1<GLdouble>(value), provider); }
		bool Process(char const * name, GLint value, GPUProgramProviderBase const * provider) { return Process(name, glm::tvec1<GLint>(value), provider); }
		bool Process(char const * name, GLuint value, GPUProgramProviderBase const * provider) { return Process(name, glm::tvec1<GLuint>(value), provider); }

		/** processing vector types */
		template<typename T> bool Process(char const * name, glm::tvec1<T> const & value, GPUProgramProviderBase const * provider)
		{
			return DoProcess(name, RecastVector<glm::tvec4<T>>(value), provider);
		}
		template<typename T> bool Process(char const * name, glm::tvec2<T> const & value, GPUProgramProviderBase const * provider)
		{
			return DoProcess(name, RecastVector<glm::tvec4<T>>(value), provider);
		}
		template<typename T> bool Process(char const * name, glm::tvec3<T> const & value, GPUProgramProviderBase const * provider)
		{
			return DoProcess(name, RecastVector<glm::tvec4<T>>(value), provider);
		}
		template<typename T> bool Process(char const * name, glm::tvec4<T> const & value, GPUProgramProviderBase const * provider)
		{
			return DoProcess(name, RecastVector<glm::tvec4<T>>(value), provider);
		}

		/** processing matrix types */
		bool Process(char const * name, glm::mat2x3 const & value, GPUProgramProviderBase const * provider) { return DoProcess(name, glm::mat4(value), provider); }
		bool Process(char const * name, glm::mat2x4 const & value, GPUProgramProviderBase const * provider) { return DoProcess(name, glm::mat4(value), provider); }
		bool Process(char const * name, glm::mat3x2 const & value, GPUProgramProviderBase const * provider) { return DoProcess(name, glm::mat4(value), provider); }
		bool Process(char const * name, glm::mat3x4 const & value, GPUProgramProviderBase const * provider) { return DoProcess(name, glm::mat4(value), provider); }
		bool Process(char const * name, glm::mat4x2 const & value, GPUProgramProviderBase const * provider) { return DoProcess(name, glm::mat4(value), provider); }
		bool Process(char const * name, glm::mat4x3 const & value, GPUProgramProviderBase const * provider) { return DoProcess(name, glm::mat4(value), provider); }
		bool Process(char const * name, glm::mat2 const & value, GPUProgramProviderBase const * provider) { return DoProcess(name, glm::mat4(value), provider); }
		bool Process(char const * name, glm::mat3 const & value, GPUProgramProviderBase const * provider) { return DoProcess(name, glm::mat4(value), provider); }
		bool Process(char const * name, glm::mat4 const & value, GPUProgramProviderBase const * provider) { return DoProcess(name, glm::mat4(value), provider); }

		bool Process(char const * name, glm::dmat2x3 const & value, GPUProgramProviderBase const * provider) { return DoProcess(name, glm::dmat4(value), provider); }
		bool Process(char const * name, glm::dmat2x4 const & value, GPUProgramProviderBase const * provider) { return DoProcess(name, glm::dmat4(value), provider); }
		bool Process(char const * name, glm::dmat3x2 const & value, GPUProgramProviderBase const * provider) { return DoProcess(name, glm::dmat4(value), provider); }
		bool Process(char const * name, glm::dmat3x4 const & value, GPUProgramProviderBase const * provider) { return DoProcess(name, glm::dmat4(value), provider); }
		bool Process(char const * name, glm::dmat4x2 const & value, GPUProgramProviderBase const * provider) { return DoProcess(name, glm::dmat4(value), provider); }
		bool Process(char const * name, glm::dmat4x3 const & value, GPUProgramProviderBase const * provider) { return DoProcess(name, glm::dmat4(value), provider); }
		bool Process(char const * name, glm::dmat2 const & value, GPUProgramProviderBase const * provider) { return DoProcess(name, glm::dmat4(value), provider); }
		bool Process(char const * name, glm::dmat3 const & value, GPUProgramProviderBase const * provider) { return DoProcess(name, glm::dmat4(value), provider); }
		bool Process(char const * name, glm::dmat4 const & value, GPUProgramProviderBase const * provider) { return DoProcess(name, glm::dmat4(value), provider); }

		/** processing texture */
		bool Process(char const * name, GPUTexture const * value, GPUProgramProviderBase const * provider) { return DoProcess(name, value, provider); }

	protected:

		/** the virtual methods to override */
		virtual bool DoProcess(char const * name, glm::tvec4<GLfloat> const & value, GPUProgramProviderBase const * provider) { return false; }
		virtual bool DoProcess(char const * name, glm::tvec4<GLdouble> const & value, GPUProgramProviderBase const * provider) { return false; }
		virtual bool DoProcess(char const * name, glm::tvec4<GLint> const & value, GPUProgramProviderBase const * provider) { return false; }
		virtual bool DoProcess(char const * name, glm::tvec4<GLuint> const & value, GPUProgramProviderBase const * provider) { return false; }

		virtual bool DoProcess(char const * name, glm::mat4 const & value, GPUProgramProviderBase const * provider) { return false; }
		virtual bool DoProcess(char const * name, glm::dmat4 const & value, GPUProgramProviderBase const * provider) { return false; }

		virtual bool DoProcess(char const * name, GPUTexture const * value, GPUProgramProviderBase const * provider) { return false; }
	};

	/**
	* GPUProgramSetUniformAction : action used to initialize an uniform
	*/

	class GPUProgramSetUniformAction : public GPUProgramAction
	{
	public:

		/** constructor */
		GPUProgramSetUniformAction(GLUniformInfo const & in_uniform): 
			uniform(in_uniform){}

	protected:

		/** the GPUProgramAction interface */
		virtual bool DoProcess(char const * name, glm::tvec4<GLfloat> const & value, GPUProgramProviderBase const * provider) override { return uniform.SetUniform(value); }
		virtual bool DoProcess(char const * name, glm::tvec4<GLdouble> const & value, GPUProgramProviderBase const * provider) override { return uniform.SetUniform(value); }
		virtual bool DoProcess(char const * name, glm::tvec4<GLint> const & value, GPUProgramProviderBase const * provider) override { return uniform.SetUniform(value); }
		virtual bool DoProcess(char const * name, glm::tvec4<GLuint> const & value, GPUProgramProviderBase const * provider) override { return uniform.SetUniform(value); }
		virtual bool DoProcess(char const * name, glm::mat4 const & value, GPUProgramProviderBase const * provider) override { return uniform.SetUniform(value); }
		virtual bool DoProcess(char const * name, glm::dmat4 const & value, GPUProgramProviderBase const * provider) override { return uniform.SetUniform(value); }
		virtual bool DoProcess(char const * name, GPUTexture const * value, GPUProgramProviderBase const * provider) override { return uniform.SetUniform(value); }

	protected:

		/** the uniform to set */
		GLUniformInfo const & uniform;
	};

	/**
	* GPUProgramSetUniformAction : action used to initialize an attribute with a default value
	*/

	class GPUProgramSetAttributeAction : public GPUProgramAction
	{
	public:

		/** constructor */
		GPUProgramSetAttributeAction(GLAttributeInfo const & in_attribute): 
			attribute(in_attribute){}

	protected:

		/** the GPUProgramAction interface */
		virtual bool DoProcess(char const * name, glm::tvec4<GLfloat> const & value, GPUProgramProviderBase const * provider) override { return false; }
		virtual bool DoProcess(char const * name, glm::tvec4<GLdouble> const & value, GPUProgramProviderBase const * provider) override { return false; }
		virtual bool DoProcess(char const * name, glm::tvec4<GLint> const & value, GPUProgramProviderBase const * provider) override { return false; }
		virtual bool DoProcess(char const * name, glm::tvec4<GLuint> const & value, GPUProgramProviderBase const * provider) override { return false; }
		virtual bool DoProcess(char const * name, glm::mat4 const & value, GPUProgramProviderBase const * provider) override { return false; }
		virtual bool DoProcess(char const * name, glm::dmat4 const & value, GPUProgramProviderBase const * provider) override { return false; }
		virtual bool DoProcess(char const * name, GPUTexture const * value, GPUProgramProviderBase const * provider) override { return false; }

	protected:

		/** the uniform to set */
		GLAttributeInfo const & attribute;
	};

	/**
	* GPUProgramGetValueAction : action used to get value for an uniform
	*/

	template<typename T>
	class GPUProgramGetValueAction : public GPUProgramAction
	{
	public:

		/** constructor */
		GPUProgramGetValueAction(T & in_result) : 
			result(in_result){}

	protected:

		/** main methods */
		virtual bool DoProcess(char const * name, glm::tvec4<GLfloat> const & value, GPUProgramProviderBase const * provider) override { return ConvertAndGet(value); }
		virtual bool DoProcess(char const * name, glm::tvec4<GLdouble> const & value, GPUProgramProviderBase const * provider) override { return ConvertAndGet(value); }
		virtual bool DoProcess(char const * name, glm::tvec4<GLint> const & value, GPUProgramProviderBase const * provider) override { return ConvertAndGet(value); }
		virtual bool DoProcess(char const * name, glm::tvec4<GLuint> const & value, GPUProgramProviderBase const * provider) override { return ConvertAndGet(value); }
		virtual bool DoProcess(char const * name, glm::mat4 const & value, GPUProgramProviderBase const * provider) override { return ConvertAndGet(value); }
		virtual bool DoProcess(char const * name, glm::dmat4 const & value, GPUProgramProviderBase const * provider) override { return ConvertAndGet(value); }

		virtual bool DoProcess(char const * name, GPUTexture const * value, GPUProgramProviderBase const * provider) override { return false; } // texture not implemented

		/** recasting vector into vector (type and arity) or  matrix into matrix (type and size) */
		template<typename U>
		bool ConvertAndGet(U const & value)
		{
			// both vector
			if constexpr (GLMTools::IsVectorType<T> && GLMTools::IsVectorType<U>)
			{
				result = RecastVector<T>(value);
				return true;
			}
			// both matrix
			else if constexpr (GLMTools::IsMatrixType<T> && GLMTools::IsMatrixType<U>)
			{
				result = T(value);
				return true;
			}
			return false;
		}


	protected:

		/** the value we get */
		T & result;
	};

}; // namespace chaos

#endif // CHAOS_FORWARD_DECLARATION

