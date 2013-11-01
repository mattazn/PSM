    SET @EVENT_ID           := 99999; -- don't forget change it!
    -- quest=12998
    UPDATE `gameobject_template` SET `data2` = @EVENT_ID  WHERE entry = 192181;
     
     
    DELETE FROM event_scripts WHERE id = @EVENT_ID;
    INSERT INTO event_scripts VALUES
    (@EVENT_ID, 0, 15, 60236, 1, 0, 0, 0, 0, 0), -- Cyclone(need correct spell)
    (@EVENT_ID, 0, 10, 30299, 15000, 0, 7311.81, -714.096, 791.609, 4.71386),
    (@EVENT_ID, 5, 8, 30299, 0, 0, 0, 0, 0, 0);
     
    -- AI: npc=30299
    UPDATE creature_template SET  AIName = 'SmartAI' WHERE entry = 30299;
    DELETE FROM `creature_ai_scripts` WHERE (`creature_id`=30299);
    DELETE FROM `smart_scripts` WHERE (`entryorguid`=30299);
    INSERT INTO `smart_scripts` VALUES
    (30299, 0, 0, 0, 1, 0, 100, 1, 1000, 1000, 0, 0, 1, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0,'Overseer Narvir - say text 1'),
    (30299, 0, 1, 0, 1, 0, 100, 1, 1000, 1000, 0, 0, 46, 10, 0, 0, 0, 0, 0, 2, 0, 0, 0, 0, 0, 0, 0,'go'),
    (30299, 0, 2, 0, 1, 0, 100, 1, 8000, 8000, 0, 0, 1, 1, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0,'Overseer Narvir - say text 2');
     
    DELETE FROM creature_text WHERE entry = 30299;
    INSERT INTO creature_text VALUES
    (30299, 0, 0, 'You didn\'t think that I was going to let you walk in here and take the Heart of the Storm, did you?', 12, 0, 100, 0,0, 0, 'Overseer Narvir say text 1'),
    (30299, 1, 0, 'You may have killed Valduran, but that will not stop me from completing the colossus.', 12, 0, 100, 0, 0, 0, 'Overseer Narvir say text 2');
