namespace chaos
{
#ifdef CHAOS_FORWARD_DECLARATION



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
	};


	template<int dimension>
	class Tree27NodeInfo
	{
	public:

		/** the level in the hierarchy of this node */
		int level = 0;
		/** the position of this node in space */
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
	Tree27NodeInfo<dimension> ComputeTreeNodeInfo(typename type_geometric<float, dimension>::box_type const& box)
	{
		Tree27NodeInfo<dimension> result;



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