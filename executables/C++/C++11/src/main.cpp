#include <chaos/chaos.h>

// ---------------------------------------------------------------------

class ConfigurableInterface;
class ObjectConfiguration;

// ---------------------------------------------------------------------














// ---------------------------------------------------------------------



// ---------------------------------------------------------------------
class B : public chaos::Object, public chaos::ConfigurableInterface
{
public:

	virtual ~B()
	{
		int i = 0;
		++i;
	}

	void Initialize()
	{
		chaos::ObjectConfiguration* conf = GetConfiguration();

		conf = conf;

	}

	virtual void OnConfigurationChanged()
	{
		int i = 0;
		++i;

	}

};




class A : public chaos::Object, public chaos::ConfigurableInterface
{
public:

	virtual ~A()
	{
		int i = 0;
		++i;
	}

	void Initialize()
	{
		chaos::ObjectConfiguration* conf = GetConfiguration();

		conf = conf;

		b = new B;
		b->SetConfiguration(conf->CreateChildConfiguration("B"));
	}

	virtual void OnConfigurationChanged()
	{
		int i = 0;
		++i;

		b = nullptr;
	}

	chaos::shared_ptr<B> b;

};

class TreeBase
{
public:

	bool IsUseful() const
	{
		return false;
	}
};



int main(int argc, char ** argv, char ** env)
{
	chaos::Tree27<2, TreeBase> T;

	auto n = T.CreateNode(chaos::box2(
		glm::vec2(100.0f, 200.0f),
		glm::vec2(5.0f, 8.0f)
	));

	bool b = n->IsUseful();

	auto r1 = T.GetRootNode();

	T.DeleteNodeIfPossible(r1);

	auto r2 = T.GetRootNode();




	return 0;


#if 0




















	chaos::Tree27Node<2, chaos::EmptyClass> Node;

	Node.ForEachNode([](auto ptr)
	{
			ptr = ptr;
			//return 0;
	});



	struct TOTO
	{
		int index_in_parent : 5 = 0;
		int children_flags : 27;
	};

	TOTO toto;
	toto.index_in_parent = 27;
	toto.children_flags = 0b0001011;

	chaos::BitTools::ForEachBitForward(toto.children_flags, [](int bit)
		{
			bit = bit;
		});

	auto s = sizeof(TOTO);


	chaos::ObjectConfiguration conf;

	chaos::Tree27NodeInfo<2> NN;

	chaos::shared_ptr<A> a = new A;


	a->SetConfiguration(conf.CreateChildConfiguration("A"));
	a->Initialize();

	conf.OnConfigurationChanged();

	a = nullptr;
#endif
	return 0;
}