<?xml version="1.0" encoding="UTF-8"?>
<tileset version="1.2" tiledversion="1.2.3" name="tileset" tilewidth="319" tileheight="300" tilecount="18" columns="0">
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
 <tile id="9" type="EnemySpawner1">
  <image width="256" height="256" source="../sprites/ange1.png"/>
 </tile>
 <tile id="10" type="EnemySpawner2">
  <image width="300" height="300" source="../sprites/ange2.png"/>
 </tile>
 <tile id="11" type="EnemySpawner3">
  <image width="269" height="170" source="../sprites/Bird2.png"/>
 </tile>
 <tile id="12" type="EnemySpawner4">
  <image width="319" height="220" source="../sprites/bird1.png"/>
 </tile>
 <tile id="13">
  <image width="240" height="180" source="../sprites/cloud1.png"/>
 </tile>
 <tile id="14">
  <image width="270" height="201" source="../sprites/cloud2.png"/>
 </tile>
 <tile id="15" type="EnemySpawner5">
  <properties>
   <property name="ENEMY_TYPE" value="ET_Type5"/>
  </properties>
  <image width="282" height="249" source="../sprites/satan1.png"/>
 </tile>
 <tile id="16" type="EnemySpawner6">
  <image width="138" height="170" source="../sprites/monster1.png"/>
 </tile>
 <tile id="17" type="EnemySpawner7">
  <image width="167" height="135" source="../sprites/monster2.png"/>
 </tile>
 <tile id="18" type="EnemySpawner8">
  <image width="206" height="220" source="../sprites/rabbit1.png"/>
 </tile>
 <tile id="19" type="EnemySpawner9">
  <image width="165" height="165" source="../sprites/rabbit2.png"/>
 </tile>
</tileset>
