#ifdef CHAOS_FORWARD_DECLARATION

namespace chaos
{

	template<typename VECTOR_TYPE>
	class AutoCastableVector;

}; // namespace chaos

#elif !defined CHAOS_TEMPLATE_IMPLEMENTATION

namespace chaos
{
	// ==============================================================================================
	// rotator functions
	// ==============================================================================================

	/** rotator to matrix */
	CHAOS_API glm::mat4x4 GetRotationMatrix(glm::quat const& rotation);
	/** rotator to matrix */
	CHAOS_API glm::dmat4x4 GetRotationMatrix(glm::dquat const& rotation);
	/** rotator to matrix */
	CHAOS_API glm::mat4x4 GetRotationMatrix(float rotation);
	/** rotator to matrix */
	CHAOS_API glm::dmat4x4 GetRotationMatrix(double rotation);

	// ==============================================================================================
	// plane functions
	// ==============================================================================================

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

	// ==============================================================================================
	// GLM vector functions
	// ==============================================================================================

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

	// ==============================================================================================
	// box_base functions
	// ==============================================================================================

	// shu46 : half_size.x == 0  -> pas vide






	/** returns the bounding circle for the box */
	template<std::floating_point T>
	sphere<2, T> GetBoundingSphere(box_base<2, T> const& b)
	{
		return IsGeometryEmpty(b) ? sphere<2, T>() : sphere<2, T>(b.position, glm::length(b.half_size));
	}

	/** returns the inner circle for the box */
	template<std::floating_point T>
	sphere<2, T> GetInnerSphere(box_base<2, T> const& b)
	{
		return IsGeometryEmpty(b) ? sphere<2, T>() : sphere<2, T>(b.position, GLMTools::GetMinComponent(b.half_size));
	}

	/** returns the bounding sphere for the box */
	template<std::floating_point T>
	sphere<3, T> GetBoundingSphere(box_base<3, T> const& b)
	{
		return IsGeometryEmpty(b) ? sphere<3, T>() : sphere<3, T>(b.position, glm::length(b.half_size));
	}

	/** returns the inner sphere for the box */
	template<std::floating_point T>
	sphere<3, T> GetInnerSphere(box_base<3, T> const& b)
	{
		return IsGeometryEmpty(b) ? sphere<3, T>() : sphere<3, T>(b.position, GLMTools::GetMinComponent(b.half_size));
	}

	/** returns the "aspect" of the box (width/height) */
	template<std::floating_point T>
	T GetBoxAspect(box_base<2, T> const& b)
	{
		return (b.half_size.y) ? (b.half_size.x / b.half_size.y) : static_cast<T>(1);
	}



	// ==============================================================================================
	// box functions
	// ==============================================================================================





	/** get the corners of the box */
	CHAOS_GEOMETRY_TEMPLATE(DIMENSION, T)
	auto GetBoxCorners(box<DIMENSION, T> const& b) // returns a std::pair<vec_type, vec_type>
	{
		if (!IsGeometryEmpty(b))
			return std::make_pair(b.position - b.half_size, b.position + b.half_size);
		return std::make_pair(b.position, b.position);
	}

	/** increase the box size with a single vertex */
	CHAOS_GEOMETRY_TEMPLATE(DIMENSION, T)
	void ExtendBox(box<DIMENSION, T>& b, typename box<DIMENSION, T>::vec_type const& v)
	{
		using geometry_type = geometry<DIMENSION, T>;
		using vec_type = typename geometry_type::vec_type;

		if (IsGeometryEmpty(b))
		{
			b.position = v;
			b.half_size = vec_type(0);
		}
		else
		{
			std::pair<vec_type, vec_type> corners = GetBoxCorners(b);
			corners.first = glm::min(corners.first, v);
			corners.second = glm::max(corners.second, v);
			b = box<DIMENSION, T>(corners);
		}
	}

