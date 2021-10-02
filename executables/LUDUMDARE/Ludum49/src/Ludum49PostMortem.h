- on a du code dans TMObject du genre

   std::string const* classname = in_typed_object->FindPropertyString("classname");

   ca ressemble a s y meprendre a du JSON. peut etre un pont a faire.

- TMObjectFactory TMLevel::DoGetExplicitObjectFactory(TMLayerInstance* in_layer_instance, TiledMap::TypedObject const * in_typed_object)

  on travail avec "classname" mais on pourrait aussi bien utiliser type non ?

- conversion automatique de float et double

class BiValue
   {

	   BiValue(double v) ...

	   operator float() const { return value1; }

	   operator double() const { return value2; }


	   float value1;

	   double value2;

   };

   chaos::pi = BiValue(3.14) ..

	   probleme sur les arrithmetique

	   pi * pi -> c'est quoi le type





-Est ce que le background ne pourrait pas etre juste une layer dans TM ?


-Pour TiledMap::GeometricObjectPolygon/GeometricObjectPolyline

le bounding box a une taille de 0.!!
cependant, la position correspond au premier point   points[0] (qui est donc toujours 0,0)
ca serait peut etre bien de calculer la bounding box pour de vrai (quitte a deplacer tous les points)





- faire un single include pour lexterieur
<chaos/chaos.h>
ne pas l'utiliser dans chaos, optimiser les dependances'



-TM BoundingBox, oriented BoundingBox pour les collisions

- position initiale du pawn semble foireuse