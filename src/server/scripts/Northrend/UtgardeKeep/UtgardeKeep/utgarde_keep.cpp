/*
 * Copyright (C) 2008-2012 TrinityCore <http://www.trinitycore.org/>
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
#include "utgarde_keep.h"
#include "SpellScript.h"
#include "SpellAuraEffects.h"

uint32 entry_search[3] =
{
    186609,
    186610,
    186611
};
enum eEnums
{
	// Random NPC text occurs on aggro
    SAY_RANDOM_0 = -1999926,
    SAY_RANDOM_1 = -1999927,
    SAY_RANDOM_2 = -1999928,
    SAY_RANDOM_3 = -1999929,
    SAY_RANDOM_4 = -1999930,
    SAY_RANDOM_5 = -1999931,
    SAY_RANDOM_6 = -1999932,
    SAY_RANDOM_7 = -1999933,
    SAY_RANDOM_8 = -1999934,
    SAY_RANDOM_9 = -1999935,

	// Dragonslayer Forge Master
	MOB_DRAGONSLAYER_FORGEMASTER = 24079,
    SPELL_BURNING_BRAND = 43757,
    H_SPELL_BURNING_BRAND = 59601,
	// Dragonslayer Ironhelm
	MOB_DRAGONSLAYER_IRONHELM = 23961,
	SPELL_HEROIC_STRIKE = 57846,
	SPELL_RINGING_SLAP = 42780,
	H_SPELL_HEROIC_STRIKE = 59607,
	H_SPELL_RINGING_SLAP = 59606,
	SPELL_SECOND_WIND = 42771,
	// Tunneling Ghoul
	SPELL_DECREPIFY = 42702,
	SPELL_STRIKE = 13446,
	// Savage Worg
	SPELL_ENRAGE = 42745,
	SPELL_POUNCE = 55077,
	// Dark Ranger Marrah
	SPELL_STEALTH = 34189,
	// Dragonflayer Bonecrusher
	SPELL_HEAD_CRACK = 9791,
	H_SPELL_HEAD_CRACK = 59599,
	SPELL_KNOCKDOWN_SPIN = 43935,
	// Dragonflayer Heartsplitter / Proto-Drake Rider
	SPELL_THROW = 43665,
	SPELL_PIERCING_JAB = 31551,
	SPELL_WING_CLIP = 32908,
	H_SPELL_THROW = 59603,
	H_SPELL_PIERCING_JAB = 59605,
	H_SPELL_WING_CLIP = 59604,
	// Dragonflayer Metalworker
	SPELL_METAL_ENRAGE = 8599,
	SPELL_SUNDER_ARMOR = 15572,
	H_SPELL_SUNDER_ARMOR = 59608,
	// Dragonflayer Overseer
	SPELL_BATTLE_SHOUT = 38232,
	SPELL_CHARGE = 35570,
	SPELL_DEMORALIZING_SHOUT = 16244,
	H_SPELL_BATTLE_SHOUT = 59614,
	H_SPELL_CHARGE = 59611,
	H_SPELL_DEMORALIZING_SHOUT = 59613,
	// Dragonflayer Weaponsmith
	SPELL_CLEAVE = 42724,
	SPELL_CONCUSSION_BLOW = 22427,
	H_SPELL_DISARM = 6713,
	// Dragonflayer Runecaster
	SPELL_RUNE_FLAME = 54965,
	SPELL_RUNE_PROTECTION = 42740,
	H_SPELL_RUNE_FLAME = 59617,
	H_SPELL_RUNE_PROTECTION = 59616,
	// Dragonflayer Strategist
	SPELL_BLIND = 42972,
	SPELL_HURL_DAGGER = 42772,
	SPELL_TICKING_BOMB = 54962,
	H_SPELL_HURL_DAGGER = 59685,
	H_SPELL_TICKING_BOMB = 60227,
	// Frenzied Geist
	SPELL_FIXATE = 40414,
	// Enslaved Proto-Drake
	SPELL_FLAME_BREATH = 50653,
	SPELL_KNOCK_AWAY = 49722,
	SPELL_REND = 43931,
	H_SPELL_FLAME_BREATH = 59692,
	H_SPELL_REND = 59691,
	// Proto-Drake Handler
	SPELL_DEBILITATING_STRIKE = 38621,
	SPELL_PROTO_THROW = 54983,
	SPELL_UNHOLY_RAGE = 43664,
	H_SPELL_DEBILITATING_STRIKE = 59695,
	H_SPELL_PROTO_THROW = 59696,
	H_SPELL_UNHOLY_RAGE = 59694,
	// Other / Shared Spells
    SPELL_CAUTERIZE = 43930,
	SPELL_DRINK_TANKARD = 42871
};

struct Position MovePos[]=
{
    {208.739f, -182.196f, 219.362f, 0.0f}, // Fly up
    //{222.382f, -171.363f, 222.51f, 0.0f}, // Fly towards
    {234.096f, -162.952f, 190.347f, 0.0f} // Fly down
};

class npc_dragonflayer_forge_master : public CreatureScript
{
public:
    npc_dragonflayer_forge_master() : CreatureScript("npc_dragonflayer_forge_master") { }

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_dragonflayer_forge_masterAI(creature);
    }

    struct npc_dragonflayer_forge_masterAI : public ScriptedAI
    {
        npc_dragonflayer_forge_masterAI(Creature* creature) : ScriptedAI(creature)
        {
            instance = creature->GetInstanceScript();
            fm_Type = 0;
        }

        InstanceScript* instance;
        uint8 fm_Type;
		uint32 BurningBrand_Timer;
		uint32 Cauterize_Timer;

        void Reset()
        {
			BurningBrand_Timer = 5000;
			Cauterize_Timer = 10000;

            if (fm_Type == 0)
                fm_Type = GetForgeMasterType();

            CheckForge();
        }

        void CheckForge()
        {
            if (instance)
            {
                switch (fm_Type)
                {
                    case 1:
                        instance->SetData(EVENT_FORGE_1, me->isAlive() ? NOT_STARTED : DONE);
                        break;

                    case 2:
                        instance->SetData(EVENT_FORGE_2, me->isAlive() ? NOT_STARTED : DONE);
                        break;

                    case 3:
                        instance->SetData(EVENT_FORGE_3, me->isAlive() ? NOT_STARTED : DONE);
                        break;
                }
            }
        }

        void JustDied(Unit* /*killer*/)
        {
            if (fm_Type == 0)
                fm_Type = GetForgeMasterType();

            if (instance)
            {
                switch (fm_Type)
                {
                    case 1:
                        instance->SetData(EVENT_FORGE_1, DONE);
                        break;

                    case 2:
                        instance->SetData(EVENT_FORGE_2, DONE);
                        break;

                    case 3:
                        instance->SetData(EVENT_FORGE_3, DONE);
                        break;
                }
            }
        }

        void EnterCombat(Unit* /*who*/)
        {
			me->CallForHelp(20);

            if (fm_Type == 0)
                fm_Type = GetForgeMasterType();

            if (instance)
            {
                switch (fm_Type)
                {
                    case 1:
                        instance->SetData(EVENT_FORGE_1, IN_PROGRESS);
                        break;

                    case 2:
                        instance->SetData(EVENT_FORGE_2, IN_PROGRESS);
                        break;

                    case 3:
                        instance->SetData(EVENT_FORGE_3, IN_PROGRESS);
                        break;
                }
            }
            me->SetUInt32Value(UNIT_NPC_EMOTESTATE, EMOTE_ONESHOT_NONE);
        }

        uint8 GetForgeMasterType()
        {
            float diff = 30.0f;
            uint8 near_f = 0;

            for (uint8 i = 0; i < 3; ++i)
            {
                if (GameObject* go = me->FindNearestGameObject(entry_search[i], 30))
                {
                    if (me->IsWithinDist(go, diff, false))
                    {
                        near_f = i + 1;
                        diff = me->GetDistance2d(go);
                    }
                }
            }

            switch (near_f)
            {
                case 1: return 1;
                case 2: return 2;
                case 3: return 3;
                default: return 0;
            }
        }

        void UpdateAI(const uint32 diff)
        {
            if (fm_Type == 0)
                fm_Type = GetForgeMasterType();

            if (!UpdateVictim())
                return;
			
			 if (Cauterize_Timer)
                {
                    if (Cauterize_Timer <= diff)
                    {
                        Cauterize_Timer = 15000;
                        if (Unit* pFriend = DoSelectLowestHpFriendly(40))
                        {
                            DoCast(pFriend, SPELL_CAUTERIZE);
							Cauterize_Timer = 15000;
                        }
                    } else Cauterize_Timer -= diff;
                }

            if (BurningBrand_Timer <= diff)
            {
                DoCast(me->getVictim(), IsHeroic() ? H_SPELL_BURNING_BRAND : SPELL_BURNING_BRAND);
                BurningBrand_Timer = 2500+rand()%5000;
            } else BurningBrand_Timer -= diff;

            DoMeleeAttackIfReady();
        }
    };
};


