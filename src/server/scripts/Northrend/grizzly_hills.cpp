/*
 * Copyright (C) 2008-2013 TrinityCore <http://www.trinitycore.org/>
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
#include "ScriptedCreature.h"
#include "ScriptedEscortAI.h"

/*######
## Quest 12027: Mr. Floppy's Perilous Adventure
######*/

enum eFloppy
{
    NPC_MRFLOPPY                = 26589,
    NPC_HUNGRY_WORG             = 26586,
    NPC_RAVENOUS_WORG           = 26590,   //RWORG
    NPC_EMILY                   = 26588,

    QUEST_PERILOUS_ADVENTURE    = 12027,

    SPELL_MRFLOPPY              = 47184,    //vehicle aura

    SAY_WORGHAGGRO1             = -1800001, //Um... I think one of those wolves is back...
    SAY_WORGHAGGRO2             = -1800002, //He's going for Mr. Floppy!
    SAY_WORGRAGGRO3             = -1800003, //Oh, no! Look, it's another wolf, and it's a biiiiiig one!
    SAY_WORGRAGGRO4             = -1800004, //He's gonna eat Mr. Floppy! You gotta help Mr. Floppy! You just gotta!
    SAY_RANDOMAGGRO             = -1800005, //There's a big meanie attacking Mr. Floppy! Help!
    SAY_VICTORY1                = -1800006, //Let's get out of here before more wolves find us!
    SAY_VICTORY2                = -1800007, //Don't go toward the light, Mr. Floppy!
    SAY_VICTORY3                = -1800008, //Mr. Floppy, you're ok! Thank you so much for saving Mr. Floppy!
    SAY_VICTORY4                = -1800009, //I think I see the camp! We're almost home, Mr. Floppy! Let's go!
    TEXT_EMOTE_WP1              = -1800010, //Mr. Floppy revives
    TEXT_EMOTE_AGGRO            = -1800011, //The Ravenous Worg chomps down on Mr. Floppy
    SAY_QUEST_ACCEPT            = -1800012, //Are you ready, Mr. Floppy? Stay close to me and watch out for those wolves!
    SAY_QUEST_COMPLETE          = -1800013  //Thank you for helping me get back to the camp. Go tell Walter that I'm safe now!
};

//emily
class npc_emily : public CreatureScript
{
public:
    npc_emily() : CreatureScript("npc_emily") { }

    struct npc_emilyAI : public npc_escortAI
    {
        npc_emilyAI(Creature* creature) : npc_escortAI(creature) { }

        uint32 m_uiChatTimer;

        uint64 RWORGGUID;
        uint64 MrfloppyGUID;

        bool Completed;

        void JustSummoned(Creature* summoned)
        {
            if (Creature* Mrfloppy = GetClosestCreatureWithEntry(me, NPC_MRFLOPPY, 50.0f))
                summoned->AI()->AttackStart(Mrfloppy);
            else
                summoned->AI()->AttackStart(me->getVictim());
        }

