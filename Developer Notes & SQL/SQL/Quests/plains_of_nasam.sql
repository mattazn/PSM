-- QUEST The Plains of Nasam
UPDATE `quest_template` SET `EndText`='', `RequiredSpellCast2`=47962, `StartScript`=11652 WHERE `Id`=11652;
DELETE FROM `quest_start_scripts` WHERE `id` IN (11652, 13000); 
INSERT INTO `quest_start_scripts` (`id`, `delay`, `command`, `datalong`, `datalong2`, `dataint`, `x`, `y`, `z`, `o`) VALUES
(11652, 0, 10, 25334, 900000, 0, 2810.58, 6715.82, 9.77441, 3.57236);
UPDATE `quest_template` SET  WHERE `Id`=11652;
UPDATE `creature_template` SET `spell1`=47849, `spell2`=50677,`spell3`=50896, `spell4`=47962, `Health_mod`=5, `Mana_mod`=5 WHERE `entry`=25334;
DELETE FROM `npc_spellclick_spells` WHERE `npc_entry`=25334 AND `spell_id`=46598;
INSERT INTO `npc_spellclick_spells` (`npc_entry`,`spell_id`,`quest_start`,`cast_flags`) VALUES
(25334, 46598, 0, 1);
DELETE FROM `creature_ai_scripts` WHERE `creature_id` IN (27106, 27107, 27108, 27110);
UPDATE `creature_template` SET `AIName`='SmartAI' WHERE `entry` IN (27106, 27107, 27108, 27110);
DELETE FROM `smart_scripts` WHERE `entryorguid` IN (27106, 27107, 27108, 27110) AND `source_type`=0;
INSERT INTO `smart_scripts` (`entryorguid`, `source_type`, `id`, `link`, `event_type`, `event_phase_mask`, `event_chance`, `event_flags`, `event_param1`, `event_param2`, `event_param3`, `event_param4`, `action_type`, `action_param1`, `action_param2`, `action_param3`, `action_param4`, `action_param5`, `action_param6`, `target_type`, `target_param1`, `target_param2`, `target_param3`, `target_x`, `target_y`, `target_z`, `target_o`, `comment`) VALUES
(27106, 0, 0, 1, 8, 0, 100, 1, 47962, -1, 0, 0, 41, 1, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 'Injured Warsong Warrior - On Spell hit - Despawn after 1sec'),
(27107, 0, 0, 1, 8, 0, 100, 1, 47962, -1, 0, 0, 41, 1, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 'Injured Warsong Mage - On Spell hit - Despawn after 1sec'),
(27108, 0, 0, 1, 8, 0, 100, 1, 47962, -1, 0, 0, 41, 1, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 'Injured Warsong Shaman - On Spell hit - Despawn after 1sec'),
(27110, 0, 0, 1, 8, 0, 100, 1, 47962, -1, 0, 0, 41, 1, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 'Injured Warsong Engineer - On Spell hit - Despawn after 1sec');
-- NPC Scourge Plague Spreader - nastaveni faction a flagy
UPDATE `creature_template` SET `faction_A`=35, `faction_H`=35, `unit_flags`=`unit_flags`|2|33554432 WHERE `entry`=25349;
UPDATE `creature_template` SET `flags_extra`=`flags_extra`|64 WHERE `entry` IN (25333, 25469, 25332);
-- NPC Abandoned Fuel Tank 
UPDATE `creature_template` SET `faction_A`=1978, `faction_H`=1978, `unit_flags`=`unit_flags`|2|33554432, `AIName`='SmartAI' WHERE `entry`=27064;    
DELETE FROM `smart_scripts` WHERE `entryorguid`=27064 AND `source_type`=0;
INSERT INTO `smart_scripts` (`entryorguid`, `source_type`, `id`, `link`, `event_type`, `event_phase_mask`, `event_chance`, `event_flags`, `event_param1`, `event_param2`, `event_param3`, `event_param4`, `action_type`, `action_param1`, `action_param2`, `action_param3`, `action_param4`, `action_param5`, `action_param6`, `target_type`, `target_param1`, `target_param2`, `target_param3`, `target_x`, `target_y`, `target_z`, `target_o`, `comment`) VALUES
(27064, 0, 0, 1, 10, 0, 100, 0, 1, 2, 30000, 35000, 85, 47916, 2, 0, 0, 0, 0, 11, 25334, 2, 0, 0, 0, 0, 0, 'Abandoned Fuel Tank - On OOC LOS 2yd - Cast Fuel');