enum TickingTimeBomb
{
    SPELL_TICKING_TIME_BOMB_EXPLODE = 59687
};
class spell_ticking_time_bomb : public SpellScriptLoader
{
    public:
        spell_ticking_time_bomb() : SpellScriptLoader("spell_ticking_time_bomb") { }

        class spell_ticking_time_bomb_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_ticking_time_bomb_AuraScript);

            bool Validate(SpellInfo const* /*spellEntry*/)
            {
                return (bool) sSpellMgr->GetSpellInfo(SPELL_TICKING_TIME_BOMB_EXPLODE);
            }

            void HandleOnEffectRemove(AuraEffect const* /* aurEff */, AuraEffectHandleModes /* mode */)
            {
                if (GetCaster() == GetTarget())
                {
                    GetTarget()->CastSpell(GetTarget(), SPELL_TICKING_TIME_BOMB_EXPLODE, true);
                }
            }

            void Register()
            {
                OnEffectRemove += AuraEffectRemoveFn(spell_ticking_time_bomb_AuraScript::HandleOnEffectRemove, EFFECT_0, SPELL_AURA_PERIODIC_DUMMY, AURA_EFFECT_HANDLE_REAL);
            }
        };

        AuraScript* GetAuraScript() const
        {
            return new spell_ticking_time_bomb_AuraScript();
        }
};

enum Fixate
{
    SPELL_FIXATE_TRIGGER = 40415
};
class spell_fixate : public SpellScriptLoader
{
    public:
        spell_fixate() : SpellScriptLoader("spell_fixate") { }