        void WaypointReached(uint32 waypointId)
        {
            Player* player = GetPlayerForEscort();
            if (!player)
                return;

            switch (waypointId)
            {
                case 9:
                    if (Creature* Mrfloppy = GetClosestCreatureWithEntry(me, NPC_MRFLOPPY, 100.0f))
                        MrfloppyGUID = Mrfloppy->GetGUID();
                    break;
                case 10:
                    if (Unit::GetCreature(*me, MrfloppyGUID))
                    {
                        DoScriptText(SAY_WORGHAGGRO1, me);
                        me->SummonCreature(NPC_HUNGRY_WORG, me->GetPositionX()+5, me->GetPositionY()+2, me->GetPositionZ()+1, 3.229f, TEMPSUMMON_TIMED_OR_DEAD_DESPAWN, 120000);
                    }
                    break;
                case 11:
                    if (Creature* Mrfloppy = Unit::GetCreature(*me, MrfloppyGUID))
                        Mrfloppy->GetMotionMaster()->MoveFollow(me, PET_FOLLOW_DIST, PET_FOLLOW_ANGLE);
                    break;
                case 17:
                    if (Creature* Mrfloppy = Unit::GetCreature(*me, MrfloppyGUID))
                        Mrfloppy->GetMotionMaster()->MovePoint(0, me->GetPositionX(), me->GetPositionY(), me->GetPositionZ());
                    DoScriptText(SAY_WORGRAGGRO3, me);
                    if (Creature* RWORG = me->SummonCreature(NPC_RAVENOUS_WORG, me->GetPositionX()+10, me->GetPositionY()+8, me->GetPositionZ()+2, 3.229f, TEMPSUMMON_TIMED_OR_DEAD_DESPAWN, 120000))
                    {
                        RWORG->setFaction(35);
                        RWORGGUID = RWORG->GetGUID();
                    }
                    break;
                case 18:
                    if (Creature* Mrfloppy = Unit::GetCreature(*me, MrfloppyGUID))
                    {
                        if (Creature* RWORG = Unit::GetCreature(*me, RWORGGUID))
                            RWORG->GetMotionMaster()->MovePoint(0, Mrfloppy->GetPositionX(), Mrfloppy->GetPositionY(), Mrfloppy->GetPositionZ());
                        DoCast(Mrfloppy, SPELL_MRFLOPPY);
                    }
                    break;
                case 19:
                    if (Creature* Mrfloppy = Unit::GetCreature(*me, MrfloppyGUID))
                    {
                        if (Mrfloppy->HasAura(SPELL_MRFLOPPY, 0))
                        {
                            if (Creature* RWORG = Unit::GetCreature(*me, RWORGGUID))
                                Mrfloppy->EnterVehicle(RWORG);
                        }
                    }
                    break;
                case 20:
                    if (Creature* RWORG = Unit::GetCreature(*me, RWORGGUID))
                        RWORG->HandleEmoteCommand(34);
                    break;
                case 21:
                    if (Creature* Mrfloppy = Unit::GetCreature(*me, MrfloppyGUID))
                    {
                        if (Creature* RWORG = Unit::GetCreature(*me, RWORGGUID))
                        {
                            RWORG->Kill(Mrfloppy);
                            Mrfloppy->ExitVehicle();
                            RWORG->setFaction(14);
                            RWORG->GetMotionMaster()->MovePoint(0, RWORG->GetPositionX()+10, RWORG->GetPositionY()+80, RWORG->GetPositionZ());
                            DoScriptText(SAY_VICTORY2, me);
                        }
                    }
                    break;
                case 22:
                    if (Creature* Mrfloppy = Unit::GetCreature(*me, MrfloppyGUID))
                    {
                        if (Mrfloppy->isDead())
                        {
                            if (Creature* RWORG = Unit::GetCreature(*me, RWORGGUID))
                                RWORG->DisappearAndDie();
                            me->GetMotionMaster()->MovePoint(0, Mrfloppy->GetPositionX(), Mrfloppy->GetPositionY(), Mrfloppy->GetPositionZ());
                            Mrfloppy->setDeathState(ALIVE);
                            Mrfloppy->GetMotionMaster()->MoveFollow(me, PET_FOLLOW_DIST, PET_FOLLOW_ANGLE);
                            DoScriptText(SAY_VICTORY3, me);
                        }
                    }
                    break;
                case 24:
                    if (player)
                    {
                        Completed = true;
                        player->GroupEventHappens(QUEST_PERILOUS_ADVENTURE, me);
                        DoScriptText(SAY_QUEST_COMPLETE, me, player);
                    }
                    me->SetWalk(false);
                    break;
                case 25:
                    DoScriptText(SAY_VICTORY4, me);
                    break;
                case 27:
                    me->DisappearAndDie();
                    if (Creature* Mrfloppy = Unit::GetCreature(*me, MrfloppyGUID))
                        Mrfloppy->DisappearAndDie();
                    break;
            }
        }

