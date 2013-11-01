/*
 * Copyright (C) 2008-2012 TrinityCore <http://www.trinitycore.org/>
 * Copyright (C) 2006-2009 ScriptDev2 <https://scriptdev2.svn.sourceforge.net/>
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms of the GNU General Public License as published by the
 * Free Software Foundation; either version 2 of the License, or (at your
 * option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for
 * more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with this program. If not, see <http://www.gnu.org/licenses/>.
 */

#include "ScriptMgr.h"
#include "InstanceScript.h"
#include "nexus.h"

#define NUMBER_OF_ENCOUNTERS      4

enum Factions
{
    FACTION_HOSTILE_FOR_ALL                       = 16
};

enum Spells
{
	// Crystalline Frayers
	SPELL_CRYSTAL_BLOOM			= 48058,
	SPELL_ENSNARE				= 48053,
	SPELL_SEED_POD				= 48082,
	SPELL_ROOTS                 = 28858,
	SPELL_AURA_OF_REGEN			= 52067,
	H_SPELL_AURA_OF_REGEN		= 57056,
	SUMMON_OBJECT_SEED_POD		= 52796,

	// Azure Magus
	SPELL_DETECT				= 18950,
	SPELL_ARCANE_SHOCK			= 37132,
	SPELL_FROSTBOLT				= 15530,
	H_SPELL_ARCANE_SHOCK		= 56776,
	H_SPELL_FROSTBOLT			= 56775,

	// Mage Hunter Ascendant
	// ARCANE
	SPELL_ARCANE_EXPLOSION		= 34933,
	SPELL_ARCANE_TRAP			= 47789,
	SPELL_AURA_ARCANE_HASTE		= 50182,
	SPELL_POLYMORPH				= 13323,
	H_SPELL_ARCANE_EXPLOSION	= 56825,
	H_SPELL_AURA_ARCANE_HASTE	= 56827,
	// FIRE
	SPELL_FIREBALL				= 12466,
	SPELL_IMMOLATION_TRAP		= 47784,
	SPELL_RAIN_OF_FIRE			= 36808,
	H_SPELL_FIREBALL			= 17290,
	H_SPELL_RAIN_OF_FIRE		= 39376,
	// FROST
	SPELL_CONE_OF_COLD			= 15244,
	SPELL_FREEZING_TRAP			= 55040,
	SPELL_ASCEND_FROSTBOLT		= 12737,
	H_SPELL_CONE_OF_COLD		= 38384,
	H_SPELL_ASCEND_FROSTBOLT	= 56837,
};

class instance_nexus : public InstanceMapScript
{
public:
    instance_nexus() : InstanceMapScript("instance_nexus", 576) { }

    InstanceScript* GetInstanceScript(InstanceMap* map) const
    {
        return new instance_nexus_InstanceMapScript(map);
    }

    struct instance_nexus_InstanceMapScript : public InstanceScript
    {
        instance_nexus_InstanceMapScript(Map* map) : InstanceScript(map) {}

        uint32 m_auiEncounter[NUMBER_OF_ENCOUNTERS];

        uint64 Anomalus;
        uint64 Keristrasza;

        uint64 AnomalusContainmentSphere;
        uint64 OrmoroksContainmentSphere;
        uint64 TelestrasContainmentSphere;

        std::string strInstData;

        void Initialize()
        {
            memset(&m_auiEncounter, 0, sizeof(m_auiEncounter));

            Anomalus = 0;
            Keristrasza = 0;
            AnomalusContainmentSphere = 0;
            OrmoroksContainmentSphere = 0;
            TelestrasContainmentSphere = 0;
        }

        void OnCreatureCreate(Creature* creature)
        {
            Map::PlayerList const &players = instance->GetPlayers();
            uint32 TeamInInstance = 0;

            if (!players.isEmpty())
            {
                if (Player* player = players.begin()->getSource())
                    TeamInInstance = player->GetTeam();
            }
            switch (creature->GetEntry())
            {
                case 26763:
                    Anomalus = creature->GetGUID();
                    break;
                case 26723:
                    Keristrasza = creature->GetGUID();
                    break;
                // Alliance npcs are spawned by default, if you are alliance, you will fight against horde npcs.
                case 26800:
                {
                    if (ServerAllowsTwoSideGroups())
                        creature->setFaction(FACTION_HOSTILE_FOR_ALL);
                    if (TeamInInstance == ALLIANCE)
                        creature->UpdateEntry(26799, HORDE);
                    break;
                }
                case 26802:
                {
                    if (ServerAllowsTwoSideGroups())
                        creature->setFaction(FACTION_HOSTILE_FOR_ALL);
                    if (TeamInInstance == ALLIANCE)
                        creature->UpdateEntry(26801, HORDE);
                    break;
                }
                case 26805:
                {
                    if (ServerAllowsTwoSideGroups())
                        creature->setFaction(FACTION_HOSTILE_FOR_ALL);
                    if (TeamInInstance == ALLIANCE)
                        creature->UpdateEntry(26803, HORDE);
                    break;
                }
                case 27949:
                {
                    if (ServerAllowsTwoSideGroups())
                        creature->setFaction(FACTION_HOSTILE_FOR_ALL);
                    if (TeamInInstance == ALLIANCE)
                        creature->UpdateEntry(27947, HORDE);
                    break;
                }
                case 26796:
                {
                    if (ServerAllowsTwoSideGroups())
                        creature->setFaction(FACTION_HOSTILE_FOR_ALL);
                    if (TeamInInstance == ALLIANCE)
                        creature->UpdateEntry(26798, HORDE);
                    break;
                }
            }
        }

