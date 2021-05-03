#ifdef CHAOS_FORWARD_DECLARATION
namespace chaos
{

	class BufferBase;
	class BufferPolicyBase;

	template<typename T> 
	class Buffer;

	template<typename T> 
	class BufferPolicy;

}; // namespace chaos
#else 

namespace chaos
{
	/**
	* BufferBase : base class for buffer
	*/

	class BufferBase
	{
		friend class BufferPolicyBase;

	public:

		/** default constructor */
		BufferBase() = default;
		/** destructor */
		~BufferBase();

		/** XXX : use with caution : get the policy */
		BufferPolicyBase* GetPolicy();
		/** XXX : use with caution : get the policy */
		BufferPolicyBase const* GetPolicy() const;
		/** XXX : use with caution : change the policy */
		void SetPolicy(BufferPolicyBase* in_policy);

	protected:

		/** an utility function for friendship */
		void CopyFromBuffer(BufferBase const* src);

	protected:

		/** the policy */
		BufferPolicyBase* policy = nullptr;
	};

	/**
	* BufferPolicyBase : base class for the Policy 
	*/

	class BufferPolicyBase
	{

		friend class BufferBase;

	public:

		/** destructor */
		virtual ~BufferPolicyBase() = default;

	protected:

		/** called whenever the buffer is destroyed */
		virtual void DestroyBuffer(BufferBase * buf){}
		/** called whenever we want to copy a buffer */
		virtual void CopyBuffer(BufferBase * dst, BufferBase const * src){}
	};

	/**
	* SharedBufferPolicy : the policy for memory management is based on a counter. Policy and data are allocated in the same chunk of memory
	*/

	template<typename T> 
	class SharedBufferPolicy : public BufferPolicyBase
	{

	public:

		/** constructor */
		SharedBufferPolicy() : reference_count(1){}

		/** generate the buffer */
		static Buffer<T> NewBuffer(size_t count)
		{
			auto obj = AllocatorTools::SingleAllocMultipleObjects<SharedBufferPolicy<T>, T>(1, count); // optimized : all in a single allocation
			Buffer<T> result(std::get<1>(obj), count);
			result.SetPolicy(std::get<0>(obj));
			return result;
		}

	protected:

		/** copy the buffer */
		virtual void CopyBuffer(BufferBase * dst, BufferBase const * src) override
		{
			assert(dst != nullptr);
			assert(src != nullptr);
			assert(src->GetPolicy() == this);

			Buffer<T> * d = (Buffer<T>*)dst;
			Buffer<T> * s = (Buffer<T>*)src;

			d->data    = s->data;
			d->bufsize = s->bufsize;
			d->SetPolicy(this);
			++reference_count;
		}

		/** destroy the buffer */
		virtual void DestroyBuffer(BufferBase * buf) override
		{
			if (--reference_count == 0)
			{
				Buffer<T> * b = (Buffer<T>*)buf;
				for (size_t i = 0 ; i < b->bufsize ; ++i)
					b->data[i].~T();      
				b->GetPolicy()->~BufferPolicyBase();
				AllocatorTools::Aligned16Free(b->GetPolicy());           
			}    
		}

	protected:

		/** count the reference on the buffer */
		mutable boost::atomic<int> reference_count;
	};

	/**
	* SharedNonOptimizedBufferPolicy : same than SharedNonOptimizedBufferPolicy, but policy is in an independent memory than the data
	*/

	template<typename T> 
	class SharedNonOptimizedBufferPolicy : public SharedBufferPolicy<T>
	{

	public:

		/** generate the buffer */
		static Buffer<T> NewBuffer(size_t count)
		{
			Buffer<T> result(new T[count], count);
			result.SetPolicy(new SharedNonOptimizedBufferPolicy<T>);
			return result;
		}

	protected:

		/** destroy the buffer */
		virtual void DestroyBuffer(BufferBase * buf) override
		{
			if (--this->reference_count == 0)
			{
				Buffer<T> * b = (Buffer<T>*)buf;
				delete [] b->data;
				delete(this);
			}    
		}
	};

	/**
	* Buffer : a buffer with typed data inside
	*/

	template<typename TYPE = char>
	class Buffer : public BufferBase
	{
		friend class BufferPolicyBase;

	public:

		/** the type of the element in the buffer */
		using type = TYPE;

		/** default constructor */    
		Buffer() = default;

		/** constructor with initialization */
		Buffer(TYPE * in_data, size_t in_bufsize):
			data(in_data),
			bufsize(in_bufsize){}

		/** copy constructor (let the policy decide what to do) */
		Buffer(Buffer<TYPE> const & other)
		{
			if (other.policy == nullptr) // buffer is unmanaged => simple copy
			{
				data    = other.data;
				bufsize = other.bufsize; 
			}
			else
			{
				CopyFromBuffer(&other);
			}   
		}

		/** move constructor */
		Buffer(Buffer<TYPE> && other) noexcept
		{
			std::swap(policy,  other.policy);
			std::swap(data,    other.data);
			std::swap(bufsize, other.bufsize);    
		}

		/** get the size */
		size_t size() const
		{
			return bufsize;
		}

		/** cast the buffer in a data pointer */
		operator type const * () const
		{
			return data;
		}
		/** cast the buffer in a data pointer */
		operator type * ()
		{
			return data;
		}

		/** copy operator */
		Buffer<TYPE> & operator = (Buffer<TYPE> const & other) 
		{
			Buffer<TYPE> to_destroy(std::move(*this)); // to destroy at the end of call

			if (other.policy == nullptr)
			{
				data    = other.data;
				bufsize = other.bufsize;
				policy  = nullptr;
			}
			else
			{
				CopyFromBuffer(&other);
			}

			return *this;
		}

		/** move operator */
		Buffer<TYPE> & operator = (Buffer<TYPE> && other) noexcept
		{
			std::swap(policy,  other.policy);
			std::swap(data,    other.data);
			std::swap(bufsize, other.bufsize);
			return *this;
		}

	public:

		/** the pointer on the data */
		type * data = nullptr;
		/** the number of elements in the buffer */
		size_t bufsize = 0;
	};

}; // namespace chaos

#endif // CHAOS_FORWARD_DECLARATION

