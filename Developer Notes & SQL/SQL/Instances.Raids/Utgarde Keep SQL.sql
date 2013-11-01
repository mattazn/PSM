//Link DragonFlayer Runecaster/Strategist/Ironhelm/Runecaster in Njorn Stair
INSERT INTO creature_formations
VALUES
(126037,126037,0,0,2),
(126037,126045,0,0,2),
(126037,126029,0,0,2),
(126037,126036,0,0,2);

//Fix bad creature formations
UPDATE creature_formations
SET dist=200
WHERE leaderGUID=126037;

//Insert dragonflayer emote text PART 1
INSERT INTO script_texts
(npc_entry,entry,content_default,type,comment)
VALUES
(0,-1999926,"Die, maggot!",0,"Utgarde Keep - Dragonflayer combat entry text"),
(0,-1999927,"Haraak foln!",0,"Utgarde Keep - Dragonflayer combat entry text"),
(0,-1999928,"I spit on you!",0,"Utgarde Keep - Dragonflayer combat entry text"),
(0,-1999929,"I will feed you to the dogs!",0,"Utgarde Keep - Dragonflayer combat entry text"),
(0,-1999930,"I will take pleasure in gutting you!",0,"Utgarde Keep - Dragonflayer combat entry text"),
(0,-1999931,"I'll eat your heart!",0,"Utgarde Keep - Dragonflayer combat entry text"),
(0,-1999932,"Sniveling pig!",0,"Utgarde Keep - Dragonflayer combat entry text"),
(0,-1999933,"Ugglin oo bjorr!",0,"Utgarde Keep - Dragonflayer combat entry text"),
(0,-1999934,"You come to die!",0,"Utgarde Keep - Dragonflayer combat entry text"),
(0,-1999935,"Your entrails will make a fine necklace.",0,"Utgarde Keep - Dragonflayer combat entry text");


//Insert Dragonflayer emote text PART 2
INSERT INTO creature_ai_scripts
(id,creature_id,event_type,event_chance,event_flags,action1_type,action1_param1,action1_param2,action1_param3,comment)
VALUES
(2396105,23961,4,100,7,1,-10025,-10026,-10027,"Dragonflayer Ironhelm - Speech on Aggro"),
(2407804,24078,4,100,7,1,-10025,-10029,-10031,"Dragonflayer Metalworker - Speech on Aggro"),
(2408004,24080,4,100,7,1,-10026,-10032,-10033,"Dragonflayer Weaponsmith - Speech on Aggro"),
(2395613,23956,4,100,7,1,-10027,-10030,-10028,"Dragonflayer Strategist - Speech on Aggro"),
(2408507,24085,4,100,7,1,-10030,-10034,-10029,"Dragonflayer Overseer - Speech on Aggro"),
(2406904,24069,4,100,7,1,-10031,-10032,-10033,"Dragonflayer Bonecrusher - Speech on Aggro"),
(2407107,24071,4,100,7,1,-10031,-10025,-10026,"Dragonflayer Heartsplitter - Speech on Aggro");

//Dragonflayer forge master immune to MC fix
UPDATE creature_template
SET mechanic_immune_mask=1
WHERE entry=24079;

//Ticking Time Bomb
INSERT INTO `spell_script_names` (`spell_id`, `ScriptName`) VALUES
(54962, 'spell_ticking_time_bomb'),
(60227, 'spell_ticking_time_bomb');
(59686, 'spell_ticking_time_bomb');

INSERT INTO waypoint_data VALUES
(1260250,1,188.018,204.563,40.8151,0,2500,0,0,100,0),
(1260250,2,186.985,214.194,40.8151,0,5000,0,0,100,0),
(1260250,3,187.486,204.291,40.8151,0,2500,0,0,100,0),
(1260250,4,194.519,196.953,40.8151,0,6000,0,0,100,0);

INSERT INTO waypoint_data VALUES
(789620,1,106.896,89.8886,65.5589,0,0,0,0,100,0),
(789620,2,101.818,104.925,65.4477,0,0,0,0,100,0),
(789620,3,91.6869,135.585,65.5096,0,0,0,0,100,0),
(789620,4,106.817,89.9248,65.5564,0,0,0,0,100,0),
(789620,5,117.012,80.603,65.7046,0,0,0,0,100,0),
(789620,6,130.087,68.4046,65.7047,0,0,0,0,100,0),
(789620,7,135.521,63.3543,65.6989,0,0,0,0,100,0),
(789620,8,148.086,66.2922,65.6806,0,0,0,0,100,0),
(789620,9,154.459,68.5741,65.6785,0,0,0,0,100,0),
(789620,10,138.874,63.3131,65.6944,0,0,0,0,100,0),
(789620,11,126.973,69.2696,65.7019,0,0,0,0,100,0);

INSERT INTO waypoint_data VALUES
(967120,1,89.1579,98.7204,87.3487,0,0,0,0,100,0),
(967120,2,85.7068,109.625,87.2954,0,0,0,0,100,0),
(967120,3,109.563,117.831,87.314,0,0,0,0,100,0),
(967120,4,112.705,110.035,87.4431,0,0,0,0,100,0),
(967120,5,116.253,101.911,92.0538,0,0,0,0,100,0),
(967120,6,119.045,93.8241,97.8189,0,0,0,0,100,0),
(967120,7,122.249,84.7635,104.482,0,0,0,0,100,0),
(967120,8,124.081,78.7505,108.668,0,0,0,0,100,0),
(967120,9,125.765,73.4764,108.789,0,0,0,0,100,0),
(967120,10,124.759,78.1706,108.626,0,0,0,0,100,0),
(967120,11,122.542,85.4192,104.115,0,0,0,0,100,0),
(967120,12,120.358,92.3933,99.0213,0,0,0,0,100,0),
(967120,13,117.784,100.922,93.0106,0,0,0,0,100,0),
(967120,14,114.895,109.062,87.4822,0,0,0,0,100,0),
(967120,15,111.656,117.446,87.3044,0,0,0,0,100,0),
(967120,16,86.269,109.652,87.2926,0,0,0,0,100,0),
(967120,17,89.562,98.9193,87.3473,0,0,0,0,100,0),
(967120,18,91.3184,93.6167,84.3561,0,0,0,0,100,0),
(967120,19,93.6907,86.2691,78.9657,0,0,0,0,100,0),
(967120,20,96.3795,78.0235,73.1297,0,0,0,0,100,0),
(967120,21,99.7444,67.0918,65.718,0,0,0,0,100,0),
(967120,22,100.861,63.5245,65.7059,0,0,0,0,100,0),
(967120,23,98.5892,70.7422,67.9729,0,0,0,0,100,0),
(967120,24,96.3183,77.6292,72.8882,0,0,0,0,100,0),
(967120,25,93.6949,85.358,78.4166,0,0,0,0,100,0),
(967120,26,90.6963,94.2678,84.9004,0,0,0,0,100,0),
(967120,27,89.4972,97.8968,87.1608,0,0,0,0,100,0);
