#include "Ludum49PCH.h"
#include "Ludum49Game.h"



using namespace chaos;




class GPUUniformSetter
{
public:

	static GPUUniformSetter const* GetUniformSetterForType(GLenum type);

#define CHAOS_SET_UNIFORM_IMPLEMENTATION(parameter_type)\
	virtual bool SetUniform(GPUUniformInfo const & uniform_info, parameter_type const& value) const\
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
};




template<typename SCALAR_TYPE>
class GPUUniformScalarSetter : public GPUUniformSetter
{
public:

#define CHAOS_SET_UNIFORM_IMPLEMENTATION(parameter_type)\
	virtual bool SetUniform(GPUUniformInfo const & uniform_info, parameter_type const& value) const override\
	{\
		if constexpr (std::is_same_v<SCALAR_TYPE, parameter_type>)\
		{\
			GLTools::SetUniform(uniform_info.location, value);\
		}\
		else\
		{\
			SCALAR_TYPE converted_scalar = SCALAR_TYPE(value);\
			GLTools::SetUniform(uniform_info.location, converted_scalar);\
		}\
		return true;\
	}

	CHAOS_SET_UNIFORM_IMPLEMENTATION(bool);
	CHAOS_SET_UNIFORM_IMPLEMENTATION(float);
	CHAOS_SET_UNIFORM_IMPLEMENTATION(double);
	CHAOS_SET_UNIFORM_IMPLEMENTATION(int);
	CHAOS_SET_UNIFORM_IMPLEMENTATION(unsigned int);

#undef CHAOS_SET_UNIFORM_IMPLEMENTATION
};

template<typename VECTOR_TYPE>
class GPUUniformVectorSetter : public GPUUniformSetter
{
public:

#define CHAOS_SET_UNIFORM_IMPLEMENTATION(parameter_type)\
	virtual bool SetUniform(GPUUniformInfo const & uniform_info, parameter_type const& value) const override\
	{\
		if constexpr (std::is_same_v<VECTOR_TYPE, parameter_type>)\
		{\
			GLTools::SetUniform(uniform_info.location, value);\
		}\
		else\
		{\
			VECTOR_TYPE converted_vector = RecastVector<VECTOR_TYPE>(value);\
			GLTools::SetUniform(uniform_info.location, converted_vector);\
		}\
		return true;\
	}

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

};

template<typename MATRIX_TYPE>
class GPUUniformMatrixSetter : public GPUUniformSetter
{
public:

#define CHAOS_SET_UNIFORM_IMPLEMENTATION(parameter_type)\
	virtual bool SetUniform(GPUUniformInfo const & uniform_info, parameter_type const& value) const override\
	{\
		if constexpr (std::is_same_v<MATRIX_TYPE, parameter_type>)\
		{\
			GLTools::SetUniform(uniform_info.location, value);\
		}\
		else\
		{\
			MATRIX_TYPE converted_matrix(value);\
			GLTools::SetUniform(uniform_info.location, converted_matrix);\
		}\
		return true;\
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
};
















GPUUniformSetter const* GPUUniformSetter::GetUniformSetterForType(GLenum type)
{
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

#define CHAOS_GET_UNIFORM_SCALAR_SETTER(enum_type, scalar_type)\
	if (type == enum_type)\
		return Singleton<GPUUniformScalarSetter<scalar_type>>::GetInstance();

	CHAOS_GET_UNIFORM_SCALAR_SETTER(GL_BOOL, bool);
	CHAOS_GET_UNIFORM_SCALAR_SETTER(GL_FLOAT, float);
	CHAOS_GET_UNIFORM_SCALAR_SETTER(GL_DOUBLE, double);
	CHAOS_GET_UNIFORM_SCALAR_SETTER(GL_INT, int);
	CHAOS_GET_UNIFORM_SCALAR_SETTER(GL_UNSIGNED_INT, unsigned int);

#undef CHAOS_GET_UNIFORM_SCALAR_SETTER

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

	return nullptr;
}

int main(int argc, char ** argv, char ** env)
{
	auto x = Singleton<GPUUniformMatrixSetter<glm::mat2>>::GetInstance();

	return RunGame<LudumGame>(argc, argv, env);
}