        void EnterCombat(Unit* /*Who*/)
        {
            DoScriptText(SAY_RANDOMAGGRO, me);
        }

        void Reset()
        {
            m_uiChatTimer = 4000;
            MrfloppyGUID = 0;
            RWORGGUID = 0;
        }

        void UpdateAI(const uint32 uiDiff)
        {
            npc_escortAI::UpdateAI(uiDiff);

            if (HasEscortState(STATE_ESCORT_ESCORTING))
            {
                if (m_uiChatTimer <= uiDiff)
                    m_uiChatTimer = 12000;
                else
                    m_uiChatTimer -= uiDiff;
            }
        }
    };

    bool OnQuestAccept(Player* player, Creature* creature, Quest const* quest)
    {
        if (quest->GetQuestId() == QUEST_PERILOUS_ADVENTURE)
        {
            DoScriptText(SAY_QUEST_ACCEPT, creature);
            if (Creature* Mrfloppy = GetClosestCreatureWithEntry(creature, NPC_MRFLOPPY, 180.0f))
                Mrfloppy->GetMotionMaster()->MoveFollow(creature, PET_FOLLOW_DIST, PET_FOLLOW_ANGLE);

            if (npc_escortAI* pEscortAI = CAST_AI(npc_emily::npc_emilyAI, (creature->AI())))
                pEscortAI->Start(true, false, player->GetGUID());
        }
        return true;
    }

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_emilyAI(creature);
    }
};

//mrfloppy
class npc_mrfloppy : public CreatureScript
{
public:
    npc_mrfloppy() : CreatureScript("npc_mrfloppy") { }

    struct npc_mrfloppyAI : public ScriptedAI
    {
        npc_mrfloppyAI(Creature* creature) : ScriptedAI(creature) {}

        uint64 EmilyGUID;
        uint64 RWORGGUID;
        uint64 HWORGGUID;

        void Reset() {}

        void EnterCombat(Unit* Who)
        {
            if (Creature* Emily = GetClosestCreatureWithEntry(me, NPC_EMILY, 50.0f))
            {
                switch (Who->GetEntry())
                {
                    case NPC_HUNGRY_WORG:
                        DoScriptText(SAY_WORGHAGGRO2, Emily);
                        break;
                    case NPC_RAVENOUS_WORG:
                        DoScriptText(SAY_WORGRAGGRO4, Emily);
                        break;
                    default:
                        DoScriptText(SAY_RANDOMAGGRO, Emily);
                }
            }
        }

        void EnterEvadeMode() {}

        void MoveInLineOfSight(Unit* /*who*/) {}

        void UpdateAI(const uint32 /*diff*/)
        {
            if (!UpdateVictim())
                return;
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_mrfloppyAI(creature);
    }
};

// Outhouse Bunny

enum eOuthouseBunny
{
    SPELL_OUTHOUSE_GROANS           = 48382,
    SPELL_CAMERA_SHAKE              = 47533,
    SPELL_DUST_FIELD                = 48329
};

enum eSounds
{
    SOUND_FEMALE        = 12671,
    SOUND_MALE          = 12670
};

class npc_outhouse_bunny : public CreatureScript
{
public:
    npc_outhouse_bunny() : CreatureScript("npc_outhouse_bunny") { }

    struct npc_outhouse_bunnyAI : public ScriptedAI
    {
        npc_outhouse_bunnyAI(Creature* creature) : ScriptedAI(creature) {}

        uint8 m_counter;
        uint8 m_gender;

        void Reset()
        {
            m_counter = 0;
            m_gender = 0;
        }

        void SetData(uint32 uiType, uint32 uiData)
        {
            if (uiType == 1)
                m_gender = uiData;
        }

