#include "chaos/Chaos.h"

// https://www.youtube.com/watch?v=pP15kDeXJU0
// https://www.youtube.com/watch?v=kSWfushlvB8

template <typename T, size_t COUNT, typename FALLBACK_ALLOCATOR = std::allocator<T>>
class InplaceAllocator
{
public:
	typedef size_t size_type;
	typedef ptrdiff_t difference_type;
	typedef T* pointer;
	typedef const T* const_pointer;
	typedef T& reference;
	typedef const T& const_reference;
	typedef T value_type;

	/** default constructor */
	InplaceAllocator(FALLBACK_ALLOCATOR const& in_fallback_allocator = {}) :
		fallback_allocator(in_fallback_allocator) {}
	/** copy constructor */
	InplaceAllocator(InplaceAllocator const& src) :
		fallback_allocator(src.fallback_allocator)
	{}
	/** constructor for an instance of allocator with another class */
	template <typename OTHER_T, size_t OTHER_COUNT, typename OTHER_FALLBACK_ALLOCATOR>
	InplaceAllocator(InplaceAllocator<OTHER_T, OTHER_COUNT, OTHER_FALLBACK_ALLOCATOR> const& src) :
		fallback_allocator(src.GetFallbackAllocator())
	{}

	/** copy operator */
	InplaceAllocator<T, COUNT, FALLBACK_ALLOCATOR>& operator=(InplaceAllocator<T, COUNT, FALLBACK_ALLOCATOR> const& src)
	{
		fallback_allocator = src.fallback_allocator;
		return *this;
	}
	/** copy operator for an instance of allocator with another class */
	template <typename OTHER_T, size_t OTHER_COUNT, typename OTHER_FALLBACK_ALLOCATOR>
	InplaceAllocator& operator=(InplaceAllocator<OTHER_T, OTHER_COUNT, OTHER_FALLBACK_ALLOCATOR> const& src)
	{
		fallback_allocator = src.GetFallbackAllocator();
		return *this;
	}

	/** reserve some memory */
	pointer allocate(size_type count)
	{
		if (count <= COUNT)
			return (pointer)buffer;
		auto result = fallback_allocator.allocate(count);


		return result;
	}

	/** free memory */
	void deallocate(pointer p, size_type count)
	{
		if (p == (pointer)buffer)
			return;
		fallback_allocator.deallocate(p, count);
	}


	template<typename U, typename... ARGS>
	void construct(U* p, ARGS && ...args)
	{
		new (p) U(std::forward<ARGS>(args)...); // in place allocation
	}

	/** destroy an element allocated */
	void destroy(pointer p)
	{
		p->~T();
	}

	/** maximum capacity of the allocator */
	size_type max_size() const
	{
		return std::max(
			COUNT,
			typename std::allocator_traits<FALLBACK_ALLOCATOR>::max_size(fallback_allocator)
		);
	}

	/** convert adresses */
	pointer       address(reference x) const { return &x; }
	const_pointer address(const_reference x) const { return &x; }

	/** gets the fallback allocator */
	FALLBACK_ALLOCATOR const& GetFallbackAllocator() const
	{
		return fallback_allocator;
	}

	/** rebind the alocator */
	template<typename OTHER_T>
	struct rebind
	{
		using other = InplaceAllocator<
			OTHER_T,
			COUNT,
			typename std::allocator_traits<FALLBACK_ALLOCATOR>::template rebind_alloc<OTHER_T>
		>;
	};

protected:

	/** internal buffer */
	char buffer[COUNT * sizeof(T)];
	/** the fallback allocator */
	FALLBACK_ALLOCATOR fallback_allocator;

};


template <typename T, size_t COUNT, typename FALLBACK_ALLOCATOR, typename OTHER_T, size_t OTHER_COUNT, typename OTHER_FALLBACK_ALLOCATOR>
bool operator==(InplaceAllocator<T, COUNT, FALLBACK_ALLOCATOR> const& left, InplaceAllocator<OTHER_T, OTHER_COUNT, OTHER_FALLBACK_ALLOCATOR> const& right)
{
	return false;
}







int main(int argc, char** argv, char** env)
{
	std::vector<int, InplaceAllocator<int, 15>> v;
	v.push_back(3);
	v.push_back(4);
	v.push_back(5);
	v.push_back(6);
	v.push_back(7);
	v.push_back(8);
	v.push_back(9);
	v.push_back(10);






	return 0;
}
