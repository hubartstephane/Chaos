<?xml version="1.0" encoding="UTF-8"?>
<tileset version="1.2" tiledversion="1.2.3" name="tileset" tilewidth="300" tileheight="300" tilecount="9" columns="0">
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
   <property name="BONUS_TYPE" value="POWERSPREAD"/>
  </properties>
  <image width="256" height="256" source="../sprites/powerspread.png"/>
 </tile>
 <tile id="9" type="EnemySpawner">
  <properties>
   <property name="ENEMY_TYPE" value="ET_Type1"/>
   <property name="PATTERN_TYPE" value="EP_Pattern2"/>
  </properties>
  <image width="256" height="256" source="../sprites/ange1.png"/>
 </tile>
 <tile id="10" type="EnemySpawner">
  <properties>
   <property name="ENEMY_TYPE" value="ET_Type2"/>
   <property name="PATTERN_TYPE" value="EP_Pattern2"/>
  </properties>
  <image width="300" height="300" source="../sprites/ange2.png"/>
 </tile>
</tileset>