        void SpellHit(Unit* pCaster, const SpellInfo* pSpell)
        {
             if (pSpell->Id == SPELL_OUTHOUSE_GROANS)
             {
                ++m_counter;
                if (m_counter < 5)
                    DoCast(pCaster, SPELL_CAMERA_SHAKE, true);
                else
                    m_counter = 0;
                DoCast(me, SPELL_DUST_FIELD, true);
                switch (m_gender)
                {
                    case GENDER_FEMALE:
                        DoPlaySoundToSet(me, SOUND_FEMALE);
                        break;

                    case GENDER_MALE:
                        DoPlaySoundToSet(me, SOUND_MALE);
                        break;
                }
            }
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_outhouse_bunnyAI(creature);
    }
};

// Tallhorn Stage

enum etallhornstage
{
    OBJECT_HAUNCH                   = 188665
};

class npc_tallhorn_stag : public CreatureScript
{
public:
    npc_tallhorn_stag() : CreatureScript("npc_tallhorn_stag") { }

    struct npc_tallhorn_stagAI : public ScriptedAI
    {
        npc_tallhorn_stagAI(Creature* creature) : ScriptedAI(creature) {}

        uint8 m_uiPhase;

        void Reset()
        {
            m_uiPhase = 1;
        }

        void UpdateAI(const uint32 /*uiDiff*/)
        {
            if (m_uiPhase == 1)
            {
                if (me->FindNearestGameObject(OBJECT_HAUNCH, 2.0f))
                {
                    me->SetStandState(UNIT_STAND_STATE_DEAD);
                    me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_IMMUNE_TO_PC);
                    me->SetUInt32Value(UNIT_DYNAMIC_FLAGS, UNIT_DYNFLAG_DEAD);
                }
                m_uiPhase = 0;
            }
            DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_tallhorn_stagAI(creature);
    }
};

// Amberpine Woodsman

enum eamberpinewoodsman
{
    TALLHORN_STAG                   = 26363
};

class npc_amberpine_woodsman : public CreatureScript
{
public:
    npc_amberpine_woodsman() : CreatureScript("npc_amberpine_woodsman") { }

    struct npc_amberpine_woodsmanAI : public ScriptedAI
    {
        npc_amberpine_woodsmanAI(Creature* creature) : ScriptedAI(creature) {}

        uint8 m_uiPhase;
        uint32 m_uiTimer;

        void Reset()
        {
            m_uiTimer = 0;
            m_uiPhase = 1;
        }

        void UpdateAI(const uint32 uiDiff)
        {
            // call this each update tick?
            if (me->FindNearestCreature(TALLHORN_STAG, 0.2f))
            {
                me->SetUInt32Value(UNIT_NPC_EMOTESTATE, EMOTE_STATE_USE_STANDING);
            }
            else
                if (m_uiPhase)
                {
                    if (m_uiTimer <= uiDiff)
                    {
                        switch (m_uiPhase)
                        {
                            case 1:
                                me->SetUInt32Value(UNIT_NPC_EMOTESTATE, EMOTE_STATE_LOOT);
                                m_uiTimer = 3000;
                                m_uiPhase = 2;
                                break;
                            case 2:
                                me->SetUInt32Value(UNIT_NPC_EMOTESTATE, EMOTE_ONESHOT_ATTACK1H);
                                m_uiTimer = 4000;
                                m_uiPhase = 1;
                                break;
                        }
                    }
                    else
                    m_uiTimer -= uiDiff;
                }
                ScriptedAI::UpdateAI(uiDiff);

            UpdateVictim();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_amberpine_woodsmanAI(creature);
    }
};
/*######
## Quest 12288: Overwhelmed!
######*/

enum eSkirmisher
{
    SPELL_RENEW_SKIRMISHER  = 48812,
    CREDIT_NPC              = 27466,

    RANDOM_SAY_1             =  -1800044,        //Ahh..better..
    RANDOM_SAY_2             =  -1800045,        //Whoa.. i nearly died there. Thank you, $Race!
    RANDOM_SAY_3             =  -1800046         //Thank you. $Class!
};

class npc_wounded_skirmisher : public CreatureScript
{
public:
    npc_wounded_skirmisher() : CreatureScript("npc_wounded_skirmisher") { }

