-- Fix quest: It's All Fun and Games http://www.wowhead.com/quest=12892#comments
-- ID indexes
-- Thanks to Vincent-Michael for condition corrections
SET @TheOcular :=         29747;
SET @Script := @TheOcular * 100;
SET @OcularShell :=       29790;
SET @CreditSpell :=       55288;
SET @DeathlyStare :=      55269;
SET @Transform :=         55162;
SET @Detection :=         18950;

-- Update templates to prevent falling
UPDATE `creature_template` SET `InhabitType`=4 WHERE `entry`=@TheOcular;
UPDATE `creature_template` SET `InhabitType`=4,`unit_flags`=unit_flags|0x00000200|0x02000000 WHERE `entry`=@OcularShell; -- the movement disable flag should be replaced by movement flag root when it gets supported by SAI or if it does

-- Add SAI support for The Ocular
DELETE FROM `smart_scripts` WHERE `entryorguid`=@TheOcular AND `source_type`=0;
DELETE FROM `smart_scripts` WHERE `entryorguid`=@Script AND `source_type`=9;
INSERT INTO `smart_scripts` (`entryorguid`,`source_type`,`id`,`link`,`event_type`,`event_phase_mask`,`event_chance`,`event_flags`,`event_param1`,`event_param2`,`event_param3`,`event_param4`,`action_type`,`action_param1`,`action_param2`,`action_param3`,`action_param4`,`action_param5`,`action_param6`,`target_type`,`target_param1`,`target_param2`,`target_param3`,`target_x`,`target_y`,`target_z`,`target_o`,`comment`) VALUES
(@TheOcular,0,0,1,25,0,100,1,0,0,0,0,75,@Transform,0,0,0,0,0,1,0,0,0,0,0,0,0,'Ocular - On spawn/reset - Add aura transform'),
(@TheOcular,0,1,0,61,0,100,1,0,0,0,0,103,1,0,0,0,0,0,1,0,0,0,0,0,0,0,'Ocular - Linked with previous event - Set root state on'),
(@TheOcular,0,2,0,25,0,100,0,0,0,0,0,8,1,0,0,0,0,0,1,0,0,0,0,0,0,0,'Ocular - On spawn/reset - Set react state defensive'),
(@TheOcular,0,3,4,8,0,100,1,30740,0,0,0,8,2,0,0,0,0,0,1,0,0,0,0,0,0,0,'Ocular - On hit by item spell - Set react state aggressive (once per reset)'),
(@TheOcular,0,4,0,61,0,100,0,0,0,0,0,11,55269,0,0,0,0,0,7,0,0,0,0,0,0,0,'Ocular - Linked with previous event - Cast spell on invoker to set in combat'),
(@TheOcular,0,5,0,0,0,100,0,1500,1500,4000,5000,11,55269,0,0,0,0,0,2,0,0,0,0,0,0,0,'Ocular - IC - Cast Deathly Stare every 3 secs'),
(@TheOcular,0,6,7,6,0,100,0,0,0,0,0,11,@CreditSpell,0,0,0,0,0,1,0,0,0,0,0,0,0,'Ocular - On death - Cast The Ocular On Death'),
(@TheOcular,0,7,0,61,0,100,0,0,0,0,0,80,@Script,2,0,0,0,0,1,0,0,0,0,0,0,0,'Ocular - Linked with previous event - Start actionlsit'),
(@Script,9,0,0,0,0,100,0,2000,2000,0,0,47,0,0,0,0,0,0,10,152166,@OcularShell,0,0,0,0,0,'Ocular - Action 0 - Set unseen on shell'),
(@Script,9,1,0,0,0,100,0,0,0,0,0,47,0,0,0,0,0,0,1,0,0,0,0,0,0,0,'Ocular - Action 1 - Set unseen on self'),
(@Script,9,2,0,0,0,100,0,0,0,0,0,41,100,0,0,0,0,0,10,152166,@OcularShell,0,0,0,0,0,'Ocular - Action 2 - Despawn shell'),
(@Script,9,3,0,0,0,100,0,0,0,0,0,41,100,0,0,0,0,0,1,0,0,0,0,0,0,0,'Ocular - Action 3 - Despawn self');

-- Creature_template_addon data
DELETE FROM `creature_template_addon` WHERE `entry` IN (@TheOcular,@OcularShell);
INSERT INTO `creature_template_addon` (`entry`, `path_id`, `mount`, `bytes1`, `bytes2`, `emote`, `auras`) VALUES
(@TheOcular,0,0,0,1,0, '18950'),
(@OcularShell,0,0,0,1,0, '18950');

-- Conditions
DELETE FROM `conditions` WHERE `SourceEntry`=@CreditSpell AND `SourceTypeOrReferenceId`=13;
INSERT INTO `conditions` (`SourceTypeOrReferenceId`, `SourceGroup`, `SourceEntry`, `SourceId`, `ElseGroup`, `ConditionTypeOrReference`, `ConditionTarget`, `ConditionValue1`, `ConditionValue2`, `ConditionValue3`, `NegativeCondition`, `ErrorTextId`, `ScriptName`, `Comment`) VALUES 
(13,1,@CreditSpell,0,1,32,0,144,0,0,0,0,'', 'Death of Ocular can hit only players'),
(13,1,@CreditSpell,0,1,9,0,12892,0,0,0,0,'', 'It''s all fun and games (H) must be taken'),
(13,1,@CreditSpell,0,1,28,0,12892,0,0,1,0,'', 'It''s all fun and games (H) must not have objectives completed'),
(13,1,@CreditSpell,0,2,32,0,144,0,0,0,0,'', 'Death of Ocular can hit only players'),
(13,1,@CreditSpell,0,2,9,0,12887,0,0,0,0,'', 'It''s all fun and games (A) must be taken'),
(13,1,@CreditSpell,0,2,28,0,12887,0,0,1,0,'', 'It''s all fun and games (A) must not have objectives completed');

-- Insert spell_script names
DELETE FROM `spell_script_names` WHERE `spell_id`=@CreditSpell;
INSERT INTO `spell_script_names` (`spell_id`,`ScriptName`) VALUES
(@CreditSpell, 'spell_q12892_12887_ocular_death');