#ifdef CHAOS_FORWARD_DECLARATION

namespace chaos
{
	class GPUProgramProviderBase;

	template<typename T>
	class GPUProgramProviderValue;

	class GPUProgramProviderTexture;
	class GPUProgramProvider;
	class GPUProgramProviderChain;

	class GPUProgramProviderSearchLock;

}; // namespace chaos

#else 

namespace chaos
{
	/**
	* GPUProgramProviderBase : a base class for filling uniforms or attributes in a program. The purpose is to take responsability to start an ACTION
	*/

	class GPUProgramProviderBase : public Object
	{
		friend class GPUProgramProvider; // WTF : GPUProgramProvider could not call DoProcessAction(...) an another instance without that !!
		friend class GPUProgramProviderChain;
		friend class GPUProgramRenderMaterialProvider;

	public:

		/** the main method : returns try whether tha action has been handled (even if failed) */
		bool ProcessAction(char const * name, GPUProgramAction & action) const
		{
			return DoProcessAction(name, action, this);
		}

		/** utility function that deserve to set uniform */
		bool BindUniform(GLUniformInfo const & uniform) const
		{
			return ProcessAction(uniform.name.c_str(), GPUProgramSetUniformAction(uniform));
		}

		/** utility function that deserve to set attribute */
		bool BindAttribute(GLAttributeInfo const & attribute) const
		{
			return ProcessAction(attribute.name.c_str(), GPUProgramSetAttributeAction(attribute));
		}

		/** get a value for the uniform / attribute */
		template<typename T>
		bool GetValue(char const * name, T & result) const
		{
			return ProcessAction(name, GPUProgramGetValueAction<T>(result));
		}

	protected:

		/** the main method : returns true whether that action has been successfully handled */
		virtual bool DoProcessAction(char const * name, GPUProgramAction & action, GPUProgramProviderBase const * top_provider) const { return false; }
	};

	/**
	* GPUProgramProviderValue : used to fill GPUProgram binding for uniforms / attribute with simple values
	*/

	template<typename T>
	class GPUProgramProviderValue : public GPUProgramProviderBase
	{
	public:

		/** constructor */
		GPUProgramProviderValue(char const * in_name, T const & in_value) :
			handled_name(in_name), value(in_value) {}

	protected:

		/** the main method */
		virtual bool DoProcessAction(char const * name, GPUProgramAction & action, GPUProgramProviderBase const * top_provider) const override
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
		GPUProgramProviderTexture(char const * in_name, shared_ptr<GPUTexture> in_value) :
			handled_name(in_name), value(in_value) {}

	protected:

		/** the main method */
		virtual bool DoProcessAction(char const * name, GPUProgramAction & action, GPUProgramProviderBase const * top_provider) const override;

	protected:

		/** the name of the uniform handled */
		std::string handled_name;
		/** the value of the uniform */
		shared_ptr<GPUTexture> value;
	};

	/**
	* GPUProgramProvider : used to fill GPUProgram binding for multiple uniforms / uniforms
	*/

	class GPUProgramProviderSearchLock
	{
		friend class GPUProgramProvider;

	public:

		/** default constructor */
		GPUProgramProviderSearchLock() = default;
		/** default move constructor */
		GPUProgramProviderSearchLock(GPUProgramProviderSearchLock&& src);
		/** no copy constructor */
		GPUProgramProviderSearchLock(GPUProgramProviderSearchLock const& src) = delete;
		/** destructor */
		~GPUProgramProviderSearchLock();
		/** the lock is 'true' if there is a match (and there is no infinite loop) */
		operator bool() const;
		/** no copy */
		GPUProgramProviderSearchLock const& operator = (GPUProgramProviderSearchLock const& src) = delete;

	protected:

		/** the name that is searched */
		char const* searched_name = nullptr;
		/** the provider that requested for the lock */
		GPUProgramProvider const* provider = nullptr;
	};

	class GPUProgramProvider : public GPUProgramProviderBase
	{
		friend class GPUResourceManager;
		friend class GPUProgramRenderMaterialProvider;
		friend class GPUProgramProviderSearchLock;

	public:

		/** register a uniform value */
		template<typename T>
		void AddVariableValue(char const * name, T const & value)
		{
			AddVariableProvider(new GPUProgramProviderValue<T>(name, value));
		}
		/** register a uniform texture */
		void AddVariableTexture(char const * name, shared_ptr<class GPUTexture> texture)
		{
			AddVariableProvider(new GPUProgramProviderTexture(name, texture));
		}
		/** register a generic uniform */
		virtual void AddVariableProvider(GPUProgramProviderBase * provider);
		/** remove all uniforms for binding */
		virtual void Clear();

		/** check for name and return a lock */
		GPUProgramProviderSearchLock DependantSearch(char const* name, char const* searched_name) const;

	protected:

		/** the main method */
		virtual bool DoProcessAction(char const * name, GPUProgramAction & action, GPUProgramProviderBase const * top_provider) const override;

	protected:

		/** the uniforms to be set */
		std::vector<shared_ptr<GPUProgramProviderBase>> children_providers;
		/** the pending searches. No need to make a deep copy of the string */
		mutable std::vector<char const*> pending_searches;
	};

	/**
	* GPUProgramProviderChain : an utility class used to enrich an existing provider with additionnal data
	*                           the reference count of this object is disabled. It deserves to be used on the stack
	*/

	class GPUProgramProviderChain : public DisableReferenceCount<GPUProgramProvider>
	{
	public:

		/** constructor */
		GPUProgramProviderChain(GPUProgramProviderBase const * in_fallback_provider) :
			fallback_provider(in_fallback_provider)
		{}

	protected:

		/** apply the actions */
		virtual bool DoProcessAction(char const * name, GPUProgramAction & action, GPUProgramProviderBase const * top_provider) const override;

	protected:

		/** another provider (use a non intrusive reference !!!) */
		GPUProgramProviderBase const * fallback_provider = nullptr;
	};

}; // namespace chaos

#endif // CHAOS_FORWARD_DECLARATION