    struct npc_wounded_skirmisherAI : public ScriptedAI
    {
        npc_wounded_skirmisherAI(Creature* creature) : ScriptedAI(creature) {}

        uint64 uiPlayerGUID;

        uint32 DespawnTimer;

        void Reset()
        {
            DespawnTimer = 5000;
            uiPlayerGUID = 0;
        }

        void MovementInform(uint32, uint32 id)
        {
            if (id == 1)
                me->DespawnOrUnsummon(DespawnTimer);
        }

        void SpellHit(Unit* caster, const SpellInfo* spell)
        {
            if (spell->Id == SPELL_RENEW_SKIRMISHER && caster->GetTypeId() == TYPEID_PLAYER
                && caster->ToPlayer()->GetQuestStatus(12288) == QUEST_STATUS_INCOMPLETE)
            {
                caster->ToPlayer()->KilledMonsterCredit(CREDIT_NPC, 0);
                DoScriptText(RAND(RANDOM_SAY_1, RANDOM_SAY_2, RANDOM_SAY_3), caster);
                if (me->IsStandState())
                    me->GetMotionMaster()->MovePoint(1, me->GetPositionX()+7, me->GetPositionY()+7, me->GetPositionZ());
                else
                {
                    me->SetStandState(UNIT_STAND_STATE_STAND);
                    me->DespawnOrUnsummon(DespawnTimer);
                }
            }
        }

        void UpdateAI(const uint32 /*diff*/)
        {
            if (!UpdateVictim())
                return;

            DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_wounded_skirmisherAI(creature);
    }
};

/*Lightning Sentry - if you kill it when you have your Minion with you, you will get a quest credit*/
enum eSentry
{
    QUEST_OR_MAYBE_WE_DONT_A                     = 12138,
    QUEST_OR_MAYBE_WE_DONT_H                     = 12198,

    NPC_LIGHTNING_SENTRY                         = 26407,
    NPC_WAR_GOLEM                                = 27017,

    SPELL_CHARGED_SENTRY_TOTEM                   = 52703,
};

class npc_lightning_sentry : public CreatureScript
{
public:
    npc_lightning_sentry() : CreatureScript("npc_lightning_sentry") { }

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_lightning_sentryAI(creature);
    }

    struct npc_lightning_sentryAI : public ScriptedAI
    {
        npc_lightning_sentryAI(Creature* creature) : ScriptedAI(creature) { }

        uint32 uiChargedSentryTotem;

        void Reset()
        {
            uiChargedSentryTotem = urand(10000, 12000);
        }

        void UpdateAI(const uint32 uiDiff)
        {
            if (!UpdateVictim())
                return;

            if (uiChargedSentryTotem <= uiDiff)
            {
                DoCast(SPELL_CHARGED_SENTRY_TOTEM);
                uiChargedSentryTotem = urand(10000, 12000);
            }
            else
                uiChargedSentryTotem -= uiDiff;

            DoMeleeAttackIfReady();
        }

        void JustDied(Unit* killer)
        {
            if (killer->ToPlayer() && killer->ToPlayer()->GetTypeId() == TYPEID_PLAYER)
            {
                if (me->FindNearestCreature(NPC_WAR_GOLEM, 10.0f, true))
                {
                    if (killer->ToPlayer()->GetQuestStatus(QUEST_OR_MAYBE_WE_DONT_A) == QUEST_STATUS_INCOMPLETE ||
                        killer->ToPlayer()->GetQuestStatus(QUEST_OR_MAYBE_WE_DONT_H) == QUEST_STATUS_INCOMPLETE)
                        killer->ToPlayer()->KilledMonsterCredit(NPC_WAR_GOLEM, 0);
                }
            }
        }
    };
};

/*Venture co. Straggler - when you cast Smoke Bomb, he will yell and run away*/
enum eSmokeEmOut
{
    SAY_SEO1                                     = -1603535,
    SAY_SEO2                                     = -1603536,
    SAY_SEO3                                     = -1603537,
    SAY_SEO4                                     = -1603538,
    SAY_SEO5                                     = -1603539,
    QUEST_SMOKE_EM_OUT_A                         = 12323,
    QUEST_SMOKE_EM_OUT_H                         = 12324,
    SPELL_SMOKE_BOMB                             = 49075,
    SPELL_CHOP                                   = 43410,
    SPELL_VENTURE_STRAGGLER_CREDIT               = 49093,
};

class npc_venture_co_straggler : public CreatureScript
{
    public:
        npc_venture_co_straggler() : CreatureScript("npc_venture_co_straggler") { }

