#pragma once

#include <chaos/StandardHeaders.h>
#include <chaos/MathTools.h>

namespace chaos
{
	// ==============================================================================================
	// Some common typedefs (outside of 'namespace' GLMTools)
	// ==============================================================================================

	using float2 = glm::vec2;
	using float3 = glm::vec3;
	using float4 = glm::vec4;

	using int2 = glm::ivec2;
	using int3 = glm::ivec3;
	using int4 = glm::ivec4;

	// ==============================================================================================
	// GLMTools
	// ==============================================================================================

	class GLMTools
	{
	public:

		/** template meta programming functions to detect whether a type is a GLM vector */
		template<typename T> class IsVectorType : public boost::mpl::false_ {};

		template<typename T> class IsVectorType<glm::tvec1<T>> : public boost::mpl::true_ {};
		template<typename T> class IsVectorType<glm::tvec2<T>> : public boost::mpl::true_ {};
		template<typename T> class IsVectorType<glm::tvec3<T>> : public boost::mpl::true_ {};
		template<typename T> class IsVectorType<glm::tvec4<T>> : public boost::mpl::true_ {};

		/** template meta programming functions to detect whether a type is a GLM matrix */
		template<typename T> class IsMatrixType : public boost::mpl::false_ {};

		template<> class IsMatrixType<glm::mat2> : public boost::mpl::true_ {};
		template<> class IsMatrixType<glm::mat3> : public boost::mpl::true_ {};
		template<> class IsMatrixType<glm::mat4> : public boost::mpl::true_ {};
		template<> class IsMatrixType<glm::mat2x3> : public boost::mpl::true_ {};
		template<> class IsMatrixType<glm::mat2x4> : public boost::mpl::true_ {};
		template<> class IsMatrixType<glm::mat3x2> : public boost::mpl::true_ {};
		template<> class IsMatrixType<glm::mat3x4> : public boost::mpl::true_ {};
		template<> class IsMatrixType<glm::mat4x2> : public boost::mpl::true_ {};
		template<> class IsMatrixType<glm::mat4x3> : public boost::mpl::true_ {};

		template<> class IsMatrixType<glm::dmat2> : public boost::mpl::true_ {};
		template<> class IsMatrixType<glm::dmat3> : public boost::mpl::true_ {};
		template<> class IsMatrixType<glm::dmat4> : public boost::mpl::true_ {};
		template<> class IsMatrixType<glm::dmat2x3> : public boost::mpl::true_ {};
		template<> class IsMatrixType<glm::dmat2x4> : public boost::mpl::true_ {};
		template<> class IsMatrixType<glm::dmat3x2> : public boost::mpl::true_ {};
		template<> class IsMatrixType<glm::dmat3x4> : public boost::mpl::true_ {};
		template<> class IsMatrixType<glm::dmat4x2> : public boost::mpl::true_ {};
		template<> class IsMatrixType<glm::dmat4x3> : public boost::mpl::true_ {};

	public:

		/** cast a vector into another vector (arity and type may change) */
		template<typename VECTOR_TYPE1, typename VECTOR_TYPE2>
		static VECTOR_TYPE1 RecastVector(VECTOR_TYPE2 const & value)
		{
			VECTOR_TYPE1 result(0);
			size_t count = std::min(result.length(), value.length());
			for (size_t i = 0; i < count; ++i)
				result[i] = static_cast<VECTOR_TYPE1::value_type>(value[i]);

			return result;
		}

		/** returns the min component of a vector */
		template<typename T>
		static T GetMinComponent(glm::tvec1<T> const & src)
		{
			return src.x;
		}

		/** returns the min component of a vector */
		template<typename T>
		static T GetMinComponent(glm::tvec2<T> const & src)
		{
			return glm::min(src.x, src.y);
		}

		/** returns the min component of a vector */
		template<typename T>
		static T GetMinComponent(glm::tvec3<T> const & src)
		{
			return glm::min(src.x, glm::min(src.y, src.z));
		}

		/** returns the min component of a vector */
		template<typename T>
		static T GetMinComponent(glm::tvec4<T> const & src)
		{
			return glm::min(glm::min(src.x, src.y), glm::min(src.z, src.w));
		}

		/** returns the max component of a vector */
		template<typename T>
		static T GetMaxComponent(glm::tvec1<T> const & src)
		{
			return src.x;
		}

