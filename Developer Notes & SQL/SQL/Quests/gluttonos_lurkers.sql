-- Quest: Gluttonous Lurkers (12527)
-- SAI for Zul'Drak Rat
SET @ENTRY :=28202;
UPDATE `creature_template` SET `AIName`='SmartAI' WHERE `entry`=@ENTRY;
DELETE FROM `smart_scripts` WHERE `source_type`=0 AND `entryorguid`=@ENTRY;
INSERT INTO `smart_scripts` (`entryorguid`,`source_type`,`id`,`link`,`event_type`,`event_phase_mask`,`event_chance`,`event_flags`,`event_param1`,`event_param2`,`event_param3`,`event_param4`,`action_type`,`action_param1`,`action_param2`,`action_param3`,`action_param4`,`action_param5`,`action_param6`,`target_type`,`target_param1`,`target_param2`,`target_param3`,`target_x`,`target_y`,`target_z`,`target_o`,`comment`) VALUES
(@ENTRY,0,0,0,8,0,100,0,50926,0,1000,1000,41,0,0,0,0,0,0,1,0,0,0,0,0,0,0, 'Zul''Drak Rat - Despawn on Spell Dummy');

-- Remove EAI Scripts
DELETE FROM creature_ai_scripts WHERE creature_id IN (28145,28203);

-- SAI for Gorged Lurking Basilisk
SET @ENTRY :=28203;
UPDATE `creature_template` SET `AIName`='SmartAI' WHERE `entry`=@ENTRY;
DELETE FROM `smart_scripts` WHERE `source_type`=0 AND `entryorguid`=@ENTRY;
INSERT INTO `smart_scripts` (`entryorguid`,`source_type`,`id`,`link`,`event_type`,`event_phase_mask`,`event_chance`,`event_flags`,`event_param1`,`event_param2`,`event_param3`,`event_param4`,`action_type`,`action_param1`,`action_param2`,`action_param3`,`action_param4`,`action_param5`,`action_param6`,`target_type`,`target_param1`,`target_param2`,`target_param3`,`target_x`,`target_y`,`target_z`,`target_o`,`comment`) VALUES
(@ENTRY,0,0,0,8,0,100,0,50918,0,1000,1000,41,0,0,0,0,0,0,1,0,0,0,0,0,0,0, 'Gorged Lurking Basilisk - Despawn on Spell Dummy');

-- SAI for Lurking Basilisk
SET @ENTRY :=28145;
UPDATE `creature_template` SET `AIName`='SmartAI' WHERE `entry`=@ENTRY;
DELETE FROM `smart_scripts` WHERE `source_type`=0 AND `entryorguid`=@ENTRY;
DELETE FROM `smart_scripts` WHERE `source_type`=9 AND `entryorguid`=@ENTRY*100;
INSERT INTO `smart_scripts` (`entryorguid`,`source_type`,`id`,`link`,`event_type`,`event_phase_mask`,`event_chance`,`event_flags`,`event_param1`,`event_param2`,`event_param3`,`event_param4`,`action_type`,`action_param1`,`action_param2`,`action_param3`,`action_param4`,`action_param5`,`action_param6`,`target_type`,`target_param1`,`target_param2`,`target_param3`,`target_x`,`target_y`,`target_z`,`target_o`,`comment`) VALUES
(@ENTRY,0,0,0,0,0,100,0,5000,5000,17000,27000,11,54470,1,0,0,0,0,2,0,0,0,0,0,0,0, 'Lurking Basilisk - Cast Venemous Bite'),
(@ENTRY,0,1,0,23,0,100,0,50894,5,0,0,80,@ENTRY*100,0,2,0,0,0,1,0,0,0,0,0,0,0, 'Lurking Basilisk - Start Script'),
(@ENTRY*100,9,0,0,0,0,100,0,0,0,0,0,24,0,0,0,0,0,0,1,0,0,0,0,0,0,0, 'Lurking Basilisk - Gorged Lurking Basilisk'),
(@ENTRY*100,9,1,0,0,0,100,0,0,0,0,0,12,28203,1,30000,0,0,0,1,0,0,0,0,0,0,0, 'Lurking Basilisk - Gorged Lurking Basilisk'),
(@ENTRY*100,9,2,0,0,0,100,0,0,0,0,0,41,0,0,0,0,0,0,1,0,0,0,0,0,0,0, 'Lurking Basilisk - Despawn');

DELETE FROM `creature_ai_scripts` WHERE `creature_id` IN (28145,28203);
UPDATE `creature_template` SET `ainame`='SmartAI' WHERE `entry` IN (28203,28145);
DELETE FROM `smart_scripts` WHERE `entryorguid` IN (28145,28203) AND `source_type`=0;
INSERT INTO `smart_scripts` VALUES
('28145','0','0','0','0','0','100','0','5000','5000','17000','27000','11','54470','0','0','0','0','0','2','0','0','0','0','0','0','0','In Combat - Cast Venemous Bite'),
('28145','0','1','2','23','0','100','1','50894','5','100','200','41','100','0','0','0','0','0','1','0','0','0','0','0','0','0','On 5 stacks of 50894 - Despawn '),
('28145','0','2','0','61','0','100','0','0','0','0','0','11','50928','2','0','0','0','0','2','0','0','0','0','0','0','0','Link With Event 1 - Summon Gorged Basilisk'),
('28203','0','0','1','8','0','100','0','50918','0','0','0','11','50919','2','0','0','0','0','7','0','0','0','0','0','0','0','On Spell Hit - Cast 50919 On Invoker'),
('28203','0','1','0','61','0','100','0','0','0','0','0','41','0','0','0','0','0','0','1','0','0','0','0','0','0','0','Link With Event 0 - Despawn');