        class spell_fixate_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_fixate_SpellScript);

            bool Validate(SpellInfo const* /*spellEntry*/)
            {
                return (bool) sSpellMgr->GetSpellInfo(SPELL_FIXATE_TRIGGER);
            }

            void HandleScriptEffect(SpellEffIndex /*effIndex*/)
            {
                // The unit has to cast the taunt on hisself, but we need the original caster for SPELL_AURA_MOD_TAUNT
                GetCaster()->CastSpell(GetCaster(), SPELL_FIXATE_TRIGGER, true, 0, 0, GetHitUnit()->GetGUID());
            }

            void Register()
            {
                OnEffectHitTarget += SpellEffectFn(spell_fixate_SpellScript::HandleScriptEffect, EFFECT_2, SPELL_EFFECT_SCRIPT_EFFECT);
            }
        };

        SpellScript* GetSpellScript() const
        {
            return new spell_fixate_SpellScript();
        }
};
class npc_dragonflayer_ironhelm : public CreatureScript
{
public:
    npc_dragonflayer_ironhelm() : CreatureScript("npc_dragonflayer_ironhelm") { }

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_dragonflayer_ironhelmAI(creature);
    }

    struct npc_dragonflayer_ironhelmAI : public ScriptedAI
    {
        npc_dragonflayer_ironhelmAI(Creature* creature) : ScriptedAI(creature)
        {
            instance = creature->GetInstanceScript();
        }

        InstanceScript* instance;
		uint32 HeroicStrike_Timer;
		uint32 RingingSlap_Timer;
		uint32 Check_Timer;

        void Reset()
        {
			HeroicStrike_Timer = 2000;
			RingingSlap_Timer = 4500+rand()%500;
        }

        void JustDied(Unit* /*killer*/)
        {
        }

        void EnterCombat(Unit* /*who*/)
        {
		uint32 uRandomSay = urand(0, 9);
		
		switch(uRandomSay) {
		case 0:
		DoScriptText(SAY_RANDOM_0,me);
		break;
		case 1:
		DoScriptText(SAY_RANDOM_1,me);
		break;
		case 2:
		DoScriptText(SAY_RANDOM_2,me);
		break;
		case 3:
		DoScriptText(SAY_RANDOM_3,me);
		break;
		case 4:
		DoScriptText(SAY_RANDOM_4,me);
		break;
		case 5:
		DoScriptText(SAY_RANDOM_5,me);
		break;
		case 6:
		DoScriptText(SAY_RANDOM_6,me);
		break;
		case 7:
		DoScriptText(SAY_RANDOM_7,me);
		break;
		case 8:
		DoScriptText(SAY_RANDOM_8,me);
		break;
		case 9:
		DoScriptText(SAY_RANDOM_9,me);
		break;
		}
		}

        void UpdateAI(const uint32 diff)
        {
            if (!UpdateVictim())
                return;
			
			if (HeroicStrike_Timer <= diff)
            {
                DoCast(me->getVictim(), IsHeroic() ? H_SPELL_HEROIC_STRIKE : SPELL_HEROIC_STRIKE);
                HeroicStrike_Timer = 5000+rand()%3000;
            } else HeroicStrike_Timer -= diff;

			if (RingingSlap_Timer <= diff)
            {
                DoCast(me->getVictim(), IsHeroic() ? H_SPELL_RINGING_SLAP : SPELL_RINGING_SLAP);
                RingingSlap_Timer = 8500+rand()%3500;
            } else RingingSlap_Timer -= diff;

            DoMeleeAttackIfReady();
        }
    };
};

class npc_savage_worg : public CreatureScript
	{
	public:
	    npc_savage_worg() : CreatureScript("npc_savage_worg") { }
	
	    struct npc_savage_worgAI : public ScriptedAI
	    {
	        npc_savage_worgAI(Creature *c) : ScriptedAI(c) { }
	
	        uint32 uiPounceTimer;
			bool bEnraged;
	
	        void Reset()
	        {
	            uiPounceTimer=0;
				bEnraged=false;
	        }
	
	        void EnterCombat(Unit *who) 
	        {
	            me->CallForHelp(15);
	        }
	
	        void UpdateAI(const uint32 diff)
	        {
	            if (!UpdateVictim())
	                return;
	
	            if (uiPounceTimer<=diff)
	            {
	                if (Unit* player = SelectTarget(SELECT_TARGET_FARTHEST,1,100,true))
	                    DoCast(player,SPELL_POUNCE);
	                uiPounceTimer=30000;
	            } else uiPounceTimer-=diff;
	
	            if (HealthBelowPct(40) && !bEnraged)
				{
	                DoCast(me,SPELL_ENRAGE);
					bEnraged = true;
				}
	
	            DoMeleeAttackIfReady();
	        }
	    };
	
	    CreatureAI* GetAI(Creature* pCreature) const
	    {
	        return new npc_savage_worgAI(pCreature);
	    }
	
	};

class npc_tunneling_ghoul : public CreatureScript
	{
	public:
	    npc_tunneling_ghoul() : CreatureScript("npc_tunneling_ghoul") { }
	
	    struct npc_tunneling_ghoulAI : public ScriptedAI
	    {
	        npc_tunneling_ghoulAI(Creature *c) : ScriptedAI(c) { }
	
	        uint32 Decrepify_Timer;
			uint32 Strike_Timer;
	
	        void Reset()
	        {
	            Decrepify_Timer = 1000+rand()%1000;
				Strike_Timer = 3000;
				me->SetUInt32Value(UNIT_NPC_EMOTESTATE, 35);
	        }
	
	        void EnterCombat(Unit *who) 
	        {
	            me->CallForHelp(15);
	        }
	
	        void UpdateAI(const uint32 diff)
	        {
	            if (!UpdateVictim())
	                return;
	
	            if (Decrepify_Timer<=diff)
	            {
	                DoCast(me->getVictim(), SPELL_DECREPIFY);
	                Decrepify_Timer=8000+rand()%5000;
	            } else Decrepify_Timer-=diff;
	
	            if (Strike_Timer<=diff)
	            {
	                DoCast(me->getVictim(), SPELL_DECREPIFY);
	                Strike_Timer=10000+rand()%11000;
	            } else Strike_Timer-=diff;
	
	            DoMeleeAttackIfReady();
	        }
	    };
	
	    CreatureAI* GetAI(Creature* pCreature) const
	    {
	        return new npc_tunneling_ghoulAI(pCreature);
	    }
	
	};

class npc_frenzied_geist : public CreatureScript
	{
	public:
	    npc_frenzied_geist() : CreatureScript("npc_frenzied_geist") { }
	
	    struct npc_frenzied_geistAI : public ScriptedAI
	    {
	        npc_frenzied_geistAI(Creature *c) : ScriptedAI(c) { }
	
	        void Reset()
	        {
	        }
	
	        void EnterCombat(Unit *who) 
	        {
			DoCast(me->getVictim(), SPELL_FIXATE);
	        }
	
	        void UpdateAI(const uint32 diff)
	        {
	            DoMeleeAttackIfReady();
	        }
	    };
	
	    CreatureAI* GetAI(Creature* pCreature) const
	    {
	        return new npc_frenzied_geistAI(pCreature);
	    }
	
	};

