#ifdef CHAOS_FORWARD_DECLARATION

#else

namespace chaos
{
	class TMLayerInstanceIterator
	{
	public:

		/** default constructor */
		TMLayerInstanceIterator() = default;
		/** copy constructor */
		TMLayerInstanceIterator(TMLayerInstanceIterator const& src) = default;
		/** constructor from a level instance */
		TMLayerInstanceIterator(TMLevelInstance const* in_level_instance, uint64_t in_collision_mask = 0);
		/** constructor from a level instance */
		TMLayerInstanceIterator(TMLayerInstance const* in_layer_instance, uint64_t in_collision_mask = 0);

		/** returns whether the iterator is still valid */
		operator bool() const;

		// pre increment iterator
		TMLayerInstanceIterator& operator ++ ();
		// post increment iterator
		TMLayerInstanceIterator operator ++ (int i);

		// indirection
		TMLayerInstance const& operator *() const;
		// indirection
		TMLayerInstance const* operator ->() const;

		/** comparaison operator */
		friend bool operator == (TMLayerInstanceIterator const& src1, TMLayerInstanceIterator const& src2);
		/** comparaison operator */
		friend bool operator != (TMLayerInstanceIterator const& src1, TMLayerInstanceIterator const& src2);

	protected:

		/** make the iterator points on the first layer instance */
		void FindFirstLayerInstance(bool ignore_first);

	protected:

		/** the object where we start the iteration (level_instance or layer_instance) */
		Object const* search_root = nullptr;
		/** the currently considered layer_instance */
		TMLayerInstance const* current_li = nullptr;
		/** the index in the current child layer lists */
		size_t child_li_index = 0;
		/** how to filter out element */
		uint64_t collision_mask = 0;

		/** the result of the search */
		TMLayerInstance const* cached_result = nullptr;
	};


}; // namespace chaos

#endif // CHAOS_FORWARD_DECLARATION