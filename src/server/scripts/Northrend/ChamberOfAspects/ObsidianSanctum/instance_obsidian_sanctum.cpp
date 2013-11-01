/*
 * Copyright (C) 2008-2013 TrinityCore <http://www.trinitycore.org/>
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
#include "obsidian_sanctum.h"

#define MAX_ENCOUNTER     4

/* Obsidian Sanctum encounters:
0 - Sartharion
*/

class instance_obsidian_sanctum : public InstanceMapScript
{
public:
    instance_obsidian_sanctum() : InstanceMapScript("instance_obsidian_sanctum", 615) { }

    InstanceScript* GetInstanceScript(InstanceMap* map) const
    {
        return new instance_obsidian_sanctum_InstanceMapScript(map);
    }

    struct instance_obsidian_sanctum_InstanceMapScript : public InstanceScript
    {
        instance_obsidian_sanctum_InstanceMapScript(Map* map) : InstanceScript(map) {}

        uint32 m_auiEncounter[MAX_ENCOUNTER];
        uint64 m_uiSartharionGUID;
        uint64 m_uiTenebronGUID;
        uint64 m_uiShadronGUID;
        uint64 m_uiVesperonGUID;

        bool m_bTenebronKilled;
        bool m_bShadronKilled;
        bool m_bVesperonKilled;

        void Initialize()
        {
            memset(&m_auiEncounter, 0, sizeof(m_auiEncounter));

            m_uiSartharionGUID = 0;
            m_uiTenebronGUID   = 0;
            m_uiShadronGUID    = 0;
            m_uiVesperonGUID   = 0;

            m_bTenebronKilled = false;
            m_bShadronKilled = false;
            m_bVesperonKilled = false;
        }

        bool IsEncounterInProgress() const
        {
            for (uint8 i = 0; i < MAX_ENCOUNTER; ++i)
                if (m_auiEncounter[i] == IN_PROGRESS)
                    return true;

            return false;
        }

        void OnCreatureCreate(Creature* creature)
        {
            switch (creature->GetEntry())
            {
                case NPC_SARTHARION:
                    m_uiSartharionGUID = creature->GetGUID();
                    break;
                //three dragons below set to active state once created.
                //we must expect bigger raid to encounter main boss, and then three dragons must be active due to grid differences
                case NPC_TENEBRON:
                    m_uiTenebronGUID = creature->GetGUID();
                    creature->setActive(true);
                    break;
                case NPC_SHADRON:
                    m_uiShadronGUID = creature->GetGUID();
                    creature->setActive(true);
                    break;
                case NPC_VESPERON:
                    m_uiVesperonGUID = creature->GetGUID();
                    creature->setActive(true);
                    break;
            }
        }

        void SetData(uint32 uiType, uint32 uiData)
        {
            switch(uiType)
            {
                case TYPE_SARTHARION_EVENT:
                    if(m_auiEncounter[0] != DONE)
                    m_auiEncounter[0] = uiData;
                    break;
                case TYPE_TENEBRON_PREKILLED:
                    m_auiEncounter[1] = DONE;
                    m_bTenebronKilled = true;
                    break;
                case TYPE_SHADRON_PREKILLED:
                    m_auiEncounter[2] = DONE;
                    m_bShadronKilled = true;
                    break;
                case TYPE_VESPERON_PREKILLED:
                    m_auiEncounter[3] = DONE;
                    m_bVesperonKilled = true;
                    break;
                default:
                    break;
            }

            if(uiData == DONE)
                SaveToDB();
        }

        uint32 GetData(uint32 uiType) const
        {
            switch(uiType)
            {
                case TYPE_SARTHARION_EVENT:
                    return m_auiEncounter[0];
                case TYPE_TENEBRON_PREKILLED:
                    return m_bTenebronKilled && m_auiEncounter[1] == DONE;
                case TYPE_SHADRON_PREKILLED:
                    return m_bShadronKilled && m_auiEncounter[2] == DONE;
                case TYPE_VESPERON_PREKILLED:
                    return m_bVesperonKilled && m_auiEncounter[3] == DONE;
                default:
                    break;
            }
            return 0;
        }

        uint64 GetData64(uint32 uiData) const
        {
            switch (uiData)
            {
                case DATA_SARTHARION:
                    return m_uiSartharionGUID;
                case DATA_TENEBRON:
                    return m_uiTenebronGUID;
                case DATA_SHADRON:
                    return m_uiShadronGUID;
                case DATA_VESPERON:
                    return m_uiVesperonGUID;
            }
            return 0;
        }

        std::string GetSaveData()
        {
            std::ostringstream saveStream;
            saveStream << "O S ";
            for(int i = 0; i < MAX_ENCOUNTER; ++i)
                saveStream << m_auiEncounter[i] << " ";

            return saveStream.str();
        }

