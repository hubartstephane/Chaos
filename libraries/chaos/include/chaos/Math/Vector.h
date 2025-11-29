#ifdef CHAOS_FORWARD_DECLARATION

namespace chaos
{
	template<typename VECTOR_TYPE>
	class AutoCastableVector;

}; // namespace chaos

#elif !defined CHAOS_TEMPLATE_IMPLEMENTATION

namespace chaos
{
	/** cast a vector into another vector (arity and type may change) */
	template<typename VECTOR_TYPE1, typename VECTOR_TYPE2>
	VECTOR_TYPE1 RecastVector(VECTOR_TYPE2 const& value)
	{
		VECTOR_TYPE1 result(0);
		size_t count = std::min(result.length(), value.length());
		for (size_t i = 0; i < count; ++i)
			result[i] = static_cast<VECTOR_TYPE1::value_type>(value[i]);

		return result;
	}

	template<typename VECTOR_TYPE>
	class AutoCastableVector
	{
	public:

		/** the constructors */
		AutoCastableVector() = default;
		AutoCastableVector(AutoCastableVector const& src) = default;
		AutoCastableVector(VECTOR_TYPE const& in_src) : src(in_src) {}

		/** 'cast' function */
		template<typename VECTOR_DST_TYPE>
		operator VECTOR_DST_TYPE () const
		{
			VECTOR_DST_TYPE result(0);
			size_t count = std::min(result.length(), src.length());
			for (size_t i = 0; i < count; ++i)
				result[i] = static_cast<VECTOR_DST_TYPE::value_type>(src[i]);
			return result;
		}

	protected:

		/** the src vector that will be 'casted' on demand */
		VECTOR_TYPE src;
	};

	/** create a delayed 'cast' */
	template<typename T>
	AutoCastableVector<T> auto_cast_vector(AutoCastableVector<T> const& src) { return src; }

	/** create a delayed 'cast' */
	template<typename T>
	AutoCastableVector<T> auto_cast_vector(T const& src) { return src; }


	template<std::floating_point T, int plane_dimension> requires (plane_dimension == 3 || plane_dimension == 4)
	glm::vec<plane_dimension - 1, T> GetPlaneNormal(glm::vec<plane_dimension, T> const& src)
	{
		return *(glm::vec<plane_dimension - 1, T>*)&src;
	}

	template<std::floating_point T, int plane_dimension> requires (plane_dimension == 3 || plane_dimension == 4)
	T GetPlaneOffset(glm::vec<plane_dimension, T> const& src)
	{
		return src[plane_dimension - 1];
	}

	template<std::floating_point T, int plane_dimension> requires (plane_dimension == 3 || plane_dimension == 4)
	void NormalizePlane(glm::vec<plane_dimension, T> & src)
	{
		 auto normal = GetPlaneNormal(src);
		 T len = glm::length2(normal);
		 if (len != T(0) && len != T(1))
			 src /= std::sqrt(len);
	}

	template<std::floating_point T, int plane_dimension> requires (plane_dimension == 3 || plane_dimension == 4)
	glm::vec<plane_dimension, T> GetNormalizedPlane(glm::vec<plane_dimension, T> const & src)
	{
		auto normal = GetPlaneNormal(src);
		T len = glm::length2(normal);
		if (len != T(0) && len != T(1))
			return src / std::sqrt(len);
		return src;
	}

}; // namespace chaos

	// XXX: for namespace resolution in JSONTools::LoadFromJSON(...) and JSONTools::SaveIntoJSON(...),
	//      it is important that the following functions belong to the same namespace than the target (glm)

namespace glm
{
	template<int DIMENSION, typename T> requires (DIMENSION >= 1 && DIMENSION <= 4)
	void DrawImGuiVariableImpl(glm::vec<DIMENSION, T>& value, chaos::DrawImGuiVariableFlags flags = chaos::DrawImGuiVariableFlags::NONE)
	{
		// ImGui does not provide a InputDoubleX method, that's why we are using InputScalarN here instead of (InputIntX and InputFloatX)
		// (InputScalarN is easier because the size can be given has an argument)	
		if constexpr (std::is_same_v<T, int>)
			ImGui::InputScalarN("", ImGuiDataType_S32, &value, DIMENSION, NULL, NULL, "%d", 0);
		else if constexpr (std::is_same_v<T, float>)
		{
			if (HasAnyFlags(flags, chaos::DrawImGuiVariableFlags::COLOR)) // special case for colors
			{
				if (DIMENSION == 3)
				{
					ImGui::ColorEdit3("", &value.x, 0);
					return;
				}
				if (DIMENSION == 4)
				{
					ImGui::ColorEdit4("", &value.x, 0);
					return;
				}
			}
			ImGui::InputScalarN("", ImGuiDataType_Float, &value, DIMENSION, NULL, NULL, "%f", 0);
		}		
		else if constexpr (std::is_same_v<T, double>)
			ImGui::InputScalarN("", ImGuiDataType_Double, &value, DIMENSION, NULL, NULL, "%lf", 0);
	}

	/** save a vector into JSON */
	template<int DIMENSION, typename T> requires (DIMENSION >= 1 && DIMENSION <= 4)
	bool DoSaveIntoJSON(nlohmann::json* json, glm::vec<DIMENSION, T> const& src)
	{
		if (!chaos::PrepareSaveArrayIntoJSON(json))
			return false;
		for (size_t i = 0 ; i < DIMENSION ; ++i)
			chaos::JSONTools::SetElement(json, i, src[i]);
		return true;
	}

	/** save a vector into JSON */
	template<int DIMENSION, typename T> requires (DIMENSION >= 1 && DIMENSION <= 4)
	bool DoLoadFromJSON(chaos::JSONReadConfiguration config, glm::vec<DIMENSION, T>& dst)
	{
		return chaos::JSONTools::ForEachSource(config, [&dst](nlohmann::json const* json)
		{
			if (json->is_object())
			{
				static char const* component_names[] = {"x", "y", "z", "w"};
				for (size_t i = 0; i < DIMENSION; ++i)
					chaos::JSONTools::GetAttribute(json, component_names[i], dst[i]);
				return true;
			}
			else if (json->is_array())
			{
				size_t count = std::min(json->size(), size_t(DIMENSION));
				for (size_t i = 0; i < count; ++i)
					dst[i] = json->operator[](i).get<T>();
				return true;
			}
			return false;
		});
	}
}; // namespace glm

#endif