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
#include "ScriptedGossip.h"
#include "naxxramas.h"
#include "InstanceScript.h"

/************************************************************************/
/*								Center					                */
/************************************************************************/
class npc_mr_bigglesworth : public CreatureScript
{
    public:
        npc_mr_bigglesworth () : CreatureScript("npc_mr_bigglesworth") {}

        struct npc_mr_bigglesworthAI: public ScriptedAI
        {
            npc_mr_bigglesworthAI(Creature* creature) : ScriptedAI(creature)
			{
				instance = creature->GetInstanceScript();
			}

			InstanceScript* instance;

            void JustDied(Unit* /*killer*/)
            {
				if(instance)
					instance->SetData(DATA_BIGGLESWORTH_KILLED, DONE);
            }
			
        };

        CreatureAI* GetAI(Creature* creature) const
        {
            return new npc_mr_bigglesworthAI(creature);
        }
};

/************************************************************************/
/*                       The Plague Quarter                             */
/************************************************************************/

class npc_stoneskin_gargoyle : public CreatureScript
{
    private:
        enum MyEvents
        {
            EVENT_ACID_VOLLEY       = 1
        };

        enum Spells
        {
            SPELL_ACID_VOLLEY       = 29325,
            SPELL_ACID_VOLLEY_H     = 54714,
            SPELL_STONESKIN         = 28995,
			SPELL_STONESKIN_H		= 54722
        };

    public:
        npc_stoneskin_gargoyle () : CreatureScript("npc_stoneskin_gargoyle") {}

        struct npc_stoneskin_gargoyleAI: public ScriptedAI
        {
            npc_stoneskin_gargoyleAI(Creature* creature) : ScriptedAI(creature) 
			{
				me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_INTERRUPT, true);
				me->ApplySpellImmune(1, IMMUNITY_MECHANIC, MECHANIC_STUN, true);
			}
			
			bool doOnce;
			float x, y, x2, y2;

            void Reset()
            {
                events.Reset();
                events.ScheduleEvent(EVENT_ACID_VOLLEY, urand(3*IN_MILLISECONDS, 5*IN_MILLISECONDS));
				
				x2 = 2840.86f;
				y2 = -3499.25f;
				doOnce = false;

				me->GetPosition(x, y);
            }

            void JustDied(Unit* /*killer*/)
            {
                if (InstanceScript* instance = me->GetInstanceScript())
                    if (instance->GetBossState(BOSS_LOATHEB) == DONE)
                        me->SetRespawnTime(604800); // Once the levi died, we will not spawn again
            }
			
			void EnterCombat(Unit* /*who*/)
			{
				me->RemoveStandFlags(UNIT_STAND_STATE_SUBMERGED);
			}

            void UpdateAI(uint32 const diff)
            {
                if (!UpdateVictim())
				{
					if(x == x2 && y == y2)
					{
						me->SetStandState(UNIT_STAND_STATE_SUBMERGED);
					}
                    return;
				}

                events.Update(diff);

                if (me->HasUnitState(UNIT_STATE_CASTING))
                    return;

				if (me->GetHealthPct() <= 30.0f && !doOnce)
				{
					doOnce = true;
					DoCast(SPELL_STONESKIN);
				}

                while (uint32 event = events.ExecuteEvent())
                {
                    switch (event)
                    {
                        case EVENT_ACID_VOLLEY:
                            DoCast(me->getVictim(), SPELL_ACID_VOLLEY);
                            events.ScheduleEvent(EVENT_ACID_VOLLEY, urand(5*IN_MILLISECONDS, 7*IN_MILLISECONDS));
                            break;
                        default:
                            break;
                    }
                }
                DoMeleeAttackIfReady();
            }

            private:
                EventMap events;
        };

        CreatureAI* GetAI(Creature* creature) const
        {
            return new npc_stoneskin_gargoyleAI(creature);
        }
};

class npc_infectious_ghoul : public CreatureScript
{
    private:
        enum MyEvents
        {
            EVENT_ROT       = 1,
			EVENT_REND		= 2,
        };

        enum Spells
        {
			SPELL_ROT		= 29915,
			SPELL_ROT_H		= 54709,
			SPELL_REND		= 54708,
			SPELL_FRENZY	= 54701
        };

    public:
         npc_infectious_ghoul () : CreatureScript("npc_infectious_ghoul") {}

        struct  npc_infectious_ghoulAI: public ScriptedAI
        {
             npc_infectious_ghoulAI(Creature* creature) : ScriptedAI(creature) {}
		
            void Reset()
            {
                events.Reset();
                events.ScheduleEvent(EVENT_ROT, urand(1*IN_MILLISECONDS, 5*IN_MILLISECONDS)); 
				events.ScheduleEvent(EVENT_REND, urand(1*IN_MILLISECONDS, 5*IN_MILLISECONDS)); 
            }

            void UpdateAI(uint32 const diff)
            {
                if (!UpdateVictim())
					return;

                events.Update(diff);
				
				if(HealthBelowPct(50))
					DoCast(me, SPELL_FRENZY);

                while (uint32 event = events.ExecuteEvent())
                {
                    switch (event)
                    {
                        case EVENT_ROT:
                            DoCast(me->getVictim(), RAID_MODE(SPELL_ROT, SPELL_ROT_H));
                            events.ScheduleEvent(EVENT_ROT, urand(12*IN_MILLISECONDS, 14*IN_MILLISECONDS));
                            break;
						case EVENT_REND:
                            DoCast(me->getVictim(), SPELL_REND);
                            events.ScheduleEvent(EVENT_REND, urand(14*IN_MILLISECONDS, 17*IN_MILLISECONDS));
                            break;
                        default:
                            break;
                    }
                }
                DoMeleeAttackIfReady();
            }

