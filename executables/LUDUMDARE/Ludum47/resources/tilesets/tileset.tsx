<?xml version="1.0" encoding="UTF-8"?>
<tileset version="1.4" tiledversion="1.4.2" name="tileset" tilewidth="128" tileheight="128" tilecount="6" columns="0" objectalignment="center">
 <grid orientation="orthogonal" width="1" height="1"/>
 <tile id="1" type="PlayerStart" terrain="0,0,,">
  <properties>
   <property name="BITMAP_NAME" value="player"/>
   <property name="PARTICLE_CREATION" type="bool" value="false"/>
  </properties>
  <image width="32" height="16" source="../sprites/player.png"/>
 </tile>
 <tile id="32">
  <image width="32" height="32" source="../sprites/RedSquare.png"/>
 </tile>
 <tile id="33">
  <image width="128" height="128" source="../sprites/Concrete.png"/>
 </tile>
 <tile id="34">
  <image width="128" height="128" source="../sprites/grass.png"/>
 </tile>
 <tile id="35" type="Opponent">
  <image width="32" height="16" source="../sprites/opponent.png"/>
 </tile>
 <tile id="36">
  <image width="128" height="128" source="../sprites/Tree1.png"/>
 </tile>
</tileset>
