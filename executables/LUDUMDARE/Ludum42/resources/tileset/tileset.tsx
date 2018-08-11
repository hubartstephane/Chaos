<?xml version="1.0" encoding="UTF-8"?>
<tileset name="tileset" tilewidth="128" tileheight="128" tilecount="4" columns="0">
 <grid orientation="orthogonal" width="1" height="1"/>
 <tile id="0">
  <properties>
   <property name="OBJECT_TYPE" type="int" value="1"/>
  </properties>
  <image width="128" height="128" source="brickwall.png"/>
 </tile>
 <tile id="1">
  <image width="128" height="128" source="ground.png"/>
  <objectgroup draworder="index"/>
 </tile>
 <tile id="2">
  <properties>
   <property name="OBJECT_TYPE" type="int" value="0"/>
  </properties>
  <image width="128" height="128" source="player.png"/>
 </tile>
 <tile id="3">
  <image width="128" height="128" source="flames.png"/>
 </tile>
</tileset>
