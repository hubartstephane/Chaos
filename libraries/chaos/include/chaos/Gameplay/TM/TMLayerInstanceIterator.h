namespace chaos
{
#if !defined CHAOS_FORWARD_DECLARATION && !defined CHAOS_TEMPLATE_IMPLEMENTATION

	template<typename CONSTNESS_OPERATOR> // should be add_const<_1> or identity<_1>
	class /*CHAOS_API*/ TMLayerInstanceIteratorBase
	{
	public:

		using layer_type  = typename boost::mpl::apply<CONSTNESS_OPERATOR, TMLayerInstance>::type;
		using level_type  = typename boost::mpl::apply<CONSTNESS_OPERATOR, TMLevelInstance>::type;
		using object_type = typename boost::mpl::apply<CONSTNESS_OPERATOR, Object>::type;

		/** default constructor */
		TMLayerInstanceIteratorBase() = default;
		/** copy constructor */
		TMLayerInstanceIteratorBase(TMLayerInstanceIteratorBase const& src) = default;
		/** constructor from a level instance */
		TMLayerInstanceIteratorBase(level_type* in_level_instance, uint64_t in_collision_mask = 0) :
			search_root(in_level_instance),
			collision_mask(in_collision_mask)
		{
			assert(in_level_instance != nullptr);
			FindElement(false);
		}

		/** constructor from a level instance */
		TMLayerInstanceIteratorBase(layer_type* in_layer_instance, uint64_t in_collision_mask = 0) :
			search_root(in_layer_instance),
			current_li(in_layer_instance),
			collision_mask(in_collision_mask)
		{
			assert(in_layer_instance != nullptr);
			FindElement(false);
		}

		/** comparaison operator */
		friend CHAOS_API bool operator == (TMLayerInstanceIteratorBase const& src1, TMLayerInstanceIteratorBase const& src2)
		{
			// pointing on different origin/vectors (maybe one is null)
			if (src1.search_root != src2.search_root)
				return false;
			// both null, both finished
			if (src1.search_root == nullptr)
				return true;
			// pointing on the same element
			if (src1.cached_result == src2.cached_result)
				return true;
			return false;
		}

		/** returns whether the iterator is still valid */
		operator bool() const
		{
			return (search_root != nullptr);
		}

		/** indirection method */
		layer_type& operator *() const
		{
			assert(search_root != nullptr); // end already reached. cannot indirect
			return *cached_result;
		}

		/** indirection method */
		layer_type* operator ->() const
		{
			assert(search_root != nullptr); // end already reached. cannot indirect
			return cached_result;
		}

		/** pre increment iterator */
		TMLayerInstanceIteratorBase& operator ++ ()
		{
			FindElement(true);
			return *this;
		}

		/** post increment iterator */
		TMLayerInstanceIteratorBase operator ++ (int i) const
		{
			TMLayerInstanceIteratorBase result = *this;
			++(*this);
			return result;
		}

protected:

		/** make the iterator points on the first layer instance */
		void FindElement(bool ignore_first)
		{
			assert(search_root != nullptr);

			// get list being iterated over (it comes from a layer/level instance)
			std::vector<shared_ptr<TMLayerInstance>> const* lis = nullptr;

			if (current_li != nullptr)
				lis = &current_li->GetLayerInstances();
			else
			{
				level_type* level_instance = auto_cast(search_root);
				lis = &level_instance->GetLayerInstances();
			}

			while (search_root != nullptr)
			{
				// iterate
				while (child_li_index < lis->size())
				{
					layer_type* li = (*lis)[child_li_index].get();
					if (li != nullptr)
					{
						// found ?
						if ((collision_mask == 0) || (li->GetCollisionMask() & collision_mask) != 0)
						{
							if (!ignore_first) // ignore first is usefull for ++ operator
							{
								cached_result = li;
								return;
							}
							ignore_first = false;
						}
						// go in depth first
						if (li->GetLayerInstances().size() > 0)
						{
							current_li = li;
							lis = &current_li->GetLayerInstances();
							child_li_index = 0;
							continue;
						}
					}
					// next sibling
					++child_li_index;
				}

				// end of iteration (cannot go up)
				if (current_li == nullptr || current_li == search_root)
				{
					*this = {};
				}
				// parent-sibling
				else
				{
					layer_type* tmp = current_li;

					std::vector<shared_ptr<TMLayerInstance>> const* parent_lis = nullptr;

					layer_type * parent_li = current_li->GetParentLayerInstance();
					if (parent_li != nullptr)
					{
						parent_lis = &parent_li->GetLayerInstances();
						current_li = parent_li;
					}
					else
					{
						parent_lis = &current_li->GetLevelInstance()->GetLayerInstances();
						current_li = nullptr;
					}

					// search the index of parent sibling
					for (size_t i = 0; i < parent_lis->size(); ++i)
					{
						if ((*parent_lis)[i] == tmp)
						{
							child_li_index = i + 1; // +1 for sibling
							break;
						}
					}

					lis = parent_lis;
				}
			}
		}

	protected:

		/** the object where we start the iteration (level_instance or layer_instance) */
		object_type* search_root = nullptr;
		/** the currently considered layer_instance */
		layer_type* current_li = nullptr;
		/** the index in the current child layer lists */
		size_t child_li_index = 0;
		/** how to filter out element */
		uint64_t collision_mask = 0;

		/** the result of the search */
		layer_type* cached_result = nullptr;
	};

	using TMLayerInstanceIterator = TMLayerInstanceIteratorBase<boost::mpl::identity<boost::mpl::_1>>;

	using TMLayerInstanceConstIterator = TMLayerInstanceIteratorBase<boost::add_const<boost::mpl::_1>>;

#endif

}; // namespace chaos