        void OnGameObjectCreate(GameObject* go)
        {
            switch (go->GetEntry())
            {
                case 188527:
                {
                    AnomalusContainmentSphere = go->GetGUID();
                    if (m_auiEncounter[1] == DONE)
                        go->RemoveFlag(GAMEOBJECT_FLAGS, GO_FLAG_NOT_SELECTABLE);
                    break;
                }
                case 188528:
                {
                    OrmoroksContainmentSphere = go->GetGUID();
                    if (m_auiEncounter[2] == DONE)
                        go->RemoveFlag(GAMEOBJECT_FLAGS, GO_FLAG_NOT_SELECTABLE);
                    break;
                }
                case 188526:
                {
                    TelestrasContainmentSphere = go->GetGUID();
                    if (m_auiEncounter[0] == DONE)
                        go->RemoveFlag(GAMEOBJECT_FLAGS, GO_FLAG_NOT_SELECTABLE);
                    break;
                }
            }
        }

        uint32 GetData(uint32 identifier) const
        {
            switch (identifier)
            {
                case DATA_MAGUS_TELESTRA_EVENT: return m_auiEncounter[0];
                case DATA_ANOMALUS_EVENT:       return m_auiEncounter[1];
                case DATA_ORMOROK_EVENT:        return m_auiEncounter[2];
                case DATA_KERISTRASZA_EVENT:    return m_auiEncounter[3];
            }
            return 0;
        }

        void SetData(uint32 identifier, uint32 data)
        {
            switch (identifier)
            {
                case DATA_MAGUS_TELESTRA_EVENT:
                {
                    if (data == DONE)
                    {
                        GameObject* Sphere = instance->GetGameObject(TelestrasContainmentSphere);
                        if (Sphere)
                            Sphere->RemoveFlag(GAMEOBJECT_FLAGS, GO_FLAG_NOT_SELECTABLE);
                    }
                    m_auiEncounter[0] = data;
                    break;
                }
                case DATA_ANOMALUS_EVENT:
                {
                    if (data == DONE)
                    {
                        if (GameObject* Sphere = instance->GetGameObject(AnomalusContainmentSphere))
                            Sphere->RemoveFlag(GAMEOBJECT_FLAGS, GO_FLAG_NOT_SELECTABLE);
                    }
                    m_auiEncounter[1] = data;
                    break;
                }
                case DATA_ORMOROK_EVENT:
                {
                    if (data == DONE)
                    {
                        if (GameObject* Sphere = instance->GetGameObject(OrmoroksContainmentSphere))
                            Sphere->RemoveFlag(GAMEOBJECT_FLAGS, GO_FLAG_NOT_SELECTABLE);
                    }
                    m_auiEncounter[2] = data;
                    break;
                }
                case DATA_KERISTRASZA_EVENT:
                    m_auiEncounter[3] = data;
                    break;
            }

            if (data == DONE)
            {
                OUT_SAVE_INST_DATA;

                std::ostringstream saveStream;
                saveStream << m_auiEncounter[0] << ' ' << m_auiEncounter[1] << ' ' << m_auiEncounter[2] << ' '
                    << m_auiEncounter[3];

                strInstData = saveStream.str();

                SaveToDB();
                OUT_SAVE_INST_DATA_COMPLETE;
            }
        }

        uint64 GetData64(uint32 uiIdentifier) const
        {
            switch (uiIdentifier)
            {
                case DATA_ANOMALUS:                 return Anomalus;
                case DATA_KERISTRASZA:              return Keristrasza;
                case ANOMALUS_CONTAINMET_SPHERE:    return AnomalusContainmentSphere;
                case ORMOROKS_CONTAINMET_SPHERE:    return OrmoroksContainmentSphere;
                case TELESTRAS_CONTAINMET_SPHERE:   return TelestrasContainmentSphere;
            }
            return 0;
        }