            private:
                EventMap events;
        };

        CreatureAI* GetAI(Creature* creature) const
        {
            return new  npc_infectious_ghoulAI(creature);
        }
};

class npc_plague_beast : public CreatureScript
{
    private:
        enum MyEvents
        {
            EVENT_SPLASH      = 1,
        };

        enum Spells
        {
			SPELL_SPLASH		= 54780,
			SPELL_SPLASH_H		= 56538
        };

    public:
         npc_plague_beast () : CreatureScript("npc_plague_beast") {}

        struct  npc_plague_beastAI: public ScriptedAI
        {
             npc_plague_beastAI(Creature* creature) : ScriptedAI(creature) {}
		
            void Reset()
            {
                events.Reset();
                events.ScheduleEvent(EVENT_SPLASH, urand(1*IN_MILLISECONDS, 5*IN_MILLISECONDS)); 
            }

            void UpdateAI(uint32 const diff)
            {
                if (!UpdateVictim())
					return;

                events.Update(diff);

                while (uint32 event = events.ExecuteEvent())
                {
                    switch (event)
                    {
                        case EVENT_SPLASH:
                            if (Unit* target = SelectTarget(SELECT_TARGET_RANDOM, 0))
                                DoCast(target, RAID_MODE(SPELL_SPLASH, SPELL_SPLASH_H));
                            events.ScheduleEvent(EVENT_SPLASH, urand(10*IN_MILLISECONDS, 15*IN_MILLISECONDS));
                            break;
                        default:
                            break;
                    }
                }
                DoMeleeAttackIfReady();
            }

            private:
                EventMap events;
        };

        CreatureAI* GetAI(Creature* creature) const
        {
            return new  npc_plague_beastAI(creature);
        }
};


class npc_mutated_grub : public CreatureScript
{
    private:
        enum MyEvents
        {
            EVENT_SPLASH      = 1,
        };

        enum Spells
        {
			SPELL_SLIME_BURST	= 30109,
			SPELL_SLIME_BURST_H	= 54769
        };

    public:
         npc_mutated_grub () : CreatureScript("npc_mutated_grub") {}

        struct  npc_mutated_grubAI: public ScriptedAI
        {
             npc_mutated_grubAI(Creature* creature) : ScriptedAI(creature) {}
		
            void Reset()
            {
                events.Reset();
                events.ScheduleEvent(EVENT_SPLASH, 3000); 
            }

            void UpdateAI(uint32 const diff)
            {
                if (!UpdateVictim())
					return;

                events.Update(diff);

                while (uint32 event = events.ExecuteEvent())
                {
                    switch (event)
                    {
                        case EVENT_SPLASH:
							DoCastAOE(RAID_MODE(SPELL_SLIME_BURST, SPELL_SLIME_BURST_H));
                            events.ScheduleEvent(EVENT_SPLASH, 3000);
                            break;
                        default:
                            break;
                    }
                }
                DoMeleeAttackIfReady();
            }

            private:
                EventMap events;
        };

        CreatureAI* GetAI(Creature* creature) const
        {
            return new  npc_mutated_grubAI(creature);
        }
};


class npc_eye_stalk : public CreatureScript
{
    private:
        enum MyEvents
        {
            EVENT_MINDFLAY		= 1
        };

        enum Spells
        {
			SPELL_NES_MIND_FLAY                     = 29407,
			SPELL_NES_MIND_FLAY_H                   = 54805
        };

    public:
        npc_eye_stalk () : CreatureScript("npc_eye_stalk") {}

        struct npc_eye_stalkAI: public ScriptedAI
        {
            npc_eye_stalkAI(Creature* creature) : ScriptedAI(creature) {}
		
			uint32 submergeTimer;
			bool submerged;

            void Reset()
            {
                events.Reset();
                events.ScheduleEvent(EVENT_MINDFLAY, 3000);
				me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_DISABLE_MOVE);
				me->SetVisible(false);
				submerged = true;
            }
			
			void EnterCombat(Unit* /*who*/)
			{
				DoZoneInCombat();
			}

            void MoveInLineOfSight(Unit* who)
            {
                if (who->IsInRange(me, 1, 40, true) && who->GetTypeId() == TYPEID_PLAYER)
                {
                    me->SetVisible(true);
					me->Attack(who, false);
					submerged = false;
				}
            }

            void UpdateAI(uint32 const diff)
            {
                if (!UpdateVictim())
				{
					submergeTimer = 1000;
					return;
				}

				if(submergeTimer <= diff)
				{
					if(!submerged)
					{
						DoCast(33928);
						submerged = true;
						submergeTimer = 1000;
					} else {
						me->SetVisible(false);
						submergeTimer = 120000;
					}
				} else submergeTimer -= diff;

                events.Update(diff);

                while (uint32 event = events.ExecuteEvent())
                {
                    switch (event)
                    {
                        case EVENT_MINDFLAY:
							if (Unit* target = SelectTarget(SELECT_TARGET_RANDOM, 0, 40, true, 0))
							{
								DoCast(target,RAID_MODE(SPELL_NES_MIND_FLAY, SPELL_NES_MIND_FLAY_H));
								events.ScheduleEvent(EVENT_MINDFLAY, 3000);
							} else {
								events.ScheduleEvent(EVENT_MINDFLAY, 500);
							}
							break;
                        default:
                            break;
                    }
                }
            }

            private:
                EventMap events;
        };

        CreatureAI* GetAI(Creature* creature) const
        {
            return new npc_eye_stalkAI(creature);
        }
};

/************************************************************************/
/*                       The Military Quarter                           */
/************************************************************************/