        struct npc_venture_co_stragglerAI : public ScriptedAI
        {
            npc_venture_co_stragglerAI(Creature* creature) : ScriptedAI(creature) { }

            uint64 uiPlayerGUID;
            uint32 uiRunAwayTimer;
            uint32 uiTimer;
            uint32 uiChopTimer;

            void Reset()
            {
                uiPlayerGUID = 0;
                uiTimer = 0;
                uiRunAwayTimer = 0;
                uiChopTimer = urand(10000, 12500);
                me->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE | UNIT_FLAG_IMMUNE_TO_PC);
                me->SetReactState(REACT_AGGRESSIVE);
            }

            void UpdateAI(const uint32 uiDiff)
            {
                if (uiPlayerGUID && uiRunAwayTimer <= uiDiff)
                {
                    if (Player* player = Unit::GetPlayer(*me, uiPlayerGUID))
                    {
                        switch (uiTimer)
                        {
                            case 0:
                                DoCast(player, SPELL_VENTURE_STRAGGLER_CREDIT);
                                me->GetMotionMaster()->MovePoint(0, me->GetPositionX()-7, me->GetPositionY()+7, me->GetPositionZ());
                                uiRunAwayTimer = 2500;
                                ++uiTimer;
                                break;
                            case 1:
                                DoScriptText(RAND(SAY_SEO1, SAY_SEO2, SAY_SEO3, SAY_SEO4, SAY_SEO5), me);
                                me->GetMotionMaster()->MovePoint(0, me->GetPositionX()-7, me->GetPositionY()-5, me->GetPositionZ());
                                uiRunAwayTimer = 2500;
                                ++uiTimer;
                                break;
                            case 2:
                                me->GetMotionMaster()->MovePoint(0, me->GetPositionX()-5, me->GetPositionY()-5, me->GetPositionZ());
                                uiRunAwayTimer = 2500;
                                ++uiTimer;
                                break;
                            case 3:
                                me->DisappearAndDie();
                                uiTimer = 0;
                                break;
                        }
                    }
                }
                else if (uiRunAwayTimer)
                    uiRunAwayTimer -= uiDiff;

                if (!UpdateVictim())
                    return;

                if (uiChopTimer <= uiDiff)
                {
                    DoCast(me->getVictim(), SPELL_CHOP);
                    uiChopTimer = urand(10000, 12000);
                }
                else
                    uiChopTimer -= uiDiff;

                DoMeleeAttackIfReady();
            }

            void SpellHit(Unit* caster, SpellInfo const* spell)
            {
                if (spell->Id == SPELL_SMOKE_BOMB && caster->GetTypeId() == TYPEID_PLAYER)
                {
                    me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE | UNIT_FLAG_IMMUNE_TO_PC);
                    me->SetReactState(REACT_PASSIVE);
                    me->CombatStop(false);
                    uiPlayerGUID = caster->GetGUID();
                    uiRunAwayTimer = 3500;
                }
            }
        };

        CreatureAI* GetAI(Creature* creature) const
        {
            return new npc_venture_co_stragglerAI(creature);
        }
};
enum DrakuruElixirQuestChain
{
    ITEM_WORG_DISGUISE              = 33618,
    ITEM_DRAKURU_ELIXIR             = 35797,

    NPC_KILL_CREDITS                = 26444,

    AURA_BLEEDING_HAND              = 50141,

