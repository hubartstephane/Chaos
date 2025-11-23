namespace chaos
{
#ifdef CHAOS_FORWARD_DECLARATION

	//
	//                                                                               width = 9/3 = 3
	//  <...............................................................................................................................................................>
	//
	// +--------------------------------------------------------------------------------+--------------------------------------------------------------------------------+ Level N + 1
	// |                                                                                                                                                                 |
	// |                                                                                                                                                                 |
	// |                                                                             width = 1                                                                           |
	// |                                                      <...................................................>                                                      |
	// |                                                                                                                                                                 |
	// |                                                     +--------------------------+--------------------------+   Level N                                           |
	// |                                                     |       1/3           1/6     1/6           1/3       |                                                     |
	// |                                                     |<...............> <......> <......> <...............>|                                                     |
	// |                                                     |                                                     |                                                     |
	// |                         1/3           1/6      1/6  |                                                     |   1/6     1/6          1/3                          |
	// |                  <...............> <......> <......>|                                                     |<......> <......> <...............>                  |
	// |                                                     |                                                     |                                                     |
	// |                 +--------------------------+--------------------------+                 +--------------------------+--------------------------+                 |
	// |       1/3       |                                                     |       1/3       |                                                     |       1/3       |
	// |<...............>|                                                     |<...............>|                                                     |<...............>|
	// |                 |                                                     |                 |                         NODE 1                      |       A         |
	// |                 |                               1/6         1/3       |  1/6      1/6   |                                                     |                 |
	// |                 |                          o<------> <--------------->|<------>o<------>|                                                     |                 |
	// |                 |                            Distance between sibling's centers         |                                                     |                 |
	// |                 |                                                     |                 |                                                     |                 |
	// |                 |                                                     |                 |                                 1/3 * 3 (overlaps Level N + 1)        |
	// |                 |                                                     |                 |                  <...................................................>|
	// |                                                                                         |                  +--------------------------------------------------------------------------------+--------------------------------------------------------------------------------+
	// |                                                                                         |                  |                                         width = 1                                                                                                               |
	// |                                                                                         |                  |                  <...................................................>                                                                                          |
	// |                                                                                         |                  |                                                                             Ensure NODE 1 & NODE 2 overlaps by 1/3                                              |
	// |                                                                                         |                  |                 +--------------------------+--------------------------+                                                                                         |
	// |                                                                                         |                  |       1/3       |                                                     |         (NODE 1 & NODE 2 have sibling parents)                                          |
	// |                                                                                         |                  |<...............>|                        NODE 2                       |                                                                                         |
	// |                                                                                         |                  |        B        |                                                     |         A + B + C = 1/3 * 3                                                             |
	// |                                                                                         |                  |                 |       1/3       |  1/6       1/6         1/3        |                                                                                         |
	// |                                                                                         |                  |                 |<...............>|<......>o<......> <...............>|         => C = 1/3      CQFD !                                                          |
	// |                                                                                         |                  |                 |        C        |                                   |                                                                                         |
	// |                                                                                         |                  |                 |                 |                                   |                                                                                         |
	// |                                                                                         |                                                      |
	// |                                                                                         |                                                      |
	// |                                                                                         |                                                      |
	// |                                                                                         +------------------------------------------------------+
	//
	// PREDICATES
	//
	//   Each node has 3 child nodes.
	//   The width of a child node is 1/3 its parent width
	//   Each node overlaps its siblings by 1/3 its width
	//
	// DEDUCTIONS	
	//
	//   The distance between 2 siblings centers is 2/3 the width
	//   Whenever an object is to be inserted in the tree, we search the level whose node width is 3x greater than the object size
	//   (it fully fits inside 1 of the 3 sibling/overlapping nodes)
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
		CHAOS_API double log3(double src);
		/** compute log 3 */
		CHAOS_API float log3(float src);
		/** compute log 3 */
		CHAOS_API int log3i(int src);
		/** compute pow 3 */
		CHAOS_API double pow3(double src);
		/** compute pow 3 */
		CHAOS_API float pow3(float src);
		/** compute pow 3 */
		CHAOS_API int pow3i(int src);

		/** a concept for node */
		template<typename T>
		concept Implement_IsUseful = requires(T t)
		{
			{t.IsUseful()} -> std::convertible_to<bool>;
		};
	};

	CHAOS_GEOMETRY_TEMPLATE(T, dimension)
	class Tree27NodeInfo : public type_geometric<T, dimension>
	{
	public:

		/** the dimension */
		static constexpr int dimension = dimension;
		/** the type for box */
		using box_type = type_geometric<T, dimension>::box_type;
		/** the type for indexation vector */
		using ivec_type = glm::vec<dimension, int>;

		/** gets info for one of the children */
		Tree27NodeInfo GetChildNodeInfo(ivec_type const & child_position) const
		{
			assert(!glm::any(glm::greaterThan(child_position, ivec_type(1))));
			assert(!glm::any(glm::lessThan(child_position, ivec_type(-1))));

			Tree27NodeInfo result;
			result.level = level - 1;
			result.position = (position * 3) + child_position;
			return result;
		}

		/** compute the bounding box of the node */
		box_type GetBoundingBox() const
		{
			box_type result;

			T width = details::pow3(T(level));
			for (int i = 0; i < dimension; ++i)
			{
				result.half_size[i] = width * T(0.5);
				result.position[i] = T(2.0 / 3.0) * T(position[i]) * width;
			}
			return result;
		}

		/** compute the range for possible descendants */
		std::pair<ivec_type, ivec_type> GetDescendantRange(int sub_level) const // sub level is number of levels relatively to current level
		{
			assert(sub_level >= 0);

			//        |                    [pow(3, L) - 1]
			//   -L   | = pow(3, L).x +/- -----------------
			//        |                       [ 3 - 1 ]

			int p3 = details::pow3i(sub_level);

			ivec_type a = p3 * position;
			ivec_type b = ivec_type((p3 - 1) / 2);

			return std::make_pair(a - b, a + b);
		}

		/** gets the index in children corresponding to this info (-1 the node is not a descendant) */
		int GetDescendantIndex(Tree27NodeInfo<T, dimension> const& info) const
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
			Tree27NodeInfo<T, dimension> central_child_node_info = GetChildNodeInfo(ivec_type(0));
			auto central_child_range = central_child_node_info.GetDescendantRange(central_child_node_info.level - info.level);

			// compute index
			//
			// index = 1.x + 3.y + 9.z          (with x,y,z inside [0..2])

			int result = 0;
			int multiplier = 1;
			for (int i = 0; i < dimension; ++i, multiplier *= dimension)
			{
				if (info.position[i] >= central_child_range.first[i])
				{
					result += multiplier;
					if (info.position[i] > central_child_range.second[i])
						result += multiplier;
				}
			}
			return result;
		}

	public:

		/** the level in the hierarchy of this node */
		int level = 0;
		/** the position of this node in space (indices) */
		ivec_type position = ivec_type(0);
	};

	CHAOS_GEOMETRY_TEMPLATE(T, dimension)
	bool operator == (Tree27NodeInfo<T, dimension> const& src1, Tree27NodeInfo<T, dimension> const& src2)
	{
		return (src1.level == src2.level) && (src1.position == src2.position);
	}

	CHAOS_GEOMETRY_TEMPLATE(T, dimension)
	Tree27NodeInfo<T, dimension> ComputeTreeNodeInfo(type_box<T, dimension> const& box)
	{
		assert(!IsGeometryEmpty(box));

		Tree27NodeInfo<T, dimension> result;

		// get size to take into account
		T box_size = T(2.0) * GLMTools::GetMaxComponent(box.half_size);

		// compute level:

		// size <= pow(3, L) / 3
		// size <= pow(3, L - 1)
		// log3(size) <= L - 1
		//
		// L >= ceilf(log3(size) + 1)

		result.level = int(std::ceil(details::log3(box_size) + T(1.0)));

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

		T widthL = details::pow3(T(result.level));
		T denum = T(2.0 / 3.0) * widthL;
		for (int i = 0; i < dimension; ++i)
		{
			T num = (box.position[i] - box.half_size[i]) + T(0.5) * widthL;
			result.position[i] = int(std::floor(num / denum));
		}

		return result;
	}

	CHAOS_GEOMETRY_TEMPLATE(T, dimension)
	Tree27NodeInfo<T, dimension> ComputeCommonParent(Tree27NodeInfo<T, dimension> info1, Tree27NodeInfo<T, dimension> info2)
	{
		auto GetParentNodePosition = [](Tree27NodeInfo<T, dimension>::ivec_type v)
		{
			for (int i = 0; i < dimension; ++i)
				v[i] = (v[i] > 0) ? (v[i] + 1) / 3 : (v[i] - 1) / 3;
			return v;
		};
		// ensure both nodes have the same level
		while (info1.level < info2.level)
		{
			info1.position = GetParentNodePosition(info1.position);
			++info1.level;
		}

		while (info2.level < info1.level)
		{
			info2.position = GetParentNodePosition(info2.position);
			++info2.level;
		}
		// go from parent to parent until they have the same position
		while (info1.position != info2.position)
		{
			info1.position = GetParentNodePosition(info1.position);
			info2.position = GetParentNodePosition(info2.position);
			++info1.level;
			++info2.level;
		}

		return info1;
	}

	CHAOS_GEOMETRY_TEMPLATE(T, dimension, typename PARENT)
	class Tree27Node : public PARENT, public type_geometric<T, dimension>
	{
		CHAOS_GEOMETRY_TEMPLATE(T, dimension, typename PARENT, template<typename> class NODE_ALLOCATOR)
		friend class LooseTree27;

	public:

		/** the dimension */
		static constexpr int dimension = dimension;
		/** the number of children this node has */
		static constexpr int children_count = details::static_pow(3, dimension);
		/** the type for vector */
		using ivec_type = glm::vec<dimension, int>;
		/** the type for box */
		using box_type = type_geometric<T, dimension>::box_type;
		/** the type for NodeInfo */
		using node_info_type = Tree27NodeInfo<T, dimension>;

		/** constructor */
		Tree27Node()
		{
			for (int i = 0; i < children_count; ++i)
				children[i] = nullptr;
		}

		/** check whether node can be removed */
		bool CanBeRemoved() const
		{
			// more than one child: can't remove node. it is used as a common parent for 2 children
			if (HasChild() && !HasSingleChild())
				return false;
			// node still useful: can't remove node
			if (IsUseful())
				return false;
			return true;
		}

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
			if constexpr (details::Implement_IsUseful<PARENT>)
				return PARENT::IsUseful();
			return true; // if the PARENT does not provide implementation for IsUseful() this node must be useful (elsewhere, it has no reason to exist)
		}

		/** gets the node info */
		node_info_type GetNodeInfo() const
		{
			return info;
		}

		/** gets the node info */
		box_type GetBoundingBox() const
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

		/** iterate over all children (non recursive) */
		template<typename FUNC>
		decltype(auto) ForEachChildren(FUNC const& func) const
		{
			return ForEachChildrenHelper(this, func);
		}

		/** iterate over all children (non recursive) */
		template<typename FUNC>
		decltype(auto) ForEachChildren(FUNC const& func)
		{
			return ForEachChildrenHelper(this, func);
		}

		/** recursively visit all children */
		template<bool DEPTH_FIRST = false, typename FUNC>
		decltype(auto) Visit(FUNC const& func) const
		{
			return VisitHelper<DEPTH_FIRST>(this, func);
		}

		/** recursively visit all children */
		template<bool DEPTH_FIRST = false, typename FUNC>
		decltype(auto) Visit(FUNC const& func)
		{
			return VisitHelper<DEPTH_FIRST>(this, func);
		}

	protected:

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

	protected:

		/** gets a child by its index */
		Tree27Node* GetChild(size_t index)
		{
			return children[index];
		}
		/** gets a child by its index */
		Tree27Node const * GetChild(size_t index) const
		{
			return children[index];
		}

		/** utility method to get iterate over all children for both CONST and NON-CONST version (non recursive) */
		template<typename SELF, typename FUNC>
		static decltype(auto) ForEachChildrenHelper(SELF* self, FUNC const& func)
		{
			return BitTools::ForEachBitForward(self->existing_children, [&](int index)
			{
				return func(self->GetChild(index));
			});
		}

		/** utility method to get recursively iterate over children for both CONST and NON-CONST version */
		template<bool DEPTH_FIRST, typename SELF, typename FUNC>
		static auto VisitHelper(SELF * self, FUNC const& func) -> meta::LambdaInfo<FUNC, SELF*>::result_type
		{
			using L = meta::LambdaInfo<FUNC, SELF*>;

			for (int i = 0; i < 2; ++i)
			{
				if ((i == 0 && DEPTH_FIRST) || (i == 1 && !DEPTH_FIRST)) // process children
				{
					if constexpr (L::convertible_to_bool)
					{
						decltype(auto) result = self->ForEachChildren([&](SELF * child)
						{
							return child->Visit<DEPTH_FIRST>(func);
						});
						if (result)
							return result;
					}
					else
					{
						self->ForEachChildren([&](SELF* child)
						{
							child->Visit<DEPTH_FIRST>(func);
						});
					}
				}
				else // process this
				{
					if constexpr (L::convertible_to_bool)
					{
						decltype(auto) result = func(self);
						if (result)
							return result;
					}
					else
					{
						func(self);
					}
				}
			}

			if constexpr (L::convertible_to_bool)
				return typename L::result_type{};
		}

	protected:

		/** the node information */
		Tree27NodeInfo<T, dimension> info;
		/** the parent node */
		Tree27Node* parent = nullptr;
		/** the children */
		Tree27Node* children[children_count];
		/** the present children */
		int existing_children = 0;
		/** the index of this node in its parent */
		int index_in_parent = 0;
	};

	CHAOS_GEOMETRY_TEMPLATE(T, dimension, typename NODE_PARENT, template<typename> class NODE_ALLOCATOR_TEMPLATE = StandardAllocator)
	class LooseTree27 : public type_geometric<T, dimension>
	{
	public:

		/** the dimension */
		static constexpr int dimension = dimension;
		/** the number of children this node has */
		static constexpr int children_count = details::static_pow(3, dimension);
		/** the type for indexation vector */
		using ivec_type = glm::vec<dimension, int>;
		/** the type for box */
		using box_type = type_geometric<T, dimension>::box_type;
		/** the type for NodeInfo */
		using node_info_type = Tree27NodeInfo<T, dimension>;
		/** the type for nodes */
		using node_type = Tree27Node<T, dimension, NODE_PARENT>;
		/** the type for allocator */
		using node_allocator_type = NODE_ALLOCATOR_TEMPLATE<node_type>;

		/** destructor */
		~LooseTree27()
		{
			Clear();
		}

		/** destroy the whole hierarchy */
		void Clear()
		{
			Visit<true>([this](node_type * node) // depth first to avoid deleting a node before its children
			{
				DeleteNode(node);
			});
			root = nullptr;
		}

		/** add a node for a given object */
		node_type* GetOrCreateNode(type_box<T, dimension> const& box)
		{
			node_info_type node_info = ComputeTreeNodeInfo(box);
			return DoGetOrCreateNode(node_info, root, nullptr, 0);
		}

		/** try to delete a node (if possible and have at most one child) */
		bool DeleteNodeIfPossible(node_type * node)
		{
			assert(node != nullptr);

			// check whether not is still in use
			if (!node->CanBeRemoved())
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
				DeleteNodeIfPossible(parent_node); // maybe parent node has become useless
			}

			// delete the useless node
			DeleteNode(node);

			return true;
		}

		/** visit the tree */
		template<bool DEPTH_FIRST = false, typename FUNC>
		decltype(auto) Visit(FUNC const& func)
		{
			using L = meta::LambdaInfo<FUNC, node_type *>;

			if (auto* root_node = GetRootNode()) // GetRootNode() is necessary to work with proper constness of the node
				return root_node->Visit<DEPTH_FIRST>(func);

			if constexpr (L::convertible_to_bool)
				return typename L::result_type{};
		}

		/** visit the tree */
		template<bool DEPTH_FIRST = false, typename FUNC>
		decltype(auto) Visit(FUNC const& func) const
		{
			using L = meta::LambdaInfo<FUNC, node_type const*>;

			if (auto* root_node = GetRootNode()) // GetRootNode() is necessary to work with proper constness of the node
				return root_node->Visit<DEPTH_FIRST>(func);

			if constexpr (L::convertible_to_bool)
				return typename L::result_type{};
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

		size_t GetNodeCount() const
		{
			return node_count;
		}

	protected:

		/** allocate a node */
		node_type* AllocateNode()
		{
			if (node_type* result = node_allocator.Allocate())
			{
				++node_count;
				return result;
			}
			return nullptr;
		}

		/** destroy the node */
		void DeleteNode(node_type* node)
		{
			assert(node != nullptr);
			--node_count;
			node_allocator.Free(node);
		}

		/** internal recursive method to create a node and insert it into the tree */
		node_type* DoGetOrCreateNode(node_info_type const & node_info, node_type* current_node, node_type * parent_node, int index_in_parent)
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
			int child_index1 = current_node->GetNodeInfo().GetDescendantIndex(node_info);
			if (child_index1 >= 0)
			{
				return DoGetOrCreateNode(node_info, current_node->children[child_index1], current_node, child_index1);
			}
			// the current node is contained by the node we want to create
			int child_index2 = node_info.GetDescendantIndex(current_node->GetNodeInfo());
			if (child_index2 >= 0)
			{
				node_type* result = DoAddNodeToParent(node_info, parent_node, index_in_parent);
				if (result != nullptr)
					result->SetChild(child_index2, current_node);
				return result;
			}
			// must create a common parent for current node and new node
			node_info_type common_parent_info = ComputeCommonParent(node_info, current_node->GetNodeInfo());

			if (node_type* common_parent = DoAddNodeToParent(common_parent_info, parent_node, index_in_parent))
			{
				int child_index3 = common_parent_info.GetDescendantIndex(current_node->GetNodeInfo());
				assert(child_index3 >= 0);
				common_parent->SetChild(child_index3, current_node);

				int child_index4 = common_parent_info.GetDescendantIndex(node_info);
				assert(child_index4 >= 0);
				assert(child_index4 != child_index3);
				return DoGetOrCreateNode(node_info, common_parent->children[child_index4], common_parent, child_index4);
			}

			return nullptr;
		}

		/** create a node and insert it into its parent or set it as root */
		node_type* DoAddNodeToParent(node_info_type const& node_info, node_type* parent_node, int index_in_parent)
		{
			if (node_type* result = AllocateNode())
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
		/** the node allocator */
		node_allocator_type node_allocator;
		/** the number of nodes */
		size_t node_count = 0;
	};


#endif

}; // namespace chaos