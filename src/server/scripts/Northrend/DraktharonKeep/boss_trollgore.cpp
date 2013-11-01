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

/*
 * Comment: TODO: spawn troll waves
 */

#include "ScriptMgr.h"
#include "ScriptedCreature.h"
#include "SpellAuras.h"
#include "drak_tharon_keep.h"

enum Spells
{
    SPELL_CRUSH                     = 49639,
    SPELL_INFECTED_WOUND            = 49637,
    SPELL_CORPSE_EXPLODE            = 49555,
    SPELL_CORPSE_EXPLODE_H          = 59807,
    SPELL_CONSUME                   = 49380,
    SPELL_CONSUME_H                 = 59803,
    SPELL_CONSUME_BUFF              = 49381,            // used to measure the achiev
    SPELL_CONSUME_BUFF_H            = 59805,

    SPELL_SUMMON_INVADER_1          = 49456,            // summon 27709
    SPELL_SUMMON_INVADER_2          = 49457,            // summon 27753
    // SPELL_SUMMON_INVADER_3        = 49458,            // summon 27754
    SPELL_INVADER_TAUNT             = 49405,            // triggers 49406

    MAX_CONSOME_STACKS              = 10,
};

Position spawn = {-260.49f, -622.96f, 26.60f, 0.0f};
enum Yells
{
    SAY_AGGRO                                     = -1600006,
    SAY_KILL                                      = -1600007,
    SAY_CONSUME                                   = -1600008,
    SAY_EXPLODE                                   = -1600009,
    SAY_DEATH                                     = -1600010
};

enum Creatures
{
    NPC_DRAKKARI_INVADER_1                        = 27753,
    NPC_DRAKKARI_INVADER_2                        = 27709
};

#define DATA_CONSUMPTION_JUNCTION                 1

Position AddSpawnPoint = { -260.493011f, -622.968018f, 26.605301f, 3.036870f };

class boss_trollgore : public CreatureScript
{
public:
    boss_trollgore() : CreatureScript("boss_trollgore") { }

    struct boss_trollgoreAI : public ScriptedAI
    {
        boss_trollgoreAI(Creature* creature) : ScriptedAI(creature), lSummons(me)
        {
            instance = creature->GetInstanceScript();
        }

        uint32 uiConsumeTimer;
        uint32 uiAuraCountTimer;
        uint32 uiCrushTimer;
        uint32 uiInfectedWoundTimer;
        uint32 uiExplodeCorpseTimer;
        uint32 uiSpawnTimer;
		uint32 uiConsumeStacks;

        bool consumptionJunction;

        SummonList lSummons;

        InstanceScript* instance;

        void Reset()
        {
            uiConsumeTimer = 15*IN_MILLISECONDS;
            uiAuraCountTimer = 15500;
            uiCrushTimer = urand(1*IN_MILLISECONDS, 5*IN_MILLISECONDS);
            uiInfectedWoundTimer = urand(6*IN_MILLISECONDS, 10*IN_MILLISECONDS);
            uiExplodeCorpseTimer = 20*IN_MILLISECONDS;
            uiSpawnTimer = 0;
			uiConsumeStacks = 0;

            consumptionJunction = true;

            lSummons.DespawnAll();

            me->RemoveAura(DUNGEON_MODE(SPELL_CONSUME_BUFF, SPELL_CONSUME_BUFF_H));

            if (instance)
                instance->SetData(DATA_TROLLGORE_EVENT, NOT_STARTED);
        }

        void EnterCombat(Unit* /*who*/)
        {
            DoScriptText(SAY_AGGRO, me);

            if (instance)
                instance->SetData(DATA_TROLLGORE_EVENT, IN_PROGRESS);
        }

