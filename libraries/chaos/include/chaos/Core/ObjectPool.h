namespace chaos
{
#ifdef CHAOS_FORWARD_DECLARATION

	template<typename T>
	class ObjectPool;

#elif !defined CHAOS_TEMPLATE_IMPLEMENTATION

	/**
	* This is an allocator that use in internal ObjectPool64 
	**/
	template<typename T>
	class ObjectPool
	{
	protected:

		/** double linked list for pool class */
		template<typename U>
		class ObjectPool64Node : public ObjectPool64<U>
		{
			template<typename V>
			friend class ObjectPool;

			using node_type = ObjectPool64Node<U>;

		protected:

			/** the next pool node in the pool */
			node_type* previous_node = nullptr;
			/** the previous pool node in the pool */
			node_type* next_node = nullptr;
		};

	public:

		using type = T;
		using node_type = ObjectPool64Node<T>;

		/** constructor */
		ObjectPool() = default;
		/** no copy constructor */
		ObjectPool(ObjectPool const& src) = delete;
		/** no copy operator */
		ObjectPool& operator = (ObjectPool const& src) = delete;

		/** destructor */
		~ObjectPool()
		{
			while (node_type* node = ExtractFirstNode(used_nodes))
				delete(node);
			while (node_type* node = ExtractFirstNode(unavailable_nodes))
				delete(node);
		}

		/** release an object inside the pool for further usage */
		void Free(type* object)
		{
			if (object != nullptr)
			{
				if (node_type* node = SearchOwningNode(used_nodes, object))
				{
					if (node->GetReservedCount() == 1) // the last object is about to be removed from the node. the node now belongs to unused
					{
						ExtractNode(used_nodes, node);
						InsertNode(unused_nodes, node);
						++unused_node_count;
					}
					node->Free(object);

					// does this node deserve to be destroyed ?
					if (max_unused_node_count.has_value() && unused_node_count > max_unused_node_count.value() && node->GetReservedCount() == 0)
					{
						ExtractNode(unused_nodes, node);
						delete(node);
						--unused_node_count;
					}
				}
				else if (node_type* node = SearchOwningNode(unavailable_nodes, object))
				{
					ExtractNode(unavailable_nodes, node); // now, the node has a single available entry. it belongs to used_nodes
					InsertNode(used_nodes, node);
					node->Free(object);
				}
				else
				{
					assert(0); // object does not belong to this pool
				}
			}
		}

		/** allocate a new object from pool */
		template<typename ...PARAMS>
		type* Allocate(PARAMS ...params)
		{
			// try nodes used_nodes then unused_nodes (we want to keep unused_nodes untouched as long as possible)
			if (used_nodes == nullptr)
			{
				// can use an unused_nodes
				if (unused_nodes != nullptr)
				{
					InsertNode(used_nodes, ExtractFirstNode(unused_nodes));
					--unused_node_count;
				}
				// need a new node
				else if (node_type* new_node = new node_type)
				{
					InsertNode(used_nodes, new_node);
				}
				// failure
				else
					return nullptr;
			}

			assert(used_nodes != nullptr);

			// allocate the object
			if (type* result = used_nodes->Allocate(std::forward<PARAMS>(params)...))
			{
				// maybe the used_nodes has no more instance available. displace node to appropriate list
				if (!used_nodes->HasAvailableInstanceLeft())
					InsertNode(unavailable_nodes, ExtractFirstNode(used_nodes));
				return result;
			}
			return nullptr;
		}

		/** change the maximum number of unused nodes */
		void SetMaxUnusedNodeCount(std::optional<size_t> count)
		{
			max_unused_node_count = count;
			if (max_unused_node_count.has_value())
			{
				while (unused_node_count > max_unused_node_count.value())
				{
					delete(ExtractFirstNode(unused_nodes));
					--unused_node_count;
				}
			}
		}

		/** get the maximum number of unused nodes */
		std::optional<size_t> GetMaxUnusedNodeCount() const
		{
			return max_unused_node_count;
		}

		/** iterator over all objects (const version) */
		template<typename FUNC>
		decltype(auto) ForEachObject(FUNC const& func) const
		{
			using L = meta::LambdaInfo<FUNC, T const*>;

			for (node_type const* node : { used_nodes , unavailable_nodes })
			{
				while (node != nullptr)
				{
					if constexpr (L::convertible_to_bool)
					{
						if (decltype(auto) result = node->ForEachObject(func))
							return result;
					}
					else
					{
						node->ForEachObject(func);
					}
					node = node->next_node;
				}
			}

			if constexpr (L::convertible_to_bool)
				return typename L::result_type{};
		}

		/** iterator over all objects (non const version) */
		template<typename FUNC>
		decltype(auto) ForEachObject(FUNC const& func)
		{
			using L = meta::LambdaInfo<FUNC, T*>;

			for (node_type* node : { used_nodes , unavailable_nodes })
			{
				while (node != nullptr)
				{
					if constexpr (L::convertible_to_bool)
					{
						if (decltype(auto) result = node->ForEachObject(func))
							return result;
					}
					else
					{
						node->ForEachObject(func);
					}
					node = node->next_node;
				}
			}

			if constexpr (L::convertible_to_bool)
				return typename L::result_type{};
		}

	protected:

		/** remove a node from the list */
		void ExtractNode(node_type*& root, node_type* node)
		{
			assert(node != nullptr);
			// unlink with siblings
			if (node->previous_node != nullptr)
				node->previous_node->next_node = node->next_node;
			if (node->next_node != nullptr)
				node->next_node->previous_node = node->previous_node;
			// change root if necessary
			if (root == node)
				root = node->next_node;
			// clear data
			node->previous_node = node->next_node = nullptr;
		}

		/** search a node that contains the object */
		node_type* SearchOwningNode(node_type* node, type const* object)
		{
			while (node != nullptr)
			{
				if (node->IsObjectInsidePool(object))
					return node;
				node = node->next_node;
			}
			return nullptr;
		}

		/** extract the first node (if any) of a given list */
		node_type* ExtractFirstNode(node_type*& root)
		{
			if (node_type* result = root)
			{
				if (result->next_node != nullptr)
					result->next_node->previous_node = nullptr;
				root = result->next_node;
				result->next_node = result->previous_node = nullptr;
				return result;
			}
			return nullptr;
		}

		/** insert a node into a given list */
		void InsertNode(node_type*& root, node_type* node)
		{
			assert(node != nullptr);
			assert(node->next_node == nullptr);
			assert(node->previous_node == nullptr);
			if (root != nullptr)
			{
				root->previous_node = node;
				node->next_node = root;
			}
			root = node;
		}

	protected:

		/** nodes that have some entries available (but not all) */
		node_type* used_nodes = nullptr;
		/** nodes that have no entries available */
		node_type* unavailable_nodes = nullptr;
		/** nodes that aren't used at all (all entries available) */
		node_type* unused_nodes = nullptr;
		/** maximum number of unused nodes */
		std::optional<size_t> max_unused_node_count;
		/** number of unused nodes */
		size_t unused_node_count = 0;
	};

#endif

}; // namespace chaos