class npc_shade_of_naxxramas : public CreatureScript
{
    private:
        enum MyEvents
        {
            EVENT_VOLLEY	= 1,
			EVENT_PORTAL	= 2
        };

        enum Spells
        {
			SPELL_VOLLEY	= 28407,
			SPELL_VOLLEY_H	= 55323,
			SPELL_PORTAL	= 28383
        };

    public:
        npc_shade_of_naxxramas () : CreatureScript("npc_shade_of_naxxramas") {}

        struct npc_shade_of_naxxramasAI: public ScriptedAI
        {
           npc_shade_of_naxxramasAI(Creature* creature) : ScriptedAI(creature) {}
		
            void Reset()
            {
                events.Reset();
				events.ScheduleEvent(EVENT_VOLLEY, 1000);
                events.ScheduleEvent(EVENT_PORTAL, 1000);
			}

            void UpdateAI(uint32 const diff)
            {
                if (!UpdateVictim())
					return;

                events.Update(diff);

                while (uint32 event = events.ExecuteEvent())
                {
                    switch (event)
                    {
                        case EVENT_VOLLEY:
							DoCastAOE(SPELL_VOLLEY);
							events.ScheduleEvent(EVENT_VOLLEY, urand(2000, 8000));
							break;
						case EVENT_PORTAL:
							DoCastAOE(SPELL_PORTAL);
							events.ScheduleEvent(EVENT_PORTAL, urand(15*IN_MILLISECONDS, 20*IN_MILLISECONDS));
							break;
                        default:
                            break;
                    }
                }
                DoMeleeAttackIfReady();
            }

            private:
                EventMap events;
        };

        CreatureAI* GetAI(Creature* creature) const
        {
            return new npc_shade_of_naxxramasAI(creature);
        }
};

class npc_skeletal_smith : public CreatureScript
{
    private:
        enum MyEvents
        {
            EVENT_CRUSH_ARMOR	= 1,
			EVENT_DISARM		= 2

        };

        enum Spells
        {
			SPELL_CRUSH_ARMOR	= 33661,
			SPELL_DISARM		= 6713
        };

    public:
        npc_skeletal_smith () : CreatureScript("npc_skeletal_smith") {}

        struct npc_skeletal_smithAI: public ScriptedAI
        {
           npc_skeletal_smithAI(Creature* creature) : ScriptedAI(creature) {}
		
            void Reset()
            {
                events.Reset();
				events.ScheduleEvent(EVENT_CRUSH_ARMOR, urand(3000, 5000));
                events.ScheduleEvent(EVENT_DISARM, urand(3*IN_MILLISECONDS, 5*IN_MILLISECONDS));
			}

            void UpdateAI(uint32 const diff)
            {
                if (!UpdateVictim())
					return;

                events.Update(diff);

                while (uint32 event = events.ExecuteEvent())
                {
                    switch (event)
                    {
                        case EVENT_CRUSH_ARMOR:
							DoCast(me->getVictim(), SPELL_CRUSH_ARMOR);
							events.ScheduleEvent(EVENT_CRUSH_ARMOR, urand(5000, 10000));
							break;
						case EVENT_DISARM:
							DoCast(me->getVictim(), SPELL_DISARM);
							events.ScheduleEvent(EVENT_DISARM, urand(8000, 12000));
							break;
                        default:
                            break;
                    }
                }
                DoMeleeAttackIfReady();
            }

            private:
                EventMap events;
        };

        CreatureAI* GetAI(Creature* creature) const
        {
            return new npc_skeletal_smithAI(creature);
        }
};

class npc_death_knight : public CreatureScript
{
    private:
        enum MyEvents
        {
            EVENT_DEATH_COIL    = 1,
			EVENT_HYSTERIA		= 2

        };

        enum Spells
        {
			SPELL_DEATH_COIL	 = 55209,
			SPELL_DEATH_COIL_H	 = 55320,
			SPELL_BLOOD_PRESENCE = 55212,
			SPELL_HYSTERIA		 = 55213
        };

    public:
        npc_death_knight () : CreatureScript("npc_death_knight") {}

        struct npc_death_knightAI: public ScriptedAI
        {
           npc_death_knightAI(Creature* creature) : ScriptedAI(creature) {}
		
            void Reset()
            {
                events.Reset();
				events.ScheduleEvent(EVENT_DEATH_COIL, urand(3000, 5000));
                events.ScheduleEvent(EVENT_HYSTERIA, urand(3*IN_MILLISECONDS, 5*IN_MILLISECONDS));
				DoCast(me, SPELL_BLOOD_PRESENCE);
			}

            void UpdateAI(uint32 const diff)
            {
                if (!UpdateVictim())
					return;

                events.Update(diff);

                while (uint32 event = events.ExecuteEvent())
                {
                    switch (event)
                    {
                        case EVENT_HYSTERIA:
                            if(Unit* ally = me->SelectNearbyFriendly(me, 30.0f)) 
									DoCast(ally, SPELL_HYSTERIA);
									events.ScheduleEvent(EVENT_HYSTERIA, urand(3*IN_MILLISECONDS, 5*IN_MILLISECONDS));
									break;
						case EVENT_DEATH_COIL:
							if(urand(0,1))
							{
								if(Unit* ally = me->SelectNearbyFriendlyMissingMostHP(me, 30.0f)) 
									DoCast(ally, SPELL_DEATH_COIL_H);
									events.ScheduleEvent(EVENT_DEATH_COIL, urand(3*IN_MILLISECONDS, 5*IN_MILLISECONDS));
									break;
							}else
								DoCast(me->getVictim(), RAID_MODE(SPELL_DEATH_COIL, SPELL_DEATH_COIL_H));
								events.ScheduleEvent(EVENT_DEATH_COIL, urand(3*IN_MILLISECONDS, 5*IN_MILLISECONDS));
								break;
                        default:
                            break;
                    }
                }
                DoMeleeAttackIfReady();
            }