    QUEST_TRUCE                     = 11989,
    QUEST_SUBJECT_TO_INTERPRETATION = 11991,
};

class npc_drakuru : public CreatureScript
{
    public:
        npc_drakuru() : CreatureScript("npc_drakuru") { }

        bool OnGossipHello(Player* player, Creature* creature)
        {
            if (player->GetQuestStatus(QUEST_TRUCE) == QUEST_STATUS_INCOMPLETE)
                if (player->HasAura(AURA_BLEEDING_HAND))
                    player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, "Shake Drakuru's outstretched hand.", GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 1);

            if (!player->HasItemCount(ITEM_DRAKURU_ELIXIR, 1, true))
                if (player->GetQuestStatus(QUEST_SUBJECT_TO_INTERPRETATION) == QUEST_STATUS_REWARDED || player->GetQuestStatus(QUEST_SUBJECT_TO_INTERPRETATION) == QUEST_STATUS_INCOMPLETE)
                    player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, "I've lost my Drakuru elixir, can you give me another one?.", GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 2);

            player->SEND_GOSSIP_MENU(player->GetGossipTextId(creature), creature->GetGUID());
            return true;
        }

        bool OnGossipSelect(Player* player, Creature* creature, uint32 /*sender*/, uint32 action)
        {
            player->PlayerTalkClass->ClearMenus();
            player->PlayerTalkClass->SendCloseGossip();

            if (action == GOSSIP_ACTION_INFO_DEF + 1)
                player->KilledMonsterCredit(NPC_KILL_CREDITS, 0);

            if (action == GOSSIP_ACTION_INFO_DEF + 2)
                player->AddItem(ITEM_DRAKURU_ELIXIR, 1);

            return true;
        }
};

enum UrsocTheBearGod
{
    NPC_URSOC                   = 26633,

    QUEST_URSOC_THE_BEAR_GOD_A  = 12249,
    QUEST_URSOC_THE_BEAR_GOD_H  = 12236,
};

class npc_tur_ragepaw : public CreatureScript
{
    public:
        npc_tur_ragepaw() : CreatureScript("npc_tur_ragepaw") { }

        bool OnGossipHello(Player* player, Creature* creature)
        {
            if (player->GetQuestStatus(QUEST_URSOC_THE_BEAR_GOD_A) == QUEST_STATUS_INCOMPLETE || player->GetQuestStatus(QUEST_URSOC_THE_BEAR_GOD_H) == QUEST_STATUS_INCOMPLETE)
                if (!creature->FindNearestCreature(NPC_URSOC, 200.0f, true))
                    player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, "Please summon Ursoc the Bear God.", GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 1);

            player->SEND_GOSSIP_MENU(player->GetGossipTextId(creature), creature->GetGUID());
            return true;
        }

        bool OnGossipSelect(Player* player, Creature* creature, uint32 /*sender*/, uint32 action)
        {
            player->PlayerTalkClass->ClearMenus();
            player->PlayerTalkClass->SendCloseGossip();

            if (action == GOSSIP_ACTION_INFO_DEF + 1)
            {
                creature->SummonCreature(NPC_URSOC, 4891.792969f, -3843.648926f, 337.545135f, 2.937676f, TEMPSUMMON_TIMED_OR_DEAD_DESPAWN, 300000);
                creature->MonsterSay("Ursoc has been spawned.", LANG_UNIVERSAL, player->GetGUID()); // Very hacky but this whole fix is hacky anyway
            }

            return true;
        }
};

enum eSteadyAsARock
{
    QUEST_STEADY_AS_ROCK        = 12014,
    GO_SEISMOGRAPH              = 188367,
    GO_BUNNY                    = 300189
};

class item_portable_seismograph : public ItemScript
{
public:
    item_portable_seismograph() : ItemScript("item_portable_seismograph") { }

