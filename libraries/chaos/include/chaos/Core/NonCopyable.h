namespace chaos
{
#ifdef CHAOS_FORWARD_DECLARATION

class NonCopyable;

#elif !defined CHAOS_TEMPLATE_IMPLEMENTATION

    /**
     * NonCopyable: this class prevent its derived classes to be copied 
     */

    class NonCopyable
    {
    protected:

        // default constructor
        NonCopyable() = default;

        // remove copy constructor 
        NonCopyable(NonCopyable const&) = delete;
        // remove copy operator
        NonCopyable& operator=(NonCopyable const&) = delete;
    };

#endif

}; // namespace chaos