            private:
                EventMap events;
        };

        CreatureAI* GetAI(Creature* creature) const
        {
            return new npc_death_knightAI(creature);
        }
};

class npc_death_knight_captain : public CreatureScript
{
    private:
        enum MyEvents
        {
            EVENT_BLOOD_PLAGUE		= 1,
			EVENT_PLAGUE_STRIKE		= 2,
			EVENT_RAISE_DEAD		= 3,

        };

        enum Spells
        {
			SPELL_BLOOD_PLAGUE		= 55264,
			SPELL_BLOOD_PLAGUE_H	= 55322,
			SPELL_PLAGUE_STRIKE		= 55255,
			SPELL_PLAGUE_STRIKE_H	= 55321,
			SPELL_UNDEAD_PRESENCE	= 55222,
			SPELL_RAISE_DEAD		= 28353
        };

    public:
        npc_death_knight_captain () : CreatureScript("npc_death_knight_captain") {}

        struct npc_death_knight_captainAI: public ScriptedAI
        {
           npc_death_knight_captainAI(Creature* creature) : ScriptedAI(creature) {}
		
            void Reset()
            {
                events.Reset();
				events.ScheduleEvent(EVENT_BLOOD_PLAGUE, urand(3000, 5000));
				events.ScheduleEvent(EVENT_RAISE_DEAD, urand(6000, 8000));
                events.ScheduleEvent(EVENT_PLAGUE_STRIKE, urand(3*IN_MILLISECONDS, 5*IN_MILLISECONDS));				
				DoCast(me, SPELL_UNDEAD_PRESENCE);            
			}

			
			void JustSummoned(Creature* summon)
			{
				if (summon->GetEntry() == 16390)
					if(Unit* pTarget = SelectTarget(SELECT_TARGET_RANDOM, 0, 20.0F, true))
						summon->AI()->AttackStart(pTarget);
			}
			
			const Position getPos()
			{
				const Position mypos = {me->GetPositionX(), me->GetPositionY(), me->GetPositionZ(), me->GetOrientation()};
				return mypos;
			}

            void UpdateAI(uint32 const diff)
            {
                if (!UpdateVictim())
					return;

                events.Update(diff);

                while (uint32 event = events.ExecuteEvent())
                {
                    switch (event)
                    {
                        case EVENT_PLAGUE_STRIKE:
							DoCast(me->getVictim(), RAID_MODE(SPELL_PLAGUE_STRIKE, SPELL_PLAGUE_STRIKE_H));
							events.ScheduleEvent(EVENT_PLAGUE_STRIKE, urand(3*IN_MILLISECONDS, 5*IN_MILLISECONDS));
							break;
						case EVENT_BLOOD_PLAGUE:
							DoCast(me->getVictim(), RAID_MODE(SPELL_BLOOD_PLAGUE, SPELL_BLOOD_PLAGUE_H));
							events.ScheduleEvent(EVENT_BLOOD_PLAGUE, urand(3*IN_MILLISECONDS, 5*IN_MILLISECONDS));
							break;
						case EVENT_RAISE_DEAD:
							DoSummon(16390, getPos());
							events.ScheduleEvent(EVENT_RAISE_DEAD, urand(15*IN_MILLISECONDS, 20*IN_MILLISECONDS));
							break;
                        default:
                            break;
                    }
                }
                DoMeleeAttackIfReady();
            }

            private:
                EventMap events;
        };

        CreatureAI* GetAI(Creature* creature) const
        {
            return new npc_death_knight_captainAI(creature);
        }
};

class npc_risen_squire : public CreatureScript
{
    private:
        enum MyEvents
        {
            EVENT_PIERCE       = 1,
        };

        enum Spells
        {
			SPELL_PIERCE		 = 55318,
        };

    public:
        npc_risen_squire () : CreatureScript("npc_risen_squire") {}

        struct npc_risen_squireAI: public ScriptedAI
        {
            npc_risen_squireAI(Creature* creature) : ScriptedAI(creature) {}
		
            void Reset()
            {
                events.Reset();
                events.ScheduleEvent(EVENT_PIERCE, urand(3*IN_MILLISECONDS, 5*IN_MILLISECONDS)); // again im leaving this empty for you
            }

            void UpdateAI(uint32 const diff)
            {
                if (!UpdateVictim())
					return;

                events.Update(diff);

                while (uint32 event = events.ExecuteEvent())
                {
                    switch (event)
                    {
                        case EVENT_PIERCE:
                            DoCast(me->getVictim(), SPELL_PIERCE);
                            events.ScheduleEvent(EVENT_PIERCE, urand(8*IN_MILLISECONDS, 10*IN_MILLISECONDS));
                            break;
                        default:
                            break;
                    }
                }
                DoMeleeAttackIfReady();
            }

            private:
                EventMap events;
        };

        CreatureAI* GetAI(Creature* creature) const
        {
            return new npc_risen_squireAI(creature);
        }
};

class npc_death_knight_cavalier : public CreatureScript
{
    private:
        enum MyEvents
        {
			EVENT_ICY_TOUCH		= 1,
			EVENT_STRANGULATE	= 2,
			EVENT_BONE_ARMOR	= 3
        };

        enum Spells
        {
			SPELL_BONE_ARMOR	= 55315,
			SPELL_BONE_ARMOR_H	= 55336,
			SPELL_ICY_TOUCH		= 55313,
			SPELL_ICY_TOUCH_H	= 55331,
			SPELL_STRANGULATE	= 55314,
			SPELL_STRANGULATE_H	= 55334,
			SPELL_DISMOUNT		= 55294
        };

