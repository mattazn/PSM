-- Slow down Erekeems adds
UPDATE creature_template
SET baseattacktime=3500
WHERE entry=29395 OR entry=31513;

-- Edit game object data for Crystals
UPDATE gameobject_template
SET flags=32,data0=1,data1=86,data4=32492,ScriptName="VioletHold_ActivationCrystals"
WHERE entry=193611;

-- NPC Data for Prison Seal Door
UPDATE creature_template
SET unit_flags=33555206,flags_extra=130
WHERE entry=30896;

-- Missing text for Lieutenant
INSERT INTO script_texts VALUES
(30658,-1608046,"I'm locking the door. Good luck, and thank you for doing this.","","","","","","","","",0,0,0,0,"sinclari SAY_SINCLARI_2");

-- Missing Alert text
INSERT INTO script_texts VALUES
(0,-1608056,"A Portal Guardian defends the new portal!","","","","","","","","",0,3,0,0,""),
(0,-1608057,"An elite Blue Dragonflight squad appears from the portal!","","","","","","","","",0,3,0,0,""),
(0,-1608058,"A Portal Keeper emerges from the portal!","","","","","","","","",0,3,0,0,"");

-- Missing text for trash
INSERT INTO script_texts VALUES
(0,-1608047,"Magic must be... contained...","","","","","","","","",0,0,0,0,"Violet Hold Trash Speech"),
(0,-1608048,"The Nexus War will not be stopped!","","","","","","","","",0,0,0,0,"Violet Hold Trash Speech"),
(0,-1608049,"You cannot stop us all!","","","","","","","","",0,0,0,0,"Violet Hold Trash Speech"),
(0,-1608050,"Dalaran must fall!","","","","","","","","",0,0,0,0,"Violet Hold Trash Speech"),
(0,-1608051,"Dalaran will burn!","","","","","","","","",0,0,0,0,"Violet Hold Trash Speech"),
(0,-1608052,"For the Spellweaver!","","","","","","","","",0,0,0,0,"Violet Hold Trash Speech"),
(0,-1608053,"The Kirin Tor must be stopped!","","","","","","","","",0,0,0,0,"Violet Hold Trash Speech"),
(0,-1608055,"Your efforts have been in vain!","","","","","","","","",0,0,0,0,"Violet Hold Trash Speech");

-- Missing emote text for Ichoron
INSERT INTO script_texts VALUES
(0,-1608059,"Ichoron's Protective Bubble shatters!","","","","","","","","",0,3,0,0,"");

-- NPC data for Defense System
UPDATE creature_template
SET scale=1,unit_flags=33554694,type_flags=1024,InhabitType=4,flags_extra=130,ScriptName="npc_defense_system"
WHERE entry=30837;

-- Fix for Xevozzs sphere
UPDATE creature_template
SET faction_A=14, faction_H=14
WHERE entry IN (29271, 31514);

-- Fix for portals
UPDATE creature_template
SET unit_flags=33555204
WHERE entry=31011;