#include "chaos/Chaos.h"


class A;
class B;
class ClassBase;

// ------------------------------------------------------

class ClassBase
{
public:

    ClassBase()
    {
        std::cout << "ClassBase::ClassBase()" << std::endl;
    }

    virtual A* Create() const = 0;
};


// ------------------------------------------------------

template<typename T>
class TClass;

template<>
class TClass<A> : public ClassBase
{
public:

    virtual A* Create() const override;
};

template<typename T>
class TClass : public TClass<typename T::Super>
{
public:

    virtual T* Create() const override
    {
        return new T;
    }
};

// ------------------------------------------------------

class A
{
public:

    static inline TClass<A> static_class_A;

    static TClass<A> const* GetStaticClass()
    {
        return &static_class_A;
    }

    virtual TClass<A> const* GetClass() const
    {
        return &static_class_A;
    }

    A()
    {
        std::cout << "A::A()" << std::endl;
    }

    virtual ~A() = default;

};

// ------------------------------------------------------

A* TClass<A>::Create() const
{
    return new A;
}

// ------------------------------------------------------

class B : public A
{
public:

    using Super = A;

    static inline TClass<B> static_class_B;

    static TClass<B> const* GetStaticClass()
    {
        return &static_class_B;
    }

    virtual TClass<B> const* GetClass() const
    {
        return &static_class_B;
    }

    B()
    {
        std::cout << "B::B()" << std::endl;
    }
};

//-----------------------------------------------

class XXX
{
};

class Res
{
public:

    Res()
    {
        myxxx = &xxx;
    }

    template<typename T>
    operator T * () const
    {
        return (T*)myxxx;
    }

protected:

    void * myxxx = nullptr;

    int xxx = 666;
};

class Manager
{
public:

    Res GetRes() const
    {
        return {};
    }
};











int main(int argc, char ** argv, char ** env)
{
    XXX * xxx = Manager().GetRes();


    TClass<A> const * CA = new TClass<B>();
    A * a = CA->Create();

	return 0;
}