    public:
        npc_death_knight_cavalier () : CreatureScript("npc_death_knight_cavalier") {}

        struct npc_death_knight_cavalierAI: public ScriptedAI
        {
            npc_death_knight_cavalierAI(Creature* creature) : ScriptedAI(creature) {}
			
            void Reset()
            {
                events.Reset();
				events.ScheduleEvent(EVENT_ICY_TOUCH, urand(1*IN_MILLISECONDS, 5*IN_MILLISECONDS));
				events.ScheduleEvent(EVENT_STRANGULATE, urand(1*IN_MILLISECONDS, 2*IN_MILLISECONDS));
				events.ScheduleEvent(EVENT_BONE_ARMOR, urand(3*IN_MILLISECONDS, 6*IN_MILLISECONDS));
            }
			
			void JustDied()
			{
				DoCast(me, SPELL_DISMOUNT);
			}

            void UpdateAI(uint32 const diff)
            {
                if (!UpdateVictim())
					return;

                events.Update(diff);

                while (uint32 event = events.ExecuteEvent())
                {
                    switch (event)
                    {
						case EVENT_ICY_TOUCH:
							DoCast(me->getVictim(), RAID_MODE(SPELL_ICY_TOUCH, SPELL_ICY_TOUCH_H));
							events.ScheduleEvent(EVENT_ICY_TOUCH, urand(3*IN_MILLISECONDS, 6*IN_MILLISECONDS));
							break;
						case EVENT_STRANGULATE:
							if (Unit* target = SelectTarget(SELECT_TARGET_RANDOM, 0))
                                DoCast(target, RAID_MODE(SPELL_STRANGULATE, SPELL_STRANGULATE_H));
							events.ScheduleEvent(EVENT_STRANGULATE, urand(6*IN_MILLISECONDS, 10*IN_MILLISECONDS));
							break;
						case EVENT_BONE_ARMOR:
							DoCast(me, RAID_MODE(SPELL_BONE_ARMOR, SPELL_BONE_ARMOR_H));
							events.ScheduleEvent(EVENT_BONE_ARMOR,20*IN_MILLISECONDS);
                        default:
                            break;
                    }
                }
                DoMeleeAttackIfReady();
            }

            private:
                EventMap events;
        };

        CreatureAI* GetAI(Creature* creature) const
        {
            return new npc_death_knight_cavalierAI(creature);
        }
};
/************************************************************************/
/*                       The Spider Quarter                             */
/************************************************************************/
class npc_venom_stalker : public CreatureScript
{
    private:
        enum MyEvents
        {
            EVENT_CHARGE       = 1,
        };

        enum Spells
        {
			SPELL_CHARGE		= 56674,	//28431 Possibly old version
			SPELL_CHARGE_H		= 59899		//53809 Possibly old version? not sure
        };

    public:
        npc_venom_stalker () : CreatureScript("npc_venom_stalker") {}

        struct npc_venom_stalkerAI: public ScriptedAI
        {
            npc_venom_stalkerAI(Creature* creature) : ScriptedAI(creature) {}
		
            void Reset()
            {
                events.Reset();
                events.ScheduleEvent(EVENT_CHARGE, urand(3*IN_MILLISECONDS, 5*IN_MILLISECONDS)); 
            }

            void UpdateAI(uint32 const diff)
            {
                if (!UpdateVictim())
					return;

                events.Update(diff);

                while (uint32 event = events.ExecuteEvent())
                {
                    switch (event)
                    {
                        case EVENT_CHARGE:
                            if (Unit* target = SelectTarget(SELECT_TARGET_RANDOM, 0))
                                DoCast(target, RAID_MODE(SPELL_CHARGE, SPELL_CHARGE_H));
                            events.ScheduleEvent(EVENT_CHARGE, urand(5*IN_MILLISECONDS, 8*IN_MILLISECONDS));
                            break;
                        default:
                            break;
                    }
                }
                DoMeleeAttackIfReady();
            }

            private:
                EventMap events;
        };

        CreatureAI* GetAI(Creature* creature) const
        {
            return new npc_venom_stalkerAI(creature);
        }
};


class npc_naxxramas_cultist : public CreatureScript
{
    private:
        enum MyEvents
        {
            EVENT_BURST       = 1,
        };

        enum Spells
        {
			SPELL_BURST		= 53850,	
			SPELL_BURST_H	= 53851		
        };

    public:
        npc_naxxramas_cultist () : CreatureScript("npc_naxxramas_cultist") {}

        struct npc_naxxramas_cultistAI: public ScriptedAI
        {
            npc_naxxramas_cultistAI(Creature* creature) : ScriptedAI(creature) {}
		
            void Reset()
            {
                events.Reset();
                events.ScheduleEvent(EVENT_BURST, urand(2*IN_MILLISECONDS, 5*IN_MILLISECONDS)); 
            }

            void UpdateAI(uint32 const diff)
            {
                if (!UpdateVictim())
					return;

                events.Update(diff);

                while (uint32 event = events.ExecuteEvent())
                {
                    switch (event)
                    {
                        case EVENT_BURST:
                            DoCast(RAID_MODE(SPELL_BURST, SPELL_BURST_H));
                            events.ScheduleEvent(EVENT_BURST, urand(3*IN_MILLISECONDS, 6*IN_MILLISECONDS));
                            break;
                        default:
                            break;
                    }
                }
                DoMeleeAttackIfReady();
            }

            private:
                EventMap events;
        };

        CreatureAI* GetAI(Creature* creature) const
        {
            return new npc_naxxramas_cultistAI(creature);
        }
};