        void Load(const char * data)
        {
            std::istringstream loadStream(data);
            char dataHead1, dataHead2;
            loadStream >> dataHead1 >> dataHead2;
            std::string newdata = loadStream.str();

            uint32 buff;
            if(dataHead1 == 'O' && dataHead2 == 'S')
            {
                for(int i = 0; i < MAX_ENCOUNTER; ++i)
                {
                    loadStream >> buff;
                    m_auiEncounter[i]= buff;
                }
            }

            m_bTenebronKilled = (m_auiEncounter[1] == DONE);
            m_bShadronKilled = (m_auiEncounter[2] == DONE);
            m_bVesperonKilled = (m_auiEncounter[3] == DONE);

            for(int i = 0; i < MAX_ENCOUNTER; ++i)
                if(m_auiEncounter[i] != DONE)
                    m_auiEncounter[i] = NOT_STARTED;
        }
    };

};

enum TrashSpells
{
	SPELL_HAMMERDROP = 57759,
	SPELL_PUMMEL = 58953,

	SPELL_DEVOTIONAURA   = 57740,
	H_SPELL_DEVOTIONAURA = 58944,
	SPELL_MORTALSTRIKE   = 13737,
	SPELL_DRACONICRAGE   = 57733,
	H_SPELL_DRACONICRAGE = 58942,
	SPELL_AVENGINGFURY   = 57742,

	SPELL_CURSEOFMENDING    = 39647,
	H_SPELL_CURSEOFMENDING  = 58948,
	SPELL_SHOCKWAVE         = 57728,
	H_SPELL_SHOCKWAVE       = 58947,
	SPELL_FRENZY            = 53801,

	SPELL_RAINOFFIRE		= 57757,
	H_SPELL_RAINOFFIRE		= 58936,
	SPELL_FLAMESHOCK		= 39529,
	H_SPELL_FLAMESHOCK		= 58940,
	SPELL_CONJUREFLAMEORB	= 57753
};

/*######
## Mob Onyx Flight Captain
######*/

class mob_onyx_flight_captain : public CreatureScript
{
public:
    mob_onyx_flight_captain() : CreatureScript("mob_onyx_flight_captain") { }

    CreatureAI* GetAI(Creature* creature) const
    {
        return new mob_onyx_flight_captainAI(creature);
    }

    struct mob_onyx_flight_captainAI : public ScriptedAI
    {
        mob_onyx_flight_captainAI(Creature* creature) : ScriptedAI(creature)
        {
            Reset();
        }

			uint32 HammerDrop_Timer;
			uint32 Pummel_Timer;

        void Reset()
        {
				HammerDrop_Timer = 5000;
				Pummel_Timer = 7000;
        }

        void UpdateAI(const uint32 uiDiff)
        {
            //Return since we have no target
            if (!UpdateVictim())
                return;

			if (HammerDrop_Timer <= uiDiff)
			{
				DoCast(me->getVictim(), SPELL_HAMMERDROP);
				HammerDrop_Timer = 5000;
			}
			else
				HammerDrop_Timer -= uiDiff;

			if (Pummel_Timer <= uiDiff)
			{
				DoCast(me->getVictim(), SPELL_PUMMEL);
				Pummel_Timer = 7000;
			}
			else
				Pummel_Timer -= uiDiff;

            DoMeleeAttackIfReady();
        }
    };
};

/*######
## Mob Onyx Brood General
######*/

class mob_onyx_brood_general : public CreatureScript
{
public:
    mob_onyx_brood_general() : CreatureScript("mob_onyx_brood_general") { }

    CreatureAI* GetAI(Creature* creature) const
    {
        return new mob_onyx_brood_generalAI(creature);
    }

    struct mob_onyx_brood_generalAI : public ScriptedAI
    {
        mob_onyx_brood_generalAI(Creature* creature) : ScriptedAI(creature)
        {
            Reset();
        }

		uint32 DevotionAura_Timer;
		uint32 MortalStrike_Timer;
		uint32 DraconicRage_Timer;
		uint32 AvengingFury_Timer;

        void Reset()
        {
			DevotionAura_Timer = 0;
			MortalStrike_Timer = urand(5000, 15000);
			DraconicRage_Timer = urand(15000, 30000);
			AvengingFury_Timer = urand(4000, 26000);
        }

        void UpdateAI(const uint32 uiDiff)
        {
            //Return since we have no target
            if (!UpdateVictim())
			{
				if (DevotionAura_Timer <= uiDiff)
				{
					DoCast(me, DUNGEON_MODE(SPELL_DEVOTIONAURA, H_SPELL_DEVOTIONAURA));
					DevotionAura_Timer = 600000;
				}
				else
					DevotionAura_Timer -= uiDiff;

			return;
			}

			if (MortalStrike_Timer <= uiDiff)
			{
				DoCast(me->getVictim(), SPELL_MORTALSTRIKE);
				MortalStrike_Timer = urand(5000, 15000);
			}
			else
				MortalStrike_Timer -= uiDiff;

			if (DraconicRage_Timer <= uiDiff)
			{
				DoCast(me, DUNGEON_MODE(SPELL_DRACONICRAGE, H_SPELL_DRACONICRAGE));
				DraconicRage_Timer = urand(15000, 30000);
			}
			else
				DraconicRage_Timer -= uiDiff;

			if (AvengingFury_Timer <= uiDiff)
			{
				DoCast(me, SPELL_AVENGINGFURY);
				AvengingFury_Timer = urand(15000, 26000);
			}
			else
				AvengingFury_Timer -= uiDiff;

            DoMeleeAttackIfReady();
        }
    };
};

