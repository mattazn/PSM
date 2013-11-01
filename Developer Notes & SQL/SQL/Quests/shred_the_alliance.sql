-- despawn
DELETE FROM `spell_scripts` WHERE `id` = 48610;
INSERT INTO `spell_scripts` (`id`, `effIndex`, `delay`, `command`, `datalong`, `datalong2`, `dataint`, `x`, `y`, `z`, `o`) VALUES
(48610, 2, 0, 18, 3, 27354, 2, 0, 0, 0, 0);

-- conditions
DELETE FROM `conditions` WHERE `SourceEntry` = 48610;
INSERT INTO `conditions` (`SourceTypeOrReferenceId`, `SourceGroup`, `SourceEntry`, `ElseGroup`, `ConditionTypeOrReference`, `ConditionValue1`, `ConditionValue2`, `ConditionValue3`, `ErrorTextId`, `ScriptName`, `Comment`) VALUES
(17, 0, 48610, 0, 29, 27423, 5, 0, 0, '', "spell hit - 5 yards from Grekk"),
(13, 0, 48610, 0, 18, 1, 27354, 0, 0, '', "self target"),
(17, 0, 48610, 1, 29, 27371, 5, 0, 0, '', "spell hit - 5 yards from Synipus"),
(13, 0, 48610, 1, 18, 1, 27354, 0, 0, '', "self target");