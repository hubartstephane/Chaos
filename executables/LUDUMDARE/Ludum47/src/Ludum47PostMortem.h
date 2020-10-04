#if 0


// Object : fixer weak_ptr/shared_ptr references circulaires
//
//
// CameraComponent : Conditionnal ApplyModifier => certains modifier devraient etre cosmetique da autre systematique
//
// Polygone -> PolyLine : creer une classe de base

// Polygone / Poyline . cela pourrait etre une surface, 
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

// ca serait bien que TMLevelInstance::Initialize puisse beneficier d un ReferenceSolver


// GameEntity : faire quelquechose de rotation . Notamment au niveau de PlayerStart


// TMObject/Particles => faire quelquechose de mieux



// on trouve "forced_tick_duratiocn" dans JSON, mais ce n'est pas utilisé


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

Calcul du Mask de Collision ComputeLayerCollisionMask devrait etre dans TMLevel et pas TMLayerInstance => on ne derrive jamais TMLayerInstance (on devrait ??)

Revoir geometry de collision :

conversion automatic de box2 => obox ??


introduce     typename type_vec<T, dimension>      it does not exists !!!


#endif