	template<std::floating_point T>
	auto GetBoxVertices(box_base<2, T> const& b, typename box_base<2, T>::vec_type* result, bool global = true) // expect an array of 4 elements
	{
		assert(result != nullptr);

		using geometry_type = geometry<2, T>;
		using vec_type = typename geometry_type::vec_type;

		T NX = -b.half_size.x;
		T PX = +b.half_size.x;

		T NY = -b.half_size.y;
		T PY = +b.half_size.y;

		result[0] = vec_type(NX, NY);
		result[1] = vec_type(PX, NY);
		result[2] = vec_type(NX, PY);
		result[3] = vec_type(PX, PY);

		if (global)
			for (int i = 0; i < 4; ++i)
				result[i] += b.position;

		return result;
	}

	template<std::floating_point T>
	auto GetBoxVertices(box_base<3, T> const& b, typename box_base<3, T>::vec_type* result, bool global = true) // expect an array of 8 elements
	{
		assert(result != nullptr);

		using geometry_type = geometry<3, T>;
		using vec_type = typename geometry_type::vec_type;

		T NX = -b.half_size.x;
		T PX = +b.half_size.x;

		T NY = -b.half_size.y;
		T PY = +b.half_size.y;

		T NZ = -b.half_size.z;
		T PZ = +b.half_size.z;

		result[0] = vec_type(NX, NY, NZ);
		result[1] = vec_type(PX, NY, NZ);
		result[2] = vec_type(NX, PY, NZ);
		result[3] = vec_type(PX, PY, NZ);
		result[4] = vec_type(NX, NY, PZ);
		result[5] = vec_type(PX, NY, PZ);
		result[6] = vec_type(NX, PY, PZ);
		result[7] = vec_type(PX, PY, PZ);

		if (global)
			for (int i = 0; i < 8; ++i)
				result[i] += b.position;

		return result;
	}

	/** encode a box2 into a vector4 */
	CHAOS_API glm::vec4 EncodeBoxToVector(box2 const& src);

	/** get planes from a box */
	template<std::floating_point T>
	box_plane<2, T> GetBoxPlanes(box_base<2, T> const& b)
	{
		box_plane<2, T> result;
		result.neg_x = { T(-1),  T(0), -b.half_size.x};
		result.pos_x = { T(+1),  T(0), -b.half_size.x };
		result.neg_y = {  T(0), T(-1), -b.half_size.y};
		result.pos_y = {  T(0), T(+1), -b.half_size.y };
		return result;
	}
	/** get planes from a box */
	template<std::floating_point T>
	box_plane<3, T> GetBoxPlanes(box_base<3, T> const& b)
	{
		box_plane<3, T> result;
		result.neg_x = { T(-1),  T(0),  T(0), -b.half_size.x};
		result.pos_x = { T(+1),  T(0),  T(0), -b.half_size.x };
		result.neg_y = {  T(0), T(-1),  T(0), -b.half_size.y };
		result.pos_y = {  T(0), T(+1),  T(0), -b.half_size.y };
		result.neg_z = {  T(0),  T(0), T(-1), -b.half_size.z };
		result.pos_z = {  T(0),  T(0), T(+1), -b.half_size.z };
		return result;
	}

	// ==============================================================================================
	// obox functions
	// ==============================================================================================



	// shu49. Ca vaut le coup de savoir dans quel ordre on genere ces vertex

	//  0    1                    0    1
	//                    ou
	//  2    3                    3    2
	//
	// histoire de faire du rendu plus facilement

	template<std::floating_point T>
	auto GetBoxVertices(obox<2, T> const& b, typename box_base<2, T>::vec_type* result, bool global = true) // expect an array of 4 elements
	{
		GetBoxVertices((box_base<2, T> const&)b, result, false); // do not integrate translation because this would produce a wrong rotation/translation combinaison

		if (global)
		{
			auto transform = GetRotationMatrix(b.rotation);
			for (int i = 0; i < 4; ++i)
			{
				result[i] = GLMTools::Mult(transform, result[i]);
				result[i] += b.position;
			}
		}
		return result;
	}

	template<std::floating_point T>
	auto GetBoxVertices(obox<3, T> const& b, typename box_base<3, T>::vec_type* result, bool global = true) // expect an array of 8 elements
	{
		GetBoxVertices((box_base<3, T> const&)b, result, false); // do not integrate translation because this would produce a wrong rotation/translation combinaison

		if (global)
		{
			auto transform = GetRotationMatrix(b.rotation);
			for (int i = 0; i < 8; ++i)
			{
				result[i] = GLMTools::Mult(transform, result[i]);
				result[i] += b.position;
			}
		}
	}

