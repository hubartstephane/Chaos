namespace chaos
{
#ifdef CHAOS_FORWARD_DECLARATION

template<typename T>
class Singleton;

#elif !defined CHAOS_TEMPLATE_IMPLEMENTATION

template<typename T>
class Singleton
{
public:

	/** get the instances */
	static T* GetInstance()
	{
		static T single_instance;
		return &single_instance;
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