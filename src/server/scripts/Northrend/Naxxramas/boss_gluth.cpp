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
#include "Player.h"


#define SPELL_MORTAL_WOUND			54378			//25646 is the old spell
#define SPELL_ENRAGE				RAID_MODE(28371, 54427)
#define SPELL_DECIMATE				RAID_MODE(28374, 54426)
#define SPELL_BERSERK				26662
#define SPELL_INFECTED_AURA			29307
#define SPELL_INFECTED_WOUND		29306

#define MOB_ZOMBIE  16360	//Zombie Chow

const Position PosSummon[3] =
{
    {3267.9f, -3172.1f, 297.42f, 0.94f},
    {3253.2f, -3132.3f, 297.42f, 0},
    {3308.3f, -3185.8f, 297.42f, 1.58f}
};

enum Events
{
    EVENT_NONE,
    EVENT_WOUND,
    EVENT_ENRAGE,
    EVENT_DECIMATE,
    EVENT_BERSERK,
    EVENT_SUMMON,
	EVENT_EATEN,
};

enum Emotes
{
	EMOTE_NEARBY		= 2,
	EMOTE_ENRAGE		= 0,
	EMOTE_DECIMATE		= 1
};


class boss_gluth : public CreatureScript
{
public:
    boss_gluth() : CreatureScript("boss_gluth") { }

    CreatureAI* GetAI(Creature* creature) const
    {
        return new boss_gluthAI (creature);
    }

    struct boss_gluthAI : public BossAI
    {
        boss_gluthAI(Creature* creature) : BossAI(creature, BOSS_GLUTH)
        {
            // Do not let Gluth be affected by zombies' debuff
            me->ApplySpellImmune(0, IMMUNITY_ID, SPELL_INFECTED_WOUND, true);
        }
		
		bool recentlyEaten;


		void Reset()
        {
			recentlyEaten = false;
            _Reset();
        }
		
		void DecimatePlayers()
		{
			Map* map = me->GetMap();
			Map::PlayerList const &PlayerList = map->GetPlayers();
			for (Map::PlayerList::const_iterator itr = PlayerList.begin(); itr != PlayerList.end(); ++itr)
				if (Player* player = itr->getSource())
					if(player->IsInRange(me, 0, 10, true))
					{
						int32 damage = int32(player->GetHealth()) - int32(player->CountPctFromMaxHealth(5));
						if (damage > 0)
							me->CastCustomSpell(28375, SPELLVALUE_BASE_POINT0, damage, player, true);
					}
		}

		void AttackGluth(Creature* pWho)
        {
            pWho->SetReactState(REACT_PASSIVE);
            pWho->AddThreat(me, 9999999);
            pWho->AI()->AttackStart(me);
            pWho->ApplySpellImmune(0, IMMUNITY_STATE, SPELL_AURA_MOD_TAUNT, true);
            pWho->ApplySpellImmune(0, IMMUNITY_EFFECT, SPELL_EFFECT_ATTACK_ME, true);
        }

        void MoveInLineOfSight(Unit* who)
        {
            if (who->GetEntry() == MOB_ZOMBIE && me->IsWithinDistInMap(who, 7))
            {
                SetGazeOn(who);
				AttackGluth(who->ToCreature());
				me->SetFacingToObject(who);
				me->GetMotionMaster()->MoveChase(who);

                Talk(EMOTE_NEARBY);
            }
            else
                BossAI::MoveInLineOfSight(who);
        }

		void EatZombie(Unit* who)
		{
            if(who->GetEntry() == MOB_ZOMBIE && !recentlyEaten)
            {
                if(me->IsWithinMeleeRange(who))
                {
					me->SetFacingToObject(who);
					Talk(EMOTE_NEARBY);
					me->GetMotionMaster()->MoveChase(who);
					recentlyEaten = true;
                    me->Kill(who);
                    me->ModifyHealth(int32(me->CountPctFromMaxHealth(5)));
					me->Attack(me->getVictim(), true);
					me->GetMotionMaster()->MoveChase(me->getVictim());
                }
			}
		}

