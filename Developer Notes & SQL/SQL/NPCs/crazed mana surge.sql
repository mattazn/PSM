-- Fix for Crazed Mana Surges in Nexus
UPDATE creature_template
SET unit_flags=537166400
WHERE entry IN (26737, 30519);