/************************************************************************/
/*                       The Construct Quarter                          */
/************************************************************************/

class npc_plague_slime : public CreatureScript
{
    private:
        enum MyEvents
        {
			EVENT_CHANGE_COLOR	= 1,
        };

        enum Spells
        {
			SPELL_MORPH_BLACK	= 28987,	//Immune to Shadow
			SPELL_MORPH_BLUE	= 28988,	//Frost
			SPELL_MORPH_GREEN	= 28989,	//Nature
			SPELL_MORPH_RED		= 28990,	//Fire

			SPELL_IMM_BLACK		= 7743,
			SPELL_IMM_BLUE		= 7940,
			SPELL_IMM_GREEN		= 7941,
			SPELL_IMM_RED		= 7942
        };

    public:
        npc_plague_slime () : CreatureScript("npc_plague_slime") {}

        struct npc_plague_slimeAI: public ScriptedAI
        {
            npc_plague_slimeAI(Creature* creature) : ScriptedAI(creature) 
			{
				me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_INFECTED, true);
			}
		
            void Reset()
            {
                events.Reset();
				events.ScheduleEvent(EVENT_CHANGE_COLOR,0);
            }


			void RemoveImmunitiesAndMorph()
			{
				me->RemoveAurasDueToSpell(SPELL_IMM_BLACK);
				me->RemoveAurasDueToSpell(SPELL_IMM_RED);
				me->RemoveAurasDueToSpell(SPELL_IMM_GREEN);
				me->RemoveAurasDueToSpell(SPELL_IMM_BLUE);
				me->RemoveAurasDueToSpell(SPELL_MORPH_BLACK);
				me->RemoveAurasDueToSpell(SPELL_MORPH_RED);
				me->RemoveAurasDueToSpell(SPELL_MORPH_GREEN);
				me->RemoveAurasDueToSpell(SPELL_MORPH_BLUE);
			}

            void UpdateAI(uint32 const diff)
            {
                if (!UpdateVictim())
					return;

                events.Update(diff);

				int LastImmune = 0;

                while (uint32 event = events.ExecuteEvent())
                {
                    switch (event)
                    {
                        case EVENT_CHANGE_COLOR:
							switch(urand(0,3))
							{
								case 0:
									if(LastImmune != 0)
									{
										RemoveImmunitiesAndMorph();
										DoCast(SPELL_MORPH_BLACK);
										DoCast(SPELL_IMM_BLACK);
										LastImmune = 0;
									}
									break;
								case 1:
									if(LastImmune != 1)
									{
										RemoveImmunitiesAndMorph();
										DoCast(SPELL_MORPH_BLUE);
										DoCast(SPELL_IMM_BLUE);
										LastImmune = 1;
									}
									break;
								case 2:
									if(LastImmune != 2)
									{
										RemoveImmunitiesAndMorph();
										DoCast(SPELL_MORPH_GREEN);
										DoCast(SPELL_IMM_GREEN);
										LastImmune = 2;
									}
									break;
								case 3:
									if(LastImmune != 3)
									{
										RemoveImmunitiesAndMorph();
										DoCast(SPELL_MORPH_RED);
										DoCast(SPELL_IMM_RED);
										LastImmune = 3;
									}
									break;
								default:
									break;
							}
                            events.ScheduleEvent(EVENT_CHANGE_COLOR, urand(10000,15000));  //Temporary until I get concrete info when to switch forms  
							break;
                        default:
                            break;
                    }
                }
                DoMeleeAttackIfReady();
            }

            private:
                EventMap events;
        };

        CreatureAI* GetAI(Creature* creature) const
        {
            return new npc_plague_slimeAI(creature);
        }
};

class npc_zombie_chow : public CreatureScript
{
    private:
        enum Spells
        {
			SPELL_INFECTED_WOUND	= 29306,
        };

    public:
        npc_zombie_chow () : CreatureScript("npc_zombie_chow") {}

        struct npc_zombie_chowAI: public ScriptedAI
        {
            npc_zombie_chowAI(Creature* creature) : ScriptedAI(creature) {}
		
            void Reset()
            {
            }

			void DamageDealt(Unit *pVictim, uint32 &damage, DamageEffectType /*damagetype*/)
			{
				uint8 stackcount = 0;

				if (pVictim->HasAura(SPELL_INFECTED_WOUND)) //if aura exists
				{
					 if (Aura* pInfectedAura = pVictim->GetAura(SPELL_INFECTED_WOUND))
					 {
						 stackcount = pInfectedAura->GetStackAmount();
						 if (stackcount < 99)
							 pInfectedAura->SetStackAmount(stackcount + 1); // add one stack
						 pInfectedAura->SetDuration(pInfectedAura->GetMaxDuration()); // reset aura duration
					 }
				}
				else DoCast(pVictim, SPELL_INFECTED_WOUND);  //else add aura        
			}

            void UpdateAI(uint32 const diff)
            {
                if (!UpdateVictim())
					return;

                DoMeleeAttackIfReady();
            }

            private:
                EventMap events;
        };

        CreatureAI* GetAI(Creature* creature) const
        {
            return new npc_zombie_chowAI(creature);
        }
};

class npc_sludge_belcher : public CreatureScript
{
    private:
        enum MyEvents
        {
            EVENT_ACIDIC_SLUDGE	= 1
        };

        enum Spells
        {
			SPELL_ACIDIC_SLUDGE		= 27891,
			SPELL_ACIDIC_SLUDGE_H	= 54331	
        };

    public:
        npc_sludge_belcher () : CreatureScript("npc_sludge_belcher") {}

        struct npc_sludge_belcherAI: public ScriptedAI
        {
            npc_sludge_belcherAI(Creature* creature) : ScriptedAI(creature) {}
		