	template<std::floating_point T>
	box<2, T> GetBoundingBox(obox<2, T> const& b)
	{
		box<2, T> result;

		// copy position
		result.position = b.position;
		// get all vertices and compute max values
		typename box<2, T>::vec_type vertices[4];
		GetBoxVertices(b, vertices, false);

		auto max_vec = glm::tvec2<T>(-std::numeric_limits<T>::max(), -std::numeric_limits<T>::max());

		auto transform = GetRotationMatrix(b.rotation);
		for (int i = 0; i < 4; ++i)
			max_vec = glm::max(max_vec, GLMTools::Mult(transform, vertices[i]));
		result.half_size = max_vec;

		return result;
	}

	template<std::floating_point T>
	box<3, T> GetBoundingBox(obox<3, T> const& b)
	{
		box<3, T> result;

		// copy position
		result.position = b.position;

		// get all vertices and compute max values
		typename box<3, T>::vec_type vertices[8];
		GetBoxVertices(b, vertices, false);

		auto max_vec = glm::tvec3<T>(-std::numeric_limits<T>::max(), -std::numeric_limits<T>::max(), -std::numeric_limits<T>::max());

		auto transform = GetRotationMatrix(b.rotation);
		for (int i = 0; i < 8; ++i)
			max_vec = glm::max(max_vec, GLMTools::Mult(transform, vertices[i]));
		result.half_size = max_vec;

		return result;
	}

	// ==============================================================================================
	// aabox functions
	// ==============================================================================================



	/** returns true whether the box is empty */
	CHAOS_GEOMETRY_TEMPLATE(DIMENSION, T)
	bool IsGeometryEmpty(aabox<DIMENSION, T> const& b)
	{
		return glm::any(glm::lessThan(b.size, aabox<DIMENSION, T>::vec_type(0)));
	}

	/** returns the perimeter of the box */
	template<std::floating_point T>
	T GetPerimeter(aabox<2, T> const& b)
	{
		return static_cast<T>(2) * (b.size.x + b.size.y);
	}

	/** returns the surface of the box */
	template<std::floating_point T>
	T GetSurface(aabox<2, T> const& b)
	{
		return (b.size.x * b.size.y);
	}

	/** return the volume of the box */
	template<std::floating_point T>
	T GetVolume(aabox<3, T> const& b)
	{
		return b.size.x * b.size.y * b.size.z;
	}

	/** return the surface of the box */
	template<std::floating_point T>
	T GetSurface(aabox<3, T> const& b)
	{
		return static_cast<T>(2) * ((b.size.x * b.size.y) + (b.size.y * b.size.z) + (b.size.z * b.size.x));
	};

	/** returns the "aspect" of the box (width/height) */
	template<std::floating_point T>
	T GetBoxAspect(aabox<2, T> const& b)
	{
		return (b.size.y) ? (b.size.x / b.size.y) : static_cast<T>(1);
	}


	/** update a box aspect */
	template<std::floating_point T>
	aabox<2, T> SetBoxAspect(aabox<2, T> const& src, typename T aspect, SetBoxAspectMethod method)
	{
		// any negative component
		if (IsGeometryEmpty(src))
			return src;

		// shrink method
		if (method == SetBoxAspectMethod::SHRINK_BOX)
		{
			auto effective_aspect = GetBoxAspect(src);
			if (effective_aspect == aspect)
				return src;

			if (effective_aspect > aspect) // width too large
				method = SetBoxAspectMethod::PREFER_UPDATE_WIDTH;
			else if (effective_aspect < aspect) // height too large
				method = SetBoxAspectMethod::PREFER_UPDATE_HEIGHT;
		}
		// other method
		else
		{
			// cannot have no size
			if (src.size.x == 0 && src.size.y == 0)
				return src;

			// if size is 0 along one axis, force/alter this axis
			if (src.size.x == 0)
				method = SetBoxAspectMethod::PREFER_UPDATE_WIDTH;
			else if (src.size.y == 0)
				method = SetBoxAspectMethod::PREFER_UPDATE_HEIGHT;
		}

		// make the update
		assert((method == SetBoxAspectMethod::PREFER_UPDATE_WIDTH) || (method == SetBoxAspectMethod::PREFER_UPDATE_HEIGHT));

		aabox<2, T> result = src;
		if (method == SetBoxAspectMethod::PREFER_UPDATE_WIDTH)
		{
			result.size.x = src.size.y * aspect;
			result.position.x -= (result.size.x - src.size.x) / static_cast<T>(2);
		}
		else if (method == SetBoxAspectMethod::PREFER_UPDATE_HEIGHT)
		{
			result.size.y = src.size.x / aspect;
			result.position.y -= (result.size.y - src.size.y) / static_cast<T>(2);
		}

		return result;
	}

