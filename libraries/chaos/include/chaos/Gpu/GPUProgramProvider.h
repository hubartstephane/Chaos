#ifdef CHAOS_FORWARD_DECLARATION

namespace chaos
{
	enum class GPUProgramProviderType;

	class GPUProgramProviderBase;

	class GPUProgramProviderExecutionData;
	class GPUProgramProviderDeduceLock;

	template<typename T>
	class GPUProgramProviderValue;

	class GPUProgramProviderTexture;
	class GPUProgramProvider;
	class GPUProgramProviderChain;
	class GPUProgramProviderDeducedTransformations;

}; // namespace chaos

#else 

namespace chaos
{
	/**
	* GPUProgramProviderType : the type of search we are currently on
	*/

	enum class GPUProgramProviderType
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
		friend class GPUProgramProviderBase;
		friend class GPUProgramProviderDeduceLock;

	public:

		/** constructor */
		GPUProgramProviderExecutionData()
		{
			deduced_searches = &internal_deduced_searches; // use our own vector
		}
		/** constructor */
		GPUProgramProviderExecutionData(GPUProgramProviderExecutionData const & src):
			top_provider(src.top_provider),
			deduced_searches(src.deduced_searches) // do not point to our internal vector, but on another one's !
		{
		}

		/** check for name and return a lock */
		GPUProgramProviderDeduceLock CanDeduce(char const* name, char const* searched_name) const;
		/** get a value for the uniform / attribute */
		template<typename T>
		bool GetValue(char const* name, T& result) const
		{
			auto action = GPUProgramGetValueAction<T>(result);

			GPUProgramProviderExecutionData other_execution_data(*this); // another data that shares the same vector than us !
			// search for explicit first ...
			other_execution_data.type = GPUProgramProviderType::EXPLICIT;
			if (top_provider->ConditionalProcessAction(name, action, other_execution_data))
				return true;
			// ... then use deduced rules
			other_execution_data.type = GPUProgramProviderType::DEDUCED;
			if (top_provider->ConditionalProcessAction(name, action, other_execution_data))
				return true;
			// ... finally accept any fallback values
			other_execution_data.type = GPUProgramProviderType::FALLBACK;
			if (top_provider->ConditionalProcessAction(name, action, other_execution_data))
				return true;
			return false;
		}

		/** gets the type */
		GPUProgramProviderType GetType() const { return type; }

	protected:

		/** the top level provider, used for deduction */
		GPUProgramProviderBase const* top_provider = nullptr;
		/** the type of provider we want to work on */
		GPUProgramProviderType type = GPUProgramProviderType::EXPLICIT;
		/** the vector on which the search is effectively done (it may comes from another execution_data) */
		std::vector<char const*> * deduced_searches = nullptr;
		/** the pending searches. No need to make a deep copy of the string */
		mutable std::vector<char const*> internal_deduced_searches;
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

	/**
	* GPUProgramProviderBase : a base class for filling uniforms or attributes in a program. The purpose is to take responsability to start an ACTION
	*/

	class GPUProgramProviderBase : public Object
	{
		friend class GPUProgramProvider; // WTF : GPUProgramProvider could not call DoProcessAction(...) an another instance without that !!
		friend class GPUProgramProviderChain;
		friend class GPUProgramProviderExecutionData;
		friend class GPUProgramRenderMaterialProvider;

	public:

		/** constructor */
		GPUProgramProviderBase(GPUProgramProviderType in_type = GPUProgramProviderType::EXPLICIT) :
			type(in_type) {}

		/** the main method : returns try whether tha action has been handled (even if failed) */
		bool ProcessAction(char const* name, GPUProgramAction& action) const;

		/** utility function that deserve to set uniform */
		bool BindUniform(GLUniformInfo const& uniform) const;
		/** utility function that deserve to set attribute */
		bool BindAttribute(GLAttributeInfo const& attribute) const;
		/** get a value for the uniform / attribute */
		template<typename T>
		bool GetValue(char const * name, T & result) const
		{
			return ProcessAction(name, GPUProgramGetValueAction<T>(result));
		}

	protected:

		/** check whether the type correspond to execution then DoProcessAction */
		bool ConditionalProcessAction(char const* name, GPUProgramAction& action, GPUProgramProviderExecutionData const& execution_data) const;
		/** the main method : returns true whether that action has been successfully handled */
		virtual bool DoProcessAction(char const* name, GPUProgramAction& action, GPUProgramProviderExecutionData const& execution_data) const;

	protected:

		/** the type of this provider */
		GPUProgramProviderType type = GPUProgramProviderType::EXPLICIT;
	};

	/**
	* GPUProgramProviderValue : used to fill GPUProgram binding for uniforms / attribute with simple values
	*/

