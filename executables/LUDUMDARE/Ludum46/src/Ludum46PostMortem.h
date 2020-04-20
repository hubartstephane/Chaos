#if 0

// shu46
//
// FindPropertyInt(...) mais return a value or a pointer depending on the additionnal DEFAULT parameter : not a good idea
// 
// SpawnParticle(...) maybe initialize automatically color to 1.0 (=> else 0 => invisible)
//
// chaos::ParticleAccessorBase<ParticleBase> => maybe some upcasting ?
//
// Level BoundingBox. Does not take care of tile/spawner very vwell rework on that

// creer un iterator/request qui permet de chercher toutes les collisions avec un type d objet donner par layer
//
//  layer->FindCollidingObject<SoulTriggerObject>(...);

// ca serait bien que dans
//
//    TiledMapLayerInstance::CreateGeometricObjectParticles(...) 
//
// on puisse recuperer les particules que l on vient de creer pour faire notre petite ttambouille ... pb : on utilise un cache de particles (et en plus elles sont typées en TiledMapParticle : aie)
//
//    TiledMapLayerInstanceParticlePopulator::AddParticle(
//
// eventuellement les objects geometrics pourraient avoir leur propre allocation !!!
//
//
///


//
// maybe review CheckCompletedLevel/ CanCompleteLevel / CheckGameOverCondition
// 

//
// UpdateParticle/DrawParticle : un prends un pointer l autre une ref !
// 

// Make ParticleTraits inherit one another / Particles inherit
//
//    class ParticleBurnedSoulTrait : public chaos::ParticleAllocationTrait<ParticleBurnedSoul, VertexBase>
//
//
// the WrapMode::Clamp is not working for GRID !!!
//
// Make a memo why
//  - there is a BurningFire.png in tileset
//  - there is a BurningFire_6x1 in sprites
//
// Make a memo how JSON textures are working
//
// Make a memo how JSON resources are working
//
//
// Refactor AtlasGeneration to make it much faster
//
//
//
// Support des tileset mode fullcarte
//
//
// J ai ajouté une bordure redondante dans l atlas, pour eciter que les access textures n interpolent sur une ligne/colonne de padding.
// C'est pas une bonne idée. 
// Les lettres ont cette ligne qui bave


// revoir les collisions
//
//  introduire soft collision ---> compte la limite ou pas
//  les methodes IsGeometryEmpty() devraient avoir un flag en plus   soft_machin
//
// TiledMap
//
//   - requete sur les collisions
//   - utiliser un collision_mask
//

namespace CollisionFlags
{
	static int const Player = (1 << 0);
	static int const Camera = (1 << 1);
	...

};

// revoir la method RecastVector : inexploitable
//  -> faire un equivalent a auto_cast()




// et si le vertex shader avait access a toutes les informations de la particle + interpolation(X, Y) => x, y e [0..1] ?


// garder reference aux sites bien

https://www.piskelapp.com/
https://ezgif.com/



// attention visual studio est capable de passer les images en 4bpp !!! (ou paint ??)


// verifier quand on fait correlation entre .png et .json qu ils sont bien dans le meme repertoire (que l un ne va pas en _DEBUG dans les dossiers visual studio, et pas l autre)

// ParticleManager : pourrait on envisager d avoir dans un meme layer des Particles differentes, mais dans meme VERTEX

// changer les conditions de l alpha blending par layer ?

// TiledMap : models !

// TiledMap : world composition ! (cool pour avoir des arrieres plans)

// TiledMap : repeat => collision !

// TiledSet : wang ? etudier

// Atlas : choisir ce qui doit etre PIXEL ART et ce qui ne le doit pas




// the Animation.json file manifest could declare    frame_duration / animation_duration /  default_animation_mode
//
// WrapMode: add a flipflip



#endif