	// ==============================================================================================
	// triangles functions
	// ==============================================================================================



	// ==============================================================================================
	// ray functions
	// ==============================================================================================



	// ==============================================================================================
	// sphere/circle functions
	// ==============================================================================================



	

	/** returns the bounding box of the circle */
	template<std::floating_point T>
	box<2, T> GetBoundingBox(sphere<2, T> const& c)
	{
		using geometry_type = geometry<2, T>;
		using vec_type = typename geometry_type::vec_type;

		return IsGeometryEmpty(c) ? box<2, T>() : box<2, T>(c.position, vec_type(c.radius, c.radius));
	}

	/** returns the bounding box of the circle (square) */
	template<std::floating_point T>
	box<2, T> GetInnerBox(sphere<2, T> const& c)
	{
		using geometry_type = geometry<2, T>;
		using vec_type = typename geometry_type::vec_type;

		static double const INV_SQRT2 = 0.707106781186547; /* 1.0 / sqrtf(2.0) */

		return IsGeometryEmpty(c) ? box<2, T>() : box<2, T>(c.position, vec_type(c.radius * static_cast<T>(INV_SQRT2)));
	}

	template<std::floating_point T>
	box<3, T> GetBoundingBox(sphere<3, T> const& s)
	{
		using geometry_type = geometry<3, T>;
		using vec_type = typename geometry_type::vec_type;

		return IsGeometryEmpty(s) ? box<3, T>() : box<3, T>(s.position, vec_type(s.radius));
	}

	template<std::floating_point T>
	box<3, T> GetInnerBox(sphere<3, T> const& s)
	{
		using geometry_type = geometry<3, T>;
		using vec_type = typename geometry_type::vec_type;

		static double const INV_SQRT3 = 0.577350269189625; /* 1.0 / sqrtf(3.0) */

		return IsGeometryEmpty(s) ? box<3, T>() : box<3, T>(s.position, vec_type(s.radius * static_cast<T>(INV_SQRT3)));
	}



	// ==============================================================================================
	// JSON functions
	// ==============================================================================================







}; // namespace chaos

	// XXX: for namespace resolution in JSONTools::LoadFromJSON(...) and JSONTools::SaveIntoJSON(...),
	//      it is important that the following functions belong to the same namespace than the target (glm)

namespace glm
{
	template<int SIZE, typename T, glm::precision P>
	void DrawImGuiVariableImpl(glm::vec<SIZE, T, P>& value, chaos::DrawImGuiVariableFlags flags = chaos::DrawImGuiVariableFlags::NONE)
	{
		// ImGui does not provide a InputDoubleX method, that's why we are using InputScalarN here instead of (InputIntX and InputFloatX)
		// (InputScalarN is easier because the size can be given has an argument)	
		if constexpr (std::is_same_v<T, int>)
			ImGui::InputScalarN("", ImGuiDataType_S32, &value, SIZE, NULL, NULL, "%d", 0);
		else if constexpr (std::is_same_v<T, float>)
		{
			if (HasAnyFlags(flags, chaos::DrawImGuiVariableFlags::COLOR)) // special case for colors
			{
				if (SIZE == 3)
				{
					ImGui::ColorEdit3("", &value.x, 0);
					return;
				}
				if (SIZE == 4)
				{
					ImGui::ColorEdit4("", &value.x, 0);
					return;
				}
			}
			ImGui::InputScalarN("", ImGuiDataType_Float, &value, SIZE, NULL, NULL, "%f", 0);
		}		
		else if constexpr (std::is_same_v<T, double>)
			ImGui::InputScalarN("", ImGuiDataType_Double, &value, SIZE, NULL, NULL, "%lf", 0);
	}

