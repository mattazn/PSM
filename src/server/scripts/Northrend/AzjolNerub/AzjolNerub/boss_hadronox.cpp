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
* Comment: No Waves atm and the doors spells are crazy...
*
* When your group enters the main room (the one after the bridge), you will notice a group of 3 Nerubians.
* When you engage them, 2 more groups like this one spawn behind the first one - it is important to pull the first group back,
* so you don't aggro all 3. Hadronox will be under you, fighting Nerubians.
*
* This is the timed gauntlet - waves of non-elite spiders
* will spawn from the 3 doors located a little above the main room, and will then head down to fight Hadronox. After clearing the
* main room, it is recommended to just stay in it, kill the occasional non-elites that will attack you instead of the boss, and wait for
* Hadronox to make his way to you. When Hadronox enters the main room, she will web the doors, and no more non-elites will spawn.
*/

#include "ScriptMgr.h"
#include "ScriptedCreature.h"
#include "azjol_nerub.h"

enum Spells
{
    SPELL_ACID_CLOUD                              = 53400, // Victim
    SPELL_LEECH_POISON                            = 53030, // Victim
    SPELL_PIERCE_ARMOR                            = 53418, // Victim
    SPELL_WEB_GRAB                                = 57731, // Victim
    SPELL_WEB_FRONT_DOORS                         = 53177, // Self
    SPELL_WEB_SIDE_DOORS                          = 53185, // Self
    H_SPELL_ACID_CLOUD                            = 59419,
    H_SPELL_LEECH_POISON                          = 59417,
    H_SPELL_WEB_GRAB                              = 59421
};

float hadronoxPositions [6][3] = 
{
	{565.239380f, 512.999023f, 698.418091f},
	{606.513245f, 512.808472f, 695.214294f},
	{617.648499f, 546.856812f, 710.106750f},
	{603.418762f, 573.650696f, 722.369324f},
	{572.625366f, 577.674622f, 727.574890f},
	{530.971924f, 548.372070f, 731.715210f}
};

float trashPositions [4][4] = 
{
	{584.529f, 603.328f, 739.157f, 1.000f}, // Spawn 1
	{576.981f, 612.537f, 771.427f, 1.000f}, // Spawn 2
	{485.250f, 610.998f, 771.475f, 1.000f}, // Spawn 3
	{530.425f, 574.516f, 733.257f, 1.000f}  // Spawn 2 and 3 Run LoC
};

enum Mobs
{
    MOB_CHAMPION		                          = 29062
};
class boss_hadronox : public CreatureScript
{
public:
    boss_hadronox() : CreatureScript("boss_hadronox") { }

    struct boss_hadronoxAI : public ScriptedAI
    {
        boss_hadronoxAI(Creature* creature) : ScriptedAI(creature)
        {
            instance = creature->GetInstanceScript();
            fMaxDistance = 50.0f;
            bFirstTime = true;
        }

        InstanceScript* instance;

        uint32 uiAcidTimer;
        uint32 uiLeechTimer;
        uint32 uiPierceTimer;
        uint32 uiGrabTimer;
        uint32 uiDoorsTimer;
        uint32 uiCheckDistanceTimer;
		
		uint8 uiStep;
		uint32 uiWalkTimer;
        bool bFirstTime;

        float fMaxDistance;

        void Reset()
        {
            me->SetFloatValue(UNIT_FIELD_BOUNDINGRADIUS, 9.0f);
            me->SetFloatValue(UNIT_FIELD_COMBATREACH, 9.0f);

            uiAcidTimer = urand(10*IN_MILLISECONDS, 14*IN_MILLISECONDS);
            uiLeechTimer = urand(3*IN_MILLISECONDS, 9*IN_MILLISECONDS);
            uiPierceTimer = urand(1*IN_MILLISECONDS, 3*IN_MILLISECONDS);
            uiGrabTimer = urand(15*IN_MILLISECONDS, 19*IN_MILLISECONDS);
            uiDoorsTimer = urand(20*IN_MILLISECONDS, 30*IN_MILLISECONDS);
            uiCheckDistanceTimer = 2*IN_MILLISECONDS;

			if(instance && (instance->GetData(DATA_HADRONOX_PRE_1) == NOT_STARTED))
			{
			uiWalkTimer = 30000;
			uiStep = 0;
			}

            if (instance && (instance->GetData(DATA_HADRONOX_EVENT) != DONE && !bFirstTime))
                instance->SetData(DATA_HADRONOX_EVENT, FAIL);

            bFirstTime = false;
        }