        void EnterCombat(Unit*)
        {
            _EnterCombat();
            events.ScheduleEvent(EVENT_WOUND, 10000);
            events.ScheduleEvent(EVENT_ENRAGE, 15000);
            events.ScheduleEvent(EVENT_DECIMATE, RAID_MODE(110000,90000));
            events.ScheduleEvent(EVENT_BERSERK, RAID_MODE(4*110000, 4*90000));  //Every four Decimates, Cast Berserk
            events.ScheduleEvent(EVENT_SUMMON, 15000);
			events.ScheduleEvent(EVENT_EATEN, 1000);
        }

        void JustSummoned(Creature* summon)
        {
            if (summon->GetEntry() == MOB_ZOMBIE)
				if(Unit* pTarget = SelectTarget(SELECT_TARGET_RANDOM, 0, 100, true))
					summon->AI()->AttackStart(pTarget);

			summon->AddAura(SPELL_INFECTED_AURA, summon);

            summons.Summon(summon);
        }
		

		void DamageTaken(Unit* who)
		{
			if(who->GetEntry() == MOB_ZOMBIE)
				EatZombie(who);
		}

        void UpdateAI(const uint32 diff)
        {
            if (!UpdateVictimWithGaze() || !CheckInRoom())
                return;

            if(me->GetDistance(me->GetHomePosition()) > 80.0f)
            {
                EnterEvadeMode();
                return;
            }

            events.Update(diff);

            while (uint32 eventId = events.ExecuteEvent())
            {
                switch (eventId)
                {
                    case EVENT_WOUND:
                        DoCast(me->getVictim(), SPELL_MORTAL_WOUND);
                        events.ScheduleEvent(EVENT_WOUND, 10000);
                        break;
                    case EVENT_ENRAGE:
                        Talk(EMOTE_ENRAGE);
						DoCast(me, SPELL_ENRAGE);
                        events.ScheduleEvent(EVENT_ENRAGE, 15000);
                        break;
                    case EVENT_DECIMATE:
                        Talk(EMOTE_DECIMATE);
                        events.ScheduleEvent(EVENT_DECIMATE, RAID_MODE(110000, 90000));

						for(std::list<uint64>::const_iterator itr = summons.begin(); itr != summons.end(); ++itr)
                        {
                            Creature* minion = Unit::GetCreature(*me, *itr);
                            if(minion && minion->isAlive())
                            {
                                //hack
                                int32 damage = int32(minion->GetHealth()) - int32(minion->CountPctFromMaxHealth(0.00095));
                                if (damage > 0)
                                    me->CastCustomSpell(28375, SPELLVALUE_BASE_POINT0, damage, minion, true);
								minion->SetWalk(true);
                                AttackGluth(minion);
							}
						}
						DecimatePlayers();
                        break;
                    case EVENT_BERSERK:
                        DoCast(me, SPELL_BERSERK);
                        events.ScheduleEvent(EVENT_BERSERK, RAID_MODE(4*120000, 4*90000));
                        break;
                    case EVENT_SUMMON:
                        for (int32 i = 0; i < RAID_MODE(1, 2); ++i)
                            DoSummon(MOB_ZOMBIE, PosSummon[rand() % RAID_MODE(1, 3)]);
                        events.ScheduleEvent(EVENT_SUMMON, 10000);
                        break;
					case EVENT_EATEN:
						if(recentlyEaten)
							recentlyEaten = false;
						if(Creature* zombie = me->FindNearestCreature(16360, 5.0f))
							EatZombie(zombie);
						events.ScheduleEvent(EVENT_EATEN, 1000);
						break;
                }
            }
			DoMeleeAttackIfReady();
        }
    };
};

void AddSC_boss_gluth()
{
    new boss_gluth();
}