		/** returns the max component of a vector */
		template<typename T>
		static T GetMaxComponent(glm::tvec2<T> const & src)
		{
			return glm::max(src.x, src.y);
		}

		/** returns the max component of a vector */
		template<typename T>
		static T GetMaxComponent(glm::tvec3<T> const & src)
		{
			return glm::max(src.x, glm::max(src.y, src.z));
		}

		/** returns the max component of a vector */
		template<typename T>
		static T GetMaxComponent(glm::tvec4<T> const & src)
		{
			return glm::max(glm::max(src.x, src.y), glm::max(src.z, src.w));
		}

		/** returns a random vec1 */
		static glm::tvec1<float> RandVec1()
		{
			return glm::tvec1<float>(MathTools::RandFloat());
		}

		/** returns a random vec2 */
		static glm::vec2 RandVec2()
		{
			return glm::vec2(MathTools::RandFloat(), MathTools::RandFloat());
		}

		/** returns a random vec3 */
		static glm::vec3 RandVec3()
		{
			return glm::vec3(MathTools::RandFloat(), MathTools::RandFloat(), MathTools::RandFloat());
		}
		/** returns a random vec4 */
		static glm::vec4 RandVec4()
		{
			return glm::vec4(MathTools::RandFloat(), MathTools::RandFloat(), MathTools::RandFloat(), MathTools::RandFloat());
		}


		/** convert argument into a glm::vector, returns value remains unchanged if it is already a vector */
		template<typename T>
		static glm::tvec1<T> const & ConvertIntoVector(glm::tvec1<T> const & value)
		{
			return value;
		}
		/** convert argument into a glm::vector, returns value remains unchanged if it is already a vector */
		template<typename T>
		static glm::tvec2<T> const & ConvertIntoVector(glm::tvec2<T> const & value)
		{
			return value;
		}
		/** convert argument into a glm::vector, returns value remains unchanged if it is already a vector */
		template<typename T>
		static glm::tvec3<T> const & ConvertIntoVector(glm::tvec3<T> const & value)
		{
			return value;
		}
		/** convert argument into a glm::vector, returns value remains unchanged if it is already a vector */
		template<typename T>
		static glm::tvec4<T> const & ConvertIntoVector(glm::tvec4<T> const & value)
		{
			return value;
		}
		/** convert argument into a glm::vector */
		template<typename T>
		static glm::tvec1<T> ConvertIntoVector(T value)
		{
			return glm::tvec1<T>(value);
		}


		/** Compute simplified cross for a 2d vector */
		template<typename T>
		static T Get2DCrossProductZ(glm::tvec2<T> const & A, glm::tvec2<T> const & B)
		{
			return (A.x * B.y) - (A.y * B.x);
		}

		/** Rotation of a vector (with Cos and Sin already computed) */
		static glm::vec2 Rotate(glm::vec2 const & src, float c, float s)
		{
			// copy form glm/gtx/rotate_vector.inl
			// here we avoid multiple computation of cosinus and sinus

			glm::vec2 result;
			result.x = src.x * c - src.y * s;
			result.y = src.x * s + src.y * c;			
			return result;
		}

		/** multiply a 2D vector by a 4x4 matrix WITHOUT translation applyed */
		template<typename T>
		static auto Mult(glm::tmat4x4<T> const & m, glm::tvec2<T> const & v)
		{
			return glm::tvec2<T>(m * glm::tvec4<T>(v, (T)0.0f, (T)0.0f));
		}

		/** multiply a 3D vector by a 4x4 matrix WITHOUT translation applyed */
		template<typename T>
		static auto Mult(glm::tmat4x4<T> const & m, glm::tvec3<T> const & v)
		{
			return glm::tvec3<T>(m * glm::tvec4<T>(v, (T)0.0f));
		}

		/** multiply a 2D vector by a 4x4 matrix WITH translation applyed */
		template<typename T>
		static auto MultWithTranslation(glm::tmat4x4<T> const & m, glm::tvec2<T> const & v)
		{
			return glm::tvec2<T>(m * glm::tvec4<T>(v, (T)0.0f, (T)1.0f));
		}

		/** multiply a 3D vector by a 4x4 matrix WITH translation applyed */
		template<typename T>
		static auto MultWithTranslation(glm::tmat4x4<T> const & m, glm::tvec3<T> const & v)
		{
			return glm::tvec3<T>(m * glm::tvec4<T>(v, (T)1.0f));
		}

	};



}; // namespace chaos

