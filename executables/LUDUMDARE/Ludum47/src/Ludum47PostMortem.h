#if 0


// CameraComponent : Conditionnal ApplyModifier => certains modifier devraient etre cosmetique da autre systematique
//
// Polygone -> PolyLine : creer une classe de base

// Polygone / Poyline . cela pourrait etre une surface, q
=> pas si evident car les points sont exprimées relativement a la position de lobjet


//
// TMObject : un post Load ?
//
// for (auto ob : IterateByClass<TOTO>(my_array))
{
ca retourne un object avec begin()/end ++
ca fait auto_cast => + travail sur les pointeur weak_shared
et ca filtre par class
}
//

// GameEntity : faire quelquechose de rotation . Notamment au niveau de PlayerStart


// TMObject/Particles => faire quelquechose de mieux



// la camera avec le zoom modifier ne semble pas rester dans le WORLD BOUNDING BOX


// faire en sorte que les particules puissent connaitre facilement (grace a l'allocation ??) le TMObjec a qui elles appartiennent
// LudumOpponent a montre ses limites


// PlayerPawn est un GameEntity.
// TMObject aussi.
//

mais PlayerPawn n'est pas referencé dans TMLevelInstance ni aucun de ces layer



// Uniformiser la gestion des parametres JSON

->json dans TMObject::Initialize()
->json dans Player::Initialize()
->json dans SpecialClass


->Update en runtime les special classes ??




Fix WrapMode::Wrap avec float

Revoir geometry de collision :

conversion automatic de box2 => obox ??


introduce     typename type_vec<T, dimension>      it does not exists !!!




utiliser : int a = int(3); au lieu de ((int)a)



Creer un vrai particle spawner
qui s'attache a un layer
qui s'alloue un ParticleAllocation pour son propre usage
avec eventuellement des debit

genre

Pawn::Tick(dt)
{
	spawner->Tick(dt, [](SmokeParticleAccessor & accessor)
	{
		le spawner decide combien de particule il va creer (avec un spawn rate),

			et on peut les initialiser ici

	})
}






On a un BeginParticleUpdate. On pourrait avoir un EndParticleUpdate()





#endif

