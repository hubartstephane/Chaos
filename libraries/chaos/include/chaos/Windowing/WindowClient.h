namespace chaos
{
#ifdef CHAOS_FORWARD_DECLARATION

	class WindowClient;

#elif !defined CHAOS_TEMPLATE_IMPLEMENTATION

	/**
	* WindowClient : an object that deserves to be plugged inside a ClientServerWindow
	*/

	class CHAOS_API WindowClient : public Object, public WindowInterface
	{
		friend class ClientServerWindowInterface;

		CHAOS_DECLARE_OBJECT_CLASS(WindowClient, Object);

	public:

		/** gets the server owning this client */
		AutoCastable<ClientServerWindowInterface> GetClientServer();
		/** gets the server owning this client */
		AutoConstCastable<ClientServerWindowInterface> GetClientServer() const;

	protected:

		/** called whenever the client is been detached from a ClientServer */
		virtual void OnClientDetached(ClientServerWindowInterface * in_client_server);
		/** called whenever the client is been attached to a ClientServer */
		virtual void OnClientAttached(ClientServerWindowInterface* in_client_server);

	protected:

		/** the server owning this client */
		ClientServerWindowInterface* client_server = nullptr;
	};

#endif

}; // namespace chaos