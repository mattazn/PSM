SET @ENTRY := 25618;
SET @SOURCETYPE := 0;
DELETE FROM `smart_scripts` WHERE `entryorguid`=@ENTRY AND `source_type`=@SOURCETYPE;
UPDATE creature_template SET AIName="SmartAI" WHERE entry=@ENTRY LIMIT 1;
INSERT INTO `smart_scripts` (`entryorguid`,`source_type`,`id`,`link`,`event_type`,`event_phase_mask`,`event_chance`,`event_flags`,`event_param1`,`event_param2`,`event_param3`,`event_param4`,`action_type`,`action_param1`,`action_param2`,`action_param3`,`action_param4`,`action_param5`,`action_param6`,`target_type`,`target_param1`,`target_param2`,`target_param3`,`target_x`,`target_y`,`target_z`,`target_o`,`comment`) VALUES 
(@ENTRY,@SOURCETYPE,0,0,6,0,100,0,0,0,0,0,15,11705,0,0,0,0,0,16,0,0,0,0.0,0.0,0.0,0.0,"ON DEATH - GIVE QUEST CREDIT"),
(@ENTRY,@SOURCETYPE,1,0,0,0,100,0,3000,6000,6000,8000,11,32711,0,0,0,0,0,2,0,0,0,0.0,0.0,0.0,0.0,"ON COMBAT - DO CAST SHADOW NOVA(32711)"),
(@ENTRY,@SOURCETYPE,2,0,4,0,100,0,3000,6000,6000,8000,11,50329,2,0,0,0,0,1,0,0,0,0.0,0.0,0.0,0.0,"ON AGGRO - DO CAST SHIELD OF SUFFERING(50329)");