	template<typename T>
	class GPUProgramProviderValue : public GPUProgramProviderBase
	{
	public:

		/** constructor */
		GPUProgramProviderValue(char const * in_name, T const & in_value, GPUProgramProviderType in_type = GPUProgramProviderType::EXPLICIT) :
			GPUProgramProviderBase(in_type), handled_name(in_name), value(in_value){}

	protected:

		/** the main method */
		virtual bool DoProcessAction(char const * name, GPUProgramAction & action, GPUProgramProviderExecutionData const & execution_data) const override
		{
			if (name != nullptr && handled_name != name)
				return false;
			return action.Process(name, value, this);
		}

	protected:

		/** the name of the uniform handled */
		std::string handled_name;
		/** the value of the uniform */
		T value;
	};

	/**
	* GPUProgramProviderTexture : used to fill GPUProgram binding for a texture
	*/

	class GPUProgramProviderTexture : public GPUProgramProviderBase
	{
		friend class GPUProgramReplaceTextureAction;

	public:

		/** constructor */
		GPUProgramProviderTexture(char const * in_name, shared_ptr<GPUTexture> in_value, GPUProgramProviderType in_type = GPUProgramProviderType::EXPLICIT) :
			GPUProgramProviderBase(in_type), handled_name(in_name), value(in_value){}

	protected:

		/** the main method */
		virtual bool DoProcessAction(char const * name, GPUProgramAction & action, GPUProgramProviderExecutionData const & execution_data) const override;

	protected:

		/** the name of the uniform handled */
		std::string handled_name;
		/** the value of the uniform */
		shared_ptr<GPUTexture> value;
	};

	/**
	* GPUProgramProvider : used to fill GPUProgram binding for multiple uniforms / uniforms
	*/

	class GPUProgramProvider : public GPUProgramProviderBase
	{
		friend class GPUResourceManager;
		friend class GPUProgramRenderMaterialProvider;

	public:

		/** constructor */
		using GPUProgramProviderBase::GPUProgramProviderBase;

		/** register a uniform value */
		template<typename T>
		void AddVariable(char const * name, T const & value, GPUProgramProviderType type = GPUProgramProviderType::EXPLICIT)
		{
			AddProvider(new GPUProgramProviderValue<T>(name, value, type));
		}
		/** register a uniform texture */
		void AddTexture(char const * name, shared_ptr<class GPUTexture> texture, GPUProgramProviderType type = GPUProgramProviderType::EXPLICIT)
		{
			AddProvider(new GPUProgramProviderTexture(name, texture, type));
		}
		/** register a generic uniform */
		virtual void AddProvider(GPUProgramProviderBase * provider);
		/** remove all uniforms for binding */
		virtual void Clear();

	protected:

		/** the main method */
		virtual bool DoProcessAction(char const * name, GPUProgramAction & action, GPUProgramProviderExecutionData const & execution_data) const override;

	protected:

		/** the uniforms to be set */
		std::vector<shared_ptr<GPUProgramProviderBase>> children_providers;
	};

	/**
	* GPUProgramProviderChain : an utility class used to enrich an existing provider with additionnal data
	*                           the reference count of this object is disabled. It deserves to be used on the stack
	*/

	class GPUProgramProviderChain : public DisableReferenceCount<GPUProgramProvider>
	{
	public:

		/** constructor */
		GPUProgramProviderChain(GPUProgramProviderBase const * in_fallback_provider, GPUProgramProviderType in_type = GPUProgramProviderType::EXPLICIT) :
			DisableReferenceCount<GPUProgramProvider>(in_type), fallback_provider(in_fallback_provider) {}

	protected:

		/** apply the actions */
		virtual bool DoProcessAction(char const * name, GPUProgramAction & action, GPUProgramProviderExecutionData const & execution_data) const override;

	protected:

		/** another provider (use a non intrusive reference !!!) */
		GPUProgramProviderBase const * fallback_provider = nullptr;
	};

	/**
	* GPUProgramProviderDeducedTransformations : a provider that help finding world_to_local, local_to_world ... transformations
	*/

	class GPUProgramProviderDeducedTransformations : public GPUProgramProvider
	{
	public:

		/** constructor */
		GPUProgramProviderDeducedTransformations(GPUProgramProviderType in_type = GPUProgramProviderType::DEDUCED) : // XXX : but default, here this is DEDUCED, not EXPLICIT !
			GPUProgramProvider(in_type) {}
		/** apply the actions */
		virtual bool DoProcessAction(char const* name, GPUProgramAction& action, GPUProgramProviderExecutionData const & execution_data) const override;
	};

}; // namespace chaos

#endif // CHAOS_FORWARD_DECLARATION



