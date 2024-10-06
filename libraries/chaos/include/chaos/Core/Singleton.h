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
};

#endif

}; // namespace chaos