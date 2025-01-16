#include "chaos/ChaosPCH.h"
#include "chaos/ChaosInternals.h"

namespace chaos
{
	GPUUniformSetter const* GPUUniformSetter::GetUniformSetterForType(GLenum type)
	{
		// matrix setters
#define CHAOS_GET_UNIFORM_MATRIX_SETTER(enum_type, matrix_type)\
		if (type == enum_type)\
			return Singleton<GPUUniformMatrixSetter<matrix_type>>::GetInstance();

		CHAOS_GET_UNIFORM_MATRIX_SETTER(GL_FLOAT_MAT2, glm::mat2);
		CHAOS_GET_UNIFORM_MATRIX_SETTER(GL_FLOAT_MAT3, glm::mat3);
		CHAOS_GET_UNIFORM_MATRIX_SETTER(GL_FLOAT_MAT4, glm::mat4);
		CHAOS_GET_UNIFORM_MATRIX_SETTER(GL_FLOAT_MAT2x3, glm::mat2x3);
		CHAOS_GET_UNIFORM_MATRIX_SETTER(GL_FLOAT_MAT2x4, glm::mat2x4);
		CHAOS_GET_UNIFORM_MATRIX_SETTER(GL_FLOAT_MAT3x2, glm::mat3x2);
		CHAOS_GET_UNIFORM_MATRIX_SETTER(GL_FLOAT_MAT3x4, glm::mat3x4);
		CHAOS_GET_UNIFORM_MATRIX_SETTER(GL_FLOAT_MAT4x2, glm::mat4x2);
		CHAOS_GET_UNIFORM_MATRIX_SETTER(GL_FLOAT_MAT4x3, glm::mat4x3);

		CHAOS_GET_UNIFORM_MATRIX_SETTER(GL_DOUBLE_MAT2, glm::dmat2);
		CHAOS_GET_UNIFORM_MATRIX_SETTER(GL_DOUBLE_MAT3, glm::dmat3);
		CHAOS_GET_UNIFORM_MATRIX_SETTER(GL_DOUBLE_MAT4, glm::dmat4);
		CHAOS_GET_UNIFORM_MATRIX_SETTER(GL_DOUBLE_MAT2x3, glm::dmat2x3);
		CHAOS_GET_UNIFORM_MATRIX_SETTER(GL_DOUBLE_MAT2x4, glm::dmat2x4);
		CHAOS_GET_UNIFORM_MATRIX_SETTER(GL_DOUBLE_MAT3x2, glm::dmat3x2);
		CHAOS_GET_UNIFORM_MATRIX_SETTER(GL_DOUBLE_MAT3x4, glm::dmat3x4);
		CHAOS_GET_UNIFORM_MATRIX_SETTER(GL_DOUBLE_MAT4x2, glm::dmat4x2);
		CHAOS_GET_UNIFORM_MATRIX_SETTER(GL_DOUBLE_MAT4x3, glm::dmat4x3);

#undef CHAOS_GET_UNIFORM_MATRIX_SETTER

		// scalar setters
#define CHAOS_GET_UNIFORM_SCALAR_SETTER(enum_type, scalar_type)\
		if (type == enum_type)\
			return Singleton<GPUUniformScalarSetter<scalar_type>>::GetInstance();

		CHAOS_GET_UNIFORM_SCALAR_SETTER(GL_BOOL, bool);
		CHAOS_GET_UNIFORM_SCALAR_SETTER(GL_FLOAT, float);
		CHAOS_GET_UNIFORM_SCALAR_SETTER(GL_DOUBLE, double);
		CHAOS_GET_UNIFORM_SCALAR_SETTER(GL_INT, int);
		CHAOS_GET_UNIFORM_SCALAR_SETTER(GL_UNSIGNED_INT, unsigned int);

#undef CHAOS_GET_UNIFORM_SCALAR_SETTER

		// uniform setters
#define CHAOS_GET_UNIFORM_VECTOR_SETTER(enum_type, vector_type)\
		if (type == enum_type)\
			return Singleton<GPUUniformVectorSetter<vector_type>>::GetInstance();

		CHAOS_GET_UNIFORM_VECTOR_SETTER(GL_BOOL_VEC2, glm::tvec2<bool>);
		CHAOS_GET_UNIFORM_VECTOR_SETTER(GL_BOOL_VEC3, glm::tvec3<bool>);
		CHAOS_GET_UNIFORM_VECTOR_SETTER(GL_BOOL_VEC4, glm::tvec4<bool>);

		CHAOS_GET_UNIFORM_VECTOR_SETTER(GL_FLOAT_VEC2, glm::tvec2<float>);
		CHAOS_GET_UNIFORM_VECTOR_SETTER(GL_FLOAT_VEC3, glm::tvec3<float>);
		CHAOS_GET_UNIFORM_VECTOR_SETTER(GL_FLOAT_VEC4, glm::tvec4<float>);

		CHAOS_GET_UNIFORM_VECTOR_SETTER(GL_DOUBLE_VEC2, glm::tvec2<double>);
		CHAOS_GET_UNIFORM_VECTOR_SETTER(GL_DOUBLE_VEC3, glm::tvec3<double>);
		CHAOS_GET_UNIFORM_VECTOR_SETTER(GL_DOUBLE_VEC4, glm::tvec4<double>);

		CHAOS_GET_UNIFORM_VECTOR_SETTER(GL_INT_VEC2, glm::tvec2<int>);
		CHAOS_GET_UNIFORM_VECTOR_SETTER(GL_INT_VEC3, glm::tvec3<int>);
		CHAOS_GET_UNIFORM_VECTOR_SETTER(GL_INT_VEC4, glm::tvec4<int>);

		CHAOS_GET_UNIFORM_VECTOR_SETTER(GL_UNSIGNED_INT_VEC2, glm::tvec2<unsigned int>);
		CHAOS_GET_UNIFORM_VECTOR_SETTER(GL_UNSIGNED_INT_VEC3, glm::tvec3<unsigned int>);
		CHAOS_GET_UNIFORM_VECTOR_SETTER(GL_UNSIGNED_INT_VEC4, glm::tvec4<unsigned int>);

#undef CHAOS_GET_UNIFORM_VECTOR_SETTER

		// sampler setters
		if (GLTools::IsSamplerType(type))
			return Singleton<GPUUniformSamplerSetter>::GetInstance();

		return nullptr;
	}

	bool GPUUniformSetter::SetUniform(GPUUniformInfo const& uniform_info, GPUTexture const* texture) const
	{
		return false;
	}

#define CHAOS_SET_UNIFORM_IMPLEMENTATION(parameter_type)\
	bool GPUUniformSetter::SetUniform(GPUUniformInfo const & uniform_info, parameter_type const& value) const\
	{\
		return false;\
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

	bool GPUUniformSamplerSetter::SetUniform(GPUUniformInfo const& uniform_info, GPUTexture const* texture) const
	{
		glBindTextureUnit(uniform_info.sampler_index, texture->GetResourceID());
		glUniform1i(uniform_info.location, uniform_info.sampler_index);
		return true;
	}

}; // namespace chaos
