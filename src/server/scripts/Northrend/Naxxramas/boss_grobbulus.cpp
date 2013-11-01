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
#include "ScriptedCreature.h"
#include "naxxramas.h"

#define EVENT_BERSERK     1
#define EVENT_CLOUD       2
#define EVENT_INJECT      3
#define EVENT_SPRAY       4
#define EVENT_SLIMESTREAM 5

enum
{
    EMOTE_SPRAY_SLIME = -1533021,
    EMOTE_INJECTION = -1533158,

	SPELL_BOMBARD_SLIME = 28280,
    SPELL_SLIME_STREAM = 28137,
    SPELL_MUTATING_INJECTION = 28169,
    SPELL_POISON_CLOUD = 28240,
    SPELL_SLIME_SPRAY = 28157,
    SPELL_SLIME_SPRAY_H = 54364,
    SPELL_BERSERK = 26662,

	SPELL_POISON_CLOUD_ADD_N = 28158,
	SPELL_POISON_CLOUD_ADD_H = 54362,

	SPELL_DISEASE_CLOUD   = 70394,
    NPC_FALLOUT_SLIME     = 16290
};


class boss_grobbulus : public CreatureScript
{
public:
    boss_grobbulus() : CreatureScript("boss_grobbulus") { }

    CreatureAI* GetAI(Creature* creature) const
    {
        return new boss_grobbulusAI (creature);
    }

    struct boss_grobbulusAI : public BossAI
    {
        boss_grobbulusAI(Creature* creature) : BossAI(creature, BOSS_GROBBULUS)
        {
            me->ApplySpellImmune(0, IMMUNITY_ID, SPELL_POISON_CLOUD_ADD_N, true);
			me->ApplySpellImmune(1, IMMUNITY_ID, SPELL_POISON_CLOUD_ADD_H, true);
        }

        void EnterCombat(Unit* /*who*/)
        {
            _EnterCombat();
            events.ScheduleEvent(EVENT_CLOUD, urand(15000, 25000));
            events.ScheduleEvent(EVENT_INJECT, 12000);
            events.ScheduleEvent(EVENT_SPRAY, 15000+rand()%15000); //not sure
            events.ScheduleEvent(EVENT_BERSERK, RAID_MODE(720000, 360000));
			events.ScheduleEvent(EVENT_SLIMESTREAM, 5000);
        }

        void SpellHitTarget(Unit* target, const SpellInfo* spell)
        {
            if (spell->Id == uint32(SPELL_SLIME_SPRAY) && target->GetTypeId() == TYPEID_PLAYER)
            {
                if (TempSummon* slime = me->SummonCreature(NPC_FALLOUT_SLIME, target->GetPositionX(), target->GetPositionY(), target->GetPositionZ(), 0, TEMPSUMMON_TIMED_DESPAWN_OUT_OF_COMBAT, 0))
                    DoZoneInCombat(slime);
            }
        }

		void JustSummoned(Creature *summon)
		{
			//if(Unit *target = me->getVictim())
				//summon->Attack(target, true);

			summon->CastSpell(summon, SPELL_DISEASE_CLOUD);
		}

        void UpdateAI(const uint32 diff)
        {
            if (!UpdateVictim())
                return;

            events.Update(diff);

            while (uint32 eventId = events.ExecuteEvent())
            {
                switch (eventId)
                {
                    case EVENT_CLOUD:
                        DoCastAOE(SPELL_POISON_CLOUD);
                        events.ScheduleEvent(EVENT_CLOUD, 15000);
                        return;
                    case EVENT_BERSERK:
                        DoCastAOE(SPELL_BERSERK);
                        return;
                    case EVENT_SPRAY:
                        DoCastAOE(SPELL_SLIME_SPRAY);
                        events.ScheduleEvent(EVENT_SPRAY, 15000+rand()%15000);
                        return;
                    case EVENT_INJECT:
                        if (Unit* target = SelectTarget(SELECT_TARGET_RANDOM, 1))
                            if (!target->HasAura(SPELL_MUTATING_INJECTION))
                                DoCast(target, SPELL_MUTATING_INJECTION);
                        events.ScheduleEvent(EVENT_INJECT, 8000 + uint32(100 * me->GetHealthPct()));
                        return;
					case EVENT_SLIMESTREAM:
					if (!(me->CanReachWithMeleeAttack(me->getVictim())))
					{
							DoCast(SPELL_SLIME_STREAM);
							// Give some time, to re-reach grobbulus
							events.ScheduleEvent(EVENT_SLIMESTREAM, 3 * IN_MILLISECONDS);
					}
					else
					events.ScheduleEvent(EVENT_SLIMESTREAM, 1000);
				}
			}
            DoMeleeAttackIfReady();
        }
    };

};

class npc_grobbulus_poison_cloud : public CreatureScript
{
public:
    npc_grobbulus_poison_cloud() : CreatureScript("npc_grobbulus_poison_cloud") { }

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_grobbulus_poison_cloudAI(creature);
    }

    struct npc_grobbulus_poison_cloudAI : public Scripted_NoMovementAI
    {
        npc_grobbulus_poison_cloudAI(Creature* creature) : Scripted_NoMovementAI(creature)
        {
            Reset();
        }

        uint32 Cloud_Timer;

        void Reset()
        {
            Cloud_Timer = 1000;
            me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
        }

        void UpdateAI(const uint32 diff)
        {
            if (Cloud_Timer <= diff)
            {
				if(me->GetMap()->Is25ManRaid())
					DoCast(me, SPELL_POISON_CLOUD_ADD_H);
				else
					DoCast(me, SPELL_POISON_CLOUD_ADD_N);

                Cloud_Timer = 10000;
            } else Cloud_Timer -= diff;
        }
    };

};

class npc_fallout_slime : public CreatureScript
{
public:
    npc_fallout_slime() : CreatureScript("npc_fallout_slime") { }

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_fallout_slimeAI(creature);
    }

    struct npc_fallout_slimeAI : public CreatureAI
    {
        npc_fallout_slimeAI(Creature* creature) : CreatureAI(creature)
        {
            Reset();
        }

        uint32 Disease_Timer;

        void Reset()
        {
            Disease_Timer = 500;
        }

        void UpdateAI(const uint32 diff)
        {
            if (Disease_Timer <= diff)
            {
                DoCast(71125);
                Disease_Timer = 2000;
            } else Disease_Timer -= diff;

			DoMeleeAttackIfReady();
        }
    };

};

void AddSC_boss_grobbulus()
{
    new boss_grobbulus();
    new npc_grobbulus_poison_cloud();
	new npc_fallout_slime();
}
