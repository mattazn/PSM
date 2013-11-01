-- Updates for 25 man loot
-- 	Sartharion
--	Tenebron
--	Shadron
--	Vesperon
--	Archavon
UPDATE creature_loot_template
SET item=40753
WHERE entry IN (31311, 31534, 31520, 31535, 31722) AND item=40752;

-- Update item loot for Large Satchel of Spoils
UPDATE item_loot_template
SET item=40753
WHERE entry=43346;