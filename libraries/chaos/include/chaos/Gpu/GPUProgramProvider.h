namespace chaos
{
#ifdef CHAOS_FORWARD_DECLARATION

	class GPUProgramProviderBase;

	template<typename T, typename MEMBER_TYPE>
	class GPUProgramProviderVariableBase;

	template<typename T>
	using GPUProgramProviderValue = GPUProgramProviderVariableBase<T, T>;
	template<typename T>
	using GPUProgramProviderReference = GPUProgramProviderVariableBase<T, T const&>;

	class GPUProgramProviderTexture;
	class GPUProgramProvider;
	class GPUProgramProviderChain;
	class GPUProgramProviderCommonTransforms;

#elif !defined CHAOS_TEMPLATE_IMPLEMENTATION

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

		/** the main method : returns try whether tha action has been handled (even if failed) */
		bool ProcessAction(char const* name, GPUProgramAction& action) const;

		/** utility function that deserve to set uniform */
		bool BindUniform(GLUniformInfo const& uniform) const;
		/** utility function that deserve to set attribute */
		bool BindAttribute(GLAttributeInfo const& attribute) const;
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
			if (execution_data.GetPassType() == pass_type && execution_data.Match(handled_name))
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

	class GPUProgramProviderTexture : public GPUProgramProviderBase
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
	* GPUProgramProvider : used to fill GPUProgram binding for multiple uniforms / uniforms
	*/

	class GPUProgramProvider : public GPUProgramProviderBase
	{
		friend class GPUResourceManager;
		friend class GPUProgramRenderMaterialProvider;

	public:

		/** constructor */
		GPUProgramProvider(std::function<bool(GPUProgramProviderExecutionData const& execution_data)> const& in_process_func = nullptr) :
			process_func(in_process_func)
		{
		}

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
		std::function<bool(GPUProgramProviderExecutionData const& execution_data)> process_func;
	};

	/**
	* GPUProgramProviderChain : an utility class used to enrich an existing provider with additionnal data
	*                           the reference count of this object is disabled. It deserves to be used on the stack
	*/

	class GPUProgramProviderChain : public DisableReferenceCount<GPUProgramProvider>
	{
	public:

		/** constructor */
		GPUProgramProviderChain(GPUProgramProviderBase const* in_fallback_provider, std::function<bool(GPUProgramProviderExecutionData const& execution_data)> const& in_process_func = nullptr) :
			DisableReferenceCount<GPUProgramProvider>(in_process_func),
			fallback_provider(in_fallback_provider) {}

	protected:

		/** apply the actions */
		virtual bool DoProcessAction(GPUProgramProviderExecutionData const& execution_data) const override;

	protected:

		/** another provider (use a non intrusive reference !!!) */
		GPUProgramProviderBase const* fallback_provider = nullptr;
	};

	/**
	* GPUProgramProviderCommonTransforms : a provider that help finding world_to_local, local_to_world ... transformations
	*/

	class GPUProgramProviderCommonTransforms : public GPUProgramProvider
	{
	public:

		/** constructor */
		using GPUProgramProvider::GPUProgramProvider;

		/** apply the actions */
		virtual bool DoProcessAction(GPUProgramProviderExecutionData const& execution_data) const override;
	};

#endif

}; // namespace chaos