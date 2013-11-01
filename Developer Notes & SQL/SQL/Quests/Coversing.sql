-- Update to quest to allow the quest to become completable)
UPDATE `quest_template` SET `SpecialFlags`=0, `RequiredNpcOrGo1`=26648, `RequiredNpcOrGoCount1`=1 WHERE `Id`=12032 LIMIT 1;

-- SAI for boss (Smart_scripts.id 5 is a hack fix but it was the only way I could get the quest to complete)
UPDATE `creature_template` set `AIName` = 'SmartAI' WHERE `entry` = 26648;
DELETE FROM `smart_scripts` WHERE `entryorguid` = 26648;
INSERT INTO `smart_scripts` (`entryorguid`, `source_type`, `id`, `link`, `event_type`, `event_phase_mask`, `event_chance`, `event_flags`, `event_param1`, `event_param2`, `event_param3`, `event_param4`, `action_type`, `action_param1`, `action_param2`, `action_param3`, `action_param4`, `action_param5`, `action_param6`, `target_type`, `target_param1`, `target_param2`, `target_param3`, `target_x`, `target_y`, `target_z`, `target_o`, `comment`) VALUES 
(26648, 0, 1, 0, 1, 0, 100, 1, 0, 0, 0, 0, 1, 1, 9000, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 'Creature yell text 1'),
(26648, 0, 2, 0, 52, 0, 100, 1, 1, 26648, 0, 0, 1, 2, 10000, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 'Creature yell text 2'),
(26648, 0, 3, 0, 52, 0, 100, 1, 2, 26648, 0, 0, 1, 3, 9000, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 'Creature yell text 3'),
(26648, 0, 4, 0, 52, 0, 100, 0, 3, 26648, 0, 0, 11, 47098, 2, 0, 0, 0, 0, 18, 500, 0, 0, 0, 0, 0, 0, 'Cast Spell ID 47098'),
(26648, 0, 5, 0, 52, 0, 100, 0, 3, 26648, 0, 0, 33, 26648, 0, 0, 0, 0, 0, 18, 500, 0, 0, 0, 0, 0, 0, 'Kill Credit for quest completion'),
(26648, 0, 6, 0, 52, 0, 100, 0, 3, 26648, 0, 0, 1, 4, 10000, 0, 0, 0, 0, 18, 500, 0, 0, 0, 0, 0, 0, 'Creature yell text 4'),
(26648, 0, 7, 0, 52, 0, 100, 1, 4, 26648, 0, 0, 1, 5, 10000, 0, 0, 0, 0, 18, 500, 0, 0, 0, 0, 0, 0, 'Creature whisper text 1'),
(26648, 0, 8, 0, 52, 0, 100, 1, 5, 26648, 0, 0, 1, 6, 9000, 0, 0, 0, 0, 18, 500, 0, 0, 0, 0, 0, 0, 'Creature whisper text 2'),
(26648, 0, 9, 0, 52, 0, 100, 1, 6, 26648, 0, 0, 1, 7, 0, 0, 0, 0, 0, 18, 500, 0, 0, 0, 0, 0, 0, 'Creature whisper text 3');

-- Creature texts
DELETE FROM `creature_text` WHERE `entry` = 26648;
INSERT INTO `creature_text` (`entry`, `groupid`, `id`, `text`, `type`, `language`, `probability`, `emote`, `duration`, `sound`, `comment`) VALUES 
(26648, 1, 1, 'Little $N, why do you call me forth? Are you working with the trolls of this land? Have you come to kill me and take my power as your own?', 14, 0, 100, 0, 0, 0, ''),
(26648, 2, 1, 'I sense uncertainty in you, and I do not trust it whether you are with them, or not. If you wish my augury for the Kalu\'ak, you will have to prove yourself first.', 14, 0, 100, 0, 0, 0, ''),
(26648, 3, 1, 'I will lay a mild compulsion upon you. Jump into the depths before me so that you put yourself into my element and thereby display your submission.', 14, 0, 100, 0, 0, 0, ''),
(26648, 4, 1, 'Well done, $N. Your display of respect is duly noted. Now, I have information for you that you must convey to the Kalu\'ak.', 14, 0, 100, 0, 0, 0, ''),
(26648, 5, 1, 'Simply put, you must tell the tuskarr that they cannot run. If they do so, their spirits will be destroyed by the evil rising within Northrend.', 15, 0, 100, 0, 0, 0, ''),
(26648, 6, 1, 'Tell the mystic that his people are to stand and fight alongside the Horde and Alliance against the forces of Malygos and the Lich King.', 15, 0, 100, 0, 0, 0, ''),
(26648, 7, 1, 'Now swim back with the knowledge I have granted you. Do what you can for them $R.', 15, 0, 100, 0, 0, 0, '');

-- Script to spawn the NPC (Temp spawn)
DELETE FROM `event_scripts` WHERE `id` = 12300;
DELETE FROM `npc_text` WHERE `id`=50300;
DELETE FROM `gossip_menu` WHERE `entry`=50300;
DELETE FROM `gossip_menu_option` WHERE `menu_id`=50300;
DELETE FROM `smart_scripts` WHERE `entryorguid`=188422;
INSERT INTO `npc_text` (`id`, `text0_0`) VALUES (50300, 'Conversing with the Depths');
INSERT INTO `gossip_menu` (`entry`, `text_id`) VALUES (50300, 50300);
INSERT INTO `gossip_menu_option` (`menu_id`, `option_text`, `option_id`, `npc_option_npcflag`) VALUES (50300, 'Commune with The Pearl of the Depths', 1, 1);
INSERT INTO `smart_scripts` (`entryorguid`, `source_type`, `event_type`, `event_flags`, `event_param1`, `action_type`, `action_param1`, `action_param2`, `action_param3`, `target_type`, `target_x`, `target_y`, `target_z`, `target_o`, `comment`) VALUES (188422, 1, 62, 1, 50300, 12, 26648, 3, 120000, 8, 2391.9, 1708.29, 50.7095, 0.221196, 'On Gossip select, spawn NPC');
UPDATE `gameobject_template` SET `AIname`='SmartGameObjectAI' WHERE `entry`=188422;
UPDATE `gameobject_template` SET `type`=2, `data2`=0, `data3`=50300 WHERE  `entry`=188422;

-- Allow Creature to remain afloat
UPDATE `creature_template` SET `InhabitType` = 4 WHERE `entry` = 26648;