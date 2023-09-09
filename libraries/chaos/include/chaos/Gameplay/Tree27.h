namespace chaos
{
#ifdef CHAOS_FORWARD_DECLARATION

	//
	//                                                                           width = 3
	// <........................................................................................................................................................>
	// 
	// +----------------------------------------------------------------------------+----------------------------------------------------------------------------+ Level N + 1
	// |                                                                                                                                                         |
	// |                                                                                                                                                         |
	// |                                                                         width = 1                                                                       |
	// |                                                  <...................................................>                                                  |
	// |                                                                                                                                                         |
	// |                                                 +--------------------------+--------------------------+   Level N                                       |
	// |                                                 |       1/3           1/6     1/6           1/3       |                                                 |
	// |                                                 |<...............> <......> <......> <...............>|                                                 |
	// |                 +------------------------+------------------------+                 +------------------------+------------------------+                 |
	// |       1/3       |                                                 |       1/3       |                                                 |       1/3       |
	// |<...............>|                                                 |<...............>|                                                 |<...............>|
	// |                 |                                                 |                 |                      Node 1                     |       A         |
	// |                 |                           1/6         1/3       |  1/6            |                                                 |                 |
	// |                 |                        o<-----><--------------->|<----->o         |                                                 |                 |
	// |                 |                        Distance between sibling's centers         |                                 1/3 * 3 (overlaps Level N + 1)    |
	// |                 |                                                 |                 |              <...................................................>|
	// |                                                                                     |             +-----------------------------------------------------+--------------------------+--------------------------------------------------------------------------------+
	// |                                                                                     |             |                                     width = 1                                                                                                                   |
	// |                                                                                     |             |                  <...............................................>                                                                                              |
	// |                                                                                     |             |                                                                             Ensure Node 1 & Node 2 overlaps by 1/3                                              |
	// |                                                                                     |             |                 +------------------------+------------------------+                                                                                             |
	// |                                                                                     |             |       1/3       |                                                 |         (Node 1 & Node 2 have sibling parents)                                              |
	// |                                                                                     |             |<...............>|                     Node 2                      |                                                                                             |
	// |                                                                                     |             |        B        |                                                 |         A + B + C = 1/3 * 3                                                                 |
	// |                                                                                     |             |                 |       1/3       |                               |                                                                                             |
	// |                                                                                     |             |                 |<...............>|                               |         => C = 1/3      CQFD !                                                              |
	// |                                                                                     |             |                 |        C        |                               |                                                                                             |
	// |                                                                                     |             |                 |                 |                               |                                                                                             |
	// |                                                                                     |                                                 |
	// |                                                                                     |                                                 |
	// |                                                                                     |                                                 |
	// |                                                                                     +-------------------------------------------------+
	// 
	// Each node has 3 child nodes.
	// The width of a child node is 1/3 its parent width
	// Each node overlaps its siblings by 1/3 its width
	// The distance between 2 siblings centers is 2/3 the width
	// Whenever an object is to be inserted in the tree, we search the level whose node width is 3x greater than the object size
	// (it fully fits inside 1 of the 3 sibling/overlapping nodes)
	//
	// Width(L) = pow(3, L)
	//
	// Distance Centers (L) = (2/3).Width(L)
	//
	// Left Corner (X)(L) =  (2/3).X.Width(L) - (1/2).Width(L)
	//
	// let Node(0)(L) be the node centered to the origin for level L
	//
	// Node(X)(L) contains Node(3X - 1)(L - 1)
	//                     Node(3X)    (L - 1)
	//                     Node(3X + 1)(L - 1)
	//
	// The following table shows how children belongs to Node(X)(0) 
	// 
	// L = 0  | x
	// -------+-------------------------------
	// L = -1 | 3x +/- 1
	// -------+-------------------------------
	// L = -2 | 3[3x +/- 1] +/- 1
	//        | = 9x +/- (3 + 1) 
	// -------+-------------------------------
	// L = -3 | 3[9x +/- (3 + 1)] +/- 1
	//        | = 27x +/- (9 + 3 + 1) 
	// -------+-------------------------------
	// L = -4 | 3[27x +/- (9 + 3 + 1)] +/- 1
	//        | = 81x +- (27 + 9 + 3 + 1)    <--- 27 + 9 + 3 + 1 is the sum of a geometric serie
	// -------+-------------------------------
	//        |                    [pow(3, L) - 1]
	//   -L   | = pow(3, L).x +/- -----------------
	//        |                       [ 3 - 1 ]
	//
	//  pow(3, L) table
	// 
	//  0 | 1
	//  1 | 3
	//  2 | 9
	//  3 | 27
	//  4 | 81
	//  5 | 243
	//  6 | 729
	//  7 | 2'187
	//  8 | 6'561
	//  9 | 19'683
	// 10 | 59'049
	// 11 | 177'147
	// 12 | 531'441
	// 13 | 1'594'323
	// 14 | 4'782'969
	// 15 | 14'348'907
	//
	// This is growing very fast. we should rarely use more than 10 levels
	//
#elif !defined CHAOS_TEMPLATE_IMPLEMENTATION

	namespace details
	{
		/** constexpr pow function */
		template<typename T>
		constexpr T static_pow(T src, T exponent)
		{
			if (exponent == 0)
				return 1;
			return src * static_pow(src, exponent - 1);
		}
		/** compute log 3 */
		float log3(float src);
		/** compute log 3 */
		int log3i(int src);
		/** compute pow 3 */
		float pow3(float src);
		/** compute pow 3 */
		int pow3i(int src);
	};

	template<int DIMENSION>
	class Tree27NodeInfo
	{
	public:

		/** the dimension */
		static constexpr int dimension = DIMENSION;
		/** the type for vector */
		using vec_type = type_geometric<int, dimension>::vec_type;
		/** the type for box */
		using box_type = type_box<float, dimension>;

		/** gets info for one of the children */
		Tree27NodeInfo GetChildNodeInfo(vec_type const & child_position) const
		{
			assert(!glm::any(glm::greaterThan(child_position, vec_type(1.0f))));
			assert(!glm::any(glm::lessThan(child_position, vec_type(-1.0f))));

			Tree27NodeInfo result;
			result.level = level - 1;
			result.position = (position * 3) + child_position;
			return result;
		}

		/** compute the bounding box of the node */
		box_type GetBoundingBox() const
		{
			box_type result;

			float width = details::pow3(float(level));
			for (int i = 0; i < dimension; ++i)
			{
				result.half_size[i] = width * 0.5f;
				result.position[i] = (2.0f / 3.0f) * float(position[i]) * width;
			}
			return result;
		}

		/** compute the range for possible descendants */
		std::pair<vec_type, vec_type> GetDescendantRange(int sub_level) const // sub level is number of levels relatively to current level
		{
			assert(sub_level >= 0);

			//        |                    [pow(3, L) - 1]
			//   -L   | = pow(3, L).x +/- -----------------
			//        |                       [ 3 - 1 ]

			int p3 = details::pow3i(sub_level);

			vec_type a = p3 * position;
			vec_type b = vec_type((p3 - 1) / 2);

			return std::make_pair(a - b, a + b);
		}

		/** gets the index in children corresponding to this info (-1 the node is not a descendant) */
		int GetDescendantIndex(Tree27NodeInfo const& info)
		{
			// check for level
			if (info.level >= level)
				return -1;
			// check whether the info is inside the descendant range
			auto descendant_range = GetDescendantRange(level - info.level);
			if (glm::any(glm::greaterThan(info.position, descendant_range.second)))
				return -1;
			if (glm::any(glm::lessThan(info.position, descendant_range.first)))
				return -1;
			// compute the central child range for info
			auto central_child_range = GetChildNodeInfo(vec_type(0)).GetDescendantRange(level - info.level + 1);

			vec_type descendant_position = vec_type(0);
			for (int i = 0; i < dimension; ++i)
			{
				if (info.position[i] < central_child_range.first)
					descendant_position = -1;
				else if (info.position[i] < central_child_range.first)
					descendant_position = -1;
			}

			if constexpr (dimension == 2)
				return glm::dot(descendant_position, glm::ivec2(1, 3));
			else if constexpr (dimension == 3)
				return glm::dot(descendant_position, glm::ivec3(1, 3, 9));
		}

	public:

		/** the level in the hierarchy of this node */
		int level = 0;
		/** the position of this node in space (indices) */
		vec_type position;
	};

	template<int DIMENSION>
	bool operator == (Tree27NodeInfo<DIMENSION> const& src1, Tree27NodeInfo<DIMENSION> const& src2)
	{
		return (src1.level == src2.level) && (src1.position == src2.position);
	}

	template<int DIMENSION>
	Tree27NodeInfo<DIMENSION> ComputeTreeNodeInfo(type_box<float, DIMENSION> const& box)
	{
		Tree27NodeInfo<DIMENSION> result;

		// get size to take into account
		float box_size = 2.0f * GLMTools::GetMaxComponent(box.half_size);
		if (box_size == 0.0f)
			box_size = 1.0f;  // any object that have size 0, is considered has size = 1

		// compute level:

		// size <= pow(3, L) / 3
		// size <= pow(3, L - 1)
		// log3(size) <= L - 1
		//
		// L >= ceilf(log3(size) + 1)

		result.level = int(std::ceil(details::log3(box_size) + 1.0f));

		// compute node position:

		// obj_left_position >= node_left_position
		//
		// obj_left_position >= (2/3).width(L).X - (1/2).width(L)
		// 
		// (2/3).L.X <= obj_left_position + (1/2).width(L)
		//
		//             obj_left_position + (1/2).width(L)
		// X = floor[ -----------------------------------]
		//                       (2/3).width(L)

		float widthL = details::pow3(float(result.level));
		float denum = (2.0f / 3.0f) * widthL;
		for (int i = 0; i < DIMENSION; ++i)
		{
			float num = (box.position[i] - box.half_size[i]) + (1.0f / 2.0f) * widthL;
			result.position[i] = int(std::floor(num / denum));
		}

		return result;
	}

	template<int DIMENSION>
	Tree27NodeInfo<DIMENSION> ComputeCommonParent(Tree27NodeInfo<DIMENSION> info1, Tree27NodeInfo<DIMENSION> info2)
	{
		// ensure both nodes have the same level
		if (info1.level < info2.level)
		{
			info1.position /= details::pow3i(info2.level - info1.level);
			info1.level = info2.level;
		}
		else if (info2.level < info1.level)
		{
			info2.position /= details::pow3i(info1.level - info2.level);
			info2.level = info1.level;
		}

		// go from parent to parent until they have the same position
		while (info1.position != info2.position)
		{
			++info1.level;
			++info2.level;
			info1.position /= 3;
			info2.position /= 3;
		}

		return info1;
	}

	template<int DIMENSION, typename PARENT>
	class Tree27Node : public PARENT
	{
		template<int DIMENSION, typename PARENT>
		friend class Tree27;

	public:

		/** the dimension */
		static constexpr int dimension = DIMENSION;
		/** the number of children this node has */
		static constexpr int children_count = details::static_pow(3, dimension);
		/** the type for NodeInfo */
		using node_info_type = Tree27NodeInfo<dimension>;

		/** check whether node has a no child */
		bool HasChild() const
		{
			return (existing_children != 0);
		}

		/** check whether node has a single child */
		bool HasSingleChild() const
		{
			return MathTools::IsPowerOf2(existing_children); // a single bit is 1 in the mask
		}

		/** check whether node is used */
		bool IsUseful() const
		{
			return PARENT::IsUseful();
		}

		/** gets the node info */
		node_info_type GetNodeInfo() const
		{
			return info;
		}

		/** gets the node info */
		type_box<float, dimension> GetBoundingBox() const
		{
			return info.GetBoundingBox();
		}

		/** gets the parent */
		Tree27Node const* GetParentNode() const
		{
			return parent;
		}

		/** gets the parent */
		Tree27Node* GetParentNode()
		{
			return parent;
		}

	protected:

		/** constructor */
		Tree27Node()
		{
			for (int i = 0; i < children_count; ++i)
				children[i] = nullptr;
		}

		/** destructor */
		~Tree27Node()
		{
			Clear();
		}

		/** destroy all children */
		void Clear()
		{
			BitTools::ForEachBitForward(existing_children, [this](int index)
			{
				delete(children[index]);
			});
			existing_children = 0;
		}

		/** if the node has a single child, extract it and return it */
		Tree27Node * ExtractSingleChildNode()
		{
			if (HasSingleChild())
			{
				int index = BitTools::bsf(existing_children);
				Tree27Node* result = children[index];
				SetChild(index, nullptr);
				return result;
			}
			return nullptr;
		}

		/** set a child for a given index */
		void SetChild(int index, Tree27Node* child)
		{
			// update previous child
			if (Tree27Node* previous_child = children[index])
			{
				previous_child->parent = nullptr;
				previous_child->index_in_parent = 0;
			}
			// update new child members
			if (child != nullptr)
			{
				child->parent = this;
				child->index_in_parent = index;
			}
			// insert new child
			children[index] = child;
			existing_children = BitTools::SetBit(existing_children, index, child != nullptr);
		}

		/** recursively visit all children */
		template<bool DEPTH_FIRST = false, typename FUNC>
		auto ForEachNode(FUNC const& func) -> boost::mpl::if_c<std::is_convertible_v<decltype(func(0)), bool>, decltype(func(0)), void>::type
		{
			using result_type = decltype(func(0));
			constexpr bool convertible_to_bool = std::is_convertible_v<result_type, bool>;

			for (int i = 0; i < 2; ++i)
			{
				if ((i == 0 && DEPTH_FIRST) || (i == 1 && !DEPTH_FIRST)) // process children
				{
					if constexpr (convertible_to_bool)
					{
						auto result = BitTools::ForEachBitForward(existing_children, [this, &func](int index)
						{
							return children[index]->ForEachNode<DEPTH_FIRST>(func);
						});

						if (result)
							return result;
					}
					else
					{
						BitTools::ForEachBitForward(existing_children, [this, &func](int index)
						{
							children[index]->ForEachNode<DEPTH_FIRST>(func);
						});
					}
				}
				else // process this
				{
					if constexpr (convertible_to_bool)
					{
						if (auto result = func(this))
							return result;
					}
					else
					{
						func(this);
					}
				}
			}

			if constexpr (convertible_to_bool)
				return result_type();
		}

	protected:

		/** the node information */
		Tree27NodeInfo<dimension> info;
		/** the parent node */
		Tree27Node* parent = nullptr;
		/** the children */
		Tree27Node* children[children_count];
		/** the present children */
		int existing_children = 0;
		/** the index of this node in its parent */
		int index_in_parent = 0;

	};

	template<int DIMENSION, typename NODE_PARENT>
	class Tree27
	{
	public:

		/** the type for nodes */
		using node_type = Tree27Node<DIMENSION, NODE_PARENT>;
		/** the type for nodes info */
		using node_info_type = Tree27NodeInfo<DIMENSION>;
		/** the dimension */
		static constexpr int dimension = DIMENSION;

		/** destructor */
		~Tree27()
		{
			Clear();
		}

		/** destroy the whole hierarchy */
		void Clear()
		{
			if (root != nullptr)
			{
				delete(root);
				root = nullptr;
			}
		}

		/** add a node for a given object */
		node_type* AddNode(type_box<float, dimension> const& box)
		{
			node_info_type node_info = ComputeTreeNodeInfo(box);

			return DoAddNode(node_info, root, nullptr, 0);
		}

		/** try to simplify a node (if possible and have at most one child) */
		bool TrySimplifyNode(node_type * node)
		{
			assert(node != nullptr);

			// more than one child: cannot simplify
			if (node->HasChild() && !node->HasSingleChild())
				return false;
			// node still useful: cannot simplify
			if (node->IsUseful())
				return false;
			// removing the root
			if (root == node)
			{
				root = node->ExtractSingleChildNode();
			}
			// removing non root node
			else
			{
				assert(node->parent != nullptr);

				node_type * parent_node = node->parent; // keep a copy of parent before SetChild(...) reset some members
				parent_node->SetChild(node->index_in_parent, node->ExtractSingleChildNode());
				TrySimplifyNode(parent_node); // maybe parent node has become useless
			}

			// delete the useless node
			delete(node);

			return true;
		}

		/** visit the tree */
		template<bool DEPTH_FIRST = false, typename FUNC>
		decltype(auto) ForEachNode(FUNC const& func) const
		{
			using result_type = decltype(func(0));
			constexpr bool convertible_to_bool = std::is_convertible_v<result_type, bool>;

			if (root != nullptr)
				return root->ForEachNode<DEPTH_FIRST>(func);

			if constexpr (convertible_to_bool)
				return result_type();
		}

		/** returns the root */
		node_type* GetRootNode()
		{
			return root;
		}

		/** returns the root */
		node_type const * GetRootNode() const
		{
			return root;
		}

	protected:

		node_type* DoAddNode(node_info_type const & node_info, node_type* current_node, node_type * parent_node, int index_in_parent)
		{
			// the node to create is a leaf (it may be a root too)
			if (current_node == nullptr)
			{
				return DoAddNodeToParent(node_info, parent_node, index_in_parent);
			}
			// the current node is the one searched
			if (current_node->GetNodeInfo() == node_info)
			{
				return current_node;
			}
			// the current node contains the node we want to create
			int child_index1 = current_node->GetNodeInfo()->GetDescendantIndex(node_info);
			if (child_index1 >= 0)
			{

			}
			// the current node is contained by the node we want to create
			int child_index2 = node_info.GetDescendantIndex(current_node->GetNodeInfo());
			if (child_index2 >= 0)
			{

			}
			// must create a common parent for current node and new node





			return nullptr;
		}

		node_type* DoAddNodeToParent(node_info_type const& node_info, node_type* parent_node, int index_in_parent)
		{
			if (node_type* result = new node_type)
			{
				result->info = node_info;
				if (parent_node != nullptr)
					parent_node->SetChild(index_in_parent, result);
				else
					root = result;
				return result;
			}
			return nullptr;
		}

	protected:

		/** the root node */
		node_type* root = nullptr;
	};


#endif

}; // namespace chaos