class npc_dark_ranger_marrah : public CreatureScript
	{
	public:
	    npc_dark_ranger_marrah() : CreatureScript("npc_dark_ranger_marrah") { }
	
	    struct npc_dark_ranger_marrahAI : public ScriptedAI
	    {
	        npc_dark_ranger_marrahAI(Creature *c) : ScriptedAI(c) { }
		
	        void Reset()
	        {
				DoCast(me,SPELL_STEALTH);
	        }
			
			void MoveInLineOfSight(Unit *who)
			{
				if(!who || who->GetTypeId() != TYPEID_PLAYER)
					return;

				if(who->ToPlayer()->GetTeamId() == TEAM_HORDE)
				{
					if(who->GetDistance2d(me) <= 5)
						me->RemoveAurasDueToSpell(SPELL_STEALTH);
				} else {
					me->SetVisible(false);
				}

				return;
			}

	        void EnterCombat(Unit *who) {}
	
	        void AttackStart(Unit *who) { return; }
	    };
	
	    CreatureAI* GetAI(Creature* pCreature) const
	    {
	        return new npc_dark_ranger_marrahAI(pCreature);
	    }
	
	};
class npc_dragonflayer_bonecrusher : public CreatureScript
{
public:
    npc_dragonflayer_bonecrusher() : CreatureScript("npc_dragonflayer_bonecrusher") { }

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_dragonflayer_bonecrusherAI(creature);
    }

    struct npc_dragonflayer_bonecrusherAI : public ScriptedAI
    {
        npc_dragonflayer_bonecrusherAI(Creature* creature) : ScriptedAI(creature)
        {
            instance = creature->GetInstanceScript();
        }

        InstanceScript* instance;
		uint32 HeadCrack_Timer;
		uint32 KnockdownSpin_Timer;

        void Reset()
        {
			HeadCrack_Timer = 4000+rand()%4000;
			KnockdownSpin_Timer = 11000+rand()%2000;
        }

        void JustDied(Unit* /*killer*/)
        {
        }

        void EnterCombat(Unit* /*who*/)
        {
		uint32 uRandomSay = urand(0, 9);
		
		switch(uRandomSay) {
		case 0:
		DoScriptText(SAY_RANDOM_0,me);
		break;
		case 1:
		DoScriptText(SAY_RANDOM_1,me);
		break;
		case 2:
		DoScriptText(SAY_RANDOM_2,me);
		break;
		case 3:
		DoScriptText(SAY_RANDOM_3,me);
		break;
		case 4:
		DoScriptText(SAY_RANDOM_4,me);
		break;
		case 5:
		DoScriptText(SAY_RANDOM_5,me);
		break;
		case 6:
		DoScriptText(SAY_RANDOM_6,me);
		break;
		case 7:
		DoScriptText(SAY_RANDOM_7,me);
		break;
		case 8:
		DoScriptText(SAY_RANDOM_8,me);
		break;
		case 9:
		DoScriptText(SAY_RANDOM_9,me);
		break;
		}
		}

        void UpdateAI(const uint32 diff)
        {
            if (!UpdateVictim())
                return;
			
			if (HeadCrack_Timer <= diff)
            {
                DoCast(me->getVictim(), IsHeroic() ? H_SPELL_HEAD_CRACK : SPELL_HEAD_CRACK);
                HeadCrack_Timer = 12000+rand()%7000;
            } else HeadCrack_Timer -= diff;

			if (KnockdownSpin_Timer <= diff)
            {
                DoCast(me->getVictim(), SPELL_KNOCKDOWN_SPIN);
                KnockdownSpin_Timer = 15000+rand()%2500;
            } else KnockdownSpin_Timer -= diff;

            DoMeleeAttackIfReady();
        }
    };
};

class npc_dragonflayer_heartsplitter : public CreatureScript
{
public:
    npc_dragonflayer_heartsplitter() : CreatureScript("npc_dragonflayer_heartsplitter") { }

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_dragonflayer_heartsplitterAI(creature);
    }

    struct npc_dragonflayer_heartsplitterAI : public ScriptedAI
    {
        npc_dragonflayer_heartsplitterAI(Creature* creature) : ScriptedAI(creature)
        {
            instance = creature->GetInstanceScript();
        }

        InstanceScript* instance;
		uint32 Throw_Timer;
		uint32 PiercingJab_Timer;
		uint32 WingClip_Timer;

        void Reset()
        {
			Throw_Timer = 500;
			PiercingJab_Timer = 6000+rand()%2000;
			WingClip_Timer = 3000+rand()%2000;
        }

        void JustDied(Unit* /*killer*/)
        {
        }

        void EnterCombat(Unit* /*who*/)
        {
		uint32 uRandomSay = urand(0, 9);
		
		switch(uRandomSay) {
		case 0:
		DoScriptText(SAY_RANDOM_0,me);
		break;
		case 1:
		DoScriptText(SAY_RANDOM_1,me);
		break;
		case 2:
		DoScriptText(SAY_RANDOM_2,me);
		break;
		case 3:
		DoScriptText(SAY_RANDOM_3,me);
		break;
		case 4:
		DoScriptText(SAY_RANDOM_4,me);
		break;
		case 5:
		DoScriptText(SAY_RANDOM_5,me);
		break;
		case 6:
		DoScriptText(SAY_RANDOM_6,me);
		break;
		case 7:
		DoScriptText(SAY_RANDOM_7,me);
		break;
		case 8:
		DoScriptText(SAY_RANDOM_8,me);
		break;
		case 9:
		DoScriptText(SAY_RANDOM_9,me);
		break;
		}
		}

        void UpdateAI(const uint32 diff)
        {
            if (!UpdateVictim())
                return;
			
			if (Throw_Timer <= diff)
            {
                DoCast(me->getVictim(), IsHeroic() ? H_SPELL_THROW : SPELL_THROW, true);
                Throw_Timer = 17000+rand()%2000;
            } else Throw_Timer -= diff;

			if (PiercingJab_Timer <= diff)
            {
                DoCast(me->getVictim(), IsHeroic() ? H_SPELL_PIERCING_JAB : SPELL_PIERCING_JAB);
                PiercingJab_Timer = 11000+rand()%6000;
            } else PiercingJab_Timer -= diff;

			if (WingClip_Timer <= diff)
            {
                DoCast(me->getVictim(), IsHeroic() ? H_SPELL_WING_CLIP : SPELL_WING_CLIP);
                WingClip_Timer = 9000+rand()%3500;
            } else WingClip_Timer -= diff;

            DoMeleeAttackIfReady();
        }
    };
};

