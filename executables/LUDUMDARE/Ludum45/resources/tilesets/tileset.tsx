<?xml version="1.0" encoding="UTF-8"?>
<tileset version="1.2" tiledversion="1.2.3" name="tileset" tilewidth="319" tileheight="300" tilecount="13" columns="0">
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
   <property name="ENEMY_PATTERN" value="EP_Pattern2"/>
   <property name="ENEMY_TYPE" value="ET_Type1"/>
  </properties>
  <image width="256" height="256" source="../sprites/ange1.png"/>
 </tile>
 <tile id="10" type="EnemySpawner">
  <properties>
   <property name="ENEMY_PATTERN" value="EP_Pattern3"/>
   <property name="ENEMY_TYPE" value="ET_Type2"/>
  </properties>
  <image width="300" height="300" source="../sprites/ange2.png"/>
 </tile>
 <tile id="11" type="EnemySpawner">
  <properties>
   <property name="ENEMY_PATTERN" value="EP_Pattern1"/>
   <property name="ENEMY_TYPE" value="ET_Type3"/>
  </properties>
  <image width="269" height="170" source="../sprites/Bird2.png"/>
 </tile>
 <tile id="12" type="EnemySpawner">
  <properties>
   <property name="ENEMY_PATTERN" value="EP_Pattern3"/>
   <property name="ENEMY_TYPE" value="ET_Type4"/>
  </properties>
  <image width="319" height="220" source="../sprites/bird1.png"/>
 </tile>
 <tile id="13">
  <image width="240" height="180" source="../sprites/cloud1.png"/>
 </tile>
 <tile id="14">
  <image width="270" height="201" source="../sprites/cloud2.png"/>
 </tile>
</tileset>
