namespace chaos
{
#ifdef CHAOS_FORWARD_DECLARATION

    class NonCopyableAndMovable;

#elif !defined CHAOS_TEMPLATE_IMPLEMENTATION

    /**
     * NonCopyableAndMovable: this class prevent its derived classes to be copied and moved
     */

    class NonCopyableAndMovable : private NonCopyable
    {
    protected:

        // default constructor
        NonCopyableAndMovable() = default;

        // remove move constructor 
        NonCopyableAndMovable(NonCopyableAndMovable&&) = delete;
        // remove move operator
        NonCopyableAndMovable& operator=(NonCopyableAndMovable&&) = delete;
    };

#endif

}; // namespace chaos