        void UpdateAI(const uint32 diff)
        {
            //Return since we have no target
            if (!UpdateVictim())
                return;

            if (uiSpawnTimer <= diff)
            {
                uint32 spawnNumber = urand(2, DUNGEON_MODE(3, 5));
                for (uint8 i = 0; i < spawnNumber; ++i)
					DoSummonDrakkariInvaders();
                    //DoSummon(RAND(NPC_DRAKKARI_INVADER_1, NPC_DRAKKARI_INVADER_2), AddSpawnPoint, 0, TEMPSUMMON_DEAD_DESPAWN);
                uiSpawnTimer = urand(30*IN_MILLISECONDS, 40*IN_MILLISECONDS);
            } else uiSpawnTimer -= diff;

            if (uiConsumeTimer <= diff)
            {
                DoScriptText(SAY_CONSUME, me);
                DoCast(DUNGEON_MODE(SPELL_CONSUME, SPELL_CONSUME_H));
                uiConsumeTimer = 15*IN_MILLISECONDS;
            } else uiConsumeTimer -= diff;

            if (consumptionJunction)
            {
                Aura* ConsumeAura = me->GetAura(DUNGEON_MODE(SPELL_CONSUME_BUFF, SPELL_CONSUME_BUFF_H));
                if (ConsumeAura && ConsumeAura->GetStackAmount() > 9)
                    consumptionJunction = false;
            }

            if (uiCrushTimer <= diff)
            {
                DoCastVictim(SPELL_CRUSH);
                uiCrushTimer = urand(10*IN_MILLISECONDS, 15*IN_MILLISECONDS);
            } else uiCrushTimer -= diff;

            if (uiInfectedWoundTimer <= diff)
            {
                DoCastVictim(SPELL_INFECTED_WOUND);
                uiInfectedWoundTimer = urand(25*IN_MILLISECONDS, 35*IN_MILLISECONDS);
            } else uiInfectedWoundTimer -= diff;

            if (uiExplodeCorpseTimer <= diff)
            {
                DoCast(DUNGEON_MODE(SPELL_CORPSE_EXPLODE, SPELL_CORPSE_EXPLODE_H));
                DoScriptText(SAY_EXPLODE, me);
                uiExplodeCorpseTimer = urand(15*IN_MILLISECONDS, 19*IN_MILLISECONDS);
            } else uiExplodeCorpseTimer -= diff;

            DoMeleeAttackIfReady();
        }

		void SpellHit(Unit* target, const SpellEntry* spell)
		{
			if (spell->Id == SPELL_CONSUME_BUFF || spell->Id == SPELL_CONSUME_BUFF_H)
			{
				++uiConsumeStacks;

				// if the boss has 10 stacks then set the achiev to fail
				if (uiConsumeStacks == MAX_CONSOME_STACKS)
				{
					if (instance)
						instance->SetData(DATA_CONSUMPTION_JUNCTION, FAIL);
				}
			}
		}

        void JustDied(Unit* /*killer*/)
        {
            DoScriptText(SAY_DEATH, me);

            lSummons.DespawnAll();

            if (instance)
                instance->SetData(DATA_TROLLGORE_EVENT, DONE);
        }

        uint32 GetData(uint32 type)
        {
            if (type == DATA_CONSUMPTION_JUNCTION)
                return consumptionJunction ? 1 : 0;

            return 0;
        }

        void KilledUnit(Unit* victim)
        {
            if (victim == me)
                return;
            DoScriptText(SAY_KILL, me);
        }

        void JustSummoned(Creature* summon)
        {
            lSummons.push_back(summon->GetGUID());
            if (summon->AI())
                summon->AI()->AttackStart(me);

        // This spell taunts the boss and the boss taunts back
        summon->CastSpell(me, SPELL_INVADER_TAUNT, true);
        }

		// Wrapper to handle the drakkari invaders summon
		void DoSummonDrakkariInvaders()
		{
			if (!instance)
				return;

			if (roll_chance_i(95))
			{
				// Summon a troll in the corner and 2 trolls in the air
				if (Creature* pTrigger = me->SummonCreature(22515, spawn, TEMPSUMMON_CORPSE_TIMED_DESPAWN, 5000))
					pTrigger->CastSpell(pTrigger, roll_chance_i(20) ? SPELL_SUMMON_INVADER_1 : SPELL_SUMMON_INVADER_2, true, NULL, NULL);
			}
		}

    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new boss_trollgoreAI(creature);
    }
};

class achievement_consumption_junction : public AchievementCriteriaScript
{
    public:
        achievement_consumption_junction() : AchievementCriteriaScript("achievement_consumption_junction")
        {
        }

        bool OnCheck(Player* /*player*/, Unit* target)
        {
            if (!target)
                return false;

            if (Creature* Trollgore = target->ToCreature())
                if (Trollgore->AI()->GetData(DATA_CONSUMPTION_JUNCTION))
                    return true;

            return false;
        }
};

void AddSC_boss_trollgore()
{
    new boss_trollgore();
    new achievement_consumption_junction();
}
