-- Gothik uptdate
UPDATE creature_template
SET unit_flags=32832,unit_flags2=2048,spell1=0,spell2=0
WHERE entry=16060;

-- Trainee update
UPDATE creature_template
SET faction_A=21, faction_H=21, unit_flags2=2048, spell8=27892
WHERE entry=16124;

-- Naxxramas trigger
UPDATE creature_template
SET minlevel=80,maxlevel=80,unit_flags=33554432,unit_flags2=2048,flags_extra=2
WHERE entry=16137;

-- Fix life <-> dead side spawns in Gothik the Harvester fight
UPDATE `creature_template` SET `spell8` = 27892 WHERE `entry` IN (16124, 29987);
UPDATE `creature_template` SET `spell8` = 27928 WHERE `entry` IN (16125, 29985);
UPDATE `creature_template` SET `spell8` = 27935 WHERE `entry` IN (16126, 29986);

DELETE FROM creature_template WHERE entry=16124;

INSERT INTO creature_template VALUES 
(16124,29987,0,0,0,0,16608,0,0,0,'Unrelenting Trainee',NULL,NULL,0,80,80,2,21,21,0,1,1.14286,1,0,422,586,0,642,1,1800,0,1,32768,2048,8,0,0,0,0,0,345,509,103,7,72,0,0,0,0,0,0,0,0,0,55604,0,0,0,0,0,0,27892,0,0,0,0,'',0,3,1,0.8,1,1,0,0,0,0,0,0,0,144,1,1811,0,0,'mob_gothik_minion',12340);

DELETE FROM creature_template WHERE entry=16060;

INSERT INTO creature_template VALUES 
(16060,29955,0,0,0,0,16279,0,0,0,'Gothik the Harvester','',NULL,0,83,83,2,14,14,0,1,1.14286,1,3,496,674,0,783,35,2000,0,2,32832,2048,8,0,0,0,0,0,365,529,98,6,76,16060,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,733851,896929,'',0,3,1,60,100,1,0,0,0,0,0,0,0,0,1,1763,617299803,1,'boss_gothik',12340);

DELETE FROM conditions WHERE SourceEntry=27892;
INSERT INTO conditions VALUES
(13, 1, 27892, 0, 0, 31, 0, 3, 16060, 0, 0, 0, 0, '', NULL);