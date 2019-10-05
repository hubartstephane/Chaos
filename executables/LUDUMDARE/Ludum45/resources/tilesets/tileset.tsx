<?xml version="1.0" encoding="UTF-8"?>
<tileset version="1.2" tiledversion="1.2.3" name="tileset" tilewidth="128" tileheight="128" tilecount="5" columns="0">
 <grid orientation="orthogonal" width="1" height="1"/>
 <tile id="0" type="BonusSpawner">
  <properties>
   <property name="BONUS_TYPE" value="DASH"/>
  </properties>
  <image width="128" height="128" source="../sprites/dash.png"/>
 </tile>
 <tile id="1" type="BonusSpawner">
  <properties>
   <property name="BONUS_TYPE" value="POWER"/>
  </properties>
  <image width="128" height="128" source="../sprites/power.png"/>
 </tile>
 <tile id="2" type="BonusSpawner">
  <properties>
   <property name="BONUS_TYPE" value="GHOST"/>
  </properties>
  <image width="128" height="128" source="../sprites/ghost.png"/>
 </tile>
 <tile id="3" type="BonusSpawner">
  <properties>
   <property name="BONUS_TYPE" value="SPECIALPOWER"/>
  </properties>
  <image width="128" height="128" source="../sprites/specialpower.png"/>
 </tile>
 <tile id="4" type="BonusSpawner">
  <properties>
   <property name="BONUS_TYPE" value="VIEW"/>
  </properties>
  <image width="128" height="128" source="../sprites/view.png"/>
 </tile>
</tileset>
