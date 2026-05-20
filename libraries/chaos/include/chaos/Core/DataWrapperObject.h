#ifdef CHAOS_FORWARD_DECLARATION

namespace chaos
{
	template<typename T>
	class DataWrapperObject;

}; // namespace chaos

#elif !defined CHAOS_TEMPLATE_IMPLEMENTATION

namespace chaos
{
	/**
	* DataWrapperObject : a data wrapped into a referenced object => while referenced object may be dynamic_casted we can test for the data inside
	*/

	template<typename T>
	class DataWrapperObject : public Object
	{
		using type = T;

	public:

		/** constructor */
		template<typename ...PARAMS>
		DataWrapperObject(PARAMS && ...params) : data(std::forward<PARAMS>(params)...)
		{
		}

	public:

		/** the wrapped data */
		T data;
	};

}; // namespace chaos

#endif