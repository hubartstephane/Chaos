#ifdef CHAOS_FORWARD_DECLARATION

namespace chaos
{
	class GLTools;

}; // namespace chaos

#else 

namespace chaos
{

	/**
	* GLTools : used to have some generic functions for OpenGL
	**/

	class GLTools
	{
	public:

		/** Set the viewport with an aspect */
		static box2 SetViewportWithAspect(glm::ivec2 const & size, float aspect);

		/** set the viewport */
		template<typename T>
		static void SetViewport(type_box<T, 2> const & viewport)
		{
			auto corners = GetBoxCorners(viewport);
			glViewport(
				(GLint)corners.first.x, 
				(GLint)corners.first.y, 
				2 * (GLsizei)(viewport.half_size.x), 
				2 * (GLsizei)(viewport.half_size.y));
		}

		/** display generic information on the OpenGL driver */
		static void DisplayGenericInformation();
		/** transform a type enumeration into a string */
		static char const * GLenumToString(GLenum type);
		/** Initialize the Debug message handler */
		static void SetDebugMessageHandler();

		/** an utility function to 'compute' offset in vertex buffer (simple cast in facts) */
		static inline void * GetOffset(size_t s)
		{
			return ((char *)nullptr) + s;
		}

		/** settings uniforms */   
		static void SetUniform(GLint location, glm::mat2x3 const & value){ glUniformMatrix2x3fv(location, 1, false, (GLfloat const *)&value);}
		static void SetUniform(GLint location, glm::mat2x4 const & value){ glUniformMatrix2x4fv(location, 1, false, (GLfloat const *)&value);}
		static void SetUniform(GLint location, glm::mat3x2 const & value){ glUniformMatrix3x2fv(location, 1, false, (GLfloat const *)&value);}
		static void SetUniform(GLint location, glm::mat3x4 const & value){ glUniformMatrix3x4fv(location, 1, false, (GLfloat const *)&value);}
		static void SetUniform(GLint location, glm::mat4x2 const & value){ glUniformMatrix4x2fv(location, 1, false, (GLfloat const *)&value);}
		static void SetUniform(GLint location, glm::mat4x3 const & value){ glUniformMatrix4x3fv(location, 1, false, (GLfloat const *)&value);}
		static void SetUniform(GLint location, glm::mat2   const & value){ glUniformMatrix2fv(location, 1, false, (GLfloat const *)&value);}
		static void SetUniform(GLint location, glm::mat3   const & value){ glUniformMatrix3fv(location, 1, false, (GLfloat const *)&value);}
		static void SetUniform(GLint location, glm::mat4   const & value){ glUniformMatrix4fv(location, 1, false, (GLfloat const *)&value);}

		static void SetUniform(GLint location, glm::dmat2x3 const & value){ glUniformMatrix2x3dv(location, 1, false, (GLdouble const *)&value);}
		static void SetUniform(GLint location, glm::dmat2x4 const & value){ glUniformMatrix2x4dv(location, 1, false, (GLdouble const *)&value);}
		static void SetUniform(GLint location, glm::dmat3x2 const & value){ glUniformMatrix3x2dv(location, 1, false, (GLdouble const *)&value);}
		static void SetUniform(GLint location, glm::dmat3x4 const & value){ glUniformMatrix3x4dv(location, 1, false, (GLdouble const *)&value);}
		static void SetUniform(GLint location, glm::dmat4x2 const & value){ glUniformMatrix4x2dv(location, 1, false, (GLdouble const *)&value);}
		static void SetUniform(GLint location, glm::dmat4x3 const & value){ glUniformMatrix4x3dv(location, 1, false, (GLdouble const *)&value);}
		static void SetUniform(GLint location, glm::dmat2   const & value){ glUniformMatrix2dv(location, 1, false, (GLdouble const *)&value);}
		static void SetUniform(GLint location, glm::dmat3   const & value){ glUniformMatrix3dv(location, 1, false, (GLdouble const *)&value);}
		static void SetUniform(GLint location, glm::dmat4   const & value){ glUniformMatrix4dv(location, 1, false, (GLdouble const *)&value);}

		static void SetUniform(GLint location, glm::tvec1<GLfloat> const & value){ glUniform1fv(location, 1, (GLfloat const *)&value);}
		static void SetUniform(GLint location, glm::tvec2<GLfloat> const & value){ glUniform2fv(location, 1, (GLfloat const *)&value);}
		static void SetUniform(GLint location, glm::tvec3<GLfloat> const & value){ glUniform3fv(location, 1, (GLfloat const *)&value);}
		static void SetUniform(GLint location, glm::tvec4<GLfloat> const & value){ glUniform4fv(location, 1, (GLfloat const *)&value);}

		static void SetUniform(GLint location, glm::tvec1<GLdouble> const & value){ glUniform1dv(location, 1, (GLdouble const *)&value);}
		static void SetUniform(GLint location, glm::tvec2<GLdouble> const & value){ glUniform2dv(location, 1, (GLdouble const *)&value);}
		static void SetUniform(GLint location, glm::tvec3<GLdouble> const & value){ glUniform3dv(location, 1, (GLdouble const *)&value);}
		static void SetUniform(GLint location, glm::tvec4<GLdouble> const & value){ glUniform4dv(location, 1, (GLdouble const *)&value);}

