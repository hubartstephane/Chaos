<?xml version="1.0" encoding="UTF-8"?>
<tileset name="tileset" tilewidth="640" tileheight="640" tilecount="10" columns="0">
 <grid orientation="orthogonal" width="1" height="1"/>
 <tile id="2">
  <properties>
   <property name="ATLAS_FREQUENCY" type="float" value="0.1"/>
   <property name="ATLAS_SIZE_X" type="int" value="1"/>
   <property name="ATLAS_SIZE_Y" type="int" value="3"/>
   <property name="ATLAS_SKIP_LAST" type="int" value="0"/>
   <property name="OBJECT_TYPE" type="int" value="0"/>
  </properties>
  <image width="563" height="226" source="player.png"/>
  <objectgroup draworder="index"/>
 </tile>
 <tile id="3">
  <properties>
   <property name="ATLAS_FREQUENCY" type="float" value="0.1"/>
   <property name="ATLAS_SIZE_X" type="int" value="3"/>
   <property name="ATLAS_SIZE_Y" type="int" value="3"/>
   <property name="ATLAS_SKIP_LAST" type="int" value="0"/>
   <property name="OBJECT_TYPE" type="int" value="2"/>
  </properties>
  <image width="128" height="128" source="flames.png"/>
 </tile>
 <tile id="4">
  <properties>
   <property name="OBJECT_TYPE" type="int" value="1"/>
  </properties>
  <image width="400" height="400" source="../sprites/planet1.png"/>
 </tile>
 <tile id="5">
  <properties>
   <property name="OBJECT_TYPE" type="int" value="1"/>
  </properties>
  <image width="400" height="400" source="../sprites/planet2.png"/>
 </tile>
 <tile id="6">
  <properties>
   <property name="OBJECT_TYPE" type="int" value="1"/>
  </properties>
  <image width="400" height="400" source="../sprites/planet3.png"/>
 </tile>
 <tile id="7">
  <properties>
   <property name="OBJECT_TYPE" type="int" value="1"/>
  </properties>
  <image width="640" height="640" source="../sprites/planet4.png"/>
  <objectgroup draworder="index"/>
 </tile>
 <tile id="8">
  <properties>
   <property name="OBJECT_TYPE" type="int" value="1"/>
  </properties>
  <image width="400" height="400" source="../sprites/planet5.png"/>
 </tile>
 <tile id="9">
  <properties>
   <property name="OBJECT_TYPE" type="int" value="1"/>
  </properties>
  <image width="500" height="500" source="../sprites/planet6.png"/>
 </tile>
 <tile id="10">
  <properties>
   <property name="OBJECT_TYPE" type="int" value="1"/>
  </properties>
  <image width="464" height="464" source="../sprites/planet7.png"/>
 </tile>
 <tile id="11">
  <properties>
   <property name="OBJECT_TYPE" type="int" value="1"/>
  </properties>
  <image width="128" height="128" source="brickwall.png"/>
  <objectgroup draworder="index">
   <properties>
    <property name="OBJECT_TYPE" type="int" value="1"/>
   </properties>
  </objectgroup>
 </tile>
</tileset>
