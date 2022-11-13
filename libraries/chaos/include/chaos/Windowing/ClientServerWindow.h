namespace chaos
{
#ifdef CHAOS_FORWARD_DECLARATION

	class ClientServerWindowInterface;
	class ClientServerWindow;

#elif !defined CHAOS_TEMPLATE_IMPLEMENTATION

	/**
	* ClientServerWindowInterface : an interface for using some WindowClient
	*/

	class CHAOS_API ClientServerWindowInterface
	{

	public:

		/** get current client */
		AutoCastable<WindowClient> GetClient();
		/** get current client */
		AutoConstCastable<WindowClient> GetClient() const;
		/** set current client */
		void SetClient(WindowClient* in_client);

	protected:

		/** the client that receives all  events */
		shared_ptr<WindowClient> client;
	};

	/**
	* ClientServerWindow : a window that may receive one client
	*/

	class CHAOS_API ClientServerWindow : public Window, public ClientServerWindowInterface
	{
		CHAOS_DECLARE_OBJECT_CLASS(ClientServerWindow, Window);

	protected:

		/** override */
		virtual bool OnWindowClosed() override;
		/** override */
		virtual void OnWindowResize(glm::ivec2 size) override;
		/** override */
		virtual void OnDropFile(int count, char const** paths) override;
		/** override */
		virtual void OnIconifiedStateChange(bool iconified) override;
		/** override */
		virtual void OnFocusStateChange(bool gain_focus) override;
		/** override */
		virtual bool OnDraw(GPURenderer* renderer, WindowDrawParams const& DrawParams, GPUProgramProviderInterface const* uniform_provider) override;

		/** override */
		virtual bool OnMouseMoveImpl(double x, double y) override;
		/** override */
		virtual bool OnMouseButtonImpl(int button, int action, int modifier) override;
		/** override */
		virtual bool OnMouseWheelImpl(double scroll_x, double scroll_y) override;
		/** override */
		virtual bool OnKeyEventImpl(KeyEvent const& event) override;
		/** override */
		virtual bool OnCharEventImpl(unsigned int c) override;
	};

#endif

}; // namespace chaos