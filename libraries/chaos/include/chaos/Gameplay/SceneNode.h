#ifdef CHAOS_FORWARD_DECLARATION

namespace chaos
{
	class SceneNode;

}; // namespace chaos

#elif defined CHAOS_TEMPLATE_IMPLEMENTATION


#else

namespace chaos
{
	class SceneNode : public GPURenderable
	{
		static constexpr int INVALID_LOCAL_TO_PARENT = 1;
		static constexpr int INVALID_PARENT_TO_LOCAL = 2;

	public:

		/** destructor */
		virtual ~SceneNode();

		/** gets the local to parent transform */
		glm::mat4 const& GetLocalToParent() const;
		/** gets the parent to local */
		glm::mat4 const& GetParentToLocal() const;

		/** get the transformation from root node */
		glm::mat4 GetWorldToLocal() const;
		/** get the transformation to root node */
		glm::mat4 GetLocalToWorld() const;

		/** get the position of the node */
		glm::vec2 const & GetPosition() const { return position; }
		/** get the scale of the node */
		glm::vec2 const& GetScale() const { return scale; }
		/** get the rotation of the node */
		float GetRotator() const { return rotator; }

		/** set the position of the node */
		void SetPosition(glm::vec2 const& in_position);
		/** set the scale of the node */
		void SetScale(glm::vec2 const& in_scale);
		/** set the rotation of the node */
		void SetRotator(float in_rotator);

		/** insert a children node */
		void AddChildNode(SceneNode* in_child);
		/** remove a children node */
		void RemoveChildNode(SceneNode* in_child);

		/** gets the children nodes */
		std::vector<shared_ptr<SceneNode>> & ChildrenNodes() { return child_nodes; }
		/** gets the children nodes */
		std::vector<shared_ptr<SceneNode>> const & ChildrenNodes() const { return child_nodes; }

	protected:

		/** override */
		virtual int DoDisplay(GPURenderer* renderer, GPUProgramProviderBase const* uniform_provider, GPURenderParams const& render_params) override;

	protected:

		/** the position of the node */
		glm::vec2 position = glm::vec2(0.0f, 0.0f);
		/** the scale of the node */
		glm::vec2 scale = glm::vec2(1.0f, 1.0f);
		/** the rotator of the node */
		float rotator = 0.0f;

		/** the cached local to parent matrix */
		mutable glm::mat4 local_to_parent;
		/** the cached parent to local matrix */
		mutable glm::mat4 parent_to_local;
		/** the cache state */
		mutable int cache_state = INVALID_LOCAL_TO_PARENT | INVALID_PARENT_TO_LOCAL;

		/** the children nodes */
		std::vector<shared_ptr<SceneNode>> child_nodes;
		/** the parent node */
		weak_ptr<SceneNode> parent_node;
	};

}; // namespace chaos

#endif // CHAOS_FORWARD_DECLARATION







