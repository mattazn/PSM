UPDATE `creature_template` SET npcflag = 1, unit_flags = 0, gossip_menu_id = 22001, `AIName`= 'SmartAI' WHERE 
`entry`=27587;    -- cosmetic 

INSERT INTO `gossip_menu_option` (`menu_id`, `id`, `option_icon`, `option_text`, `option_id`, `npc_option_npcflag`, `action_menu_id`, `action_poi_id`, `box_coded`, `box_money`, `box_text`) VALUES 
(22001, 0, 0, '<Ride Alliance Steam Tank>', 1, 1, 0, 0, 0, 0, '');    -- random text and random ID

INSERT INTO `conditions` (`SourceTypeOrReferenceId`, `SourceGroup`, `SourceEntry`, `ElseGroup`, 
`ConditionTypeOrReference`, `ConditionValue1`, `ConditionValue2`, `ConditionValue3`, `ErrorTextId`, `ScriptName`, `Comment`) VALUES 
(15, 22001, 0, 0, 9, 12326, 0, 0, 0, '', NULL);    -- gossip only on quest

DELETE FROM `smart_scripts` WHERE `entryorguid`=27587;
INSERT INTO `smart_scripts` VALUES
(27587,0,3,1,62,0,100,0,22001,0,0,0,12,127587,1,300000,0,0,0,7,0,0,0,0.0,0.0,0.0,0.0,'On gossip - summ');   -- summon duplicate tank

INSERT INTO npc_spellclick_spells VALUES 
(127587,46598,12326,1,12326,1,0,0,0);  -- ride only with quest

UPDATE `creature_template` SET `AIName`= 'SmartAI' WHERE `entry`=27607;
DELETE FROM `smart_scripts` WHERE `entryorguid` = 27607;
INSERT INTO `smart_scripts` VALUES
(27607,0,1,0,8,0,100,0x1,49109,0,0,0,33,27625,0,0,0,0,0,18,10,0,0,0,0,0,0, 'wagon  - On Spell Hit - Credit'),
(27607,0,2,0,8,0,100,0x1,49109,0,0,0,47,0,0,0,0,0,0,1,0,0,0,0,0,0,0, 'wagon - On Spell Hit - Invisible'),
(27607,0,3,0,8,0,100,0x1,49109,0,0,0,41,1000,0,0,0,0,0,1,0,0,0,0,0,0,0, 'wagon - On Spell Hit - Despawn');  -- on drop gnome credit

INSERT INTO `conditions` (`SourceTypeOrReferenceId`, `SourceGroup`, SourceEntry`, `SourceId`, `ElseGroup`, 
`ConditionTypeOrReference`, `ConditionTarget`, `ConditionValue1`, `ConditionValue2`, `ConditionValue3`, 
`NegativeCondition`, `ErrorTextId`, `ScriptName`, `Comment`) VALUES
(13,0,49109,0,0,18,0,1,27607,0,0,0,'','wagon'); -- condition for tank hit wagon

INSERT INTO conditions (`SourceTypeOrReferenceId`, `SourceGroup`, SourceEntry`, `SourceId`, `ElseGroup`, 
`ConditionTypeOrReference`, `ConditionTarget`, `ConditionValue1`, `ConditionValue2`, `ConditionValue3`, 
`NegativeCondition`, `ErrorTextId`, `ScriptName`, `Comment`) VALUES 
(17,0,49081,0,0,23,0,4246,0,0,0,0,'','Drop Off Soldier in Wintergarde Mausoleum');   -- condition on drop NPC in zone of Wintergarde Mausoleum

INSERT INTO `creature_template` VALUES
(127587, 0, 0, 0, 0, 0, 24240, 25341, 0, 0, 'Alliance Steam Tank', '', 'vehichleCursor', 0, 70, 70, 2, 1892, 1892, 
16777216, 1, 2, 1, 0, 234, 331, 0, 286, 1, 2000, 0, 2, 0, 8, 0, 0, 0, 0, 0, 198, 295, 33, 9, 8, 0, 0, 0, 0, 0, 0, 
0, 0, 0, 49315, 49333, 49109, 49081, 0, 0, 0, 0, 0, 56, 0, 0, '', 0, 3, 6, 5, 1, 0, 0, 0, 0, 0, 0, 0, 180, 1, 0, 
0, 0, '', 12340); -- duplicate NPC