            void Reset()
            {
                events.Reset();
                events.ScheduleEvent(EVENT_ACIDIC_SLUDGE, urand(5000,10000)); 
            }

            void UpdateAI(uint32 const diff)
            {
                if (!UpdateVictim())
					return;

                events.Update(diff);

                while (uint32 event = events.ExecuteEvent())
                {
                    switch (event)
                    {
                        case EVENT_ACIDIC_SLUDGE:
							if (Unit* target = SelectTarget(SELECT_TARGET_RANDOM, 0))
								DoCast(target, RAID_MODE(SPELL_ACIDIC_SLUDGE, SPELL_ACIDIC_SLUDGE_H));
                            events.ScheduleEvent(EVENT_ACIDIC_SLUDGE, urand(10000,15000));  
							break;
                        default:
                            break;
                    }
                }
                DoMeleeAttackIfReady();
            }

            private:
                EventMap events;
        };

        CreatureAI* GetAI(Creature* creature) const
        {
            return new npc_sludge_belcherAI(creature);
        }
};

class npc_embalming_slime : public CreatureScript
{
    private:
        enum MyEvents
        {
            EVENT_CLOUD		= 1,
        };

        enum Spells
        {
			SPELL_CLOUD	= 28322,	
        };

    public:
        npc_embalming_slime () : CreatureScript("npc_embalming_slime") {}

        struct npc_embalming_slimeAI: public ScriptedAI
        {
            npc_embalming_slimeAI(Creature* creature) : ScriptedAI(creature) {}
		
            void Reset()
            {
                events.Reset();
                events.ScheduleEvent(EVENT_CLOUD, 1000); 
            }

            void UpdateAI(uint32 const diff)
            {
                if (!UpdateVictim())
					return;

                events.Update(diff);

                while (uint32 event = events.ExecuteEvent())
                {
                    switch (event)
                    {
                        case EVENT_CLOUD:
                            DoCast(SPELL_CLOUD);
                            events.ScheduleEvent(EVENT_CLOUD, 3000);  //These should mobs should cast this all the time.
                            break;
                        default:
                            break;
                    }
                }
                DoMeleeAttackIfReady();
            }

            private:
                EventMap events;
        };

        CreatureAI* GetAI(Creature* creature) const
        {
            return new npc_embalming_slimeAI(creature);
        }
};

class npc_mad_scientist : public CreatureScript
{
    private:
        enum MyEvents
        {
            EVENT_MANA_BURN		= 1,
			EVENT_GREAT_HEAL	= 2
        };

        enum Spells
        {
			SPELL_MANA_BURN		= 28301,	
			SPELL_MANA_BURN_H	= 54338,
			SPELL_GREAT_HEAL	= 28306,
			SPELL_GREAT_HEAL_H	= 54337
        };

    public:
        npc_mad_scientist () : CreatureScript("npc_mad_scientist") {}

        struct npc_mad_scientistAI: public ScriptedAI
        {
            npc_mad_scientistAI(Creature* creature) : ScriptedAI(creature) {}
		
            void Reset()
            {
                events.Reset();
                events.ScheduleEvent(EVENT_MANA_BURN, urand(3000,5000));
				events.ScheduleEvent(EVENT_GREAT_HEAL, urand(3000,5000)); 
            }

            void UpdateAI(uint32 const diff)
            {
                if (!UpdateVictim())
					return;

                events.Update(diff);

                while (uint32 event = events.ExecuteEvent())
                {
                    switch (event)
                    {
                        case EVENT_MANA_BURN:
							if (Unit* target = SelectTarget(SELECT_TARGET_RANDOM, 0))
								if (target->getPowerType() == POWER_MANA)
								{
									DoCast(target, RAID_MODE(SPELL_MANA_BURN, SPELL_MANA_BURN_H));
									events.ScheduleEvent(EVENT_MANA_BURN, urand(3*IN_MILLISECONDS, 6*IN_MILLISECONDS));
									break;
								}
								else
									events.ScheduleEvent(EVENT_MANA_BURN, 50);
									break;
						case EVENT_GREAT_HEAL:
							if(Unit* ally = me->FindNearestCreature(16021, 30.0f))
									DoCast(ally, RAID_MODE(SPELL_GREAT_HEAL, SPELL_GREAT_HEAL_H));
							events.ScheduleEvent(EVENT_GREAT_HEAL, urand(6*IN_MILLISECONDS, 12*IN_MILLISECONDS));
							break;
                        default:
                            break;
                    }
                }
                DoMeleeAttackIfReady();
            }

            private:
                EventMap events;
        };

        CreatureAI* GetAI(Creature* creature) const
        {
            return new npc_mad_scientistAI(creature);
        }
};

class npc_patchwork_golem : public CreatureScript
{
    private:
        enum MyEvents
        {
            EVENT_CLEAVE		= 1,
			EVENT_EXECUTE		= 2,
			EVENT_WAR_STOMP		= 3
        };

        enum Spells
        {
			SPELL_CLEAVE		= 27794,	
			SPELL_EXECUTE		= 56426,
			SPELL_WAR_STOMP		= 27758,
			SPELL_WAR_STOMP_H	= 56427,
			SPELL_CLOUD			= 27793
        };

    public:
        npc_patchwork_golem () : CreatureScript("npc_patchwork_golem") {}

        struct npc_patchwork_golemAI: public ScriptedAI
        {
            npc_patchwork_golemAI(Creature* creature) : ScriptedAI(creature) {}
		
            void Reset()
            {
                events.Reset();
                events.ScheduleEvent(EVENT_CLEAVE, urand(3000,5000));
				events.ScheduleEvent(EVENT_EXECUTE, 1000); 
				events.ScheduleEvent(EVENT_WAR_STOMP, urand(5000,8000));
            }
			