        std::string GetSaveData()
        {
            return strInstData;
        }

        void Load(const char *chrIn)
        {
            if (!chrIn)
            {
                OUT_LOAD_INST_DATA_FAIL;
                return;
            }

            OUT_LOAD_INST_DATA(chrIn);

            std::istringstream loadStream(chrIn);
            loadStream >> m_auiEncounter[0] >> m_auiEncounter[1] >> m_auiEncounter[2] >> m_auiEncounter[3];

            for (uint8 i = 0; i < NUMBER_OF_ENCOUNTERS; ++i)
                if (m_auiEncounter[i] == IN_PROGRESS)
                    m_auiEncounter[i] = NOT_STARTED;

            OUT_LOAD_INST_DATA_COMPLETE;
        }
    };

};


class mob_crystalline_frayer : public CreatureScript
{
    public:
        mob_crystalline_frayer() : CreatureScript("mob_crystalline_frayer") { }

        struct mob_crystalline_frayerAI : public ScriptedAI
        {
            mob_crystalline_frayerAI(Creature* creature) : ScriptedAI(creature)
            {
                instance = me->GetInstanceScript();
            }

            InstanceScript* instance;

    uint32 CrystalBloom_Timer;
	uint32 Ensnare_Timer;
	uint32 SeedPod_Timer;
	
	uint32 Res_Timer;
	bool isDead;

    void Reset()
    {
		me->SetReactState(REACT_AGGRESSIVE);
		me->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);
        me->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);

        Ensnare_Timer = RAND(3000, 4000);
		isDead = false;
    }

	void EnterCombat(Unit* who)
    {
        DoCast(me, SPELL_CRYSTAL_BLOOM);
    }

    void UpdateAI(const uint32 diff)
    {
		if(instance->GetData(DATA_ORMOROK_EVENT) == DONE && !isDead)
		{
		me->DisappearAndDie();
		isDead = true;
		}

		if(Res_Timer <= diff)
			{
					me->setFaction(14);
					me->RemoveAllAuras();
					me->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);
					me->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
			} else Res_Timer -=diff;

		if (!UpdateVictim())
        {
        }

		if ((me->GetHealth() <= (me->GetMaxHealth() * 0.15)) && !isDead)
        {
			//This is a hack but until I find a better way
			me->SetHealth(me->GetMaxHealth());
			DoCast(me, SUMMON_OBJECT_SEED_POD);
			DoCast(me, SPELL_SEED_POD);
			me->SetReactState(REACT_PASSIVE);
			me->CombatStop(false);
			//me->CastStop();
			//me->AttackStop();
			me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);
            me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
			Res_Timer = 100000;
			DoCast(me, IsHeroic() ? H_SPELL_AURA_OF_REGEN : SPELL_AURA_OF_REGEN);
        }

        if (Ensnare_Timer <= diff)
        {
            DoCast(me->getVictim(), SPELL_ROOTS);

            Ensnare_Timer = RAND(9500, 15000);
        } else Ensnare_Timer -=diff;

		DoMeleeAttackIfReady();
	}
		};

        CreatureAI* GetAI(Creature* creature) const
        {
            return new mob_crystalline_frayerAI(creature);
        }
};
class mob_azure_magus : public CreatureScript
{
    public:
        mob_azure_magus() : CreatureScript("mob_azure_magus") { }

        struct mob_azure_magusAI : public ScriptedAI
        {
            mob_azure_magusAI(Creature* creature) : ScriptedAI(creature)
            {
                instance = me->GetInstanceScript();
            }

            InstanceScript* instance;

    uint32 Frostbolt_Timer;
	uint32 ArcaneShock_Timer;

    void Reset()
    {
		DoCast(me, SPELL_DETECT);
		Frostbolt_Timer = 500+rand()%500;
		ArcaneShock_Timer = 9000+rand()%2000;
    }

    void UpdateAI(const uint32 diff)
    {
		if(!UpdateVictim())
			return;

		if(Frostbolt_Timer <= diff)
		{
			DoCast(me->getVictim(), IsHeroic() ? H_SPELL_FROSTBOLT : SPELL_FROSTBOLT);
			Frostbolt_Timer = 3400+rand()%1400;
		} else Frostbolt_Timer -= diff;

		if(ArcaneShock_Timer <= diff)
		{
			if(Unit* target = SelectTarget(SELECT_TARGET_RANDOM, 0, 75, true))
			DoCast(target, IsHeroic() ? H_SPELL_ARCANE_SHOCK : SPELL_ARCANE_SHOCK);

			ArcaneShock_Timer = 9000+rand()%6000;
		} else ArcaneShock_Timer -= diff;

		DoMeleeAttackIfReady();
	}
		};

