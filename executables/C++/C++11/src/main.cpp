#include <chaos/chaos.h>

// ---------------------------------------------------------------------

class ConfigurableInterface;
class ObjectConfiguration;

// ---------------------------------------------------------------------

template<typename T>
class ObjectPool64
{

public:

	using type = T;

	static constexpr size_t pool_size = 64;

	/** constructor */
	ObjectPool64() = default;
	/** no copy constructor */
	ObjectPool64(ObjectPool64 const& src) = delete;
	/** no copy operator */
	ObjectPool64& operator = (ObjectPool64 const& src) = delete;

	/** destructor */
	~ObjectPool64()
	{
		// destroy all objects
		chaos::BitTools::ForEachBitForward(used_instanced, [this](int64_t index)
		{
			Free(GetObjectPtr(index));
		});
	}

	/** release an object inside the pool for further usage */
	void Free(type* object)
	{
		if (object != nullptr)
		{
			assert(IsObjectInsidePool(object));
			// update the available flag
			int64_t index = GetObjectIndex(object);
			assert((used_instanced >> index) & 1); // ensure the object was not already freed
			used_instanced = chaos::BitTools::SetBit(used_instanced, index, false);
			--reserved_count;
			// manually call the destructor
			object->~type();
		}
	}

	/** allocate a new object from pool */
	template<typename ...PARAMS>
	type* Allocate(PARAMS ...params)
	{
		if (!HasAvailableInstanceLeft())
			return nullptr;
		// update the available flag
		int64_t index = chaos::BitTools::bsr(~used_instanced);
		used_instanced = chaos::BitTools::SetBit(used_instanced, index, true);
		++reserved_count;
		// manually call constructor
		type* result = GetObjectPtr(index);
		new (result) type(std::forward<PARAMS>(params)...);
		return result;
	}

	/** check whether an object is inside the pool */
	bool IsObjectInsidePool(type const* object) const
	{
		assert(object != nullptr);
		return (object >= GetObjectPtr(0)) && (object <= GetObjectPtr(pool_size - 1));
	}

	/** returns true whether all instanced have allready been allocated */
	bool HasAvailableInstanceLeft() const
	{
		return (used_instanced != int64_t(-1));
	}

	/** gets the number of reserved object */
	size_t GetReservedCount() const
	{
		return reserved_count;
	}

protected:

	/** gets the index of an object inside the pool */
	int64_t GetObjectIndex(type const* object) const
	{
		assert(object != nullptr);
		return int64_t(object - GetObjectPtr(0));
	}

	/** gets the address of an object inside the pool */
	T* GetObjectPtr(int64_t index) const
	{
		assert(index >= 0);
		assert(index < pool_size);
		return ((type*)data) + index;
	}

protected:

	/** a bitfield indicating with instances are in use */
	int64_t used_instanced = 0;
	/** number of reserved object */
	size_t reserved_count = 0;
	/** the block of data where instanced are being used */
	alignas(8) char data[pool_size * sizeof(T)];
};

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
				if (max_unused_node_count > 0 && unused_node_count > max_unused_node_count && node->GetReservedCount() == 0)
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
	void SetMaxUnusedNodeCount(size_t count)
	{
		max_unused_node_count = count;
		if (max_unused_node_count > 0)
		{
			while (unused_node_count > max_unused_node_count)
			{
				delete(ExtractFirstNode(unused_nodes));
				--unused_node_count;
			}
		}
	}

	/** get the maximum number of unused nodes */
	size_t GetMaxUnusedNodeCount() const
	{
		return unused_node_count;
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
	/** maximum number of unused nodes (0 for unlimited) */
	size_t max_unused_node_count = 0;
	/** number of unused nodes */
	size_t unused_node_count = 0;
};












// ---------------------------------------------------------------------



// ---------------------------------------------------------------------
class B : public chaos::Object, public chaos::ConfigurableInterface
{
public:

	virtual ~B()
	{
		int i = 0;
		++i;
	}

	void Initialize()
	{
		chaos::ObjectConfiguration* conf = GetConfiguration();

		conf = conf;

	}

	virtual void OnConfigurationChanged()
	{
		int i = 0;
		++i;

	}

};




class A : public chaos::Object, public chaos::ConfigurableInterface
{
public:

	virtual ~A()
	{
		int i = 0;
		++i;
	}

	void Initialize()
	{
		chaos::ObjectConfiguration* conf = GetConfiguration();

		conf = conf;

		b = new B;
		b->SetConfiguration(conf->CreateChildConfiguration("B"));
	}

	virtual void OnConfigurationChanged()
	{
		int i = 0;
		++i;

		b = nullptr;
	}

	chaos::shared_ptr<B> b;

};


class G
{
public:

	G(int a, int b, int c)
	{
		a = a;
	}

	~G()
	{
		int i = 0;
		++i;
	}

};

int main(int argc, char ** argv, char ** env)
{
	std::vector<G*> v;

	ObjectPool<G> pool;
	pool.SetMaxUnusedNodeCount(1);

	for (int i = 0; i < 200; ++i)
	{
		G* ptr = pool.Allocate(1,2,3);

		v.push_back(ptr);

		//pool.Free(ptr);
		if (ptr == nullptr)
			i = i;
		else
			i = i;


	}

	int ii = 0;
	for (G* g : v)
	{
		if (++ii > 100)
			break;
		pool.Free(g);
	}

	pool.SetMaxUnusedNodeCount(1);


	argc = argc;


#if 0


	ObjectPoolNode<G> pool_node;

	auto s = sizeof(pool_node);
	auto ss = pool_node.pool_size;


	for (int j = 0; j < 60; ++j)
	{

		G* i = pool_node.Allocate(5, 6, 7);
		if (i == nullptr)
			i = i;
	}

	G* i = pool_node.Allocate(9, 10, 11);

	pool_node.Free(i);
	pool_node.Free(i);

	G gg(1, 2, 3);
	pool_node.Free(&gg);
#endif


	return 0;


#if 0




















	chaos::Tree27Node<2, chaos::EmptyClass> Node;

	Node.ForEachNode([](auto ptr)
	{
			ptr = ptr;
			//return 0;
	});



	struct TOTO
	{
		int index_in_parent : 5 = 0;
		int children_flags : 27;
	};

	TOTO toto;
	toto.index_in_parent = 27;
	toto.children_flags = 0b0001011;

	chaos::BitTools::ForEachBitForward(toto.children_flags, [](int bit)
		{
			bit = bit;
		});

	auto s = sizeof(TOTO);


	chaos::ObjectConfiguration conf;

	chaos::Tree27NodeInfo<2> NN;

	chaos::shared_ptr<A> a = new A;


	a->SetConfiguration(conf.CreateChildConfiguration("A"));
	a->Initialize();

	conf.OnConfigurationChanged();

	a = nullptr;
#endif
	return 0;
}