        //when Hadronox kills any enemy (that includes a party member) she will regain 10% of her HP if the target had Leech Poison on
        void KilledUnit(Unit* Victim)
        {
            // not sure if this aura check is correct, I think it is though
            if (!Victim || !Victim->HasAura(DUNGEON_MODE(SPELL_LEECH_POISON, H_SPELL_LEECH_POISON)) || !me->isAlive())
                return;

            me->ModifyHealth(int32(me->CountPctFromMaxHealth(10)));
        }

        void JustDied(Unit* /*killer*/)
        {
            if (instance)
                instance->SetData(DATA_HADRONOX_EVENT, DONE);
        }

        void EnterCombat(Unit* /*who*/)
        {
            if (instance)
                instance->SetData(DATA_HADRONOX_EVENT, IN_PROGRESS);
            me->SetInCombatWithZone();
        }

        void CheckDistance(float dist, const uint32 uiDiff)
        {
            if (!me->isInCombat())
                return;

            float x=0.0f, y=0.0f, z=0.0f;
            me->GetRespawnPosition(x, y, z);

            if (uiCheckDistanceTimer <= uiDiff)
                uiCheckDistanceTimer = 5*IN_MILLISECONDS;
            else
            {
                uiCheckDistanceTimer -= uiDiff;
                return;
            }
            if (me->IsInEvadeMode() || !me->getVictim())
                return;
            if (me->GetDistance(x, y, z) > dist)
                EnterEvadeMode();
        }

		void Summon()
        {
               // me->SummonCreature(MOB_CHAMPION, trashPositions[0], TEMPSUMMON_TIMED_DESPAWN, 25*IN_MILLISECONDS);
				//me->SummonCreature(MOB_CHAMPION, trashPositions[1], TEMPSUMMON_TIMED_DESPAWN, 25*IN_MILLISECONDS);
				//me->SummonCreature(MOB_CHAMPION, trashPositions[2], TEMPSUMMON_TIMED_DESPAWN, 25*IN_MILLISECONDS);
        }

        void UpdateAI(const uint32 diff)
        {
			if(uiWalkTimer <= diff && (instance->GetData(DATA_KRIKTHIR_THE_GATEWATCHER_EVENT) == DONE) && uiStep < 6)
			{
				Summon();
				float oldSpeed = me->GetSpeed(MOVE_WALK);
				me->SetSpeed(MOVE_WALK,3,true);
				me->GetMotionMaster()->MovePoint(uiStep, hadronoxPositions[uiStep][0], hadronoxPositions[uiStep][1], hadronoxPositions[uiStep][2]);
				if(uiStep < 5)
					instance->SetData(DATA_HADRONOX_PRE_1,IN_PROGRESS);

				if(uiStep == 5)
					instance->SetData(DATA_HADRONOX_PRE_1, DONE);
				
				uiStep = uiStep + 1;
				uiWalkTimer = 20000;
				me->SetSpeed(MOVE_WALK, oldSpeed, true);
			} else uiWalkTimer -=diff;

			if(instance->GetData(DATA_KRIKTHIR_THE_GATEWATCHER_EVENT) != DONE)
			{
				me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_IMMUNE_TO_PC|UNIT_FLAG_NON_ATTACKABLE);
			} else {
				me->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_IMMUNE_TO_PC|UNIT_FLAG_NON_ATTACKABLE);
				me->SetInCombatWithZone();
			}

            //Return since we have no target
            if (!UpdateVictim())
                return;

            // Without he comes up through the air to players on the bridge after krikthir if players crossing this bridge!
            CheckDistance(fMaxDistance, diff);

            if (me->HasAura(SPELL_WEB_FRONT_DOORS) || me->HasAura(SPELL_WEB_SIDE_DOORS))
            {
                if (IsCombatMovementAllowed())
                    SetCombatMovement(false);
            }
            else if (!IsCombatMovementAllowed())
                SetCombatMovement(true);

