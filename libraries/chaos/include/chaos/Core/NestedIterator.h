namespace chaos
{
#ifdef CHAOS_FORWARD_DECLARATION

	template<typename OUTER_ITERATOR, typename INNER_ITERATOR, std::pair<INNER_ITERATOR, INNER_ITERATOR>(*GetInnerIterators)(OUTER_ITERATOR)>
	class NestedIterator;

#elif !defined CHAOS_TEMPLATE_IMPLEMENTATION

	template<typename OUTER_ITERATOR, typename INNER_ITERATOR, std::pair<INNER_ITERATOR, INNER_ITERATOR>(*GetInnerIterators)(OUTER_ITERATOR)>
	class /*CHAOS_API*/ NestedIterator
	{
	public:

		/** default constructor */
		NestedIterator() = default;
		/** copy constructor */
		NestedIterator(NestedIterator const& src) = default;
		/** constructor */
		NestedIterator(OUTER_ITERATOR it, OUTER_ITERATOR end_it = {}) :
			outer_it(it),
			outer_end_it(end_it)
		{
			// start the search
			if (outer_it != outer_end_it)
			{
				std::tie(inner_it, inner_end_it) = GetInnerIterators(outer_it);
				FindElement(false);
			}
			// finalize the iterators.
			// make sure that an iterator default constructed, is equal to an iterator that reached 'end'
			else
			{
				*this = {};
			}
		}

		/** comparaison operator */
		friend CHAOS_API bool operator == (NestedIterator const& src1, NestedIterator const& src2)
		{
			return (src1.outer_it == src2.outer_it) && (src1.inner_it == src2.inner_it);
		}

		/** pre increment iterator */
		NestedIterator& operator ++()
		{
			FindElement(true);
			return *this;
		}

		/** post increment iterator */
		NestedIterator operator ++(int) const
		{
			NestedIterator result = *this;
			++(*this);
			return result;
		}

		/** returns whether the iterator is still valid */
		operator bool() const
		{
			return (outer_it != outer_end_it);
		}

		/** indirection method */
		auto& operator *() const
		{
			assert(outer_it != outer_end_it); // end already reached. cannot indirect
			return *inner_it;
		}

		/** indirection method */
		auto* operator ->() const
		{
			assert(outer_it != outer_end_it); // end already reached. cannot indirect
			return *inner_it;
		}

	protected:

		/** utility to iterate over elements */
		void FindElement(bool ignore_first)
		{
			assert(outer_it != outer_end_it); // end already reached ?
			while (true)
			{
				// inner loop
				while (inner_it != inner_end_it)
				{
					if (!ignore_first)
						return;
					++inner_it;
					ignore_first = false;
				}
				++outer_it;
				// finalize the iterator if end is reached
				if (outer_it == outer_end_it)
				{
					*this = {};
					return;
				}
				// get the inner iterators
				std::tie(inner_it, inner_end_it) = GetInnerIterators(outer_it);
			}
		}

	protected:

		/** the outer current iterator */
		OUTER_ITERATOR outer_it;
		/** the outer end iterator */
		OUTER_ITERATOR outer_end_it;

		/** the inner current iterator */
		INNER_ITERATOR inner_it;
		/** the inner end iterator */
		INNER_ITERATOR inner_end_it;
	};

#endif

}; // namespace chaos