	template<typename T, glm::precision P>
	bool DoSaveIntoJSON(nlohmann::json* json, glm::tvec2<T, P> const& src)
	{
		if (!chaos::PrepareSaveArrayIntoJSON(json))
			return false;
		chaos::JSONTools::SetElement(json, 0, src.x);
		chaos::JSONTools::SetElement(json, 1, src.y);
		return true;
	}

	template<typename T, glm::precision P>
	bool DoLoadFromJSON(chaos::JSONReadConfiguration config, glm::tvec2<T, P>& dst)
	{
		return chaos::JSONTools::ForEachSource(config, [&dst](nlohmann::json const* json)
		{
			if (json->is_object())
			{
				chaos::JSONTools::GetAttribute(json, "x", dst.x);
				chaos::JSONTools::GetAttribute(json, "y", dst.y);
				return true;
			}
			else if (json->is_array())
			{
				size_t count = std::min(json->size(), (size_t)dst.length());
				for (size_t i = 0; i < count; ++i)
					dst[i] = json->operator[](i).get<T>();
				return true;
			}
			return false;
		});
	}

	template<typename T, glm::precision P>
	bool DoSaveIntoJSON(nlohmann::json* json, glm::tvec3<T, P> const& src)
	{
		if (!chaos::PrepareSaveArrayIntoJSON(json))
			return false;
		chaos::JSONTools::SetElement(json, 0, src.x);
		chaos::JSONTools::SetElement(json, 1, src.y);
		chaos::JSONTools::SetElement(json, 2, src.z);
		return true;
	}

	template<typename T, glm::precision P>
	bool DoLoadFromJSON(chaos::JSONReadConfiguration config, glm::tvec3<T, P>& dst)
	{
		return chaos::JSONTools::ForEachSource(config, [&dst](nlohmann::json const* json)
		{
			if (json->is_object())
			{
				chaos::JSONTools::GetAttribute(json, "x", dst.x);
				chaos::JSONTools::GetAttribute(json, "y", dst.y);
				chaos::JSONTools::GetAttribute(json, "z", dst.z);
				return true;
			}
			else if (json->is_array())
			{
				size_t count = std::min(json->size(), (size_t)dst.length());
				for (size_t i = 0; i < count; ++i)
					dst[i] = json->operator[](i).get<T>();
				return true;
			}
			return false;
		});
	}

	template<typename T, glm::precision P>
	bool DoSaveIntoJSON(nlohmann::json* json, glm::tvec4<T, P> const& src)
	{
		if (!chaos::PrepareSaveArrayIntoJSON(json))
			return false;
		chaos::JSONTools::SetElement(json, 0, src.x);
		chaos::JSONTools::SetElement(json, 1, src.y);
		chaos::JSONTools::SetElement(json, 2, src.z);
		chaos::JSONTools::SetElement(json, 3, src.w);
		return true;
	}

	template<typename T, glm::precision P>
	bool DoLoadFromJSON(chaos::JSONReadConfiguration config, glm::tvec4<T, P>& dst)
	{
		return chaos::JSONTools::ForEachSource(config, [&dst](nlohmann::json const* json)
		{
			if (json->is_object())
			{
				chaos::JSONTools::GetAttribute(json, "x", dst.x);
				chaos::JSONTools::GetAttribute(json, "y", dst.y);
				chaos::JSONTools::GetAttribute(json, "z", dst.z);
				chaos::JSONTools::GetAttribute(json, "w", dst.w);
				return true;
			}
			else if (json->is_array())
			{
				size_t count = std::min(json->size(), (size_t)dst.length());
				for (size_t i = 0; i < count; ++i)
					dst[i] = json->operator[](i).get<T>();
				return true;
			}
			return false;
		});
	}

}; // namespace glm

#endif