class npc_dragonflayer_metalworker : public CreatureScript
{
public:
    npc_dragonflayer_metalworker() : CreatureScript("npc_dragonflayer_metalworker") { }

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_dragonflayer_metalworkerAI(creature);
    }

    struct npc_dragonflayer_metalworkerAI : public ScriptedAI
    {
        npc_dragonflayer_metalworkerAI(Creature* creature) : ScriptedAI(creature)
        {
            instance = creature->GetInstanceScript();
        }

        InstanceScript* instance;
		uint32 SunderArmor_Timer;
		bool isEnraged;

        void Reset()
        {
			isEnraged = false;
			SunderArmor_Timer = 2000;
        }

        void EnterCombat(Unit* /*who*/)
        {
		me->CallForHelp(15);
		uint32 uRandomSay = urand(0, 9);
		
		switch(uRandomSay) {
		case 0:
		DoScriptText(SAY_RANDOM_0,me);
		break;
		case 1:
		DoScriptText(SAY_RANDOM_1,me);
		break;
		case 2:
		DoScriptText(SAY_RANDOM_2,me);
		break;
		case 3:
		DoScriptText(SAY_RANDOM_3,me);
		break;
		case 4:
		DoScriptText(SAY_RANDOM_4,me);
		break;
		case 5:
		DoScriptText(SAY_RANDOM_5,me);
		break;
		case 6:
		DoScriptText(SAY_RANDOM_6,me);
		break;
		case 7:
		DoScriptText(SAY_RANDOM_7,me);
		break;
		case 8:
		DoScriptText(SAY_RANDOM_8,me);
		break;
		case 9:
		DoScriptText(SAY_RANDOM_9,me);
		break;
		}
		}

        void UpdateAI(const uint32 diff)
        {
            if (!UpdateVictim())
                return;
			
			if (SunderArmor_Timer <= diff)
            {
                DoCast(me->getVictim(), IsHeroic() ? H_SPELL_SUNDER_ARMOR : SPELL_SUNDER_ARMOR);
                SunderArmor_Timer = 7000;
            } else SunderArmor_Timer -= diff;

	            if (HealthBelowPct(30) && !isEnraged)
				{
	                DoCast(me,SPELL_METAL_ENRAGE);
					isEnraged = true;
				}

            DoMeleeAttackIfReady();
        }
    };
};

