namespace chaos
{
#ifdef CHAOS_FORWARD_DECLARATION

	class GlobalVariableInfo;
	class GlobalVariableBase;

	class GlobalVariableImGUIRendererBase;
	template<typename T>
	class GlobalVariableImGUIRenderer;

	class GlobalVariableManager;

	template<typename T>
	class GlobalVariable;

#define CHAOS_GLOBAL_VARIABLE(TYPE, VARIABLE_NAME, ...)\
static inline chaos::GlobalVariable<TYPE> const & VARIABLE_NAME = *chaos::GlobalVariableManager::GetInstance()->RegisterVariable<TYPE>(#VARIABLE_NAME, __VA_ARGS__);\

#elif !defined CHAOS_TEMPLATE_IMPLEMENTATION

	/**
	 * GlobalVariableImGUIRendererBase: a base class dedicated to render GlobalVariable with ImGui
	 */

	class CHAOS_API GlobalVariableImGUIRendererBase
	{
		friend class GlobalVariableInfo;

	public:

		/** the main method to override */
		virtual void DrawImGui(GlobalVariableBase* target) const
		{
			target = target;
			int i = 0;
			++i;
		}

	protected:

		/** getter on the static instance */
		template<typename T>
		static GlobalVariableImGUIRendererBase const * GetInstance()
		{
			static GlobalVariableImGUIRenderer<T> result;
			return &result;
		}
	};

	/**
	 * GlobalVariableImGUIRenderer: the generic template for GlobalVariableImGUIRendererBase inheriting class
	 */

	template<typename T>
	class GlobalVariableImGUIRenderer : public GlobalVariableImGUIRendererBase
	{
	};

	/**
	 * GlobalVariableInfo: some meta data used by GlobalVariable
	 */

	class CHAOS_API GlobalVariableInfo
	{
		template<typename T>
		friend class GlobalVariable;

	public:

		/** gets the type_info for this entry */
		std::type_info const* GetTypeInfo() const { return type_info; }
		/** get the imgui renderer */
		GlobalVariableImGUIRendererBase const* GetImGUIRenderer() const { return imgui_renderer; }

	protected:

		/** getter on the static instance */
		template<typename T>
		static GlobalVariableInfo const * GetInstance()
		{
			static GlobalVariableInfo result = GlobalVariableInfo
			(
				&typeid(T),
				GlobalVariableImGUIRendererBase::GetInstance<T>()
			);
			return &result;
		}

		/** constructor */
		GlobalVariableInfo(std::type_info const* in_type_info, GlobalVariableImGUIRendererBase const * in_imgui_renderer):
			type_info(in_type_info),
			imgui_renderer(in_imgui_renderer){}

	protected:
		
		/** the type information */
		std::type_info const* type_info = nullptr;
		/** how to display the data with ImGui */
		GlobalVariableImGUIRendererBase const* imgui_renderer = nullptr;
	};

	/**
	 * GlobalVariableBase: the base class for global variable
	 */

	class CHAOS_API GlobalVariableBase : public NamedInterface
	{
	public:

		/** get information related to this variable */
		GlobalVariableInfo const * GetVariableInfo() const { return variable_info; }
		/** register an option into a description */
		virtual void RegisterProgramOption(boost::program_options::options_description& desc) = 0;

	protected:

		/** some commong information for this variable */
		GlobalVariableInfo const * variable_info = nullptr;
	};

	/**
	 * GlobalVariableBase: the specified class for global variable
	 */

	template<typename T>
	class GlobalVariable : public GlobalVariableBase
	{
		friend class GlobalVariableManager;

	public:

		/** constructor */
		GlobalVariable(T in_value = {}) : value(std::move(in_value))
		{
			variable_info = GlobalVariableInfo::GetInstance<T>();
		}

		/** destructor */
		virtual ~GlobalVariable() = default;

		/** override */
		virtual void RegisterProgramOption(boost::program_options::options_description& desc) override
		{
			if constexpr (meta::is_vector_type_v<T>)
			{
				// zero_tokens
				//   accepts empty lists as
				//     --PARAM
				// composing
				//    --PARAM a b c --PARAM d e
				//   is equivalent to
				//    --PARAM a b c d e
				desc.add_options()
					(GetName(), boost::program_options::value<T>(&value)->multitoken()->zero_tokens()->composing(), "");
			}
			else if constexpr (std::is_same_v<T, bool>)
			{
				// implicit value:
				//     --PARAM
				//   is equivalent to
				//     --PARAM true
				desc.add_options()
					(GetName(), boost::program_options::value<T>(&value)->implicit_value(true), "");
			}
			else
			{
				desc.add_options()
					(GetName(), boost::program_options::value<T>(&value), "");
			}
		}

		/** const getter */
		T const& Get() const
		{
			return value;
		}
		/** getter */
		T& Get()
		{
			return value;
		}

	protected:

		/** the value of the variable */
		T value = {};
	};

	/**
	 * GlobalVariableManager: a singleton to manage all global variables
	 */

	class CHAOS_API GlobalVariableManager : public Singleton<GlobalVariableManager>
	{
	public:

		/** register a variable */
		template<typename T>
		GlobalVariable<T> const * RegisterVariable(char const* name, T default_value = {})
		{
			// a variable with the same name already exists ?
			if (GlobalVariableBase* result = ObjectRequest(name).FindObject(variables))
			{
				// the type matches ?
				// (you may use CHAOS_GLOBAL_VARIABLE(XXX) several times with the same string in several places. They will all point the same data)
				if (*result->GetVariableInfo()->GetTypeInfo() == typeid(T))
				{
					return dynamic_cast<GlobalVariable<T>*>(result);
				}
				else
				{
					assert(0);
					return nullptr;
				}
			}
			// create an new entry
			GlobalVariable<T>* result = new GlobalVariable<T>(std::move(default_value));
			assert(result != nullptr);
			result->SetName(name);
			variables.push_back(result);
			return result;
		}

		/** parse the argument line */
		void ParseArguments(int argc, char** argv);

		/** gets the option string */
		char const* GetOptionString() const;

		/** gets the variables */
		std::vector<GlobalVariableBase*> const& GetVariables() const { return variables; }

	protected:

		/** options descriptions */
		std::string options_string;
		/** list of all variables */
		std::vector<GlobalVariableBase*> variables;
	};

	/**
	 * GlobalVariableImGUIRenderer: the generic template for GlobalVariableImGUIRendererBase inheriting class
	 */

	template<>
	class GlobalVariableImGUIRenderer<bool> : public GlobalVariableImGUIRendererBase
	{
	public:

		virtual void DrawImGui(GlobalVariableBase* target) const
		{
			bool& target_value = ((GlobalVariable<bool> *)target)->Get();
			ImGuiTools::PushID(target);
			ImGui::Checkbox("", &target_value);
			ImGui::PopID();
		}
	};



#endif

}; // namespace chaos