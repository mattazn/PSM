-- Fix Rallying the Troops quest credit condition by nelegalno

UPDATE `quest_template` SET `RequiredSpellCast1` = 47394 WHERE `Id` = 12070;