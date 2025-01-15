namespace chaos
{
#ifdef CHAOS_FORWARD_DECLARATION

	class GPUProgramProviderInterface;

	class GPUProgramProviderBase;

	template<typename T, typename MEMBER_TYPE>
	class GPUProgramProviderVariableBase;

	template<typename T>
	using GPUProgramProviderValue = GPUProgramProviderVariableBase<T, T>;
	template<typename T>
	using GPUProgramProviderReference = GPUProgramProviderVariableBase<T, T const&>;

	class GPUProgramProviderTexture;
	class GPUProgramProviderCustom;
	class GPUProgramProvider;
	class GPUProgramProviderChainEntry;
	class GPUProgramProviderChain;
	class GPUProgramProviderCommonTransforms;

	using GPUProgramProviderFunc = std::function<bool(GPUProgramProviderExecutionData const&)>;

#elif !defined CHAOS_TEMPLATE_IMPLEMENTATION

	/**
	* GPUProgramProviderInterface : a base class for filling uniforms or attributes in a program. The purpose is to take responsability to start an ACTION
	*/

	class CHAOS_API GPUProgramProviderInterface
	{
		friend class GPUProgramProvider; // WTF : GPUProgramProvider could not call DoProcessAction(...) an another instance without that !!
		friend class GPUProgramProviderChainEntry;
		friend class GPUProgramProviderChain;
		friend class GPUProgramProviderExecutionData;
		friend class GPUProgramRenderMaterialProvider;

	public:

		/** the main method : returns true whether the action has been handled (even if failed) */
		bool ProcessAction(char const* name, GPUProgramAction& action) const;

		/** utility function that deserve to set uniform */
		bool BindUniform(GPUUniformInfo const& uniform) const;
		/** utility function that deserve to set attribute */
		bool BindAttribute(GPUAttributeInfo const& attribute) const;
		/** get a value for the uniform / attribute */
		template<typename T>
		bool GetValue(char const* name, T& result) const
		{
			return ProcessAction(name, GPUProgramGetValueAction<T>(result));
		}

	protected:

		/** the main method : returns true whether that action has been successfully handled */
		virtual bool DoProcessAction(GPUProgramProviderExecutionData const& execution_data) const;
	};

	/**
	* GPUProgramProviderInterface : an object that implements GPUProgramProviderInterface
	*/

	class CHAOS_API GPUProgramProviderBase : public Object, public GPUProgramProviderInterface
	{

	};

	/**
	* GPUProgramProviderVariableBase : used to fill GPUProgram binding for uniforms / attribute with simple values
	*/

	template<typename T, typename MEMBER_TYPE>
	class GPUProgramProviderVariableBase : public GPUProgramProviderBase
	{
	public:

		/** constructor */
		GPUProgramProviderVariableBase(char const* in_name, T const& in_value, GPUProgramProviderPassType in_pass_type = GPUProgramProviderPassType::EXPLICIT) :
			handled_name(in_name), value(in_value), pass_type(in_pass_type) {}

	protected:

		/** the main method */
		virtual bool DoProcessAction(GPUProgramProviderExecutionData const& execution_data) const override
		{
			if (execution_data.Match(handled_name.c_str(), pass_type))
				return execution_data.Process(value, this);
			return false;
		}

	protected:

		/** the name of the uniform handled */
		std::string handled_name;
		/** the value of the uniform */
		MEMBER_TYPE value;
		/** the type of this provider */
		GPUProgramProviderPassType pass_type = GPUProgramProviderPassType::EXPLICIT;
	};

	/**
	* GPUProgramProviderTexture : used to fill GPUProgram binding for a texture
	*/

	class CHAOS_API GPUProgramProviderTexture : public GPUProgramProviderBase
	{
		friend class GPUProgramReplaceTextureAction;

	public:

		/** constructor */
		GPUProgramProviderTexture(char const* in_name, shared_ptr<GPUTexture> in_value, GPUProgramProviderPassType in_pass_type = GPUProgramProviderPassType::EXPLICIT) :
			handled_name(in_name), value(in_value), pass_type(in_pass_type) {}

	protected:

		/** the main method */
		virtual bool DoProcessAction(GPUProgramProviderExecutionData const& execution_data) const override;

	protected:

		/** the name of the uniform handled */
		std::string handled_name;
		/** the value of the uniform */
		shared_ptr<GPUTexture> value;
		/** the type of this provider */
		GPUProgramProviderPassType pass_type = GPUProgramProviderPassType::EXPLICIT;
	};



	/**
	* GPUProgramProviderCustom : used to fill GPUProgram binding with a lambda
	*/

	class CHAOS_API GPUProgramProviderCustom : public GPUProgramProviderBase
	{
		friend class GPUResourceManager;
		friend class GPUProgramRenderMaterialProvider;

	public:

		/** constructor */
		GPUProgramProviderCustom(GPUProgramProviderFunc const& in_process_func) :
			process_func(in_process_func)
		{
		}

	protected:

		/** the main method */
		virtual bool DoProcessAction(GPUProgramProviderExecutionData const& execution_data) const override;

	protected:

		/** some in place code */
		GPUProgramProviderFunc process_func;
	};

	/**
	* GPUProgramProvider : used to fill GPUProgram binding for multiple uniforms / uniforms
	*/

	class CHAOS_API GPUProgramProvider : public GPUProgramProviderBase
	{
		friend class GPUResourceManager;
		friend class GPUProgramRenderMaterialProvider;

	public:

		/** register a uniform value */
		template<typename T>
		void AddVariable(char const* name, T const& value, GPUProgramProviderPassType pass_type = GPUProgramProviderPassType::EXPLICIT)
		{
			AddProvider(new GPUProgramProviderValue<T>(name, value, pass_type));
		}
		/** register a uniform reference */
		template<typename T>
		void AddVariableReference(char const* name, T const& value, GPUProgramProviderPassType pass_type = GPUProgramProviderPassType::EXPLICIT)
		{
			AddProvider(new GPUProgramProviderReference<T>(name, value, pass_type));
		}
		/** register a uniform texture */
		void AddTexture(char const* name, shared_ptr<class GPUTexture> texture, GPUProgramProviderPassType pass_type = GPUProgramProviderPassType::EXPLICIT)
		{
			AddProvider(new GPUProgramProviderTexture(name, texture, pass_type));
		}
		/** register a generic uniform */
		virtual void AddProvider(GPUProgramProviderBase* provider);
		/** remove all uniforms for binding */
		virtual void Clear();

	protected:

		/** the main method */
		virtual bool DoProcessAction(GPUProgramProviderExecutionData const& execution_data) const override;

	protected:

		/** the uniforms to be set */
		std::vector<shared_ptr<GPUProgramProviderBase>> children_providers;
		/** some in place code */
		GPUProgramProviderFunc process_func;
	};


	/**
	* GPUProgramProviderChainEntry : an entry for the chain provider
	*/

	class CHAOS_API GPUProgramProviderChainEntry
	{
		friend class GPUProgramProviderChain;

	public:

		/** default constructor */
		GPUProgramProviderChainEntry() = default;
		/** copy constructor */
		GPUProgramProviderChainEntry(GPUProgramProviderChainEntry const& src) = default;
		/** constructor with functor */
		GPUProgramProviderChainEntry(GPUProgramProviderFunc const& in_process_func) : process_func(in_process_func) {}
		/** constructor with a provider */
		GPUProgramProviderChainEntry(GPUProgramProviderInterface const * in_provider) : provider(in_provider) {}
		/** constructor with a provider */
		GPUProgramProviderChainEntry(GPUProgramProviderInterface const & in_provider) : provider(&in_provider) {}

	protected:

		/** functor */
		GPUProgramProviderFunc process_func;
		/** provider */
		GPUProgramProviderInterface const* provider = nullptr;
	};

	/**
	* GPUProgramProviderChain : an utility class used to enrich an existing provider with additionnal data
	*                           the reference count of this object is disabled. It deserves to be used on the stack
	*/

#define CHAOS_PROVIDER_CHAIN_COUNT 10

#define CHAOS_PROVIDER_CHAIN_ARGUMENT_PARAMS(z, n, unused)\
BOOST_PP_COMMA_IF(n) GPUProgramProviderChainEntry const BOOST_PP_CAT(in_param, n) = {}

#define CHAOS_PROVIDER_CHAIN_ARGUMENT_INITS(z, n, unused)\
entries[n] = BOOST_PP_CAT(in_param, n);

	class CHAOS_API GPUProgramProviderChain : public DisableReferenceCount<GPUProgramProvider>
	{

	public:

		/** constructor */
		GPUProgramProviderChain(
			BOOST_PP_REPEAT(CHAOS_PROVIDER_CHAIN_COUNT, CHAOS_PROVIDER_CHAIN_ARGUMENT_PARAMS, ~)
		)
		{
			BOOST_PP_REPEAT(CHAOS_PROVIDER_CHAIN_COUNT, CHAOS_PROVIDER_CHAIN_ARGUMENT_INITS, ~);
		}

	protected:

		/** apply the actions */
		virtual bool DoProcessAction(GPUProgramProviderExecutionData const& execution_data) const override;

	protected:

		/** the other provider/functors */
		std::array<GPUProgramProviderChainEntry, CHAOS_PROVIDER_CHAIN_COUNT> entries;
	};

#undef CHAOS_PROVIDER_CHAIN_ARGUMENT_PARAMS
#undef CHAOS_PROVIDER_CHAIN_ARGUMENT_INITS
#undef CHAOS_PROVIDER_CHAIN_COUNT

	/**
	* GPUProgramProviderCommonTransforms : a provider that help finding world_to_local, local_to_world ... transformations
	*/

	class CHAOS_API GPUProgramProviderCommonTransforms : public GPUProgramProvider
	{
	public:

		/** constructor */
		using GPUProgramProvider::GPUProgramProvider;

		/** apply the actions */
		virtual bool DoProcessAction(GPUProgramProviderExecutionData const& execution_data) const override;
	};

#endif

}; // namespace chaos