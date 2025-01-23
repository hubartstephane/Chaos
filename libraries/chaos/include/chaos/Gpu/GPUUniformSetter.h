namespace chaos
{
#ifdef CHAOS_FORWARD_DECLARATION

	class GPUUniformSetter;

	template<typename SCALAR_TYPE>
	class GPUUniformScalarSetter;

	template<typename MATRIX_TYPE>
	class GPUUniformMatrixSetter;

	template<typename VECTOR_TYPE>
	class GPUUniformVectorSetter;

	class GPUUniformSamplerSetter;

#elif !defined CHAOS_TEMPLATE_IMPLEMENTATION

	/**
	 * GPUUniformSetter: an utility class to cast parameter and set the uniform if possible
	 */

	class GPUUniformSetter
	{
	public:

		/** getting a singleton setter depending on the type of the uniform */
		static GPUUniformSetter const* GetUniformSetterForType(GLenum type);

		/** setting uniform for a texture */
		virtual bool SetUniform(GPUUniformInfo const& uniform_info, GPUTexture const* texture) const;

		/** setting uniform for some scalar/vector/matrix types */
#define CHAOS_SET_UNIFORM_IMPLEMENTATION(parameter_type)\
		virtual bool SetUniform(GPUUniformInfo const & uniform_info, parameter_type const& value) const\
		{\
			return SetUniformImpl(uniform_info, value);\
		}

		CHAOS_SET_UNIFORM_IMPLEMENTATION(bool);
		CHAOS_SET_UNIFORM_IMPLEMENTATION(float);
		CHAOS_SET_UNIFORM_IMPLEMENTATION(double);
		CHAOS_SET_UNIFORM_IMPLEMENTATION(int);
		CHAOS_SET_UNIFORM_IMPLEMENTATION(unsigned int);

		CHAOS_SET_UNIFORM_IMPLEMENTATION(glm::mat2);
		CHAOS_SET_UNIFORM_IMPLEMENTATION(glm::mat3);
		CHAOS_SET_UNIFORM_IMPLEMENTATION(glm::mat4);
		CHAOS_SET_UNIFORM_IMPLEMENTATION(glm::mat2x3);
		CHAOS_SET_UNIFORM_IMPLEMENTATION(glm::mat2x4);
		CHAOS_SET_UNIFORM_IMPLEMENTATION(glm::mat3x2);
		CHAOS_SET_UNIFORM_IMPLEMENTATION(glm::mat3x4);
		CHAOS_SET_UNIFORM_IMPLEMENTATION(glm::mat4x2);
		CHAOS_SET_UNIFORM_IMPLEMENTATION(glm::mat4x3);

		CHAOS_SET_UNIFORM_IMPLEMENTATION(glm::dmat2);
		CHAOS_SET_UNIFORM_IMPLEMENTATION(glm::dmat3);
		CHAOS_SET_UNIFORM_IMPLEMENTATION(glm::dmat4);
		CHAOS_SET_UNIFORM_IMPLEMENTATION(glm::dmat2x3);
		CHAOS_SET_UNIFORM_IMPLEMENTATION(glm::dmat2x4);
		CHAOS_SET_UNIFORM_IMPLEMENTATION(glm::dmat3x2);
		CHAOS_SET_UNIFORM_IMPLEMENTATION(glm::dmat3x4);
		CHAOS_SET_UNIFORM_IMPLEMENTATION(glm::dmat4x2);
		CHAOS_SET_UNIFORM_IMPLEMENTATION(glm::dmat4x3);

		CHAOS_SET_UNIFORM_IMPLEMENTATION(glm::tvec2<bool>);
		CHAOS_SET_UNIFORM_IMPLEMENTATION(glm::tvec3<bool>);
		CHAOS_SET_UNIFORM_IMPLEMENTATION(glm::tvec4<bool>);
		CHAOS_SET_UNIFORM_IMPLEMENTATION(glm::tvec2<float>);
		CHAOS_SET_UNIFORM_IMPLEMENTATION(glm::tvec3<float>);
		CHAOS_SET_UNIFORM_IMPLEMENTATION(glm::tvec4<float>);
		CHAOS_SET_UNIFORM_IMPLEMENTATION(glm::tvec2<double>);
		CHAOS_SET_UNIFORM_IMPLEMENTATION(glm::tvec3<double>);
		CHAOS_SET_UNIFORM_IMPLEMENTATION(glm::tvec4<double>);
		CHAOS_SET_UNIFORM_IMPLEMENTATION(glm::tvec2<int>);
		CHAOS_SET_UNIFORM_IMPLEMENTATION(glm::tvec3<int>);
		CHAOS_SET_UNIFORM_IMPLEMENTATION(glm::tvec4<int>);
		CHAOS_SET_UNIFORM_IMPLEMENTATION(glm::tvec2<unsigned int>);
		CHAOS_SET_UNIFORM_IMPLEMENTATION(glm::tvec3<unsigned int>);
		CHAOS_SET_UNIFORM_IMPLEMENTATION(glm::tvec4<unsigned int>);

#undef CHAOS_SET_UNIFORM_IMPLEMENTATION

	protected:

		/** default noaction implementation */
		template<typename PARAMETER_TYPE>
		bool SetUniformImpl(GPUUniformInfo const& uniform_info, PARAMETER_TYPE const& value) const
		{
			return false;
		}
	};

	/**
	 * GPUUniformScalarSetter: a uniform setter dedicated to scalars
	 */

	template<typename SCALAR_TYPE>
	class GPUUniformScalarSetter : public GPUUniformSetter
	{
	public:

		/** overrides */
#define CHAOS_SET_UNIFORM_IMPLEMENTATION(parameter_type)\
		virtual bool SetUniform(GPUUniformInfo const & uniform_info, parameter_type const& value) const override\
		{\
			return SetUniformImpl(uniform_info, value);\
		}

		CHAOS_SET_UNIFORM_IMPLEMENTATION(bool);
		CHAOS_SET_UNIFORM_IMPLEMENTATION(float);
		CHAOS_SET_UNIFORM_IMPLEMENTATION(double);
		CHAOS_SET_UNIFORM_IMPLEMENTATION(int);
		CHAOS_SET_UNIFORM_IMPLEMENTATION(unsigned int);

		CHAOS_SET_UNIFORM_IMPLEMENTATION(glm::tvec2<bool>);
		CHAOS_SET_UNIFORM_IMPLEMENTATION(glm::tvec3<bool>);
		CHAOS_SET_UNIFORM_IMPLEMENTATION(glm::tvec4<bool>);
		CHAOS_SET_UNIFORM_IMPLEMENTATION(glm::tvec2<float>);
		CHAOS_SET_UNIFORM_IMPLEMENTATION(glm::tvec3<float>);
		CHAOS_SET_UNIFORM_IMPLEMENTATION(glm::tvec4<float>);
		CHAOS_SET_UNIFORM_IMPLEMENTATION(glm::tvec2<double>);
		CHAOS_SET_UNIFORM_IMPLEMENTATION(glm::tvec3<double>);
		CHAOS_SET_UNIFORM_IMPLEMENTATION(glm::tvec4<double>);
		CHAOS_SET_UNIFORM_IMPLEMENTATION(glm::tvec2<int>);
		CHAOS_SET_UNIFORM_IMPLEMENTATION(glm::tvec3<int>);
		CHAOS_SET_UNIFORM_IMPLEMENTATION(glm::tvec4<int>);
		CHAOS_SET_UNIFORM_IMPLEMENTATION(glm::tvec2<unsigned int>);
		CHAOS_SET_UNIFORM_IMPLEMENTATION(glm::tvec3<unsigned int>);
		CHAOS_SET_UNIFORM_IMPLEMENTATION(glm::tvec4<unsigned int>);

#undef CHAOS_SET_UNIFORM_IMPLEMENTATION

	protected:

		/** generic method to set uniforms from a scalar */
		template<typename PARAMETER_TYPE>
		bool SetUniformImpl(GPUUniformInfo const& uniform_info, PARAMETER_TYPE const& value) const
		{
			if constexpr (std::is_same_v<SCALAR_TYPE, PARAMETER_TYPE>)
			{
				GLTools::SetUniform(uniform_info.location, value);
			}
			else
			{
				SCALAR_TYPE converted_scalar = SCALAR_TYPE(value);
				GLTools::SetUniform(uniform_info.location, converted_scalar);
			}
			return true;
		}
		/** generic method to set uniforms from a vector */
		template<int ARITY, typename PARAMETER_TYPE>
		bool SetUniformImpl(GPUUniformInfo const& uniform_info, glm::vec<ARITY, PARAMETER_TYPE> const& value) const
		{
			return SetUniformImpl(uniform_info, value.x);
		}
	};

	/**
	 * GPUUniformVectorSetter: a uniform setter dedicated for vectors
	 */

	template<typename VECTOR_TYPE>
	class GPUUniformVectorSetter : public GPUUniformSetter
	{
	public:

		/** overrides */
#define CHAOS_SET_UNIFORM_IMPLEMENTATION(parameter_type)\
		virtual bool SetUniform(GPUUniformInfo const & uniform_info, parameter_type const& value) const override\
		{\
			return SetUniformImpl(uniform_info, value);\
		}

		CHAOS_SET_UNIFORM_IMPLEMENTATION(bool);
		CHAOS_SET_UNIFORM_IMPLEMENTATION(float);
		CHAOS_SET_UNIFORM_IMPLEMENTATION(double);
		CHAOS_SET_UNIFORM_IMPLEMENTATION(int);
		CHAOS_SET_UNIFORM_IMPLEMENTATION(unsigned int);

		CHAOS_SET_UNIFORM_IMPLEMENTATION(glm::tvec2<bool>);
		CHAOS_SET_UNIFORM_IMPLEMENTATION(glm::tvec3<bool>);
		CHAOS_SET_UNIFORM_IMPLEMENTATION(glm::tvec4<bool>);
		CHAOS_SET_UNIFORM_IMPLEMENTATION(glm::tvec2<float>);
		CHAOS_SET_UNIFORM_IMPLEMENTATION(glm::tvec3<float>);
		CHAOS_SET_UNIFORM_IMPLEMENTATION(glm::tvec4<float>);
		CHAOS_SET_UNIFORM_IMPLEMENTATION(glm::tvec2<double>);
		CHAOS_SET_UNIFORM_IMPLEMENTATION(glm::tvec3<double>);
		CHAOS_SET_UNIFORM_IMPLEMENTATION(glm::tvec4<double>);
		CHAOS_SET_UNIFORM_IMPLEMENTATION(glm::tvec2<int>);
		CHAOS_SET_UNIFORM_IMPLEMENTATION(glm::tvec3<int>);
		CHAOS_SET_UNIFORM_IMPLEMENTATION(glm::tvec4<int>);
		CHAOS_SET_UNIFORM_IMPLEMENTATION(glm::tvec2<unsigned int>);
		CHAOS_SET_UNIFORM_IMPLEMENTATION(glm::tvec3<unsigned int>);
		CHAOS_SET_UNIFORM_IMPLEMENTATION(glm::tvec4<unsigned int>);

#undef CHAOS_SET_UNIFORM_IMPLEMENTATION

	protected:

		/** generic method to set uniforms from a vector */
		template<int ARITY, typename PARAMETER_TYPE>
		bool SetUniformImpl(GPUUniformInfo const& uniform_info, glm::vec<ARITY, PARAMETER_TYPE> const& value) const
		{
			if constexpr (std::is_same_v<VECTOR_TYPE, glm::vec<ARITY, PARAMETER_TYPE>>)
			{
				GLTools::SetUniform(uniform_info.location, value);
			}
			else
			{
				VECTOR_TYPE converted_vector = RecastVector<VECTOR_TYPE>(value);
				GLTools::SetUniform(uniform_info.location, converted_vector);
			}
			return true;
		}

		/** generic method to set uniforms from a scalar */
		template<typename PARAMETER_TYPE>
		bool SetUniformImpl(GPUUniformInfo const& uniform_info, PARAMETER_TYPE const& value) const
		{
			glm::vec<1, PARAMETER_TYPE> value_as_vec1 = glm::vec<1, PARAMETER_TYPE>(value);
			VECTOR_TYPE converted_vector = RecastVector<VECTOR_TYPE>(value_as_vec1);
			GLTools::SetUniform(uniform_info.location, converted_vector);
			return true;
		}
	};

	/**
	 * GPUUniformMatrixSetter: a uniform setter dedicated for matrixes
	 */

	template<typename MATRIX_TYPE>
	class GPUUniformMatrixSetter : public GPUUniformSetter
	{
	public:

		/** overrides */
#define CHAOS_SET_UNIFORM_IMPLEMENTATION(parameter_type)\
		virtual bool SetUniform(GPUUniformInfo const & uniform_info, parameter_type const& value) const override\
		{\
			return SetUniformImpl(uniform_info, value);\
		}

		CHAOS_SET_UNIFORM_IMPLEMENTATION(glm::mat2);
		CHAOS_SET_UNIFORM_IMPLEMENTATION(glm::mat3);
		CHAOS_SET_UNIFORM_IMPLEMENTATION(glm::mat4);
		CHAOS_SET_UNIFORM_IMPLEMENTATION(glm::mat2x3);
		CHAOS_SET_UNIFORM_IMPLEMENTATION(glm::mat2x4);
		CHAOS_SET_UNIFORM_IMPLEMENTATION(glm::mat3x2);
		CHAOS_SET_UNIFORM_IMPLEMENTATION(glm::mat3x4);
		CHAOS_SET_UNIFORM_IMPLEMENTATION(glm::mat4x2);
		CHAOS_SET_UNIFORM_IMPLEMENTATION(glm::mat4x3);
		CHAOS_SET_UNIFORM_IMPLEMENTATION(glm::dmat2);
		CHAOS_SET_UNIFORM_IMPLEMENTATION(glm::dmat3);
		CHAOS_SET_UNIFORM_IMPLEMENTATION(glm::dmat4);
		CHAOS_SET_UNIFORM_IMPLEMENTATION(glm::dmat2x3);
		CHAOS_SET_UNIFORM_IMPLEMENTATION(glm::dmat2x4);
		CHAOS_SET_UNIFORM_IMPLEMENTATION(glm::dmat3x2);
		CHAOS_SET_UNIFORM_IMPLEMENTATION(glm::dmat3x4);
		CHAOS_SET_UNIFORM_IMPLEMENTATION(glm::dmat4x2);
		CHAOS_SET_UNIFORM_IMPLEMENTATION(glm::dmat4x3);

#undef CHAOS_SET_UNIFORM_IMPLEMENTATION

	protected:

		/** generic method to set uniforms from a matrix */
		template<typename PARAMETER_TYPE>
		bool SetUniformImpl(GPUUniformInfo const& uniform_info, PARAMETER_TYPE const& value) const
		{
			if constexpr (std::is_same_v<MATRIX_TYPE, PARAMETER_TYPE>)
			{
				GLTools::SetUniform(uniform_info.location, value);
			}
			else
			{
				MATRIX_TYPE converted_matrix(value);
				GLTools::SetUniform(uniform_info.location, converted_matrix);
			}
			return true;
		}
	};

	/**
	 * GPUUniformSamplerSetter: a uniform setter dedicated for samplers
	 */

	class GPUUniformSamplerSetter : public GPUUniformSetter
	{
	public:

		/** override */
		virtual bool SetUniform(GPUUniformInfo const& uniform_info, GPUTexture const* texture) const override;
	};

#endif

}; // namespace chaos