#if 0


  ------------------------------------------------------

TMTileCollisionIterator TMLevelInstance::GetTileCollisionIterator(box2 const& in_collision_box, uint64_t in_collision_mask, bool in_open_geometry)

->collision avec n importe quel type de primitive box/sphere...

class CollisionRequest()
{
	box2 / sphere2 / always;
	bool open_geometry;
}

reecriture de l iterator ... combinaison d iterator

	------------------------------------------------------

revoir DisplacementComponent qui est carrement vide

GetVelocity() ...
GetMaxVelocity() ...
GetMaxDistance() ... (pour les grosses chutes de framerate) ???

------------------------------------------------------

faire meilleure gestion du tile (pas forcement par particle libre)

------------------------------------------------------

revoir CollisionMask

  permettre d ajouter ses propre flag plus facilement. Est ce que cela ne pourrait pas devenir des tag ?

------------------------------------------------------

certaines fonction sont virtual dans TMLevel, d autres dans TMLevelInstance. Faire le tri


	GetCollisionFlagByName

	------------------------------------------------------

dans

	ParticleLayerBase* LudumLevel::DoCreateParticleLayer(TMLayerInstance* layer_instance)

	on creer des layers de particles avec

	   new ParticleLayer<ParticleAnimatedLayerTrait>();

    on est donc oblig� de creer de LayerTrait alors qu on pourrait faire de l inplace


		new ParticleLayer<PARTICLE, VERTEX>();

	------------------------------------------------------

dans

bool LudumLevel::FinalizeLayerParticles(TMLayerInstance* layer_instance, ParticleAllocationBase* allocation)

for (GameObjectParticle& particle : accessor)

on a particle.bitmap_info   ... mais on ne trouve pas l information du tileset qui a permis sa creation

il ya

TMParticle::gid ... mais un acces directement au TileData serait il pas mieux ?


------------------------------------------------------

ne devrait on pas s 'affranchir de GetTiledMap() depuis TMLayer/Level Instance

------------------------------------------------------

les TMparticle ne savent pas ou elles sont sur la grille. Passer en mode grille est tres fastidieux


------------------------------------------------------

TMTileCollisionIterator it = GetTileCollisionIterator(GetBoundingBox(), COLLISION_GAMEOBJECT, false);
	while (it)

	reflechir a avoir un begin() et end() pour des meilleurs iterations

		------------------------------------------------------

numeric_limits for glm ??

		glm::vec2 min_position = { std::numeric_limits<float>::max(), std::numeric_limits<float>::max() };
	glm::vec2 max_position = { std::numeric_limits<float>::min(), std::numeric_limits<float>::min() };

	------------------------------------------------------

		glm::vec2 std::min(v1, v2); --> il y a glm::min(...)

	------------------------------------------------------

	ParticleTools devrait disparaitre

		------------------------------------------------------

		DrawInterface / GetDebugDrawInterface();

	pourquoi ne pas avoir des parametres par defaut

	DI->SetColor(...)
	DI->SetFill(...)

		------------------------------------------------------

		shared_ptr -> ref_ptr  (pour eviter le conflit avec STL)

		------------------------------------------------------


autorecenter camera : le recentrage automatique fait des marches d escaliers visibles sur dans grandes zone de camra

meilleur parametrage du recentrage

------------------------------------------------------

s 'affranchir de ' dans les projets ludums ???

------------------------------------------------------

quelques ennumeration de couleur YELLOW, GREEN ...

------------------------------------------------------

Dans GameHUD la notion de text/format n'est pas tres claire'


------------------------------------------------------

 spawning partilces ... on perd le GID

		------------------------------------------------------

		un flag par particle DESTROY ??

		------------------------------------------------------

		le system de respawn est pourri
		si le pawn n'as plus de particle c'est la merde

		------------------------------------------------------


	CjeckPlayerAlive() -> peut etre inverser la logique avec des kill ??

		------------------------------------------------------

	CameraComponent(player_index)  bahhh

		l index dans le constructeur c'est mal'


		------------------------------------------------------

		Player::OnReadConfigurableProperties ... Player::LoadFromJSON
		ca fait double emploui,
		l un est utilis� pour le hot reload, l autre les checkpoints

		------------------------------------------------------

		la recherche dans l'atlas /sprite/truc/bitmap est tres fastidieuse


		------------------------------------------------------

		Particle Spawner ... voir si on ne peut pas avoir

		EIGHT_BIT_FLAG AUTOMATIQUEMENT ... voir d autres choses

		AtlasBitmapInfo a coller dans une particle ou GID


		------------------------------------------------------


		creer un iterator sur glm:vec2 ?


		------------------------------------------------------

		une vraie rotation de sprite en mode HEIGHT_BPP (avec les texels qui restent align�s avec l ecran)

		------------------------------------------------------

		HEIGHT BIT TEXT

		------------------------------------------------------

		il manque des balides [DPAD] ... pour le text format�


		------------------------------------------------------

		faire que l on puisse desactiver certains elements du HUD par fichier de config (cf scrore)

		------------------------------------------------------


		bug layer whole is clipped if initial bounding box is outside the camera
		(even if particles have moved outside
		)




#endif