/*######
## Mob Onyx Sanctum Guardian
######*/

class mob_onyx_sanctum_guardian : public CreatureScript
{
public:
    mob_onyx_sanctum_guardian() : CreatureScript("mob_onyx_sanctum_guardian") { }

    CreatureAI* GetAI(Creature* creature) const
    {
        return new mob_onyx_sanctum_guardianAI(creature);
    }

    struct mob_onyx_sanctum_guardianAI : public ScriptedAI
    {
        mob_onyx_sanctum_guardianAI(Creature* creature) : ScriptedAI(creature)
        {
            Reset();
        }

			uint32 CurseOfMending_Timer;
			uint32 Shockwave_Timer;
			bool doOnce;

        void Reset()
        {
			CurseOfMending_Timer = urand(4600, 16000);
			Shockwave_Timer = urand(5500, 12000);
			doOnce = false;
        }

        void UpdateAI(const uint32 uiDiff)
        {
            //Return since we have no target
            if (!UpdateVictim())
				return;

			if (CurseOfMending_Timer <=uiDiff)
			{

				DoCast(me->getVictim(), DUNGEON_MODE(SPELL_CURSEOFMENDING, H_SPELL_CURSEOFMENDING));
				CurseOfMending_Timer = urand(15000, 30000);
			}
			else
				CurseOfMending_Timer -= uiDiff;

			if (Shockwave_Timer <=uiDiff)
			{
				DoCast(me->getVictim(), DUNGEON_MODE(SPELL_SHOCKWAVE, H_SPELL_SHOCKWAVE));
				Shockwave_Timer = urand(5500, 12000);
			}
			else
				Shockwave_Timer -= uiDiff;

			if(me->GetHealthPct() <= 25 && doOnce)
			{
				DoCast(me, SPELL_FRENZY);
				doOnce = true;
			}

            DoMeleeAttackIfReady();
        }
    };
};

/*######
## Mob Onyx Blaze Mistress
######*/

class mob_onyx_blaze_mistress : public CreatureScript
{
public:
    mob_onyx_blaze_mistress() : CreatureScript("mob_onyx_blaze_mistress") { }

    CreatureAI* GetAI(Creature* creature) const
    {
        return new mob_onyx_blaze_mistressAI(creature);
    }

    struct mob_onyx_blaze_mistressAI : public ScriptedAI
    {
        mob_onyx_blaze_mistressAI(Creature* creature) : ScriptedAI(creature)
        {
            Reset();
        }

		uint32 RainOfFire_Timer;
		uint32 FlameShock_Timer;
		//uint32 ConjureFlameOrb_Timer;

        void Reset()
        {
			RainOfFire_Timer = urand(8000, 17000);
			FlameShock_Timer = urand(5000, 15000);
			//ConjureFlameOrb_Timer = 30000;
        }

        void UpdateAI(const uint32 uiDiff)
        {
            //Return since we have no target
            if (!UpdateVictim())
				return;

			if (RainOfFire_Timer <= uiDiff)
			{
				if (Unit *pTarget = SelectTarget(SELECT_TARGET_RANDOM, 0))
							DoCast(pTarget, DUNGEON_MODE(SPELL_RAINOFFIRE, H_SPELL_RAINOFFIRE));
				RainOfFire_Timer = urand(8000, 17000);
			}
			else
				RainOfFire_Timer -= uiDiff;

			if (FlameShock_Timer <= uiDiff)
			{
				if (Unit *pTarget = SelectTarget(SELECT_TARGET_RANDOM, 0))
							DoCast(pTarget, DUNGEON_MODE(SPELL_FLAMESHOCK, H_SPELL_FLAMESHOCK));
				FlameShock_Timer = urand(5000, 15000);
			}
			else
				FlameShock_Timer -= uiDiff;

			/*if (ConjureFlameOrb_Timer <= uiDiff)
			{
				if (Unit *pTarget = SelectTarget(SELECT_TARGET_RANDOM, 0))
							DoCast(pTarget, SPELL_CONJUREFLAMEORB);
				ConjureFlameOrb_Timer = 30000;
			}
			else
				ConjureFlameOrb_Timer -= uiDiff;*/

            DoMeleeAttackIfReady();
        }
    };
};

void AddSC_instance_obsidian_sanctum()
{
    new instance_obsidian_sanctum();
	new mob_onyx_blaze_mistress();
	new mob_onyx_sanctum_guardian();
	new mob_onyx_brood_general();
	new mob_onyx_flight_captain();
}