            if (uiPierceTimer <= diff)
            {
                DoCast(me->getVictim(), SPELL_PIERCE_ARMOR);
                uiPierceTimer = 8*IN_MILLISECONDS;
            } else uiPierceTimer -= diff;

            if (uiAcidTimer <= diff)
            {
                if (Unit* target = SelectTarget(SELECT_TARGET_RANDOM, 0, 100, true))
                    DoCast(target, SPELL_ACID_CLOUD);

                uiAcidTimer = urand(20*IN_MILLISECONDS, 30*IN_MILLISECONDS);
            } else uiAcidTimer -= diff;

            if (uiLeechTimer <= diff)
            {
                if (Unit* target = SelectTarget(SELECT_TARGET_RANDOM, 0, 100, true))
                    DoCast(target, SPELL_LEECH_POISON);

                uiLeechTimer = urand(11*IN_MILLISECONDS, 14*IN_MILLISECONDS);
            } else uiLeechTimer -= diff;

            if (uiGrabTimer <= diff)
            {
                if (Unit* target = SelectTarget(SELECT_TARGET_RANDOM, 0)) // Draws all players (and attacking Mobs) to itself.
                    DoCast(target, SPELL_WEB_GRAB);

                uiGrabTimer = urand(15*IN_MILLISECONDS, 30*IN_MILLISECONDS);
            } else uiGrabTimer -= diff;

            if (uiDoorsTimer <= diff)
            {
                uiDoorsTimer = urand(30*IN_MILLISECONDS, 60*IN_MILLISECONDS);
            } else uiDoorsTimer -= diff;

            DoMeleeAttackIfReady();
        }
		void JustSummoned(Creature* summoned)
        {
			if(summoned->GetPositionX() == 576.981445f || summoned->GetPositionX() == 485.250763f)
				summoned->GetMotionMaster()->MovePoint(0, trashPositions[3][0], trashPositions[3][1], trashPositions[3][2]);

			summoned->Attack(me, true);
			//summoned->GetMotionMaster()->MovePoint(0, me->GetPositionX(), me->GetPositionY(), me->GetPositionZ());
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new boss_hadronoxAI(creature);
    }
};
enum PrimeSpells
{
	SPELL_MARK_OF_DARKNESS	 = 54309,
	SPELL_DRAIN_POWER		 = 54314,
	H_SPELL_MARK_OF_DARKNESS = 59352,
	H_SPELL_DRAIN_POWER		 = 59354
};

class npc_anubar_prime_guard : public CreatureScript
	{
	public:
	    npc_anubar_prime_guard() : CreatureScript("npc_anubar_prime_guard") { }
	
	    struct npc_anubar_prime_guardAI : public ScriptedAI
	    {
	        npc_anubar_prime_guardAI(Creature *c) : ScriptedAI(c) { }
	
	        uint32 uiMark_Timer;
			uint32 uiDrainPower_Timer;
	
	        void Reset()
	        {
	            uiMark_Timer=10000+rand()%10000;;
				uiDrainPower_Timer=1500+rand()%2500;
	        }
	
	        void EnterCombat(Unit *who) 
	        {
	            DoCast(me->getVictim(), SPELL_MARK_OF_DARKNESS);
	        }
	
	        void UpdateAI(const uint32 diff)
	        {
	            if (!UpdateVictim())
	                return;
	
	            if (uiMark_Timer<=diff)
	            {
	                if (Unit* player = SelectTarget(SELECT_TARGET_RANDOM,0,100,true))
	                    DoCast(player, SPELL_MARK_OF_DARKNESS);
	                uiMark_Timer=10000+rand()%10000;;
	            } else uiMark_Timer-=diff;
	
	            if (uiDrainPower_Timer<=diff)
	            {
	                DoCast(SPELL_DRAIN_POWER);
	                uiDrainPower_Timer=3000+rand()%10000;;
	            } else uiDrainPower_Timer-=diff;

	            DoMeleeAttackIfReady();
	        }
	    };
	
	    CreatureAI* GetAI(Creature* pCreature) const
	    {
	        return new npc_anubar_prime_guardAI(pCreature);
	    }
	
	};

void AddSC_boss_hadronox()
{
    new boss_hadronox;
	new npc_anubar_prime_guard();
}
