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
#include "gundrak.h"

//Spells
enum Spells
{
    SPELL_ENRAGE                                  = 55285,
    H_SPELL_ENRAGE                                = 59828,
    SPELL_IMPALING_CHARGE                         = 54956,
    H_SPELL_IMPALING_CHARGE                       = 59827,
    SPELL_STOMP                                   = 55292,
    H_SPELL_STOMP                                 = 59829,
    SPELL_PUNCTURE                                = 55276,
    H_SPELL_PUNCTURE                              = 59826,
    SPELL_STAMPEDE                                = 55218,
    SPELL_WHIRLING_SLASH                          = 55250,
    H_SPELL_WHIRLING_SLASH                        = 59824,
    SPELL_IMPALING_CHARGE_VEHICLE                 = 54958,
    SPELL_ECK_RESIDUE                             = 55817,
    //rhino spirit spells
    SPELL_STAMPEDE_DMG                            = 55220,
    H_SPELL_STAMPEDE_DMG                          = 59823
};

//Yells
enum Yells
{
    SAY_AGGRO                                     = -1604000,
    SAY_SLAY_1                                    = -1604001,
    SAY_SLAY_2                                    = -1604002,
    SAY_SLAY_3                                    = -1604003,
    SAY_DEATH                                     = -1604004,
    SAY_SUMMON_RHINO_1                            = -1604005,
    SAY_SUMMON_RHINO_2                            = -1604006,
    SAY_SUMMON_RHINO_3                            = -1604007,
    SAY_TRANSFORM_1                               = -1604008,  //Phase change
    SAY_TRANSFORM_2                               = -1604009
};

enum Achievements
{
    ACHIEV_WHAT_THE_ECK                           = 1864,
    ACHIEV_SHARE_THE_LOVE                         = 2152
};

enum Displays
{
    DISPLAY_RHINO                                 = 26265,
    DISPLAY_TROLL                                 = 27061
};

enum CombatPhase
{
    TROLL,
    RHINO
};

#define DATA_SHARE_THE_LOVE                       1

class boss_gal_darah : public CreatureScript
{
public:
    boss_gal_darah() : CreatureScript("boss_gal_darah") { }

    CreatureAI* GetAI(Creature* creature) const
    {
        return new boss_gal_darahAI (creature);
    }

    struct boss_gal_darahAI : public ScriptedAI
    {
        boss_gal_darahAI(Creature* creature) : ScriptedAI(creature)
        {
            instance = creature->GetInstanceScript();
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_INTERRUPT, true);
			me->ApplySpellImmune(1, IMMUNITY_MECHANIC, MECHANIC_STUN, true);
        }

        uint32 uiStampedeTimer;
        uint32 uiWhirlingSlashTimer;
        uint32 uiPunctureTimer;
        uint32 uiEnrageTimer;
        uint32 uiImpalingChargeTimer;
        uint32 uiStompTimer;
        uint32 uiTransformationTimer;
        std::list<uint64> impaledList;
        uint8 shareTheLove;

        CombatPhase Phase;

        uint8 uiPhaseCounter;

        bool bStartOfTransformation;
        bool bTransformed;

        InstanceScript* instance;

        void Reset()
        {
            uiStampedeTimer = 9*IN_MILLISECONDS;
            uiWhirlingSlashTimer = 21*IN_MILLISECONDS;
            uiPunctureTimer = 5*IN_MILLISECONDS;
            uiEnrageTimer = 15*IN_MILLISECONDS;
            uiImpalingChargeTimer = 21*IN_MILLISECONDS;
            uiStompTimer = 10*IN_MILLISECONDS;
            uiTransformationTimer = 9*IN_MILLISECONDS;
            uiPhaseCounter = 0;

            impaledList.clear();
            shareTheLove = 0;

            bStartOfTransformation = true;
            bTransformed = false;

            Phase = TROLL;

            me->SetDisplayId(DISPLAY_TROLL);

            if (instance)
                instance->SetData(DATA_GAL_DARAH_EVENT, NOT_STARTED);
        }

        void EnterCombat(Unit* /*who*/)
        {
            DoScriptText(SAY_AGGRO, me);

            if (instance)
                instance->SetData(DATA_GAL_DARAH_EVENT, IN_PROGRESS);
        }

