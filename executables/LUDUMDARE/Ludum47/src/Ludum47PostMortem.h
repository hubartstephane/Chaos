#if 0


// Object : fixer weak_ptr/shared_ptr references circulaires
//
//
// CameraComponent : Conditionnal ApplyModifier => certains modifier devraient etre cosmetique da autre systematique
//
// Polygone -> PolyLine : creer une classe de base
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



#endif