    bool OnUse(Player* player, Item* item, const SpellCastTargets & /*targets*/)
    {
        if (player->GetQuestStatus(QUEST_STEADY_AS_ROCK) == QUEST_STATUS_INCOMPLETE)
        {
            if (Object* go = player->FindNearestGameObject(GO_BUNNY, 20.0f))
            {
                player->SummonGameObject(GO_SEISMOGRAPH, 5048.567f, -4818.506f, 219.500f, 2.62651f, 2.62651f, 0.0f, 0.966986f, 0.254829f, 0);
                return false;
            } else
            player->SendEquipError(EQUIP_ERR_OUT_OF_RANGE, item, NULL);
        } else
            player->SendEquipError(EQUIP_ERR_CANT_DO_RIGHT_NOW, item, NULL);
        return true;
    }
};

/*######
## go_amberpine_outhouse
######*/

#define GOSSIP_USE_OUTHOUSE "Use the outhouse."
#define GO_ANDERHOLS_SLIDER_CIDER_NOT_FOUND "Quest item Anderhol's Slider Cider not found."

enum eAmberpineOuthouse
{
    ITEM_ANDERHOLS_SLIDER_CIDER     = 37247,
    NPC_OUTHOUSE_BUNNY              = 27326,
    QUEST_DOING_YOUR_DUTY           = 12227,
    SPELL_INDISPOSED                = 53017,
    SPELL_INDISPOSED_III            = 48341,
    SPELL_CREATE_AMBERSEEDS         = 48330,
    GOSSIP_OUTHOUSE_INUSE           = 12775,
    GOSSIP_OUTHOUSE_VACANT          = 12779
};

class go_amberpine_outhouse : public GameObjectScript
{
public:
    go_amberpine_outhouse() : GameObjectScript("go_amberpine_outhouse") { }

    bool OnGossipHello(Player* player, GameObject* pGO)
    {
        QuestStatus status = player->GetQuestStatus(QUEST_DOING_YOUR_DUTY);
        if(status == QUEST_STATUS_INCOMPLETE || status == QUEST_STATUS_COMPLETE || status == QUEST_STATUS_REWARDED)
        {
            player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, GOSSIP_USE_OUTHOUSE, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 1);
            player->SEND_GOSSIP_MENU(GOSSIP_OUTHOUSE_VACANT, pGO->GetGUID());
            return true;
        }
        else
            player->SEND_GOSSIP_MENU(GOSSIP_OUTHOUSE_INUSE, pGO->GetGUID());
            return true;
    }

    bool OnGossipSelect(Player* player, GameObject* pGO, uint32 /*uiSender*/, uint32 uiAction)
    {
        player->PlayerTalkClass->ClearMenus();
        if(uiAction == GOSSIP_ACTION_INFO_DEF +1)
        {
            player->CLOSE_GOSSIP_MENU();
            Creature* target = GetClosestCreatureWithEntry(player, NPC_OUTHOUSE_BUNNY, 3.0f);
            if(target)
            {
                target->AI()->SetData(1, player->getGender());
                pGO->CastSpell(target, SPELL_INDISPOSED_III);
            }
            pGO->CastSpell(player, SPELL_INDISPOSED);
            if(player->HasItemCount(ITEM_ANDERHOLS_SLIDER_CIDER, 1))
                pGO->CastSpell(player, SPELL_CREATE_AMBERSEEDS);
            return true;
        }
        else
        {
            player->CLOSE_GOSSIP_MENU();
            player->GetSession()->SendNotification(GO_ANDERHOLS_SLIDER_CIDER_NOT_FOUND);
            return false;
        }
    }
};

void AddSC_grizzly_hills()
{
    new npc_emily();
    new npc_mrfloppy();
    new npc_outhouse_bunny();
    new npc_tallhorn_stag();
    new npc_amberpine_woodsman();
    new npc_wounded_skirmisher();
    new npc_lightning_sentry();
    new npc_venture_co_straggler();
	new npc_drakuru();
	new npc_tur_ragepaw();
	new item_portable_seismograph();
	new go_amberpine_outhouse();
}