class npc_dragonflayer_overseer : public CreatureScript
{
public:
    npc_dragonflayer_overseer() : CreatureScript("npc_dragonflayer_overseer") { }

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_dragonflayer_overseerAI(creature);
    }

    struct npc_dragonflayer_overseerAI : public ScriptedAI
    {
        npc_dragonflayer_overseerAI(Creature* creature) : ScriptedAI(creature)
        {
            instance = creature->GetInstanceScript();
        }

        InstanceScript* instance;
		uint32 BattleShout_Timer;
		uint32 DemoralizingShout_Timer;

        void Reset()
        {
			BattleShout_Timer = 2000+rand()%1000;
			DemoralizingShout_Timer = 7500+rand()%500;
        }

        void EnterCombat(Unit* /*who*/)
        {
		me->CallForHelp(15);

		uint32 uRandomSay = urand(0, 9);
		
		switch(uRandomSay) {
		case 0:
		DoScriptText(SAY_RANDOM_0,me);
		break;
		case 1:
		DoScriptText(SAY_RANDOM_1,me);
		break;
		case 2:
		DoScriptText(SAY_RANDOM_2,me);
		break;
		case 3:
		DoScriptText(SAY_RANDOM_3,me);
		break;
		case 4:
		DoScriptText(SAY_RANDOM_4,me);
		break;
		case 5:
		DoScriptText(SAY_RANDOM_5,me);
		break;
		case 6:
		DoScriptText(SAY_RANDOM_6,me);
		break;
		case 7:
		DoScriptText(SAY_RANDOM_7,me);
		break;
		case 8:
		DoScriptText(SAY_RANDOM_8,me);
		break;
		case 9:
		DoScriptText(SAY_RANDOM_9,me);
		break;
		}

		DoCast(me->getVictim(), IsHeroic() ? H_SPELL_CHARGE : SPELL_CHARGE);
		}

        void UpdateAI(const uint32 diff)
        {
            if (!UpdateVictim())
                return;
			
			if (BattleShout_Timer <= diff)
            {
                DoCast(me->getVictim(), IsHeroic() ? H_SPELL_BATTLE_SHOUT : SPELL_BATTLE_SHOUT);
                BattleShout_Timer = 18000+rand()%2000;
            } else BattleShout_Timer -= diff;

			if (DemoralizingShout_Timer <= diff)
            {
                DoCast(me->getVictim(), IsHeroic() ? H_SPELL_DEMORALIZING_SHOUT : SPELL_DEMORALIZING_SHOUT);
                DemoralizingShout_Timer = 20000+rand()%10000;
            } else DemoralizingShout_Timer -= diff;

            DoMeleeAttackIfReady();
        }
    };
};
class npc_dragonflayer_weaponsmith : public CreatureScript
{
public:
    npc_dragonflayer_weaponsmith() : CreatureScript("npc_dragonflayer_weaponsmith") { }

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_dragonflayer_weaponsmithAI(creature);
    }

    struct npc_dragonflayer_weaponsmithAI : public ScriptedAI
    {
        npc_dragonflayer_weaponsmithAI(Creature* creature) : ScriptedAI(creature)
        {
            instance = creature->GetInstanceScript();
        }

        InstanceScript* instance;
		uint32 Cleave_Timer;
		uint32 ConcussionBlow_Timer;
		uint32 Disarm_Timer;

        void Reset()
        {
			Cleave_Timer = 2500+rand()%500;
			ConcussionBlow_Timer = 3500;
			Disarm_Timer = 8000+rand()%500;
        }

        void EnterCombat(Unit* /*who*/)
        {
		uint32 uRandomSay = urand(0, 9);
		
		switch(uRandomSay) {
		case 0:
		DoScriptText(SAY_RANDOM_0,me);
		break;
		case 1:
		DoScriptText(SAY_RANDOM_1,me);
		break;
		case 2:
		DoScriptText(SAY_RANDOM_2,me);
		break;
		case 3:
		DoScriptText(SAY_RANDOM_3,me);
		break;
		case 4:
		DoScriptText(SAY_RANDOM_4,me);
		break;
		case 5:
		DoScriptText(SAY_RANDOM_5,me);
		break;
		case 6:
		DoScriptText(SAY_RANDOM_6,me);
		break;
		case 7:
		DoScriptText(SAY_RANDOM_7,me);
		break;
		case 8:
		DoScriptText(SAY_RANDOM_8,me);
		break;
		case 9:
		DoScriptText(SAY_RANDOM_9,me);
		break;
		}
		}

        void UpdateAI(const uint32 diff)
        {
            if (!UpdateVictim())
                return;
			
			if (Cleave_Timer <= diff)
            {
                DoCast(me->getVictim(), SPELL_CLEAVE);
                Cleave_Timer = 8000+rand()%2000;
            } else Cleave_Timer -= diff;

			if (ConcussionBlow_Timer <= diff)
            {
                DoCast(me->getVictim(), SPELL_CONCUSSION_BLOW);
                ConcussionBlow_Timer = 12000+rand()%4000;
            } else ConcussionBlow_Timer -= diff;

			if(IsHeroic()) {
			if (Disarm_Timer <= diff)
            {
                DoCast(me->getVictim(), H_SPELL_DISARM);
                Disarm_Timer = 9000+rand()%2000;
            } else Disarm_Timer -= diff;
			}

            DoMeleeAttackIfReady();
        }
    };
};
class npc_dragonflayer_runecaster : public CreatureScript
{
public:
    npc_dragonflayer_runecaster() : CreatureScript("npc_dragonflayer_runecaster") { }

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_dragonflayer_runecasterAI(creature);
    }

    struct npc_dragonflayer_runecasterAI : public ScriptedAI
    {
        npc_dragonflayer_runecasterAI(Creature* creature) : ScriptedAI(creature)
        {
            instance = creature->GetInstanceScript();
        }

        InstanceScript* instance;
		uint32 Flame_Timer;
		uint32 Protection_Timer;
		bool bMeOrFriend;

        void Reset()
        {
			Flame_Timer = 9500+rand()%6500;
			Protection_Timer = 17000+rand()%8000;
        }

        void EnterCombat(Unit* /*who*/)
        {
		uint32 uRandomSpell = urand(0, 1);
		bMeOrFriend = urand(0, 1);
		uint32 uRandomSay = urand(0, 9);
		Unit* pFriend = DoSelectLowestHpFriendly(40);

		switch(uRandomSpell) {
		case 0:
			DoCast(bMeOrFriend ? me : pFriend, IsHeroic() ? H_SPELL_RUNE_FLAME : SPELL_RUNE_FLAME);
		break;
		case 1:
			DoCast(bMeOrFriend ? me : pFriend, IsHeroic() ? H_SPELL_RUNE_PROTECTION : SPELL_RUNE_PROTECTION);
		}

		switch(uRandomSay) {
		case 0:
		DoScriptText(SAY_RANDOM_0,me);
		break;
		case 1:
		DoScriptText(SAY_RANDOM_1,me);
		break;
		case 2:
		DoScriptText(SAY_RANDOM_2,me);
		break;
		case 3:
		DoScriptText(SAY_RANDOM_3,me);
		break;
		case 4:
		DoScriptText(SAY_RANDOM_4,me);
		break;
		case 5:
		DoScriptText(SAY_RANDOM_5,me);
		break;
		case 6:
		DoScriptText(SAY_RANDOM_6,me);
		break;
		case 7:
		DoScriptText(SAY_RANDOM_7,me);
		break;
		case 8:
		DoScriptText(SAY_RANDOM_8,me);
		break;
		case 9:
		DoScriptText(SAY_RANDOM_9,me);
		break;
		}
		}

        void UpdateAI(const uint32 diff)
        {
            if (!UpdateVictim())
                return;
			
			Unit* pFriend = DoSelectLowestHpFriendly(40);

			if (Flame_Timer <= diff)
            {
				bMeOrFriend = urand(0,1);
                DoCast(bMeOrFriend ? me : pFriend, IsHeroic() ? H_SPELL_RUNE_FLAME : SPELL_RUNE_FLAME);
                Flame_Timer = 9500+rand()%6500;
            } else Flame_Timer -= diff;

			if (Protection_Timer <= diff)
            {
				bMeOrFriend = urand(0,1);
                DoCast(bMeOrFriend ? me : pFriend, IsHeroic() ? H_SPELL_RUNE_PROTECTION : SPELL_RUNE_PROTECTION);
                Protection_Timer = 17000+rand()%8000;
            } else Protection_Timer -= diff;

            DoMeleeAttackIfReady();
        }
    };
};

