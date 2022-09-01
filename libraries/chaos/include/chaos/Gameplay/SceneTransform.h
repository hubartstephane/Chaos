namespace chaos
{
#ifdef CHAOS_FORWARD_DECLARATION

	template<typename T, int dimension>
	class SceneTransform;

#elif !defined CHAOS_TEMPLATE_IMPLEMENTATION

	// Translation.Rotation.Scaling
	// ----------------------------
	//
	//   Translation.Rotation =
	//
	//  |  1  0  0 |   | a  b  0 |   | a  b  0 |       X = a.tx + c.ty
	//  |  0  1  1 | x | c  d  0 | = | c  d  0 |  with
	//  | tx ty  1 |   | 0  0  1 |   | X  Y  1 |       Y = b.tx + d.ty
	//
	//   Translation.Rotation.Scaling =
	//
	//  | a  b  0 |   | sx  0  0 |   | sx.a  sy.b  0 |
	//  | c  d  0 | x |  0 sy  0 | = | sx.c  sy.d  0 |
	//  | X  Y  1 |   |  0  0  1 |   | sx.X  sy.Y  1 |
	//
	//
	// Scaling.Rotation.Translation
	// ----------------------------
	//
	//  Scaling.Rotation =
	//
	//  | sx  0  0 |   | a  b  0 |   | sx.a  sx.b  0 |
	//  |  0 sy  0 | x | c  d  0 | = | sy.c  sy.d  0 |
	//  |  0  0  1 |   | 0  0  1 |   |    0     0  1 |
	//
	// Scaling.Rotation.Translation =
	//
	//  | sx.a  sx.b  0 |   |  1   0  0 |   |  sx.a  sx.b  0 |
	//  | sy.c  sy.d  0 | x |  0   1  0 | = |  sy.c  sy.d  0 |
	//  |    0     0  1 |   | tx  ty  1 |   |    tx    ty  1 |
	//
	//
	// The SceneTransform is a T.R.S
	// by definition, the Inverse(SceneTransform) is a S.R.T with
	//
	// Translation(V).Rotation(A).Scaling(S)    x    Scaling(1/S).Rotation(-A).Translation(-V) = identity
	//
	// To convert T.R.S into S.R.T we should be able to transform
	//
	//  | sx.a  sy.b  0 |      |  sx'.a'  sx'.b'  0 |
	//  | sx.c  sy.d  0 | into |  sy'.c'  sy'.d'  0 |
	//  | sx.X  sy.Y  1 |      |     tx'      ty  1 |
	//
	//  This is not possible due to non-uniform scale (see how scale values are spread in column for first matrix, but in row for second one)
	//

	template<typename T, int dimension>
	class CHAOS_API SceneTransform
	{
		using vec_type = typename type_geometric<T, dimension>::vec_type;
		using rot_type = typename type_geometric<T, dimension>::rot_type;
		using mat_type = typename type_geometric<T, dimension>::mat_type;

	public:

		/** gets the local to parent transform */
		mat_type GetLocalToParent() const
		{
			mat_type s, r, t;

			if constexpr (dimension == 2)
				s = glm::scale(glm::tvec3<T>(scale, 1));
			else if constexpr (dimension == 2)
				s = glm::scale(position);

			r = GetRotatorMatrix(rotator);

			if constexpr (dimension == 2)
				t = glm::translate(glm::tvec3<T>(position, 0));
			else if constexpr (dimension == 2)
				t = glm::translate(position);

			return s * r * t;
		}
		/** gets the parent to local */
		mat_type GetParentToLocal() const
		{
			mat_type t, r, s;

			if constexpr (dimension == 2)
				t = glm::translate(glm::tvec3<T>(-position, 0));
			else if constexpr (dimension == 2)
				t = glm::translate(-position);

			r = GetRotatorMatrix(-rotator);

			if constexpr (dimension == 2)
				s = glm::scale(glm::tvec3<T>(T(1) / scale, 1));
			else if constexpr (dimension == 2)
				s = glm::scale(T(1) / scale);

			return t * r * s;
		}

	public:

		/** the position */
		vec_type position = vec_type(0);
		/** the scale */
		vec_type scale = vec_type(1);
		/** the rotator */
		rot_type rotator = zero_rotator();
	};

#endif

}; // namespace chaos