		static void SetUniform(GLint location, glm::tvec1<GLint> const & value){ glUniform1iv(location, 1, (GLint const *)&value);}
		static void SetUniform(GLint location, glm::tvec2<GLint> const & value){ glUniform2iv(location, 1, (GLint const *)&value);}
		static void SetUniform(GLint location, glm::tvec3<GLint> const & value){ glUniform3iv(location, 1, (GLint const *)&value);}
		static void SetUniform(GLint location, glm::tvec4<GLint> const & value){ glUniform4iv(location, 1, (GLint const *)&value);}

		static void SetUniform(GLint location, glm::tvec1<GLuint> const & value){ glUniform1uiv(location, 1, (GLuint const *)&value);}
		static void SetUniform(GLint location, glm::tvec2<GLuint> const & value){ glUniform2uiv(location, 1, (GLuint const *)&value);}
		static void SetUniform(GLint location, glm::tvec3<GLuint> const & value){ glUniform3uiv(location, 1, (GLuint const *)&value);}
		static void SetUniform(GLint location, glm::tvec4<GLuint> const & value){ glUniform4uiv(location, 1, (GLuint const *)&value);}

		static void SetUniform(GLint location, GLfloat  value){ glUniform1f (location, value);}
		static void SetUniform(GLint location, GLdouble value){ glUniform1d (location, value);}
		static void SetUniform(GLint location, GLint    value){ glUniform1i (location, value);}
		static void SetUniform(GLint location, GLuint   value){ glUniform1ui(location, value);}

		/** settings default attribute values */
		static void SetVertexAttrib(GLint location, GLfloat value){ glVertexAttrib1f(location, value);}
		static void SetVertexAttrib(GLint location, glm::tvec2<GLfloat> const & value){ glVertexAttrib2fv(location, (GLfloat const *)&value);}
		static void SetVertexAttrib(GLint location, glm::tvec3<GLfloat> const & value){ glVertexAttrib3fv(location, (GLfloat const *)&value);}
		static void SetVertexAttrib(GLint location, glm::tvec4<GLfloat> const & value){ glVertexAttrib4fv(location, (GLfloat const *)&value);}

		static void SetVertexAttrib(GLint location, GLdouble value){ glVertexAttrib1d(location, value);}
		static void SetVertexAttrib(GLint location, glm::tvec2<GLdouble> const & value){ glVertexAttrib2dv(location, (GLdouble const *)&value);}
		static void SetVertexAttrib(GLint location, glm::tvec3<GLdouble> const & value){ glVertexAttrib3dv(location, (GLdouble const *)&value);}
		static void SetVertexAttrib(GLint location, glm::tvec4<GLdouble> const & value){ glVertexAttrib4dv(location, (GLdouble const *)&value);}

		static void SetVertexAttrib(GLint location, GLshort value){ glVertexAttrib1s(location, value);}
		static void SetVertexAttrib(GLint location, glm::tvec2<GLshort> const & value){ glVertexAttrib2sv(location, (GLshort const *)&value);}
		static void SetVertexAttrib(GLint location, glm::tvec3<GLshort> const & value){ glVertexAttrib3sv(location, (GLshort const *)&value);}
		static void SetVertexAttrib(GLint location, glm::tvec4<GLshort> const & value){ glVertexAttrib4sv(location, (GLshort const *)&value);}

		// in OpenGL, for some types (GLint, GLbyte, GLshort, GLushort), there are only definition for vec4 */
		static void SetVertexAttrib(GLint location, glm::tvec4<GLint>    const & value){ glVertexAttrib4iv (location, (GLint const *)&value);}
		static void SetVertexAttrib(GLint location, glm::tvec4<GLbyte>   const & value){ glVertexAttrib4bv (location, (GLbyte const *)&value);}
		static void SetVertexAttrib(GLint location, glm::tvec4<GLubyte>  const & value){ glVertexAttrib4ubv(location, (GLubyte const *)&value);}
		static void SetVertexAttrib(GLint location, glm::tvec4<GLushort> const & value){ glVertexAttrib4usv(location, (GLushort const *)&value);}


		/** returns true whether the enum is a matrix type */
		static bool IsMatrixType(GLenum type);
		/** returns true whether the enum is a sampler type */
		static bool IsSamplerType(GLenum type);
		/** returns the arity of the vector whether the enum is a vector type */
		static int GetEnumVectorArity(GLenum type);
		/** returns the arity of the vector whether the enum is a (bool) vector type */
		static int GetEnumVectorArityBool(GLenum type);
		/** returns the arity of the vector whether the enum is a (float) vector type */
		static int GetEnumVectorArityFloat(GLenum type);
		/** returns the arity of the vector whether the enum is a (double) vector type */
		static int GetEnumVectorArityDouble(GLenum type);
		/** returns the arity of the vector whether the enum is a (int) vector type */
		static int GetEnumVectorArityInt(GLenum type);
		/** returns the arity of the vector whether the enum is a (unsigned int) vector type */
		static int GetEnumVectorArityUnsignedInt(GLenum type);

		/** default template for enum vector arity */
		template<typename T>
		static int GetTypedEnumVectorArity(GLenum type)
		{
			if constexpr (std::is_same_v<T, GLfloat>)
				return GetEnumVectorArityFloat(type);
			if constexpr (std::is_same_v<T, GLdouble>)
				return GetEnumVectorArityDouble(type);
			if constexpr (std::is_same_v<T, GLint>)
				return GetEnumVectorArityInt(type);
			if constexpr (std::is_same_v<T, GLuint>)
				return GetEnumVectorArityUnsignedInt(type);
			return 0;
		}
	};

}; // namespace chaos

#endif // CHAOS_FORWARD_DECLARATION
