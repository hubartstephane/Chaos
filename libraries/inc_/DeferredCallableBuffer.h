#ifndef CHAOS_DEFERREDCALLABLEBUFFER_H
#define CHAOS_DEFERREDCALLABLEBUFFER_H

namespace chaos
{

#if 0

class DeferredBuffer

{

public:

    void Call()

    {

        size_t index = 0;

        size_t count = buffer.size();

        while (index < count)

        {

            DeferredCallableBase * callable = (DeferredCallableBase *)&buffer[index];

            index += callable->Call();           

        }

    }

    template<typename T> void Push(T const & src)

    {

        char const * data = (char const *)&src;

        size_t count = sizeof(T);

        buffer.reserve(buffer.size() + count);

        for (size_t i = 0 ; i < count ; ++i)

            buffer.push_back(data[i]);

    }

protected:

    std::vector<char> buffer;

 

};

 
#endif 

}; // namespace chaos

#endif // CHAOS_DEFERREDCALLABLEBUFFER_H
 

 
