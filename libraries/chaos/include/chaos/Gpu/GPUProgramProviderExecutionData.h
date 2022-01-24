namespace chaos
{
#ifdef CHAOS_FORWARD_DECLARATION

	enum class GPUProgramProviderPassType;

	class GPUProgramProviderExecutionData;
	class GPUProgramProviderDeduceLock;

#elif !defined CHAOS_TEMPLATE_IMPLEMENTATION

	/**
	* GPUProgramProviderPassType : the type of search we are currently on
	*/

	enum class GPUProgramProviderPassType
	{
		FALLBACK = 1,
		DEDUCED = 2,
		EXPLICIT = 4
	};

	/**
	* GPUProgramProviderExecutionData : some data used for deduction
	*/

	class GPUProgramProviderExecutionData
	{
		friend class GPUProgramProviderInterface;
		friend class GPUProgramProviderDeduceLock;

	public:

		/** constructor */
		GPUProgramProviderExecutionData(char const* in_searched_name, GPUProgramAction& in_action, GPUProgramProviderExecutionData const* base_execution = nullptr);

		/** check for name and return a lock */
		GPUProgramProviderDeduceLock CanDeduce(char const* searched_name) const;
		/** get a value for the uniform / attribute */
		template<typename T>
		bool GetValue(char const* name, T& result) const;

		/** get the name searched */
		char const* GetSearchedName() const { return searched_name; }
		/** get the wanted action */
		GPUProgramAction const& GetAction() const { return action; }
		/** get the wanted action */
		GPUProgramAction& GetAction() { return action; }

		/** returns whether the proposed name + type match the initial request */
		bool Match(char const* other_name, GPUProgramProviderPassType in_pass_type = GPUProgramProviderPassType::EXPLICIT) const;

		/** gets the pass type */
		GPUProgramProviderPassType GetPassType() const { return pass_type; }

		/** process the action with any data */
		template<typename T>
		bool Process(T const& value, GPUProgramProviderBase const* provider = nullptr) const
		{
			return action.Process(searched_name, value, provider);
		}

		/** process the action with texture */
		bool Process(GPUTexture const* value, GPUProgramProviderBase const* provider) const;

	protected:

		/** the top level provider, used for deduction */
		GPUProgramProviderInterface const* top_provider = nullptr;
		/** the type of provider we want to work on */
		GPUProgramProviderPassType pass_type = GPUProgramProviderPassType::EXPLICIT;
		/** the vector on which the search is effectively done (it may comes from another execution_data) */
		std::vector<char const*>* deduced_searches = nullptr;
		/** the pending searches. No need to make a deep copy of the string */
		mutable std::vector<char const*> internal_deduced_searches;
		/** the name searched */
		char const* searched_name = nullptr;
		/** the action to trigger */
		GPUProgramAction& action;
	};

	class GPUProgramProviderDeduceLock
	{
		friend class GPUProgramProviderExecutionData;

	public:

		/** default constructor */
		GPUProgramProviderDeduceLock() = default;
		/** default move constructor */
		GPUProgramProviderDeduceLock(GPUProgramProviderDeduceLock&& src);
		/** no copy constructor */
		GPUProgramProviderDeduceLock(GPUProgramProviderDeduceLock const& src) = delete;
		/** destructor */
		~GPUProgramProviderDeduceLock();
		/** the lock is 'true' if there is a match (and there is no infinite loop) */
		operator bool() const;
		/** no copy */
		GPUProgramProviderDeduceLock const& operator = (GPUProgramProviderDeduceLock const& src) = delete;

	protected:

		/** the name that is searched */
		char const* searched_name = nullptr;
		/** the execution data that requested for the lock */
		GPUProgramProviderExecutionData const* execution_data = nullptr;
	};

#else // => defined CHAOS_TEMPLATE_IMPLEMENTATION

	template<typename T>
	bool GPUProgramProviderExecutionData::GetValue(char const* name, T& result) const
	{
		auto action = GPUProgramGetValueAction<T>(result);

		GPUProgramProviderExecutionData other_execution_data(name, action, this); // another data that shares the same vector than us !
		// search for explicit first ...
		other_execution_data.pass_type = GPUProgramProviderPassType::EXPLICIT;
		if (top_provider->DoProcessAction(other_execution_data))
			return true;
		// ... then use deduced rules
		other_execution_data.pass_type = GPUProgramProviderPassType::DEDUCED;
		if (top_provider->DoProcessAction(other_execution_data))
			return true;
		// ... finally accept any fallback values
		other_execution_data.pass_type = GPUProgramProviderPassType::FALLBACK;
		if (top_provider->DoProcessAction(other_execution_data))
			return true;
		return false;
	}

#endif

}; // namespace chaos