			void EnterCombat(Unit* /*who*/)
			{
				DoCast(me, SPELL_CLOUD);
			}

            void UpdateAI(uint32 const diff)
            {
                if (!UpdateVictim())
					return;

                events.Update(diff);

                while (uint32 event = events.ExecuteEvent())
                {
                    switch (event)
                    {
                        case EVENT_CLEAVE:
							DoCast(me->getVictim(),SPELL_CLEAVE);
							events.ScheduleEvent(EVENT_CLEAVE, urand(3000,5000));
							break;
						case EVENT_EXECUTE:
							if(me->getVictim()->GetHealthPct() <= 20.0f)
							{
								DoCast(me->getVictim(), SPELL_EXECUTE);
								events.ScheduleEvent(EVENT_EXECUTE, urand(5000, 8000));
								break;
							}else
								events.ScheduleEvent(EVENT_EXECUTE, 500);
								break;
						case EVENT_WAR_STOMP:
							DoCastAOE(RAID_MODE(SPELL_WAR_STOMP, SPELL_WAR_STOMP_H));
							events.ScheduleEvent(EVENT_WAR_STOMP,urand(5000,8000));
							break;
                        default:
                            break;
                    }
                }
                DoMeleeAttackIfReady();
            }

            private:
                EventMap events;
        };

        CreatureAI* GetAI(Creature* creature) const
        {
            return new npc_patchwork_golemAI(creature);
        }
};


class npc_stitched_giant : public CreatureScript
{
    private:
        enum MyEvents
        {
            EVENT_KNOCKBACK		= 1,
        };

        enum Spells
        {
			SPELL_KNOCKBACK	= 28405,	
			SPELL_ENRAGE	= 54356
        };

    public:
        npc_stitched_giant () : CreatureScript("npc_stitched_giant") {}

        struct npc_stitched_giantAI: public ScriptedAI
        {
            npc_stitched_giantAI(Creature* creature) : ScriptedAI(creature) {}
		
            void Reset()
            {
                events.Reset();
                events.ScheduleEvent(EVENT_KNOCKBACK, 1000); 
            }

            void UpdateAI(uint32 const diff)
            {
                if (!UpdateVictim())
					return;

                events.Update(diff);
				
				if(HealthBelowPct(50) && !me->HasAura(SPELL_ENRAGE))
				{
					DoCast(me, SPELL_ENRAGE);
				}

                while (uint32 event = events.ExecuteEvent())
                {
                    switch (event)
                    {
                        case EVENT_KNOCKBACK:
                            DoCastAOE(SPELL_KNOCKBACK);
                            events.ScheduleEvent(EVENT_KNOCKBACK, urand(10*IN_MILLISECONDS, 15*IN_MILLISECONDS));  
                            break;
                        default:
                            break;
                    }
                }
                DoMeleeAttackIfReady();
            }

            private:
                EventMap events;
        };

        CreatureAI* GetAI(Creature* creature) const
        {
            return new npc_stitched_giantAI(creature);
        }
};

class npc_living_poison : public CreatureScript
{
    private:
        enum MyEvents
        {
            EVENT_MOVE		= 1
        };

        enum Spells
        {
			SPELL_EXPLODE		= 28433,	
        };

    public:
        npc_living_poison () : CreatureScript("npc_living_poison") {}

        struct npc_living_poisonAI: public ScriptedAI
        {
            npc_living_poisonAI(Creature* creature) : ScriptedAI(creature) {}
		
            void Reset()
            {
                events.Reset();
                events.ScheduleEvent(EVENT_MOVE, urand(3000,5000));
            }
			
			void MoveInLineOfSight(Unit* who)
			{
				if(who->GetTypeId() != TYPEID_PLAYER)
					return;

				if(who->IsWithinDist(me, 2, true) && who->isInFront(me))
				{
					DoCast(SPELL_EXPLODE);
					me->Kill(who, true);
					me->Kill(me, false);
				} else if(who->IsWithinDist(me, 1, true))
				{
					DoCast(SPELL_EXPLODE);
					me->Kill(who, true);
					me->Kill(me, false);
				}
			}

            void UpdateAI(uint32 const diff)
            {
                if (!UpdateVictim())
					return;

                /*events.Update(diff);

                while (uint32 event = events.ExecuteEvent())
                {
                    switch (event)
                    {
                        case EVENT_MOVE:
							DoCast(me->getVictim(),SPELL_CLEAVE);
							events.ScheduleEvent(EVENT_MOVE, urand(3000,5000));
							break;
                        default:
                            break;
                    }
                }*/
            }

            private:
                EventMap events;
        };

        CreatureAI* GetAI(Creature* creature) const
        {
            return new npc_living_poisonAI(creature);
        }
};

void AddSC_trash_naxxramas()
{
    // PQ
	new npc_mr_bigglesworth();
    new npc_stoneskin_gargoyle();
	new npc_infectious_ghoul();
	new npc_plague_beast();
	new npc_death_knight();
	new npc_death_knight_captain();
	new npc_risen_squire();
	new npc_death_knight_cavalier();
	new npc_venom_stalker();
	new npc_naxxramas_cultist();
	new npc_embalming_slime();
	new npc_mad_scientist();
	new npc_patchwork_golem();
	new npc_sludge_belcher();
	new npc_skeletal_smith();
	new npc_plague_slime();
	new npc_mutated_grub();
	new npc_stitched_giant();
	new npc_eye_stalk();
	new npc_shade_of_naxxramas();
	new npc_zombie_chow();
	new npc_living_poison();
}		