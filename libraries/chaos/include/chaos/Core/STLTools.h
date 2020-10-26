#ifdef CHAOS_FORWARD_DECLARATION

namespace chaos
{
	namespace STLTools
	{
	}; // namespace STLTools
	
}; // namespace chaos
	
#else	
	
namespace chaos
{
	namespace STLTools
	{
		namespace details
		{
			template<typename VECTOR, typename IT, typename VALUE, typename COMPARE>
			IT FindSortedVector(VECTOR& v, IT b, IT e, VALUE value, COMPARE compare)
			{
				// early exit
				if (e < b)
					return v.end();

				// check for the middle
				IT it = b + (e - b) / 2;

				auto check = compare(*it, value);
				if (check == 0)
					return it;
				else if (check > 0)
				{
					if (v.end() - it <= 1) // it + 1 is out of range
						return v.end();
					return FindSortedVector(v, it + 1, e, value, compare);
				}
				else
				{ 
					if (it == v.begin()) // it - 1 is out of range
						return v.end();
					return FindSortedVector(v, b, it - 1, value, compare);
				}
			}

		}; // namespace details

		/** utility function that search in a sorted vector an element based on a value and a compare function that gives whether the direction to search next (0 : found, < 0 : before, > 0 : next) */
		template<typename VECTOR, typename VALUE, typename COMPARE>
		auto FindSortedVector(VECTOR& v, VALUE value, COMPARE compare)
		{
			if (v.begin() == v.end())
				return v.end();
			return details::FindSortedVector(v, v.begin(), v.end(), value, compare);
		}

	}; // namespace STLTools

}; // namespace chaos

#endif // CHAOS_FORWARD_DECLARATION