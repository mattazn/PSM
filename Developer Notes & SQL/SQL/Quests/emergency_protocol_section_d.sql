-- Fix for quest Emergency Protocol: Section 8.2, Paragraph D
SET @EAST := 25847;
SET @SOUTH := 25846;
SET @NORTH := 25845;
SET @SPELL := 46171;
SET @GUID := 1023015;
SET @GOGUID := 164403;
SET @GO := 300181;

DELETE FROM `conditions` WHERE `SourceEntry` = @SPELL AND `SourceTypeOrReferenceId`=13;
DELETE FROM `conditions` WHERE `SourceEntry` = @SPELL AND `SourceTypeOrReferenceId`=17;
INSERT INTO `conditions` (`SourceTypeOrReferenceId`, `SourceGroup`, `SourceEntry`, `SourceId`, `ElseGroup`, `ConditionTypeOrReference`, `ConditionTarget`, `ConditionValue1`, `ConditionValue2`, `ConditionValue3`, `NegativeCondition`, `ErrorTextId`, `ScriptName`, `Comment`) VALUES
(13,1,@SPELL,0,0,31,0,3,@EAST,0,0,0, "", "Target Emergency Torch on East Crash"),
(13,1,@SPELL,0,1,31,0,3,@NORTH,0,0,0, "", "Target Emergency Torch on Northwest Crash"),
(13,1,@SPELL,0,2,31,0,3,@SOUTH,0,0,0, "", "Target Emergency Torch on South Crash"),
(17,0,@SPELL,0,0,29,0,@EAST,30,0,0,0, "", "Scuttle Wrecked Flying Machine can only be used within 15y from a bunny."),
(17,0,@SPELL,0,1,29,0,@NORTH,30,0,0,0, "", "Scuttle Wrecked Flying Machine can only be used within 15y from a bunny."),
(17,0,@SPELL,0,2,29,0,@SOUTH,30,0,0,0, "", "Scuttle Wrecked Flying Machine can only be used within 15y from a bunny.");

DELETE FROM `creature` WHERE `id` IN (@EAST, @NORTH, @SOUTH);
INSERT INTO `creature` (`guid`, `id`, `map`, `spawnMask`, `phaseMask`, `modelid`, `equipment_id`, `position_x`, `position_y`, `position_z`, `orientation`, `spawntimesecs`, `spawndist`, `currentwaypoint`, `curhealth`, `curmana`, `MovementType`, `npcflag`, `unit_flags`, `dynamicflags`) VALUES
(@GUID,@EAST,571,1,1,17188,0,3476.53,4916.39,13.7663,2.8963,300,0,0,8982,3155,0,0,0,0),
(@GUID+1,@SOUTH,571,1,1,17188,0,3390.49,5136.02,13.1713,1.30735,300,0,0,8982,3155,0,0,0,0),
(@GUID+2,@NORTH,571,1,1,17188,0,3606.58,5063.78,13.1667,1.30735,300,0,0,8982,3155,0,0,0,0);

DELETE FROM `gameobject` WHERE `id`=@GO;
INSERT INTO `gameobject` (`guid`,`id`,`map`,`spawnMask`,`phaseMask`,`position_x`,`position_y`,`position_z`,`orientation`,`rotation0`,`rotation1`,`rotation2`,`rotation3`,`spawntimesecs`,`animprogress`,`state`) VALUES
(@GOGUID,@GO,571,1,1,3476.53,4916.39,13.7663,2.8963,0,0,0.884491,0.466557,300,0,0),
(@GOGUID+1,@GO,571,1,1,3390.49,5136.02,13.1713,1.30735,0,0,0.884491,0.466557,300,0,0),
(@GOGUID+2,@GO,571,1,1,3606.58,5063.78,13.1667,1.30735,0,0,0.884491,0.466557,300,0,0);

-- Wrecked Flying Machine Bunny SAI
DELETE FROM `smart_scripts` WHERE `entryorguid` BETWEEN 25845 AND 25847;
UPDATE `creature_template` SET AIName="SmartAI", `MovementType`=0, `unit_flags`=33554948 WHERE entry BETWEEN 25845 AND 25847;
INSERT INTO `smart_scripts` (`entryorguid`,`source_type`,`id`,`link`,`event_type`,`event_phase_mask`,`event_chance`,`event_flags`,`event_param1`,`event_param2`,`event_param3`,`event_param4`,`action_type`,`action_param1`,`action_param2`,`action_param3`,`action_param4`,`action_param5`,`action_param6`,`target_type`,`target_param1`,`target_param2`,`target_param3`,`target_x`,`target_y`,`target_z`,`target_o`,`comment`) VALUES
-- North
(@NORTH,0,0,0,8,0,100,0,@SPELL,0,0,0,33,@NORTH,0,0,0,0,0,7,0,0,0,0.0,0.0,0.0,0.0,"Northwest Crash - On Spellhit - Give Quest Credit"),
(@NORTH,0,1,0,8,0,100,0,@SPELL,0,0,0,11,71597,0,0,0,0,0,20,@GO,0,0,0.0,0.0,0.0,0.0,"Northwest Crash - On Spellhit - Cast Cosmetic Fire"),
-- South
(@SOUTH,0,0,0,8,0,100,0,@SPELL,0,0,0,33,@SOUTH,0,0,0,0,0,7,0,0,0,0.0,0.0,0.0,0.0,"South Crash - On Spellhit - Give Quest Credit"),
(@SOUTH,0,1,0,8,0,100,0,@SPELL,0,0,0,11,71597,0,0,0,0,0,20,@GO,0,0,0.0,0.0,0.0,0.0,"South Crash - On Spellhit - Cast Cosmetic Fire"),
-- East
(@EAST,0,0,0,8,0,100,0,@SPELL,0,0,0,33,@EAST,0,0,0,0,0,7,0,0,0,0.0,0.0,0.0,0.0,"East Crash - On Spellhit - Give Quest Credit"),
(@EAST,0,1,0,8,0,100,0,@SPELL,0,0,0,11,71597,0,0,0,0,0,20,@GO,0,0,0.0,0.0,0.0,0.0,"East Crash - On Spellhit - Cast Cosmetic Fire");

UPDATE `creature_template` SET `ScriptName`='npc_recon_pilot',`AIName`='' WHERE `entry`=25841;