class npc_dragonflayer_strategist : public CreatureScript
{
public:
    npc_dragonflayer_strategist() : CreatureScript("npc_dragonflayer_strategist") { }

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_dragonflayer_strategistAI(creature);
    }

    struct npc_dragonflayer_strategistAI : public ScriptedAI
    {
        npc_dragonflayer_strategistAI(Creature* creature) : ScriptedAI(creature)
        {
            instance = creature->GetInstanceScript();
        }

        InstanceScript* instance;
		uint32 HurlDagger_Timer;
		uint32 TickingBomb_Timer;
		uint32 Blind_Timer;
		uint32 Tankard_Timer;
		uint32 Phase;

        void Reset()
        {
			Phase = 1;
			me->GetMotionMaster()->MoveIdle();
			HurlDagger_Timer = 0;
			TickingBomb_Timer = 4500+rand()%500;
			Blind_Timer = 7500+rand()%1500;
			Tankard_Timer = 1000+rand()%19000;
        }

        void EnterCombat(Unit* /*who*/)
        {
		Phase = 1;
		me->GetMotionMaster()->MoveIdle();
		uint32 uRandomSay = urand(0, 9);
		
		switch(uRandomSay) {
		case 0:
		DoScriptText(SAY_RANDOM_0,me);
		break;
		case 1:
		DoScriptText(SAY_RANDOM_1,me);
		break;
		case 2:
		DoScriptText(SAY_RANDOM_2,me);
		break;
		case 3:
		DoScriptText(SAY_RANDOM_3,me);
		break;
		case 4:
		DoScriptText(SAY_RANDOM_4,me);
		break;
		case 5:
		DoScriptText(SAY_RANDOM_5,me);
		break;
		case 6:
		DoScriptText(SAY_RANDOM_6,me);
		break;
		case 7:
		DoScriptText(SAY_RANDOM_7,me);
		break;
		case 8:
		DoScriptText(SAY_RANDOM_8,me);
		break;
		case 9:
		DoScriptText(SAY_RANDOM_9,me);
		break;
		}
		}

        void UpdateAI(const uint32 diff)
        {
			if (!me->getVictim())
			{
			if (Tankard_Timer <= diff)
            {
				if(me->getStandState() == UNIT_STAND_STATE_SIT){
                DoCast(me, SPELL_DRINK_TANKARD);
                Tankard_Timer = 1000+rand()%19000;
				}
            } else Tankard_Timer -= diff;
			}

            if (!UpdateVictim())
                return;

				if(me->getVictim()->GetDistance2d(me) <= 15) {
					Phase = 0;
					me->GetMotionMaster()->MoveChase(me->getVictim());
				} else {
				Phase = 1;
				me->GetMotionMaster()->MoveIdle();
				}


			if(Phase == 1) {
			if (HurlDagger_Timer <= diff)
            {
                DoCast(me->getVictim(), IsHeroic() ? H_SPELL_HURL_DAGGER : SPELL_HURL_DAGGER);
                HurlDagger_Timer = 2300+rand()%200;
            } else HurlDagger_Timer -= diff;
			}

			if (TickingBomb_Timer <= diff)
            {
                DoCast(me->getVictim(), IsHeroic() ? H_SPELL_TICKING_BOMB : SPELL_TICKING_BOMB);
                TickingBomb_Timer = 11000+rand()%5000;
            } else TickingBomb_Timer -= diff;

			if (Blind_Timer <= diff)
            {
                DoCast(me->getVictim(), SPELL_BLIND);
                Blind_Timer = 13000+rand()%2000;
            } else Blind_Timer -= diff;

			if(Phase == 0)
            DoMeleeAttackIfReady();
        }
    };
};

class npc_enslaved_proto_drake : public CreatureScript
{
public:
    npc_enslaved_proto_drake() : CreatureScript("npc_enslaved_proto_drake") { }

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_enslaved_proto_drakeAI(creature);
    }

    struct npc_enslaved_proto_drakeAI : public ScriptedAI
    {
        npc_enslaved_proto_drakeAI(Creature* creature) : ScriptedAI(creature)
        {
            instance = creature->GetInstanceScript();
        }

        InstanceScript* instance;
		uint32 FlameBreath_Timer;
		uint32 KnockAway_Timer;
		uint32 Rend_Timer;

        void Reset()
        {
			FlameBreath_Timer = 5500+rand()%1500;
			KnockAway_Timer = 3500+rand()%6000;
			Rend_Timer = 2000+rand()%1000;
        }

	    void EnterCombat(Unit *who) 
	    {
			me->CallForHelp(10);
	    }

		void UpdateAI(const uint32 diff)
        {
            if (!UpdateVictim())
                return;
			
			if (FlameBreath_Timer <= diff)
            {
                DoCast(me->getVictim(), IsHeroic() ? H_SPELL_FLAME_BREATH : SPELL_FLAME_BREATH);
                FlameBreath_Timer = 11000+rand()%1000;
            } else FlameBreath_Timer -= diff;

			if (KnockAway_Timer <= diff)
            {
                DoCast(me->getVictim(), SPELL_KNOCK_AWAY);
                KnockAway_Timer = 7000+rand()%1500;
            } else KnockAway_Timer -= diff;

			if (Rend_Timer <= diff)
            {
                DoCast(me->getVictim(), IsHeroic() ? H_SPELL_REND : SPELL_REND);
                Rend_Timer = 15000+rand()%5000;
            } else Rend_Timer -= diff;

            DoMeleeAttackIfReady();
        }
    };
};

