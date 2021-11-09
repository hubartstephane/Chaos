namespace chaos
{
#ifdef CHAOS_FORWARD_DECLARATION

	template<typename T, int dimension>
	class SceneTransform;

#elif !defined CHAOS_TEMPLATE_IMPLEMENTATION

	template<typename T, int dimension>
	class SceneTransform
	{
		using vec_type = typename type_geometric<T, dimension>::vec_type;
		using rot_type = typename type_geometric<T, dimension>::rot_type;
		using mat_type = typename type_geometric<T, dimension>::mat_type;

	public:

		/** gets the local to parent transform */
		mat_type const& GetLocalToParent() const
		{
			return {};
		}
		/** gets the parent to local */
		mat_type const& GetParentToLocal() const
		{

			return {};
		}

		/** get the position */
		vec_type const& GetPosition() const { return position; }
		/** get the scale */
		vec_type const& GetScale() const { return scale; }
		/** get the rotation */
		rot_type const& GetRotator() const { return rotator; }

		/** set the position */
		void SetPosition(vec_type const& in_position) { position = in_position; }
		/** set the scale */
		void SetScale(vec_type const& in_scale) { scale = in_scale; }
		/** set the rotation */
		void SetRotator(rot_type const &in_rotator) { rotator = in_rotator; }

	protected:

		/** the position */
		vec_type position = vec_type(0);
		/** the scale */
		vec_type scale = vec_type(1);
		/** the rotator */
		rot_type rotator = zero_rotator();
	};


#endif

}; // namespace chaos