        CreatureAI* GetAI(Creature* creature) const
        {
            return new mob_azure_magusAI(creature);
        }
};
class mob_mage_hunter_ascendant : public CreatureScript
{
    public:
        mob_mage_hunter_ascendant() : CreatureScript("mob_mage_hunter_ascendant") { }

        struct mob_mage_hunter_ascendantAI : public ScriptedAI
        {
            mob_mage_hunter_ascendantAI(Creature* creature) : ScriptedAI(creature)
            {
                instance = me->GetInstanceScript();
            }

            InstanceScript* instance;

	uint32 AoE_Timer;
	uint32 Polymorph_Timer;
	uint32 Trap_Timer;
	uint32 Aura_Timer;
	uint32 Missile_Timer;
	uint8 Spec;

    void Reset()
    {
		Spec = urand(1,3);
		AoE_Timer = 4000+rand()%6000;
		Polymorph_Timer = 9000+rand()%5000;
		Trap_Timer = 4000+rand()%5000;
		Aura_Timer = 1000+rand()%200;
		Missile_Timer = 500+rand()%500;
    }

    void UpdateAI(const uint32 diff)
    {
		if(Spec == 1)
		{
		if(Aura_Timer <= diff)
		{
			DoCast(me, IsHeroic() ? H_SPELL_AURA_ARCANE_HASTE : SPELL_AURA_ARCANE_HASTE);
			Aura_Timer = 60000;
		} else Aura_Timer -= diff;
		}

        if (!UpdateVictim())
           return;

		if(Spec == 1)
		{
		if(Polymorph_Timer <= diff)
		{
			if(Unit* target = SelectTarget(SELECT_TARGET_RANDOM, 0, 75, true))
			DoCast(target, SPELL_POLYMORPH);
			Polymorph_Timer = 19000+rand()%3000;
		} else Polymorph_Timer -= diff;
		}

		if(Missile_Timer <= diff)
		{
			switch(Spec)
			{
			case 2:
				DoCast(me->getVictim(), IsHeroic() ? H_SPELL_ASCEND_FROSTBOLT : SPELL_ASCEND_FROSTBOLT);
			Missile_Timer = 2400+rand()%1400;
			break;
			case 3:
				DoCast(me->getVictim(), IsHeroic() ? H_SPELL_FIREBALL : SPELL_FIREBALL);
			Missile_Timer = 2400+rand()%1400;
			break;
			}
		} else Missile_Timer -= diff;

		if(AoE_Timer <= diff)
		{
			switch(Spec)
			{
			case 1:
			if(Unit* target = SelectTarget(SELECT_TARGET_RANDOM, 0, 10, true))
			DoCast(me->getVictim(), IsHeroic() ? H_SPELL_ARCANE_EXPLOSION : SPELL_ARCANE_EXPLOSION);
			AoE_Timer = 8000+rand()%3000;
			break;
			case 2:
				DoCast(me->getVictim(), IsHeroic() ? H_SPELL_CONE_OF_COLD : SPELL_CONE_OF_COLD);
			AoE_Timer = 11000+rand()%4000;
			break;
			case 3:
			if(Unit* target = SelectTarget(SELECT_TARGET_RANDOM, 0, 75, true))
			DoCast(target, IsHeroic() ? H_SPELL_RAIN_OF_FIRE : SPELL_RAIN_OF_FIRE);
			AoE_Timer = 15000+rand()%13000;
			break;
			}
		} else AoE_Timer -= diff;

		if(Trap_Timer <= diff)
		{
			switch(Spec)
			{
			case 1:
			DoCast(me, SPELL_ARCANE_TRAP);
			Trap_Timer = 11000+rand()%7000;
			break;
			case 2:
			DoCast(me, SPELL_FREEZING_TRAP);
			Trap_Timer = 11000+rand()%8000;
			break;
			case 3:
			DoCast(me, SPELL_IMMOLATION_TRAP);
			Trap_Timer = 11000+rand()%7500;
			break;
			}
		} else Trap_Timer -= diff;

		DoMeleeAttackIfReady();
	}
		};

        CreatureAI* GetAI(Creature* creature) const
        {
            return new mob_mage_hunter_ascendantAI(creature);
        }
};
void AddSC_instance_nexus()
{
    new instance_nexus();
	new mob_crystalline_frayer();
	new mob_azure_magus();
	new mob_mage_hunter_ascendant();
}
