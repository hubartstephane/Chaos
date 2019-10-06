<?xml version="1.0" encoding="UTF-8"?>
<tileset version="1.2" tiledversion="1.2.3" name="tileset" tilewidth="512" tileheight="512" tilecount="7" columns="0">
 <grid orientation="orthogonal" width="1" height="1"/>
 <tile id="0" type="BonusSpawner">
  <properties>
   <property name="BONUS_TYPE" value="DASH"/>
  </properties>
  <image width="256" height="256" source="../sprites/dash.png"/>
 </tile>
 <tile id="2" type="BonusSpawner">
  <properties>
   <property name="BONUS_TYPE" value="GHOST"/>
  </properties>
  <image width="256" height="256" source="../sprites/ghost.png"/>
 </tile>
 <tile id="3" type="BonusSpawner">
  <properties>
   <property name="BONUS_TYPE" value="SPECIALPOWER"/>
  </properties>
  <image width="256" height="256" source="../sprites/specialpower.png"/>
 </tile>
 <tile id="5" type="BonusSpawner">
  <properties>
   <property name="BONUS_TYPE" value="SPEED"/>
  </properties>
  <image width="256" height="256" source="../sprites/speed.png"/>
 </tile>
 <tile id="6" type="BonusSpawner">
  <properties>
   <property name="BONUS_TYPE" value="DAMAGE"/>
  </properties>
  <image width="256" height="256" source="../sprites/damage.png"/>
 </tile>
 <tile id="7" type="BonusSpawner">
  <properties>
   <property name="BONUS_TYPE" value="POWERRATE"/>
  </properties>
  <image width="256" height="256" source="../sprites/powerrate.png"/>
 </tile>
 <tile id="8" type="BonusSpawner">
  <properties>
   <property name="BONUS_TYPE" value="BONUSSPREAD"/>
  </properties>
  <image width="256" height="256" source="../sprites/powerspread.png"/>
 </tile>
</tileset>
