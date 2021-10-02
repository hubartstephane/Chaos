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