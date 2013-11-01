-- Sniffing Out the Perpetrator
SET @FROSTHOUND = 29677;
SET @PURSUER = 29696;
-- Creature Text For FrostHound
DELETE FROM `creature_text` WHERE `entry` = @FROSTHOUND;
INSERT INTO `creature_text` (`entry`, `groupid`, `id`, `text`, `type`, `language`, `probability`, `emote`, `duration`, `sound`, `comment`) VALUES 
(@FROSTHOUND, 0, 0, 'The %s sniffs the ground to pickup the trail.', 16, 0, 0, 377, 0, 0, 'FrostHound'),
(@FROSTHOUND, 1, 0, 'You''ve been seen! Use the net and ice slick to keep the dwarves away!', 42, 0, 0, 0, 0, 0, 'FrostHound'),
(@FROSTHOUND, 2, 0, 'The %s has tracked the scent to its source.', 16, 0, 0, 0, 0, 0, 'FrostHound'),
(@FROSTHOUND, 3, 0, 'The frosthound has located the thief''s hiding place. Confront him!', 42, 0, 0, 0, 0, 0, 'FrostHound');
-- Add Waypoint FrostHound
DELETE FROM `waypoints` WHERE `entry` = @FROSTHOUND;
INSERT INTO `waypoints` (`entry`, `pointid`, `position_x`, `position_y`, `position_z`, `point_comment`) VALUES 
(@FROSTHOUND, 1, 7164.398, -764.5809, 892.4199, 'FrostHound'),
(@FROSTHOUND, 2, 7173.071, -777.0275, 899.1846, 'FrostHound'),
(@FROSTHOUND, 3, 7182.944, -781.4821, 904.6174, 'FrostHound'),
(@FROSTHOUND, 4, 7193.046, -787.1729, 910.843, 'FrostHound'),
(@FROSTHOUND, 5, 7205.287, -793.2994, 917.2863, 'FrostHound'),
(@FROSTHOUND, 6, 7218.268, -802.1098, 921.6022, 'FrostHound'),
(@FROSTHOUND, 7, 7230.444, -816.3606, 924.8938, 'FrostHound'),
(@FROSTHOUND, 8, 7232.167, -834.2318, 926.6146, 'FrostHound'),
(@FROSTHOUND, 9, 7247.474, -848.1854, 925.8355, 'FrostHound'),
(@FROSTHOUND, 10, 7263.333, -875.3375, 925.127, 'FrostHound'),
(@FROSTHOUND, 11, 7275.556, -895.3395, 926.4728, 'FrostHound'),
(@FROSTHOUND, 12, 7284.952, -947.7603, 918.976, 'FrostHound'),
(@FROSTHOUND, 13, 7294.833, -985.7026, 915.6023, 'FrostHound'),
(@FROSTHOUND, 14, 7310.106, -1018.042, 913.7565, 'FrostHound'),
(@FROSTHOUND, 15, 7341.228, -1082.531, 906.1441, 'FrostHound'),
(@FROSTHOUND, 16, 7355.963, -1127.951, 907.6265, 'FrostHound'),
(@FROSTHOUND, 17, 7359.741, -1156.937, 910.1899, 'FrostHound'),
(@FROSTHOUND, 18, 7352.817, -1172.477, 912.4103, 'FrostHound'),
(@FROSTHOUND, 19, 7340.049, -1188.073, 914.7924, 'FrostHound'),
(@FROSTHOUND, 20, 7330.804, -1201.472, 915.7355, 'FrostHound'),
(@FROSTHOUND, 21, 7323.127, -1228.335, 909.517, 'FrostHound'),
(@FROSTHOUND, 22, 7316.839, -1268.214, 902.8563, 'FrostHound'),
(@FROSTHOUND, 23, 7316.317, -1300.765, 904.0386, 'FrostHound'),
(@FROSTHOUND, 24, 7315.278, -1332.342, 904.7114, 'FrostHound'),
(@FROSTHOUND, 25, 7313.02, -1366.275, 907.1307, 'FrostHound'),
(@FROSTHOUND, 26, 7312.081, -1399.743, 910.574, 'FrostHound'),
(@FROSTHOUND, 27, 7312.83, -1434.1, 912.8854, 'FrostHound'),
(@FROSTHOUND, 28, 7311.606, -1466.343, 916.7098, 'FrostHound'),
(@FROSTHOUND, 29, 7309.714, -1498.702, 921.2195, 'FrostHound'),
(@FROSTHOUND, 30, 7306.879, -1531.19, 928.3399, 'FrostHound'),
(@FROSTHOUND, 31, 7305.797, -1558.925, 939.4229, 'FrostHound'),
(@FROSTHOUND, 32, 7305.19, -1566.019, 941.0005, 'FrostHound'),
(@FROSTHOUND, 33, 7305.19, -1566.019, 941.0005, 'FrostHound');
-- Add FrostHound abilites (Cast Net, Ice Slick)
UPDATE `creature_template` SET `spell1`=54997, `spell2`=54996, `speed_run`=1.2857 WHERE `entry`=@FROSTHOUND;
-- SmartAI FrostHound
UPDATE `creature_template` SET `AIName`='SmartAI' WHERE `entry` = @FROSTHOUND;
DELETE FROM `smart_scripts` WHERE `entryorguid` IN (@FROSTHOUND,@FROSTHOUND*100);
INSERT INTO `smart_scripts` (`entryorguid`, `source_type`, `id`, `link`, `event_type`, `event_phase_mask`, `event_chance`, `event_flags`, `event_param1`, `event_param2`, `event_param3`, `event_param4`, `action_type`, `action_param1`, `action_param2`, `action_param3`, `action_param4`, `action_param5`, `action_param6`, `target_type`, `target_param1`, `target_param2`, `target_param3`, `target_x`, `target_y`, `target_z`, `target_o`, `comment`) VALUES 
(@FROSTHOUND, 0, 0, 1, 27, 0, 100, 0, 0, 0, 0, 0, 8, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 'Frost Hound - On Boarded - set react state passive'),
(@FROSTHOUND, 0, 1, 0, 61, 0, 100, 0, 0, 0, 0, 0, 80, @FROSTHOUND*100, 2, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 'Frost Hound - On Boarded - Run script'),
(@FROSTHOUND*100, 9, 0, 0, 0, 0, 100, 0, 0, 0, 0, 0, 69, 0, 0, 0, 0, 0, 0, 8, 0, 0, 0, 7152.63, -747.3795, 890.9706, 0, 'Frost Hound - Script - Move to pos 1'),
(@FROSTHOUND*100, 9, 1, 0, 0, 0, 100, 0, 2000, 2000, 0, 0, 11, 54993, 2, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 'Frost Hound - Script - Cast FrostHound Periodic on self'),
(@FROSTHOUND*100, 9, 2, 0, 0, 0, 100, 0, 0, 0, 0, 0, 69, 0, 0, 0, 0, 0, 0, 8, 0, 0, 0, 7157.296, -750.2237, 891.4164, 0, 'Frost Hound - Script - Move to pos 2'),
(@FROSTHOUND*100, 9, 3, 0, 0, 0, 100, 0, 3000, 3000, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 'Frost Hound - Script - Emote Sniff'),
(@FROSTHOUND*100, 9, 4, 0, 0, 0, 100, 0, 2000, 2000, 0, 0, 11, 55026, 2, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 'Frost Hound - Script - Cast FrostHound Boss Emote'),
(@FROSTHOUND*100, 9, 5, 0, 0, 0, 100, 0, 0, 0, 0, 0, 1, 1, 0, 0, 0, 0, 0, 23, 0, 0, 0, 0, 0, 0, 0, 'Frost Hound - Script - Whisper player has been seen'),
(@FROSTHOUND*100, 9, 6, 0, 0, 0, 100, 0, 0, 0, 0, 0, 53, 1, @FROSTHOUND, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 'Frost Hound - Script - Start Waypoint'),
(@FROSTHOUND, 0, 2, 3, 40, 0, 100, 0, 32, 0, 0, 0, 1, 2, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 'Frost Hound - On Waypoint Reached - Emote Tracked'),
(@FROSTHOUND, 0, 3, 0, 61, 0, 100, 0, 0, 0, 0, 0, 33, @FROSTHOUND, 0, 0, 0, 0, 0, 23, 0, 0, 0, 0, 0, 0, 0, 'Frost Hound - On Waypoint Reached - Quest credit'),
(@FROSTHOUND, 0, 4, 5, 40, 0, 100, 0, 33, 0, 0, 0, 1, 3, 0, 0, 0, 0, 0, 23, 0, 0, 0, 0, 0, 0, 0, 'Frost Hound - On Waypoint End - Whisper Confront tracker'),
(@FROSTHOUND, 0, 5, 0, 61, 0, 100, 0, 0, 0, 0, 0, 41, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 'Frost Hound - On Waypoint End - Despawn');
-- Update Faction Stormforged Pursuer
UPDATE `creature_template` SET `faction_A`=1954, `faction_H`=1954, `unit_flags`=0, `speed_run`=1.64285 WHERE `entry` = @PURSUER;
-- Stormforged Pursuer creature equip template
DELETE FROM `creature_equip_template` WHERE `entry`=@PURSUER;
INSERT INTO `creature_equip_template` (`entry`, `itemEntry1`, `itemEntry2`, `itemEntry3`) VALUES
(@PURSUER, 35727, 0, 0);
-- remove Stormforged Pursuer EventAI
DELETE FROM `creature_ai_scripts` WHERE `creature_id`=@PURSUER;
-- SmartAI Stormforged Pursuer
UPDATE `creature_template` SET `AIName`='SmartAI' WHERE `entry` = @PURSUER;
DELETE FROM `smart_scripts` WHERE `entryorguid` = @PURSUER;
INSERT INTO `smart_scripts` (`entryorguid`, `source_type`, `id`, `link`, `event_type`, `event_phase_mask`, `event_chance`, `event_flags`, `event_param1`, `event_param2`, `event_param3`, `event_param4`, `action_type`, `action_param1`, `action_param2`, `action_param3`, `action_param4`, `action_param5`, `action_param6`, `target_type`, `target_param1`, `target_param2`, `target_param3`, `target_x`, `target_y`, `target_z`, `target_o`, `comment`) VALUES 
(@PURSUER, 0, 0, 0, 25, 0, 100, 0, 0, 0, 0, 0, 49, 0, 0, 0, 0, 0, 0, 5, 0, 0, 0, 0, 0, 0, 0, 'Stormforged Pursuer - summoned - attack start'),
(@PURSUER, 0, 1, 0, 9, 0, 100, 0, 5, 30, 7000, 11000, 11, 55007, 2, 0, 0, 0, 0, 5, 0, 0, 0, 0, 0, 0, 0, 'Stormforged Pursuer - Cast Throw Hammer');