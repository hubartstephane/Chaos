namespace chaos
{
#ifdef CHAOS_FORWARD_DECLARATION

template<typename T>
class Singleton;

#elif !defined CHAOS_TEMPLATE_IMPLEMENTATION

template<typename T>
class Singleton : private NonCopyableAndMovable
{
public:

	/** get the instances */
	static T* GetInstance()
	{
		if constexpr (std::is_base_of_v<ReferenceCountedInterface, T>)
		{
			static shared_ptr<T> result = new T;
			return result.get();	
		}
		else
		{
			class ReferencedInstance : public T, public ReferenceCountedInterface
			{
			public:

				using T::T;
			};

			static shared_ptr<ReferencedInstance> result = new ReferencedInstance;
			return result.get();
		}
	}

protected:

	/** constructor */
	Singleton()
	{
		assert(instance_count == 0);
		++instance_count;
	}

	/** destructor */
	~Singleton()
	{
		--instance_count;
		assert(instance_count == 0);
	}

protected:

	/** get the number of existing instance */
	static inline int instance_count = 0;
};

#endif

}; // namespace chaos