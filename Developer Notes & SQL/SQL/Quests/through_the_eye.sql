-- Through The Eye (http://www.wowhead.com/quest=13121)
-- http://www.youtube.com/watch?v=w-LEpPnIwj0
SET @VARDMADRA        :=30836;  -- Image of Vardmadra
SET @SUMMON_VARDMADRA :=57891;  -- Spell to summon Image of Vardmadra
SET @LK_EYE           :=192861; -- Eye of the Lich King gameobject
SET @GOSSIP_CAST      :=57888;  -- Force Cast on gossip select
SET @LK_EYE_CAST      :=57889;  -- Eye of the Lich King visual aura
SET @SHADOW_CULTIST   :=30835;  -- Image of Shadow Cultist
SET @SUMMON_CULTIST   :=57885;  -- Spell to summon Image of Shadow Cultist
SET @ELM_BUNNY        :=23837;  -- ELM General Purpose Bunny, used as spell target
SET @CREDIT_NPC       :=30750;  -- Credit NPC for quest, not spawned just used as a reference
/*
SET @CREDIT           :=57884; (Not working, broken spell most likely.)
*/
-- Sniffed texts
DELETE FROM `creature_text` WHERE `entry` IN (@VARDMADRA,@SHADOW_CULTIST);
INSERT INTO `creature_text` (`entry`, `groupid`, `id`, `text`, `type`, `language`, `probability`, `emote`, `duration`, `sound`, `comment`) VALUES 
(@SHADOW_CULTIST, 0, 0, 'Scrying upon many insignificant situations within Icecrown, you stumble upon something interesting...', 41, 0, 100, 0, 0, 0, 'Eye of Lich King Ambient'),
(@SHADOW_CULTIST, 1, 0, 'My lady.', 12, 0, 100, 1, 0, 0, 'Shadow Cultist'),
(@SHADOW_CULTIST, 2, 0, 'There is word from Jotunheim. The sleep-watchers there believe that they have found someone of significance.', 12, 0, 100, 0, 0, 0, 'Shadow Cultist'),
(@SHADOW_CULTIST, 3, 0, 'The cultist practically sputters.', 16, 0, 100, 0, 0, 0, 'Shadow Cultist'),
(@SHADOW_CULTIST, 4, 0, 'Look like, my lady? A vrykul, I suppose. They did not actually show him to me. Ever since The Shadow Vault....', 12, 0, 100, 0, 0, 0, 'Shadow Cultist'),
(@SHADOW_CULTIST, 5, 0, 'A name? Oh, yes, the name! I believe it was Iskalder.', 12, 0, 100, 0, 0, 0, 'Shadow Cultist'),
(@SHADOW_CULTIST, 6, 0, 'Right away, my lady.', 12, 0, 100, 0, 0, 0, 'Shadow Cultist'),
(@VARDMADRA, 0, 0, 'Report.', 12, 0, 100, 1, 0, 0, 'Vardmadra'),
(@VARDMADRA, 1, 0, 'Describe this vrykul. What does he look like?', 12, 0, 100, 0, 0, 0, 'Vardmadra'),
(@VARDMADRA, 2, 0, 'I am not interested in excuses. Perhaps they gave you a name?', 12, 0, 100, 0, 0, 0, 'Vardmadra'),
(@VARDMADRA, 3, 0, 'Iskalder?! You fool! Have you no idea who that is? He''s only the greatest vrykul warrior who ever lived!', 12, 0, 100, 1, 0, 0, 'Vardmadra'),
(@VARDMADRA, 4, 0, 'Return to Jotunheim and tell them to keep him asleep until I arrive. I will judge this vrykul with my own eyes.', 12, 0, 100, 0, 0, 0, 'Vardmadra');


UPDATE `gameobject_template` SET `AIName`='SmartGameObjectAI' WHERE `entry`=@LK_EYE;
DELETE FROM `smart_scripts` WHERE source_type IN (1,9) AND entryorguid IN (@LK_EYE,@LK_EYE*100);
INSERT INTO `smart_scripts` (`entryorguid`, `source_type`, `id`, `link`, `event_type`, `event_phase_mask`, `event_chance`, `event_flags`, `event_param1`, `event_param2`, `event_param3`, `event_param4`, `action_type`, `action_param1`, `action_param2`, `action_param3`, `action_param4`, `action_param5`, `action_param6`, `target_type`, `target_param1`, `target_param2`, `target_param3`, `target_x`, `target_y`, `target_z`, `target_o`, `comment`) VALUES 
(@LK_EYE, 1, 0, 1, 62, 0, 100, 0, 10005, 0, 0, 0, 72, 0, 0, 0, 0, 0, 0, 7, 0, 0, 0, 0, 0, 0, 0, 'On gossip select - Close gossip - Invoker'),
(@LK_EYE, 1, 1, 2, 61, 0, 100, 0, 0, 0, 0, 0, 85, @GOSSIP_CAST, 0, 0, 0, 0, 0, 7, 0, 0, 0, 0, 0, 0, 0, 'On gossip select - Cast spell - Invoker'),
(@LK_EYE, 1, 2, 0, 61, 0, 100, 0, 0, 0, 0, 0, 80, @LK_EYE*100, 2, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 'On gossip select - Run script - Self'),
(@LK_EYE*100, 9, 0, 0, 0, 0, 100, 0, 6000, 6000, 0, 0, 86, @SUMMON_VARDMADRA, 2, 9, @ELM_BUNNY, 15, 25, 19, @ELM_BUNNY, 10, 0, 0, 0, 0, 0, 'On script - Cross Cast spell - Creature in range'),
(@LK_EYE*100, 9, 1, 0, 0, 0, 100, 0, 1000, 1000, 0, 0, 86, @SUMMON_CULTIST, 2, 9, @ELM_BUNNY, 0, 10, 19, @ELM_BUNNY, 10, 0, 0, 0, 0, 0, 'On script - Cross Cast spell - Creature in range'),
(@LK_EYE*100, 9, 2, 0, 0, 0, 100, 0, 1000, 1000, 0, 0, 45, 1, 1, 0, 0, 0, 0, 19, @SHADOW_CULTIST, 0, 0, 0, 0, 0, 0, 'On script - Set Data - Creature'),
(@LK_EYE*100, 9, 3, 0, 0, 0, 100, 0, 3000, 3000, 0, 0, 45, 1, 2, 0, 0, 0, 0, 19, @SHADOW_CULTIST, 0, 0, 0, 0, 0, 0, 'On script - Set Data - Creature'),
(@LK_EYE*100, 9, 4, 0, 0, 0, 100, 0, 2000, 2000, 0, 0, 45, 1, 1, 0, 0, 0, 0, 19, @VARDMADRA, 0, 0, 0, 0, 0, 0, 'On script - Set Data - Creature'),
(@LK_EYE*100, 9, 5, 0, 0, 0, 100, 0, 7000, 7000, 0, 0, 45, 1, 3, 0, 0, 0, 0, 19, @SHADOW_CULTIST, 0, 0, 0, 0, 0, 0, 'On script - Set Data - Creature'),
(@LK_EYE*100, 9, 6, 0, 0, 0, 100, 0, 3000, 3000, 0, 0, 45, 1, 2, 0, 0, 0, 0, 19, @VARDMADRA, 0, 0, 0, 0, 0, 0, 'On script - Set Data - Creature'),
(@LK_EYE*100, 9, 7, 0, 0, 0, 100, 0, 4000, 4000, 0, 0, 45, 1, 4, 0, 0, 0, 0, 19, @SHADOW_CULTIST, 0, 0, 0, 0, 0, 0, 'On script - Set Data - Creature'),
(@LK_EYE*100, 9, 8, 0, 0, 0, 100, 0, 6000, 6000, 0, 0, 45, 1, 5, 0, 0, 0, 0, 19, @SHADOW_CULTIST, 0, 0, 0, 0, 0, 0, 'On script - Set Data - Creature'),
(@LK_EYE*100, 9, 9, 0, 0, 0, 100, 0, 4000, 4000, 0, 0, 45, 1, 3, 0, 0, 0, 0, 19, @VARDMADRA, 0, 0, 0, 0, 0, 0, 'On script - Set Data - Creature'),
(@LK_EYE*100, 9, 10, 0, 0, 0, 100, 0, 4000, 4000, 0, 0, 45, 1, 6, 0, 0, 0, 0, 19, @SHADOW_CULTIST, 0, 0, 0, 0, 0, 0, 'On script - Set Data - Creature'),
(@LK_EYE*100, 9, 11, 0, 0, 0, 100, 0, 8000, 8000, 0, 0, 45, 1, 4, 0, 0, 0, 0, 19, @VARDMADRA, 0, 0, 0, 0, 0, 0, 'On script - Set Data - Creature'),
(@LK_EYE*100, 9, 12, 0, 0, 0, 100, 0, 5000, 5000, 0, 0, 45, 1, 5, 0, 0, 0, 0, 19, @VARDMADRA, 0, 0, 0, 0, 0, 0, 'On script - Set Data - Creature'),
(@LK_EYE*100, 9, 13, 0, 0, 0, 100, 0, 4000, 4000, 0, 0, 45, 1, 7, 0, 0, 0, 0, 19, @SHADOW_CULTIST, 0, 0, 0, 0, 0, 0, 'On script - Set Data - Creature'),
(@LK_EYE*100, 9, 14, 0, 0, 0, 100, 0, 0, 0, 0, 0, 33, @CREDIT_NPC, 0, 0, 0, 0, 0, 7, 0, 0, 0, 0, 0, 0, 0, 'On script - KillCredit - Invoker'),
(@LK_EYE*100, 9, 15, 0, 0, 0, 100, 0, 10000, 10000, 0, 0, 28, @LK_EYE_CAST, 0, 0, 0, 0, 0, 7, 0, 0, 0, 0, 0, 0, 0, 'On script - Remove Aura - Invoker');
-- Sniffed creature info
UPDATE `creature_template` SET `AIName`='SmartAI',`minlevel`=80,`maxlevel`=80,`exp`=2,`faction_A`=1610,`faction_H`=1610,`unit_flags`=33544 WHERE `entry`=@VARDMADRA;
UPDATE `creature_template` SET `AIName`='SmartAI',`minlevel`=80,`maxlevel`=80,`exp`=2,`faction_A`=1610,`faction_H`=1610,`unit_flags`=33544 WHERE `entry`=@SHADOW_CULTIST;
DELETE FROM `smart_scripts` WHERE source_type=0 AND entryorguid IN (@VARDMADRA,@SHADOW_CULTIST);
INSERT INTO `smart_scripts` (`entryorguid`, `source_type`, `id`, `link`, `event_type`, `event_phase_mask`, `event_chance`, `event_flags`, `event_param1`, `event_param2`, `event_param3`, `event_param4`, `action_type`, `action_param1`, `action_param2`, `action_param3`, `action_param4`, `action_param5`, `action_param6`, `target_type`, `target_param1`, `target_param2`, `target_param3`, `target_x`, `target_y`, `target_z`, `target_o`, `comment`) VALUES 
(@SHADOW_CULTIST, 0, 0, 0, 54, 0, 100, 0, 0, 0, 0, 0, 69, 0, 0, 0, 0, 0, 0, 8, 0, 0, 0, 6829.18, 3814.89, 621.07, 0, 'On spawn - Move to pos - Pos'),
(@SHADOW_CULTIST, 0, 1, 0, 38, 0, 100, 0, 1, 1, 0, 0, 1, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 'On Data Set - Talk - Self'),
(@SHADOW_CULTIST, 0, 2, 0, 38, 0, 100, 0, 1, 2, 0, 0, 1, 1, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 'On Data Set - Talk - Self'),
(@SHADOW_CULTIST, 0, 3, 0, 38, 0, 100, 0, 1, 3, 0, 0, 1, 2, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 'On Data Set - Talk - Self'),
(@SHADOW_CULTIST, 0, 4, 0, 38, 0, 100, 0, 1, 4, 0, 0, 1, 3, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 'On Data Set - Talk - Self'),
(@SHADOW_CULTIST, 0, 5, 0, 38, 0, 100, 0, 1, 5, 0, 0, 1, 4, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 'On Data Set - Talk - Self'),
(@SHADOW_CULTIST, 0, 6, 0, 38, 0, 100, 0, 1, 6, 0, 0, 1, 5, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 'On Data Set - Talk - Self'),
(@SHADOW_CULTIST, 0, 7, 8, 38, 0, 100, 0, 1, 7, 0, 0, 1, 6, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 'On Data Set - Talk - Self'),
(@SHADOW_CULTIST, 0, 8, 9, 61, 0, 100, 0, 0, 0, 0, 0, 69, 5, 0, 0, 0, 0, 0, 8, 0, 0, 0, 6817.69, 3802.57, 621.07, 0, 'On Data Set - Move to pos - Pos'),
(@SHADOW_CULTIST, 0, 9, 0, 61, 0, 100, 0, 0, 0, 0, 0, 41, 5000, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 'On Data Set - Despawn - Self'),
(@VARDMADRA, 0, 0, 0, 38, 0, 100, 0, 1, 1, 0, 0, 1, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 'On Data Set - Talk - Self'),
(@VARDMADRA, 0, 1, 0, 38, 0, 100, 0, 1, 2, 0, 0, 1, 1, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 'On Data Set - Talk - Self'),
(@VARDMADRA, 0, 2, 0, 38, 0, 100, 0, 1, 3, 0, 0, 1, 2, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 'On Data Set - Talk - Self'),
(@VARDMADRA, 0, 3, 0, 38, 0, 100, 0, 1, 4, 0, 0, 1, 3, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 'On Data Set - Talk - Self'),
(@VARDMADRA, 0, 4, 5, 38, 0, 100, 0, 1, 5, 0, 0, 1, 4, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 'On Data Set - Talk - Self'),
(@VARDMADRA, 0, 5, 0, 61, 0, 100, 0, 0, 0, 0, 0, 41, 9000, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 'On Data Set - Despawn - Self');

DELETE FROM `conditions` WHERE (`SourceTypeOrReferenceId`=13 AND `SourceEntry` IN (@SUMMON_VARDMADRA,@SUMMON_CULTIST,@GOSSIP_CAST)) OR (`SourceTypeOrReferenceId`=15 AND `SourceGroup`=10005);
INSERT INTO `conditions` (`SourceTypeOrReferenceId`,`SourceGroup`,`SourceEntry`,`SourceId`,`ElseGroup`,`ConditionTypeOrReference`,`ConditionTarget`,`ConditionValue1`,`ConditionValue2`,`ConditionValue3`,`NegativeCondition`,`ErrorType`,`ErrorTextId`,`ScriptName`,`Comment`) VALUES
(13,1,@SUMMON_VARDMADRA,0,0,31,0,3,@ELM_BUNNY,0,0,0,0,'',''),
(13,1,@SUMMON_CULTIST,0,0,31,0,3,@ELM_BUNNY,0,0,0,0,'',''),
(15,10005,0,0,0,9,0,13121,0,0,0,0,0,'','Show gossip only when quest is taken'),
(15,10005,0,0,0,28,0,13121,0,0,1,0,0,'','Show gossip if quest is not completed'),
(15,10005,0,0,0,1,0,@LK_EYE_CAST,0,0,1,0,0,'','Hide gossip when aura is present');