        void UpdateAI(const uint32 diff)
        {
            if (!UpdateVictim())
                return;

            if(!bTransformed && HealthBelowPct(50)) //transform at least once at 50% health
            {
                bTransformed = true;
                uiPhaseCounter = 2;
            }

            switch (Phase)
            {
                case TROLL:
                    if (uiPhaseCounter == 2)
                    {
                        if (uiTransformationTimer <= diff)
                        {
                            me->SetDisplayId(DISPLAY_RHINO);
                            Phase = RHINO;
                            uiPhaseCounter = 0;
                            DoScriptText(SAY_TRANSFORM_1, me);
                            uiTransformationTimer = 5*IN_MILLISECONDS;
                            bStartOfTransformation = true;
                            bTransformed = true;
                            me->ClearUnitState(UNIT_STATE_STUNNED|UNIT_STATE_ROOT);
                            me->SetReactState(REACT_AGGRESSIVE);
                        }
                        else
                        {
                            uiTransformationTimer -= diff;

                            if (bStartOfTransformation)
                            {
                                bStartOfTransformation = false;
                                me->AddUnitState(UNIT_STATE_STUNNED|UNIT_STATE_ROOT);
                                me->SetReactState(REACT_PASSIVE);
                            }
                        }
                    }
                    else
                    {
                        if (uiStampedeTimer <= diff)
                        {
                            DoCast(me->getVictim(), SPELL_STAMPEDE);
                            DoScriptText(RAND(SAY_SUMMON_RHINO_1, SAY_SUMMON_RHINO_2, SAY_SUMMON_RHINO_3), me);
                            uiStampedeTimer = urand(10*IN_MILLISECONDS, 15*IN_MILLISECONDS);
                        } else uiStampedeTimer -= diff;

                        if (uiWhirlingSlashTimer <= diff)
                        {
                            DoCast(me->getVictim(), DUNGEON_MODE(SPELL_WHIRLING_SLASH, H_SPELL_WHIRLING_SLASH));
                            uiWhirlingSlashTimer = urand(18*IN_MILLISECONDS, 22*IN_MILLISECONDS);;
                            ++uiPhaseCounter;
                        } else uiWhirlingSlashTimer -= diff;
                    }
                break;
                case RHINO:
                    if (uiPhaseCounter == 2)
                    {
                        if (uiTransformationTimer <= diff)
                        {
                            me->SetDisplayId(DISPLAY_TROLL);
                            Phase = TROLL;
                            uiPhaseCounter = 0;
                            DoScriptText(SAY_TRANSFORM_2, me);
                            uiTransformationTimer = 9*IN_MILLISECONDS;
                            bStartOfTransformation = true;
                            me->ClearUnitState(UNIT_STATE_STUNNED|UNIT_STATE_ROOT);
                            me->SetReactState(REACT_AGGRESSIVE);
                        }
                        else
                        {
                            uiTransformationTimer -= diff;

                            if (bStartOfTransformation)
                            {
                                bStartOfTransformation = false;
                                me->AddUnitState(UNIT_STATE_STUNNED|UNIT_STATE_ROOT);
                                me->SetReactState(REACT_PASSIVE);
                            }
                        }
                    }
                    else
                    {
                        if (uiPunctureTimer <= diff)
                        {
                            DoCast(me->getVictim(), DUNGEON_MODE(SPELL_PUNCTURE, H_SPELL_PUNCTURE));
                            uiPunctureTimer = 15*IN_MILLISECONDS;
                        } else uiPunctureTimer -= diff;

                        if (uiEnrageTimer <= diff)
                        {
                            DoCast(me->getVictim(), DUNGEON_MODE(SPELL_ENRAGE, H_SPELL_ENRAGE));
                            uiEnrageTimer = 20*IN_MILLISECONDS;
                        } else uiEnrageTimer -= diff;

                        if (uiStompTimer <= diff)
                        {
                            DoCast(me->getVictim(), DUNGEON_MODE(SPELL_STOMP, H_SPELL_STOMP));
                            uiStompTimer = urand(10*IN_MILLISECONDS, 15*IN_MILLISECONDS);
                        } else uiStompTimer -= diff;

                        if (uiImpalingChargeTimer <= diff)
                        {
                            if (Unit* target = SelectTarget(SELECT_TARGET_RANDOM, 0, 100, true))
                            {
                                DoCast(target, DUNGEON_MODE(SPELL_IMPALING_CHARGE, H_SPELL_IMPALING_CHARGE));
                                target->CastSpell(me, SPELL_IMPALING_CHARGE_VEHICLE, true);  // needs vehicle id and take dmg while seated
                                //impaledList.insert(target->GetGUID());
								CheckAchievement(target->GetGUID());
                            }
                            uiImpalingChargeTimer = 20*IN_MILLISECONDS;
                            ++uiPhaseCounter;
                        } else uiImpalingChargeTimer -= diff;
                    }
                break;
            }

            DoMeleeAttackIfReady();
        }

