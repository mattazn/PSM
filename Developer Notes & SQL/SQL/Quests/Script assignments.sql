-- Assign missing scripts in DB
UPDATE creature_template
SET ScriptName='npc_leviroth'
WHERE entry=26452;

UPDATE creature_template
SET ScriptName='npc_tur_ragepaw'
WHERE entry=27328;

UPDATE creature_template
SET ScriptName='npc_the_big_gun'
WHERE entry=24992;

UPDATE creature_template
SET ScriptName='npc_attracted_reef_bull'
WHERE entry=28404;

DELETE FROM spell_script_names WHERE ScriptName='spell_anuniaqs_net';
INSERT INTO spell_script_names VALUES
(21014, 'spell_anuniaqs_net');

UPDATE gameobject_template
SET ScriptName='go_hebjins_drums'
WHERE entry=190695;

DELETE FROM spell_script_names WHERE ScriptName='spell_stormcrow_amulet';
INSERT INTO spell_script_names VALUES
(31606, 'spell_stormcrow_amulet');

UPDATE creature_template
SET ScriptName='mob_steam_rager'
WHERE entry=24601;

UPDATE `gameobject_template` SET `scriptname`='go_tadpole_cage' WHERE `entry`=187373;

DELETE FROM spell_script_names WHERE ScriptName='spell_q12810_gore_bladder';
INSERT INTO spell_script_names VALUES
(6509, 'spell_q12810_gore_bladder');

UPDATE creature_template
SET  ScriptName='npc_eye_of_acherus'
WHERE entry=28511;

-- Quests 12470 & 13343
UPDATE creature_template SET ScriptName='npc_hourglass' WHERE entry IN (27840,32327);

UPDATE gameobject_template SET scriptName =  'go_still_tools' WHERE entry IN (190639, 190638, 190637, 190636, 190635);

UPDATE creature_template SET AIName='', ScriptName='npc_fallen_caravan_guard' WHERE entry IN (25342,25343);

UPDATE item_template SET ScriptName='item_snq_control_unit' WHERE entry=34981;

UPDATE creature_template SET AIName='', ScriptName='npc_q11796_trigger' WHERE entry IN (25845,25846,25847);

DELETE FROM spell_script_names WHERE ScriptName='spell_q12915_hurl_boulder';
INSERT INTO spell_script_names VALUES
(55818, 'spell_q12915_hurl_boulder');

DELETE FROM spell_script_names WHERE ScriptName='spell_zuldrak_rat';
INSERT INTO spell_script_names VALUES
(50894, 'spell_zuldrak_rat');

UPDATE gameobject_template
SET ScriptName='go_focusing_iris'
WHERE entry IN (193958, 193960);

UPDATE creature_template
SET ScriptName='npc_hover_disc'
WHERE entry IN (30248, 30234);

UPDATE creature_template
SET ScriptName='npc_nexus_lord'
WHERE entry = 30245;

UPDATE creature_template
SET ScriptName='npc_scion_of_eternity'
WHERE entry = 30249;

UPDATE creature_template
SET ScriptName='npc_alexstrasza'
WHERE entry=32295;

UPDATE creature_template
SET ScriptName='npc_power_spark'
WHERE entry=30084;

UPDATE creature_template
SET ScriptName='npc_vortex_vehicle'
WHERE entry=30090;

