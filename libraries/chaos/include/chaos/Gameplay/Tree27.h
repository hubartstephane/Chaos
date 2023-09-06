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
	// let Node(0)(L) be the node centered to the origin for level L
	//
	// Node(X)(L) contains Node(3X - 1)(L - 1)
	//                     Node(3X)    (L - 1)
	//                     Node(3X + 1)(L - 1)
	//
	// Distance Centers (L) = (2/3).Width(L)
	//
	// Left Corner (X)(L) =  (2/3).X.Width(L) - (1/2).Width(L)
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
		float log3(float value);
		/** compute pow 3 */
		float pow3(float level);
	};

	template<int dimension>
	class Tree27NodeInfo
	{
	public:

		/** compute the bounding box of the node */
		type_box<float, dimension> GetBox() const
		{
			type_box<float, dimension> result;

			float width = details::pow3(float(level));

			for (int i = 0; i < dimension; ++i)
			{
				result.half_size[i] = width * 0.5f;
				result.position[i] = (2.0f / 3.0f) * float(position[i]) * width;
			}
			return result;
		}

	public:

		/** the level in the hierarchy of this node */
		int level = 0;
		/** the position of this node in space (indices) */
		type_geometric<int, dimension>::vec_type position;
	};

	template<int dimension, typename PARENT>
	class Tree27Node : public PARENT
	{
	public:

		/** the number of children this node has */
		static constexpr size_t children_count = details::static_pow(3, dimension);

		/** constructor */
		Tree27Node()
		{
			for (size_t i = 0; i < children_count; ++i)
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

	public:

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

	template<int dimension>
	Tree27NodeInfo<dimension> ComputeTreeNodeInfo(type_box<float, dimension> const& box)
	{
		Tree27NodeInfo<dimension> result;

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
		for (int i = 0; i < dimension; ++i)
		{
			float num   = (box.position[i] - box.half_size[i]) + (1.0f / 2.0f) * widthL;
			result.position[i] = int(std::floor(num / denum));
		}

		return result;
	}

	template<int dimension, typename NODE_PARENT>
	class Tree27
	{
	public:

		/** the type for nodes */
		using node_type = Tree27Node<dimension, NODE_PARENT>;

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
		node_type* AddNodeFor(type_box<float, dimension> const& box)
		{
			Tree27NodeInfo<dimension> node_info = ComputeTreeNodeInfo(box);







			return nullptr;
		}



		/** visit the tree */
		template<bool DEPTH_FIRST = false, typename FUNC>
		decltype(auto) ForEachNode(FUNC const& func)
		{
			using result_type = decltype(func(0));
			constexpr bool convertible_to_bool = std::is_convertible_v<result_type, bool>;

			if (root != nullptr)
				return root->ForEachNode<DEPTH_FIRST>(func);

			if constexpr (convertible_to_bool)
				return result_type();
		}

	protected:

		/** the root node */
		node_type* root = nullptr;
	};


#endif

}; // namespace chaos