        // 5 UNIQUE party members
        void CheckAchievement(uint64 guid)
        {
            bool playerExists = false;
            for (std::list<uint64>::iterator itr = impaledList.begin(); itr != impaledList.end(); ++itr)
                if (guid != *itr)
                    playerExists = true;

            if (playerExists)
                ++shareTheLove;

            impaledList.push_back(guid);
        }

        uint32 GetData(uint32 type)
        {
            if (type == DATA_SHARE_THE_LOVE)
                return shareTheLove;

            return 0;
        }

        void JustDied(Unit* /*killer*/)
        {
            DoScriptText(SAY_DEATH, me);

            if (instance)
			{
                /*if(IsHeroic())
                {
                    AchievementEntry const *achievWhatTheEck = GetAchievementStore()->LookupEntry(ACHIEV_WHAT_THE_ECK);
                    if(achievWhatTheEck)
                    {
                        Map::PlayerList const &players = instance->instance->GetPlayers();
                        for(Map::PlayerList::const_iterator itr = players.begin(); itr != players.end(); ++itr)
                            if(itr->getSource()->HasAura(SPELL_ECK_RESIDUE))
                                itr->getSource()->CompletedAchievement(achievWhatTheEck);
                    }
                }*/
                instance->SetData(DATA_GAL_DARAH_EVENT, DONE);
			}
        }

        void JustSummoned(Creature* summon)
        {
			summon->SetInCombatWithZone();
        }

        void KilledUnit(Unit* victim)
        {
            if (victim == me)
                return;

            DoScriptText(RAND(SAY_SLAY_1, SAY_SLAY_2, SAY_SLAY_3), me);
        }
    };

};

class mob_rhino_spirit : public CreatureScript
{
public:
    mob_rhino_spirit() : CreatureScript("mob_rhino_spirit") { }

    CreatureAI* GetAI(Creature* pCreature) const
    {
        return new mob_rhino_spiritAI (pCreature);
    }

    struct mob_rhino_spiritAI : public ScriptedAI
    {
        mob_rhino_spiritAI(Creature* pCreature) : ScriptedAI(pCreature) { }

        uint32 uiDespawnTimer;
		bool doOnce;

        void Reset()
        {
            uiDespawnTimer = 1500;
			doOnce = false;
        }

        void UpdateAI(const uint32 diff)
        {
            if(!UpdateVictim())
               return;

			if(!doOnce)
				if (Unit* target = SelectTarget(SELECT_TARGET_RANDOM, 0))
				{
					me->CastSpell(target, DUNGEON_MODE(SPELL_STAMPEDE_DMG, H_SPELL_STAMPEDE_DMG));
					doOnce = true;
				}

            if(uiDespawnTimer <= diff)
            {
				me->DisappearAndDie();
            } else uiDespawnTimer -= diff;
        }
    };
};

class achievement_share_the_love : public AchievementCriteriaScript
{
    public:
        achievement_share_the_love() : AchievementCriteriaScript("achievement_share_the_love")
        {
        }

        bool OnCheck(Player* /*player*/, Unit* target)
        {
            if (!target)
                return false;

            if (Creature* GalDarah = target->ToCreature())
                if (GalDarah->AI()->GetData(DATA_SHARE_THE_LOVE) >= 5)
                    return true;

            return false;
        }
};

void AddSC_boss_gal_darah()
{
    new boss_gal_darah();
    new mob_rhino_spirit();
    new achievement_share_the_love();
}