class npc_proto_drake_handler : public CreatureScript
{
public:
    npc_proto_drake_handler() : CreatureScript("npc_proto_drake_handler") { }

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_proto_drake_handlerAI(creature);
    }

    struct npc_proto_drake_handlerAI : public ScriptedAI
    {
        npc_proto_drake_handlerAI(Creature* creature) : ScriptedAI(creature)
        {
            instance = creature->GetInstanceScript();
        }

        InstanceScript* instance;
		uint32 DebilitatingStrike_Timer;
		uint32 Throw_Timer;
		uint32 UnholyRage_Timer;

        void Reset()
        {
			DebilitatingStrike_Timer = 2000+rand()%1500;
			Throw_Timer = 2300+rand()%2700;
			UnholyRage_Timer = 15000+rand()%6000;
        }

		void UpdateAI(const uint32 diff)
        {
            if (!UpdateVictim())
                return;
			
			if (DebilitatingStrike_Timer <= diff)
            {
                DoCast(me->getVictim(), IsHeroic() ? H_SPELL_DEBILITATING_STRIKE : SPELL_DEBILITATING_STRIKE);
                DebilitatingStrike_Timer = 8000+rand()%1500;
            } else DebilitatingStrike_Timer -= diff;

			if (Throw_Timer <= diff)
            {
                DoCast(me->getVictim(), IsHeroic() ? H_SPELL_PROTO_THROW : SPELL_PROTO_THROW);
                Throw_Timer = 2300+rand()%2700;
            } else Throw_Timer -= diff;

			 if (UnholyRage_Timer)
                {
                    if (UnholyRage_Timer <= diff)
                    {
                        UnholyRage_Timer = 15000+rand()%6000;
                        if (Unit* pFriend = DoSelectLowestHpFriendly(40))
                        {
                            DoCast(pFriend, IsHeroic() ? H_SPELL_UNHOLY_RAGE : SPELL_UNHOLY_RAGE);
							UnholyRage_Timer = 15000+rand()%6000;
                        }
                    } else UnholyRage_Timer -= diff;
			 }

            DoMeleeAttackIfReady();
        }
    };
};

class npc_proto_drake_rider : public CreatureScript
{
public:
    npc_proto_drake_rider() : CreatureScript("npc_proto_drake_rider") { }

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_proto_drake_riderAI(creature);
    }

    struct npc_proto_drake_riderAI : public ScriptedAI
    {
        npc_proto_drake_riderAI(Creature* creature) : ScriptedAI(creature)
        {
            instance = creature->GetInstanceScript();
        }

        InstanceScript* instance;
		uint32 Throw_Timer;
		uint32 PiercingJab_Timer;
		uint32 WingClip_Timer;
		uint32 Movement_Timer;
		bool triggeredMovement;
		Position _current;
		bool testOnce;

        void Reset()
        {
			me->SetUnitMovementFlags(MOVEMENTFLAG_DISABLE_GRAVITY);
			Throw_Timer = 500;
			PiercingJab_Timer = 6000+rand()%2000;
			WingClip_Timer = 3000+rand()%2000;
			me->StopMoving();
			Movement_Timer = 500;
			triggeredMovement = false;
			testOnce = false;
        }

		void MoveInLineOfSight(Unit *who)
		{
			if(!who || who->GetTypeId() != TYPEID_PLAYER)
				return;
			
			if(me->GetDBTableGUIDLow() == 216584)
			{
			if(who->GetTypeId() == TYPEID_PLAYER)
			{
				if((who->GetDistance2d(me) <= 120) && (!triggeredMovement) && (!testOnce)) {
					BeginMovement(1);
					triggeredMovement = true;
					testOnce = true;
				}
			}
			}
			return;
		}

		void BeginMovement(int step)
		{
			me->SetUInt32Value(UNIT_NPC_EMOTESTATE, 444);
			if(step == 1)
			{
			me->GetMotionMaster()->MovePoint(0, MovePos[0]);
			}

			if(step == 2)
			{
			me->GetMotionMaster()->MovePoint(1, MovePos[1]);
			}

			if(step == 3)
			{
			me->GetMotionMaster()->MovePoint(2, MovePos[1]);
			Movement_Timer = 0;
			triggeredMovement = false;
			//me->StopMoving();
			}
		}

        void JustDied(Unit* /*killer*/)
        {
        }

        void EnterCombat(Unit* /*who*/)
        {
		}

        void UpdateAI(const uint32 diff)
        {
			if(triggeredMovement)
			{
				if (Movement_Timer <= diff)
				{
					me->GetPosition(&_current);

					if (_current.GetPositionX() == MovePos[0].GetPositionX() && _current.GetPositionY() == MovePos[0].GetPositionY())
					BeginMovement(3);

					//if (_current.GetPositionX() == MovePos[1].GetPositionX() && _current.GetPositionY() == MovePos[1].GetPositionY())
					//BeginMovement(3);
				} else Movement_Timer -= diff;
			}

            if (!UpdateVictim())
                return;

			if (Throw_Timer <= diff)
            {
                DoCast(me->getVictim(), IsHeroic() ? H_SPELL_THROW : SPELL_THROW, true);
                Throw_Timer = 17000+rand()%2000;
            } else Throw_Timer -= diff;

			if (PiercingJab_Timer <= diff)
            {
                DoCast(me->getVictim(), IsHeroic() ? H_SPELL_PIERCING_JAB : SPELL_PIERCING_JAB);
                PiercingJab_Timer = 11000+rand()%6000;
            } else PiercingJab_Timer -= diff;

			if (WingClip_Timer <= diff)
            {
                DoCast(me->getVictim(), IsHeroic() ? H_SPELL_WING_CLIP : SPELL_WING_CLIP);
                WingClip_Timer = 9000+rand()%3500;
            } else WingClip_Timer -= diff;

            DoMeleeAttackIfReady();
        }
    };
};


void AddSC_utgarde_keep()
{
    new npc_dragonflayer_forge_master();
    new spell_ticking_time_bomb();
    new spell_fixate();
	new npc_dragonflayer_ironhelm();
	new npc_savage_worg();
	new npc_tunneling_ghoul();
	new npc_dark_ranger_marrah();
	new npc_dragonflayer_bonecrusher();
	new npc_dragonflayer_heartsplitter();
	new npc_dragonflayer_metalworker();
	new npc_dragonflayer_overseer();
	new npc_dragonflayer_weaponsmith();
	new npc_dragonflayer_runecaster();
	new npc_dragonflayer_strategist();
	new npc_frenzied_geist();
	new npc_enslaved_proto_drake();
	new npc_proto_drake_handler();
	new npc_proto_drake_rider();
}
