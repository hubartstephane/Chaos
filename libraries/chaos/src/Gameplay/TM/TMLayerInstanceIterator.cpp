#include <chaos/Chaos.h>

namespace chaos
{
	TMLayerInstanceIterator::TMLayerInstanceIterator(TMLevelInstance const* in_level_instance, uint64_t in_collision_mask):
		search_root(in_level_instance),
		collision_mask(in_collision_mask)
	{
		assert(in_level_instance != nullptr);
		FindFirstLayerInstance(false);
	}
	
	TMLayerInstanceIterator::TMLayerInstanceIterator(TMLayerInstance const* in_layer_instance, uint64_t in_collision_mask) :
		search_root(in_layer_instance),
		current_li(in_layer_instance),
		collision_mask(in_collision_mask)
	{
		assert(in_layer_instance != nullptr);
		FindFirstLayerInstance(false);
	}

	bool operator == (TMLayerInstanceIterator const& src1, TMLayerInstanceIterator const& src2)
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

	bool operator != (TMLayerInstanceIterator const& src1, TMLayerInstanceIterator const& src2)
	{
		return !(src1 == src2);
	}


	void TMLayerInstanceIterator::FindFirstLayerInstance(bool ignore_first)
	{
		assert(search_root != nullptr);

		// get list being iterated over (it comes from a layer/level instance)
		std::vector<shared_ptr<TMLayerInstance>> const* lis = nullptr;

		if (current_li != nullptr)
			lis = &current_li->layer_instances;
		else
		{
			TMLevelInstance const* level_instance = auto_cast(search_root);
			lis = &level_instance->layer_instances;
		}

		while (search_root != nullptr)
		{
			// iterate
			while (child_li_index < lis->size())
			{
				TMLayerInstance const* li = (*lis)[child_li_index].get();
				if (li != nullptr)
				{
					// found ?
					if ((collision_mask == 0) || (li->collision_mask & collision_mask) != 0)
					{
						if (!ignore_first) // ignore first is usefull for ++ operator
						{
							cached_result = li;
							return;
						}
						ignore_first = false;
					}
					// go in depth first
					if (li->layer_instances.size() > 0)
					{
						current_li = li;
						lis = &current_li->layer_instances;
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
				TMLayerInstance const* tmp = current_li;

				std::vector<shared_ptr<TMLayerInstance>> const* parent_lis = nullptr;

				if (current_li->parent_layer != nullptr)
				{
					parent_lis = &current_li->parent_layer->layer_instances;
					current_li = current_li->parent_layer.get();
				}
				else
				{
					parent_lis = &current_li->level_instance->layer_instances;
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

	TMLayerInstanceIterator::operator bool() const
	{
		return (search_root != nullptr);
	}

	TMLayerInstance const & TMLayerInstanceIterator::operator *() const
	{
		assert(search_root != nullptr); // end already reached. cannot indirect
		return *cached_result;
	}

	TMLayerInstance const * TMLayerInstanceIterator::operator ->() const
	{
		assert(search_root != nullptr); // end already reached. cannot indirect
		return cached_result;
	}

	TMLayerInstanceIterator& TMLayerInstanceIterator::operator ++ ()
	{
		FindFirstLayerInstance(true);
		return *this;
	}

	TMLayerInstanceIterator TMLayerInstanceIterator::operator ++ (int i)
	{
		TMLayerInstanceIterator result = *this;
		++(*this);
		return result;
	}

}; // namespace chaos
