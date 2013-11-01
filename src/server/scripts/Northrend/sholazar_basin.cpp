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

/* ScriptData
SDName: Sholazar_Basin
SD%Complete: 100
SDComment: Quest support: 12570, 12573, 12621, 12726
SDCategory: Sholazar_Basin
EndScriptData */

/* ContentData
npc_injured_rainspeaker_oracle
npc_vekjik
avatar_of_freya
npc_haiphoon (Quest: "Song of Wind and Water")
EndContentData */

#include "ScriptMgr.h"
#include "ScriptedCreature.h"
#include "ScriptedGossip.h"
#include "ScriptedEscortAI.h"
#include "SpellScript.h"
#include "SpellAuras.h"
#include "Vehicle.h"
#include "CombatAI.h"
#include "Player.h"

/*######
## npc_injured_rainspeaker_oracle
######*/

#define GOSSIP_ITEM1 "I am ready to travel to your village now."

enum eRainspeaker
{
    SAY_START_IRO                       = -1571000,
    SAY_QUEST_ACCEPT_IRO                = -1571001,
    SAY_END_IRO                         = -1571002,

    QUEST_FORTUNATE_MISUNDERSTANDINGS   = 12570,
    FACTION_ESCORTEE_A                  = 774,
    FACTION_ESCORTEE_H                  = 775
};

class npc_injured_rainspeaker_oracle : public CreatureScript
{
public:
    npc_injured_rainspeaker_oracle() : CreatureScript("npc_injured_rainspeaker_oracle") { }

    struct npc_injured_rainspeaker_oracleAI : public npc_escortAI
    {
        npc_injured_rainspeaker_oracleAI(Creature* creature) : npc_escortAI(creature) { c_guid = creature->GetGUID(); }

        uint64 c_guid;

        void Reset()
        {
            me->RestoreFaction();
            // if we will have other way to assign this to only one npc remove this part
            if (GUID_LOPART(me->GetGUID()) != 101030)
            {
                me->RemoveFlag(UNIT_NPC_FLAGS, UNIT_NPC_FLAG_QUESTGIVER);
                me->RemoveFlag(UNIT_NPC_FLAGS, UNIT_NPC_FLAG_GOSSIP);
            }
        }

        void WaypointReached(uint32 waypointId)
        {
            Player* player = GetPlayerForEscort();
            if (!player)
                return;

            switch (waypointId)
            {
                case 1:
                    SetRun();
                    break;
                case 10:
                case 11:
                case 12:
                case 13:
                case 14:
                case 15:
                case 16:
                case 17:
                case 18:
                    me->RemoveUnitMovementFlag(MOVEMENTFLAG_SWIMMING);
                    me->RemoveUnitMovementFlag(MOVEMENTFLAG_FALLING);
                    me->SetSpeed(MOVE_SWIM, 0.85f, true);
                    me->AddUnitMovementFlag(MOVEMENTFLAG_SWIMMING | MOVEMENTFLAG_DISABLE_GRAVITY);
                    break;
                case 19:
                    me->SetUnitMovementFlags(MOVEMENTFLAG_FALLING);
                    break;
                case 28:
                    player->GroupEventHappens(QUEST_FORTUNATE_MISUNDERSTANDINGS, me);
                    // me->RestoreFaction();
                    DoScriptText(SAY_END_IRO, me);
                    SetRun(false);
                    break;
            }
        }

        void JustDied(Unit* /*killer*/)
        {
            if (!HasEscortState(STATE_ESCORT_ESCORTING))
                return;

            if (Player* player = GetPlayerForEscort())
            {
                if (player->GetQuestStatus(QUEST_FORTUNATE_MISUNDERSTANDINGS) != QUEST_STATUS_COMPLETE)
                    player->FailQuest(QUEST_FORTUNATE_MISUNDERSTANDINGS);
            }
        }
    };

    bool OnGossipHello(Player* player, Creature* creature)
    {
        if (creature->isQuestGiver())
            player->PrepareQuestMenu(creature->GetGUID());

        if (player->GetQuestStatus(QUEST_FORTUNATE_MISUNDERSTANDINGS) == QUEST_STATUS_INCOMPLETE)
            player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, GOSSIP_ITEM1, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF+1);

        player->SEND_GOSSIP_MENU(player->GetGossipTextId(creature), creature->GetGUID());

        return true;
    }

    bool OnGossipSelect(Player* player, Creature* creature, uint32 /*sender*/, uint32 action)
    {
        player->PlayerTalkClass->ClearMenus();
        if (action == GOSSIP_ACTION_INFO_DEF+1)
        {
            CAST_AI(npc_escortAI, (creature->AI()))->Start(true, false, player->GetGUID());
            CAST_AI(npc_escortAI, (creature->AI()))->SetMaxPlayerDistance(35.0f);
            creature->SetUnitMovementFlags(MOVEMENTFLAG_FALLING);
            DoScriptText(SAY_START_IRO, creature);

            switch (player->GetTeam()){
            case ALLIANCE:
                creature->setFaction(FACTION_ESCORTEE_A);
                break;
            case HORDE:
                creature->setFaction(FACTION_ESCORTEE_H);
                break;
            }
        }
        return true;
    }

    bool OnQuestAccept(Player* /*player*/, Creature* creature, Quest const* /*_Quest*/)
    {
        DoScriptText(SAY_QUEST_ACCEPT_IRO, creature);
        return false;
    }

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_injured_rainspeaker_oracleAI(creature);
    }
};

/*######
## npc_vekjik
######*/

#define GOSSIP_VEKJIK_ITEM1 "Shaman Vekjik, I have spoken with the big-tongues and they desire peace. I have brought this offering on their behalf."
#define GOSSIP_VEKJIK_ITEM2 "No no... I had no intentions of betraying your people. I was only defending myself. it was all a misunderstanding."

enum eVekjik
{
    GOSSIP_TEXTID_VEKJIK1       = 13137,
    GOSSIP_TEXTID_VEKJIK2       = 13138,

    SAY_TEXTID_VEKJIK1          = -1000208,

    SPELL_FREANZYHEARTS_FURY    = 51469,

    QUEST_MAKING_PEACE          = 12573
};

class npc_vekjik : public CreatureScript
{
public:
    npc_vekjik() : CreatureScript("npc_vekjik") { }

    bool OnGossipHello(Player* player, Creature* creature)
    {
        if (creature->isQuestGiver())
            player->PrepareQuestMenu(creature->GetGUID());

        if (player->GetQuestStatus(QUEST_MAKING_PEACE) == QUEST_STATUS_INCOMPLETE)
        {
            player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, GOSSIP_VEKJIK_ITEM1, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF+1);
            player->SEND_GOSSIP_MENU(GOSSIP_TEXTID_VEKJIK1, creature->GetGUID());
            return true;
        }

        player->SEND_GOSSIP_MENU(player->GetGossipTextId(creature), creature->GetGUID());
        return true;
    }

    bool OnGossipSelect(Player* player, Creature* creature, uint32 /*sender*/, uint32 action)
    {
        player->PlayerTalkClass->ClearMenus();
        switch (action)
        {
            case GOSSIP_ACTION_INFO_DEF+1:
                player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, GOSSIP_VEKJIK_ITEM2, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF+2);
                player->SEND_GOSSIP_MENU(GOSSIP_TEXTID_VEKJIK2, creature->GetGUID());
                break;
            case GOSSIP_ACTION_INFO_DEF+2:
                player->CLOSE_GOSSIP_MENU();
                DoScriptText(SAY_TEXTID_VEKJIK1, creature, player);
                player->AreaExploredOrEventHappens(QUEST_MAKING_PEACE);
                creature->CastSpell(player, SPELL_FREANZYHEARTS_FURY, false);
                break;
        }

        return true;
    }
};

/*######
## avatar_of_freya
######*/

#define GOSSIP_ITEM_AOF1 "I want to stop the Scourge as much as you do. How can I help?"
#define GOSSIP_ITEM_AOF2 "You can trust me. I am no friend of the Lich King."
#define GOSSIP_ITEM_AOF3 "I will not fail."

enum eFreya
{
    QUEST_FREYA_PACT         = 12621,

    SPELL_FREYA_CONVERSATION = 52045,

    GOSSIP_TEXTID_AVATAR1    = 13303,
    GOSSIP_TEXTID_AVATAR2    = 13304,
    GOSSIP_TEXTID_AVATAR3    = 13305
};

class npc_avatar_of_freya : public CreatureScript
{
public:
    npc_avatar_of_freya() : CreatureScript("npc_avatar_of_freya") { }

    bool OnGossipHello(Player* player, Creature* creature)
    {
        if (creature->isQuestGiver())
            player->PrepareQuestMenu(creature->GetGUID());

        if (player->GetQuestStatus(QUEST_FREYA_PACT) == QUEST_STATUS_INCOMPLETE)
            player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, GOSSIP_ITEM_AOF1, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF+1);

        player->PlayerTalkClass->SendGossipMenu(GOSSIP_TEXTID_AVATAR1, creature->GetGUID());
        return true;
    }

    bool OnGossipSelect(Player* player, Creature* creature, uint32 /*sender*/, uint32 action)
    {
        player->PlayerTalkClass->ClearMenus();
        switch (action)
        {
        case GOSSIP_ACTION_INFO_DEF+1:
            player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, GOSSIP_ITEM_AOF2, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF+2);
            player->PlayerTalkClass->SendGossipMenu(GOSSIP_TEXTID_AVATAR2, creature->GetGUID());
            break;
        case GOSSIP_ACTION_INFO_DEF+2:
            player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, GOSSIP_ITEM_AOF3, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF+3);
            player->PlayerTalkClass->SendGossipMenu(GOSSIP_TEXTID_AVATAR3, creature->GetGUID());
            break;
        case GOSSIP_ACTION_INFO_DEF+3:
            player->CastSpell(player, SPELL_FREYA_CONVERSATION, true);
            player->CLOSE_GOSSIP_MENU();
            break;
        }
        return true;
    }
};

/*######
## npc_bushwhacker
######*/

class npc_bushwhacker : public CreatureScript
{
public:
    npc_bushwhacker() : CreatureScript("npc_bushwhacker") { }

    struct npc_bushwhackerAI : public ScriptedAI
    {
        npc_bushwhackerAI(Creature* creature) : ScriptedAI(creature)
        {
        }

        void InitializeAI()
        {
            if (me->isDead())
                return;

            if (TempSummon* summ = me->ToTempSummon())
                if (Unit* summoner = summ->GetSummoner())
                    me->GetMotionMaster()->MovePoint(0, summoner->GetPositionX(), summoner->GetPositionY(), summoner->GetPositionZ());

            Reset();
        }

        void UpdateAI(const uint32 /*uiDiff*/)
        {
            if (!UpdateVictim())
                return;

            DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_bushwhackerAI(creature);
    }
};

/*######
## npc_engineer_helice
######*/

enum eEnums
{
    SPELL_EXPLODE_CRYSTAL       = 62487,
    SPELL_FLAMES                = 64561,

    SAY_WP_7                    = -1800047,
    SAY_WP_6                    = -1800048,
    SAY_WP_5                    = -1800049,
    SAY_WP_4                    = -1800050,
    SAY_WP_3                    = -1800051,
    SAY_WP_2                    = -1800052,
    SAY_WP_1                    = -1800053,

    QUEST_DISASTER              = 12688
};

class npc_engineer_helice : public CreatureScript
{
public:
    npc_engineer_helice() : CreatureScript("npc_engineer_helice") { }

    struct npc_engineer_heliceAI : public npc_escortAI
    {
        npc_engineer_heliceAI(Creature* creature) : npc_escortAI(creature) { }

        uint32 m_uiChatTimer;

        void WaypointReached(uint32 waypointId)
        {
            Player* player = GetPlayerForEscort();

            switch (waypointId)
            {
                case 0:
                    DoScriptText(SAY_WP_2, me);
                    break;
                case 1:
                    DoScriptText(SAY_WP_3, me);
                    me->CastSpell(5918.33f, 5372.91f, -98.770f, SPELL_EXPLODE_CRYSTAL, true);
                    me->SummonGameObject(184743, 5918.33f, 5372.91f, -98.770f, 0, 0, 0, 0, 0, TEMPSUMMON_MANUAL_DESPAWN);     //approx 3 to 4 seconds
                    me->HandleEmoteCommand(EMOTE_ONESHOT_LAUGH);
                    break;
                case 2:
                    DoScriptText(SAY_WP_4, me);
                    break;
                case 7:
                    DoScriptText(SAY_WP_5, me);
                    break;
                case 8:
                    me->CastSpell(5887.37f, 5379.39f, -91.289f, SPELL_EXPLODE_CRYSTAL, true);
                    me->SummonGameObject(184743, 5887.37f, 5379.39f, -91.289f, 0, 0, 0, 0, 0, TEMPSUMMON_MANUAL_DESPAWN);      //approx 3 to 4 seconds
                    me->HandleEmoteCommand(EMOTE_ONESHOT_LAUGH);
                    break;
                case 9:
                    DoScriptText(SAY_WP_6, me);
                    break;
                case 13:
                    if (player)
                    {
                        player->GroupEventHappens(QUEST_DISASTER, me);
                        DoScriptText(SAY_WP_7, me);
                    }
                    break;
            }
        }

        void Reset()
        {
            m_uiChatTimer = 4000;
        }

        void JustDied(Unit* /*killer*/)
        {
            if (HasEscortState(STATE_ESCORT_ESCORTING))
            {
                if (Player* player = GetPlayerForEscort())
                    player->FailQuest(QUEST_DISASTER);
            }
        }

        void UpdateAI(const uint32 uiDiff)
        {
            npc_escortAI::UpdateAI(uiDiff);

            if (HasEscortState(STATE_ESCORT_ESCORTING))
            {
                if (m_uiChatTimer <= uiDiff)
                {
                    m_uiChatTimer = 12000;
                }
                else
                    m_uiChatTimer -= uiDiff;
            }
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_engineer_heliceAI(creature);
    }

    bool OnQuestAccept(Player* player, Creature* creature, const Quest* quest)
    {
        if (quest->GetQuestId() == QUEST_DISASTER)
        {
            if (npc_engineer_heliceAI* pEscortAI = CAST_AI(npc_engineer_helice::npc_engineer_heliceAI, creature->AI()))
            {
                creature->GetMotionMaster()->MoveJumpTo(0, 0.4f, 0.4f);
                creature->setFaction(113);

                pEscortAI->Start(false, false, player->GetGUID());
                DoScriptText(SAY_WP_1, creature);
            }
        }
        return true;
    }
};

/*#####
## npc_jungle_punch_target
#####*/

#define SAY_OFFER     "Care to try Grimbooze Thunderbrew's new jungle punch?"
#define SAY_HEMET_1   "Aye, I'll try it."
#define SAY_HEMET_2   "That's exactly what I needed!"
#define SAY_HEMET_3   "It's got my vote! That'll put hair on your chest like nothing else will."
#define SAY_HADRIUS_1 "I'm always up for something of Grimbooze's."
#define SAY_HADRIUS_2 "Well, so far, it tastes like something my wife would drink..."
#define SAY_HADRIUS_3 "Now, there's the kick I've come to expect from Grimbooze's drinks! I like it!"
#define SAY_TAMARA_1  "Sure!"
#define SAY_TAMARA_2  "Oh my..."
#define SAY_TAMARA_3  "Tastes like I'm drinking... engine degreaser!"

enum utils
{
    NPC_HEMET   = 27986,
    NPC_HADRIUS = 28047,
    NPC_TAMARA  = 28568,
    SPELL_OFFER = 51962,
    QUEST_ENTRY = 12645,
};

class npc_jungle_punch_target : public CreatureScript
{
public:
    npc_jungle_punch_target() : CreatureScript("npc_jungle_punch_target") { }

    struct npc_jungle_punch_targetAI : public ScriptedAI
    {
        npc_jungle_punch_targetAI(Creature* creature) : ScriptedAI(creature) {}

        uint16 sayTimer;
        uint8 sayStep;

        void Reset()
        {
            sayTimer = 3500;
            sayStep = 0;
        }

        void UpdateAI(const uint32 uiDiff)
        {
            if (!sayStep)
                return;

            if (sayTimer < uiDiff)
            {
                switch (sayStep)
                {
                    case 0:
                    {
                        switch (me->GetEntry())
                        {
                            case NPC_HEMET:   me->MonsterSay(SAY_HEMET_1, LANG_UNIVERSAL, 0);   break;
                            case NPC_HADRIUS: me->MonsterSay(SAY_HADRIUS_1, LANG_UNIVERSAL, 0); break;
                            case NPC_TAMARA:  me->MonsterSay(SAY_TAMARA_1, LANG_UNIVERSAL, 0);  break;
                        }
                        sayTimer = 3000;
                        sayStep++;
                        break;
                    }
                    case 1:
                    {
                        switch (me->GetEntry())
                        {
                            case NPC_HEMET:   me->MonsterSay(SAY_HEMET_2, LANG_UNIVERSAL, 0);   break;
                            case NPC_HADRIUS: me->MonsterSay(SAY_HADRIUS_2, LANG_UNIVERSAL, 0); break;
                            case NPC_TAMARA:  me->MonsterSay(SAY_TAMARA_2, LANG_UNIVERSAL, 0);  break;
                        }
                        sayTimer = 3000;
                        sayStep++;
                        break;
                    }
                    case 2:
                    {
                        switch (me->GetEntry())
                        {
                            case NPC_HEMET:   me->MonsterSay(SAY_HEMET_3, LANG_UNIVERSAL, 0);   break;
                            case NPC_HADRIUS: me->MonsterSay(SAY_HADRIUS_3, LANG_UNIVERSAL, 0); break;
                            case NPC_TAMARA:  me->MonsterSay(SAY_TAMARA_3, LANG_UNIVERSAL, 0);  break;
                        }
                        sayTimer = 3000;
                        sayStep = 0;
                        break;
                    }
                }
            }
            else
                sayTimer -= uiDiff;
        }

        void SpellHit(Unit* caster, const SpellInfo* proto)
        {
            if (!proto || proto->Id != SPELL_OFFER)
                return;

            if (!caster->ToPlayer())
                return;

            QuestStatusMap::const_iterator itr = caster->ToPlayer()->getQuestStatusMap().find(QUEST_ENTRY);
            if (itr->second.Status != QUEST_STATUS_INCOMPLETE)
                return;

            for (uint8 i=0; i<3; i++)
            {
                switch (i)
                {
                   case 0:
                       if (NPC_HEMET != me->GetEntry())
                           continue;
                       else
                           break;
                   case 1:
                       if (NPC_HADRIUS != me->GetEntry())
                           continue;
                       else
                           break;
                   case 2:
                       if (NPC_TAMARA != me->GetEntry())
                           continue;
                       else
                           break;
                }

                if (itr->second.CreatureOrGOCount[i] != 0)
                    continue;

                caster->ToPlayer()->KilledMonsterCredit(me->GetEntry(), 0);
                caster->ToPlayer()->Say(SAY_OFFER, LANG_UNIVERSAL);
                sayStep = 0;
                break;
            }
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_jungle_punch_targetAI(creature);
    }
};

/*######
## npc_adventurous_dwarf
######*/

#define GOSSIP_OPTION_ORANGE    "Can you spare an orange?"
#define GOSSIP_OPTION_BANANAS   "Have a spare bunch of bananas?"
#define GOSSIP_OPTION_PAPAYA    "I could really use a papaya."

enum eAdventurousDwarf
{
    QUEST_12634         = 12634,

    ITEM_BANANAS        = 38653,
    ITEM_PAPAYA         = 38655,
    ITEM_ORANGE         = 38656,

    SPELL_ADD_ORANGE    = 52073,
    SPELL_ADD_BANANAS   = 52074,
    SPELL_ADD_PAPAYA    = 52076,

    GOSSIP_MENU_DWARF   = 13307,

    SAY_DWARF_OUCH      = -1571042,
    SAY_DWARF_HELP      = -1571043
};

class npc_adventurous_dwarf : public CreatureScript
{
public:
    npc_adventurous_dwarf() : CreatureScript("npc_adventurous_dwarf") { }

    CreatureAI* GetAI(Creature* creature) const
    {
        DoScriptText(SAY_DWARF_OUCH, creature);
        return NULL;
    }

    bool OnGossipHello(Player* player, Creature* creature)
    {
        if (player->GetQuestStatus(QUEST_12634) != QUEST_STATUS_INCOMPLETE)
            return false;

        if (player->GetItemCount(ITEM_ORANGE) < 1)
            player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, GOSSIP_OPTION_ORANGE, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 1);

        if (player->GetItemCount(ITEM_BANANAS) < 2)
            player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, GOSSIP_OPTION_BANANAS, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 2);

        if (player->GetItemCount(ITEM_PAPAYA) < 1)
            player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, GOSSIP_OPTION_PAPAYA, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 3);

        player->PlayerTalkClass->SendGossipMenu(GOSSIP_MENU_DWARF, creature->GetGUID());
        return true;
    }

    bool OnGossipSelect(Player* player, Creature* creature, uint32 /*sender*/, uint32 action)
    {
        player->PlayerTalkClass->ClearMenus();
        uint32 spellId = 0;
        switch (action)
        {
            case GOSSIP_ACTION_INFO_DEF + 1: spellId = SPELL_ADD_ORANGE;     break;
            case GOSSIP_ACTION_INFO_DEF + 2: spellId = SPELL_ADD_BANANAS;    break;
            case GOSSIP_ACTION_INFO_DEF + 3: spellId = SPELL_ADD_PAPAYA;     break;
        }
        if (spellId)
            player->CastSpell(player, spellId, true);
        DoScriptText(SAY_DWARF_HELP, creature);
        creature->DespawnOrUnsummon();
        return true;
    }
};

/*######
## Quest The Lifewarden's Wrath
######*/

enum MiscLifewarden
{
    NPC_PRESENCE = 28563, // Freya's Presence
    NPC_SABOTEUR = 28538, // Cultist Saboteur
    NPC_SERVANT = 28320, // Servant of Freya

    WHISPER_ACTIVATE = 0,

    SPELL_FREYA_DUMMY = 51318,
    SPELL_LIFEFORCE = 51395,
    SPELL_FREYA_DUMMY_TRIGGER = 51335,
    SPELL_LASHER_EMERGE = 48195,
    SPELL_WILD_GROWTH = 52948,
};

class spell_q12620_the_lifewarden_wrath : public SpellScriptLoader
{
public:
    spell_q12620_the_lifewarden_wrath() : SpellScriptLoader("spell_q12620_the_lifewarden_wrath") { }

    class spell_q12620_the_lifewarden_wrath_SpellScript : public SpellScript
    {
        PrepareSpellScript(spell_q12620_the_lifewarden_wrath_SpellScript);

        void HandleSendEvent(SpellEffIndex effIndex)
        {
            PreventHitDefaultEffect(effIndex);

            if (Unit* caster = GetCaster())
            {
                if (Creature* presence = caster->FindNearestCreature(NPC_PRESENCE, 50.0f))
                {
                    presence->AI()->Talk(WHISPER_ACTIVATE, caster->GetGUID());
                    presence->CastSpell(presence, SPELL_FREYA_DUMMY, true); // will target plants
                    // Freya Dummy could be scripted with the following code

                    // Revive plants
                    std::list<Creature*> servants;
                    GetCaster()->GetCreatureListWithEntryInGrid(servants, NPC_SERVANT, 200.0f);
                    for (std::list<Creature*>::iterator itr = servants.begin(); itr != servants.end(); ++itr)
                    {
                        // Couldn't find a spell that does this
                        if ((*itr)->isDead())
                            (*itr)->Respawn(true);

                        (*itr)->CastSpell(*itr, SPELL_FREYA_DUMMY_TRIGGER, true);
                        (*itr)->CastSpell(*itr, SPELL_LASHER_EMERGE, false);
                        (*itr)->CastSpell(*itr, SPELL_WILD_GROWTH, false);

                        if (Unit* target = (*itr)->SelectNearestTarget(150.0f))
                            (*itr)->AI()->AttackStart(target);
                    }

                    // Kill nearby enemies
                    std::list<Creature*> saboteurs;
                    caster->GetCreatureListWithEntryInGrid(saboteurs, NPC_SABOTEUR, 200.0f);
                    for (std::list<Creature*>::iterator itr = saboteurs.begin(); itr != saboteurs.end(); ++itr)
                        if ((*itr)->isAlive())
                            // Lifeforce has a cast duration, it should be cast at all saboteurs one by one
                            presence->CastSpell((*itr), SPELL_LIFEFORCE, false);
                }
            }
        }

        void Register()
        {
            OnEffectHit += SpellEffectFn(spell_q12620_the_lifewarden_wrath_SpellScript::HandleSendEvent, EFFECT_0, SPELL_EFFECT_SEND_EVENT);
        }
    };

    SpellScript* GetSpellScript() const
    {
        return new spell_q12620_the_lifewarden_wrath_SpellScript();
    }
};

/*######
## Quest Kick, What Kick? (12589)
######*/

enum KickWhatKick
{
    NPC_LUCKY_WILHELM = 28054,
    NPC_APPLE = 28053,
    NPC_DROSTAN = 28328,
    NPC_CRUNCHY = 28346,
    NPC_THICKBIRD = 28093,

    SPELL_HIT_APPLE = 51331,
    SPELL_MISS_APPLE = 51332,
    SPELL_MISS_BIRD_APPLE = 51366,
    SPELL_APPLE_FALL = 51371,
    SPELL_BIRD_FALL = 51369,

    EVENT_MISS = 0,
    EVENT_HIT = 1,
    EVENT_MISS_BIRD = 2,

    SAY_WILHELM_MISS = 0,
    SAY_WILHELM_HIT = 1,
    SAY_DROSTAN_REPLY_MISS = 0,
};

class spell_q12589_shoot_rjr : public SpellScriptLoader
{
public:
    spell_q12589_shoot_rjr() : SpellScriptLoader("spell_q12589_shoot_rjr") { }

    class spell_q12589_shoot_rjr_SpellScript : public SpellScript
    {
        PrepareSpellScript(spell_q12589_shoot_rjr_SpellScript);

        SpellCastResult CheckCast()
        {
            if (Unit* target = GetExplTargetUnit())
                if (target->GetEntry() == NPC_LUCKY_WILHELM)
                    return SPELL_CAST_OK;

            SetCustomCastResultMessage(SPELL_CUSTOM_ERROR_MUST_TARGET_WILHELM);
            return SPELL_FAILED_CUSTOM_ERROR;
        }

        void HandleDummy(SpellEffIndex /*effIndex*/)
        {
            uint32 roll = urand(1, 100);

            uint8 ev;
            if (roll <= 50)
                ev = EVENT_MISS;
            else if (roll <= 83)
                ev = EVENT_HIT;
            else
                ev = EVENT_MISS_BIRD;

            Unit* shooter = GetCaster();
            Creature* wilhelm = GetHitUnit()->ToCreature();
            Creature* apple = shooter->FindNearestCreature(NPC_APPLE, 30);
            Creature* drostan = shooter->FindNearestCreature(NPC_DROSTAN, 30);

            if (!wilhelm || !apple || !drostan)
                return;

            switch (ev)
            {
                case EVENT_MISS_BIRD:
                {
                    Creature* crunchy = shooter->FindNearestCreature(NPC_CRUNCHY, 30);
                    Creature* bird = shooter->FindNearestCreature(NPC_THICKBIRD, 30);

                    if (!bird || !crunchy)
                        ; // fall to EVENT_MISS
                    else
                    {
                        shooter->CastSpell(bird, SPELL_MISS_BIRD_APPLE);
                        bird->CastSpell(bird, SPELL_BIRD_FALL);
                        wilhelm->AI()->Talk(SAY_WILHELM_MISS);
                        drostan->AI()->Talk(SAY_DROSTAN_REPLY_MISS);

                        bird->Kill(bird);
                        crunchy->GetMotionMaster()->MovePoint(0, bird->GetPositionX(), bird->GetPositionY(),
                            bird->GetMap()->GetWaterOrGroundLevel(bird->GetPositionX(), bird->GetPositionY(), bird->GetPositionZ()));
                        // TODO: Make crunchy perform emote eat when he reaches the bird

                        break;
                    }
                }
                case EVENT_MISS:
                {
                    shooter->CastSpell(wilhelm, SPELL_MISS_APPLE);
                    wilhelm->AI()->Talk(SAY_WILHELM_MISS);
                    drostan->AI()->Talk(SAY_DROSTAN_REPLY_MISS);
                    break;
                }
                case EVENT_HIT:
                {
                    shooter->CastSpell(apple, SPELL_HIT_APPLE);
                    apple->CastSpell(apple, SPELL_APPLE_FALL);
                    wilhelm->AI()->Talk(SAY_WILHELM_HIT);
                    if (Player* player = shooter->ToPlayer())
                        player->KilledMonsterCredit(NPC_APPLE, 0);
                    apple->DespawnOrUnsummon();

                    break;
                }
            }
        }

        void Register()
        {
            OnCheckCast += SpellCheckCastFn(spell_q12589_shoot_rjr_SpellScript::CheckCast);
            OnEffectHitTarget += SpellEffectFn(spell_q12589_shoot_rjr_SpellScript::HandleDummy, EFFECT_0, SPELL_EFFECT_DUMMY);
        }
    };

    SpellScript* GetSpellScript() const
    {
        return new spell_q12589_shoot_rjr_SpellScript();
    }
};
enum OraclesFrenzyheart
{
    NPC_MOODLE                              = 28122,
    NPC_JALOOT                              = 28121,
    NPC_JALOOT_SWITCH                       = 28667,
    NPC_LAFOO                               = 28120,
    NPC_GOREGEK                             = 28214,
    NPC_DAJIK                               = 28215,
    NPC_ZEPIK                               = 28216,
    NPC_ZEPIK_SWITCH                        = 28668,
    NPC_HIGH_ORACLE_SOO_SAY                 = 28027,
    NPC_RAVENOUS_MANGAL_CROCOLISK           = 28325,
    NPC_SHAMAN_VEKJIK                       = 28327,
    NPC_ARTRUIS_THE_HEARTLESS               = 28659,
    NPC_SERFEX_THE_REAVER                   = 28083,
    NPC_WASP_QUEEN                          = 28087,
    NPC_HARDKNUCKLE_MATRIARCH               = 28213,
    NPC_SOFTKNUCKLE                         = 28127,
    NPC_SAPPHIRE_HIVE_WASP                  = 28086,
    NPC_HARDKNUCKLE_CHARGER                 = 28096,
    NPC_HARDKNUCKLE_FORAGER                 = 28098,
    NPC_FIRST_BLOOD_CREDIT_1                = 28040,
    NPC_FIRST_BLOOD_CREDIT_2                = 36189,
    NPC_FIRST_BLOOD_CREDIT_3                = 29043,
    NPC_ORACLE_TRIGGER                      = 28455,
    NPC_CHICKEN_ESCAPEE                     = 28161,
    NPC_MISTWHISPER_ORACLE                  = 28110,
    NPC_MISTWHISPER_WARRIOR                 = 28109,
    NPC_PITCH                               = 28097,
    NPC_WARLORD_TARTEK                      = 28105,
    NPC_GLIMMERING_PILLAR_CREDIT            = 29056,
    NPC_MOSSLIGHT_PILLAR_CREDIT             = 29057,
    NPC_SKYREACH_PILLAR_CREDIT              = 29069,
    NPC_SUNTOUCHED_PILLAR_CREDIT            = 29058,
    NPC_MONSOON_REVENANT_CREDIT             = 29008,
    NPC_STORM_REVENANT_CREDIT               = 29009,
    NPC_FRENZYHEART_BERSERKER               = 28076,
    NPC_FRENZYHEART_TRACKER                 = 28077,
    NPC_FRENZYHEART_RAVAGER                 = 28078,
    NPC_FRENZYHEART_HUNTER                  = 28079,
    NPC_FRENZYHEART_SPEARBEARER             = 28080,
    NPC_FRENZYHEART_SCAVENGER               = 28081,
    NPC_SPARKTOUCHED_ORACLE                 = 28112,
    NPC_VOLATILE_TRAP_BUNNY                 = 241810,
    NPC_SPIKE_TRAP_BUNNY                    = 241811,
    NPC_ENSNARING_TRAP_BUNNY                = 241812,
    NPC_CRYSTAL_OF_THE_FROZEN_GRIP          = 29125,

    GAMEOBJECT_ARTRUISS_PHYLACTERY          = 190777,
    GAMEOBJECT_SOAKED_FERTILE_DIRT          = 191136,
    GAMEOBJECT_VOLATILE_TRAP                = 191147,
    GAMEOBJECT_SPIKE_TRAP                   = 191145,
    GAMEOBJECT_ENSNARING_TRAP               = 191146,
    GAMEOBJECT_REJEKS_SWORD                 = 191125,

    SPELL_DIG_UP_GLINTING_BUG               = 51443,
    SPELL_DIG_UP_SPARKLING_HARE             = 51441,
    SPELL_DIG_UP_GLINTING_ARMOR             = 51398,
    SPELL_DIG_UP_GLOWING_GEM                = 51400,
    SPELL_DIG_UP_POLISHED_PLATTER           = 51401,
    SPELL_DIG_UP_SHINING_CRYSTAL            = 51397,
    SPELL_DIG_UP_SPARKLING_TREASURE         = 51402,
    SPELL_TREASURE_SPARKLE                  = 51444,
    SPELL_EARTHS_FURY                       = 54193,
    SPELL_EARTHS_WRATH                      = 54195,
    SPELL_DAJIK_HEAL_OWNER                  = 54194,
    SPELL_AURA_DAJIKS_BALANCE               = 52734,
    SPELL_AURA_GOREGEKS_ENDURANCE           = 54178,
    SPELL_AURA_JALOOTS_INTENSITY            = 52119,
    SPELL_AURA_ZEPIKS_FERVOR                = 54176,
    SPELL_ARTRUIS_FROST_NOVA                = 11831,
    SPELL_ARTRUIS_FROSTBOLT                 = 15530,
    SPELL_ARTRUIS_ICY_VEINS                 = 54792,
    SPELL_ARTRUIS_ICE_LANCE                 = 54261,
    SPELL_ARTRUIS_BINDINGS                  = 52185,
    SPELL_ARTRUIS_TOMB_OF_HEARTLESS         = 52182,
    SPELL_OPEN_WOUND                        = 52873,
    SPELL_PLACE_SPIKE_TRAP                  = 52886,
    SPELL_LIGHTNING_WHIRL                   = 52943,
    SPELL_SPARK_FRENZY                      = 52964,
    SPELL_BLOOD_REJEKS_SWORD                = 52992,
    SPELL_WASP_STINGER_RAGE                 = 34392,
    SPELL_CHARGER_CHARGE                    = 49758,
    SPELL_ORACLE_LIGHTNING_CLOUD            = 54921,
    SPELL_WARRIOR_FLIP_ATTACK               = 50533,
    SPELL_AURA_SCARED_SOFTKNUCKLE           = 50979,
    SPELL_SUNDER_ARMOR                      = 54188,
    SPELL_DISARM                            =  6713,
    SPELL_SEE_INVISIBLE_CHICKEN             = 50735,
    SPELL_NET_CHICKEN                       = 51959,
    SPELL_ZEPIK_HEAL_OWNER                  = 52895,
    SPELL_BARBED_NET                        = 52761,
    SPELL_PIERCING_ARROW                    = 52758,
    SPELL_AURA_RECENTLY_BANDAGED            = 11196,
    SPELL_ENVENOMED_SHOT                    = 52889,
    SPELL_SUMMON_ARTRUIS_URN                = 52518,
    SPELL_TOMB_OF_HEARTLESS                 = 52182,
    SPELL_AURA_PERIODIC_TREASURE_SEARCH     = 51407,
    SPELL_GOREGEK_LIGHTNING_SHIELD          = 50726,
    SPELL_VORACIOUS_APPETITE                = 52749,
    SPELL_PIERCING_HOWL                     = 52744,
    SPELL_REACHED_FRENZYHEART_HILL          = 51234,
    SPELL_REACHED_MOSSWALKER_VILLAGE        = 51644,
    SPELL_REACHED_MISTWHISPER_REFUGE        = 51239,
    SPELL_REACHED_RAINSPEAKER_CANOPY        = 51639,
    SPELL_REACHED_SKYREACH_PILLAR           = 51645,
    SPELL_REACHED_LIFEBLOOD_PILLAR          = 51649,
    SPELL_REACHED_RIVERS_HEART              = 51652,
    SPELL_REACHED_KARTAKS_HOLD              = 51643,
    SPELL_REACHED_SPEARBORN_ENCAMPMENT      = 51642, // todo
    SPELL_REACHED_SAPPHIRE_HIVE             = 51651, // todo
    SPELL_REACHED_SPARKTOUCHED_HAVEN        = 51641, // todo
    SPELL_SPIKE_TRAP                        = 53074,
    SPELL_VOLATILE_TRAP                     = 53076,
    SPELL_ENSNARING_TRAP                    = 53077,
    SPELL_CREATE_TRUE_POWER_OF_THE_TEMPEST  = 53067,
    SPELL_CALL_LIGHTNING                    = 32018,
    SPELL_THROW_VENTURE_CO_EXPLOSIVES       = 53145,
    SPELL_SUMMON_STORMWATCHERS_HEAD         = 53162,
    SPELL_PERMANENT_FEIGN_DEATH             = 29266,
    SPELL_DISPERSAL                         = 52972,
    SPELL_FRIGID_ABSORBTION                 = 52979,
    SPELL_FROST_BREATH                      = 52973,
    SPELL_VICIOUS_ROAR                      = 52974,
    SPELL_WHIRLWIND                         = 52977,
    SPELL_SLEEPY_TIME                       = 52940,
    SPELL_TOXIC_SPIT                        = 52931,
    SPELL_DIG_UP_DISGRUNTLED_BUG            = 52917,
    SPELL_AURA_DETECT_INVIS_TREASURE        = 52214,
    SPELL_FROZEN_GRIP                       = 53219,

    QUEST_APPEASING_THE_GREAT_RAIN_STONE    = 12704,
    QUEST_GODS_LIKE_SHINEY_THINGS           = 12572,
    QUEST_MAKE_THE_BAD_SNAKE_GO_AWAY        = 12571,
    QUEST_JUST_FOLLOWING_ORDERS             = 12540,
    QUEST_BACK_SO_SOON                      = 12574,
    QUEST_FORCED_HAND                       = 12576,
    QUEST_THE_LOST_MISTWHISPER_TRESURE      = 12575,
    QUEST_THE_ANGRY_GORLOC                  = 12578,
    QUEST_A_HEROS_BURDEN                    = 12581,
    QUEST_FRENZYHEART_CHAMPION              = 12582,
    QUEST_HAND_OF_THE_ORACLES               = 12689,
    QUEST_TORMENTING_THE_SOFTKNUCKLES       = 12530,
    QUEST_FLOWN_THE_COOP                    = 12532,
    QUEST_CHICKEN_PARTY                     = 12702,
    QUEST_THE_LOST_MISTWHISPER_TREASURE     = 12575,
    QUEST_SONG_OF_FECUNDITY                 = 12737,
    QUEST_SONG_OF_REFLECTION                = 12736,
    QUEST_SONG_OF_WIND_AND_WATER            = 12726,
    QUEST_STRENGTH_OF_THE_TEMPEST           = 12741,
    QUEST_REJEK_FIRST_BLOOD                 = 12734,

    AREA_GLIMMERING_PILLAR                  = 4307,
    AREA_MOSSLIGHT_PILLAR                   = 4400,
    AREA_SKYREACH_PILLAR                    = 4302,
    AREA_SUNTOUCHED_PILLAR                  = 4389,
    AREA_MOSSWALKER_VILLAGE                 = 4297,
    AREA_HARDKNUCKLE_CLEARING               = 4303,
    AREA_FRENZYHEART_HILL                   = 4292,
    AREA_MISTWHISPER_REFUGE                 = 4306,
    AREA_RAINSPEAKER_CANOPY                 = 4291,
    ZONE_SHOLAZAR_BASIN                     = 3711,

    FACTION_ORACLES                         = 1105,
    FACTION_FRENZYHEART                     = 1104,
    FACTION_ZEPIK_JALOOT_OFFENSIVE          = 14,

    ITEM_ESSENCE_OF_THE_STORM               = 39643,
    ITEM_ESSENCE_OF_THE_MOONSOON            = 39616,
    ITEM_ZEPIKS_SUMMONER                    = 38512,
    ITEM_DAJIKS_SUMMONER                    = 38621,
    ITEM_GOREGEKS_SUMMONER                  = 38619,
    ITEM_LAFOOS_SUMMONER                    = 38622,
    ITEM_JALOOTS_SUMMONER                   = 38623,
    ITEM_MOODLES_SUMMONER                   = 38624,
    ITEM_RAINSPEAKER_PEACE_OFFERING         = 40364,

    EVENT_ALL_COMPANIONS_RANDOM_SAY         = 0,
    EVENT_ALL_FOLLOW_AGAIN,
    EVENT_ALL_CHECK_AREA_ID,
    EVENT_ALL_CHECK_ZONE_ID,
    EVENT_ALL_CAST_NET_ON_CHICKENS,
    EVENT_ALL_CHECK_OWNERS_HEALTH,
    EVENT_ALL_BECOME_AGGRESSIVE,
    EVENT_ALL_CHECK_OWNER_AFK,
    EVENT_ALL_SEARCH_FOR_TREASURE,
    EVENT_ALL_RESET_RECENTLY_REACHED_BOOL,
    EVENT_ALL_CHECK_OWNER_HAS_SUMMONER_ITEM,

    EVENT_DAJIK_EARTHS_FURY,
    EVENT_DAJIK_EARTHS_WRATH,

    EVENT_GOREGEK_SUNDER_ARMOR,
    EVENT_GOREGEK_DISARM,
    EVENT_GOREGEK_PIERCING_HOWL,

    EVENT_MOODLE_FROST_BREATH,
    EVENT_MOODLE_FRIGID_ABSORBTION,
    EVENT_MOODLE_VICIOUS_ROAR,
    EVENT_MOODLE_WHIRLWIND,

    EVENT_ZEPIK_PLACE_SPIKE_TRAP,
    EVENT_ZEPIK_OPEN_WOUND,
    EVENT_ZEPIK_PIERCING_ARROW,

    EVENT_JALOOT_LIGHTNING_WHIRL,
    EVENT_JALOOT_SPARK_FRENZY,

    EVENT_LAFOO_CHECK_HEALTH,
    EVENT_LAFOO_TOXIC_SPIT,

    EVENT_ARTRUIS_FROSTBOLT,
    EVENT_ARTRUIS_ICE_LANCE,
    EVENT_ARTRUIS_ICY_VEINS,
    EVENT_ARTRUIS_FROST_NOVA,

    EVENT_RAINSPEAKER_TREASURE_SPARKLE,
    EVENT_RAINSPEAKER_TREASURE_RESET_BOOL,
    EVENT_RAINSPEAKER_TREASURE_REMOVE_SHINE_EFFECT,

    POINT_MOVE_TO_ORACLE_TRIGGER            = 5000,
    POINT_MOVE_TO_UNDEAD_HUMAN_BODY         = 5001,

    ACTION_ARTRUIS_UNBIND                   = 0,
    ACTION_ARTRUIS_BIND                     = 1,
    ACTION_ARTRUIS_SET_QUESTGIVER           = 2,
    ACTION_ORACLE_TRIGGER_DUG_UP            = 3,

    SAY_VEKJIK_FRENZYHEART_KILL_YOU         = 0,

    SAY_INJURED_RAINSPEAKER_START           = 0,
    SAY_INJURED_RAINSPEAKER_QUEST_ACCEPT    = 1,
    SAY_INJURED_RAINSPEAKER_END             = 2,

    SAY_RANDOM_LINE_OOC                     = 0,
    SAY_ALL_DUG_RANDOM_STUFF                = 2,
    SAY_ALL_SEE_TREASURE                    = 5,
    SAY_ALL_DONE_DIGGING                    = 6,
    SAY_ALL_DUG_UP_RABBIT                   = 7,
    SAY_ALL_DUG_UP_BUG                      = 8,

    SAY_GOREGEK_HARDKNUCKLES_CLEARING       = 1,
    SAY_GOREGEK_FRENZYHEART_HILL            = 2,
    SAY_GOREGEK_MARTRIACH_SUMMONED          = 3,
    SAY_GOREGEK_KILLED_PITCH                = 4,
    SAY_GOREGEK_KILLED_BY_HARDKNUCKLE       = 5,
    SAY_GOREGEK_NETTED_CHICKEN              = 6,
    SAY_GOREGEK_ENTER_COMBAT                = 7,
    SAY_GOREGEK_ENTER_COMBAT_HARDKNUCKLE    = 8,
    SAY_GOREGEK_KILLED_UNIT                 = 9,
    SAY_GOREGEK_OWNER_SCARED_SOFTKNUCKLE    = 10,
    SAY_GOREGEK_KARTAKS_HOLD                = 11,

    SAY_DAJIK_KILLED_UNIT                   = 1,
    SAY_DAJIK_AGGRO_SERFEX                  = 2,
    SAY_DAJIK_POKE_KNUCKLES                 = 3,
    SAY_DAJIK_FRENZYHEART_HILL              = 4,
    SAY_DAJIK_KILLED_SERFEX                 = 5,
    SAY_DAJIK_KILLED_WASP_QUEEN             = 6,
    SAY_DAJIK_NETTED_CHICKEN                = 7,
    SAY_DAJIK_JUST_DIED                     = 8,
    SAY_DAJIK_SKYREACH_LIFEBLOOD_PILLAR     = 9,
    SAY_DAJIK_KILLED_MISTWHISPERS           = 10,

    SAY_ZEPIK_ON_JUST_SUMMONED              = 2,
    SAY_ZEPIK_FRENZYHEART_HILL              = 3,
    SAY_ZEPIK_KILLED_MISTWHISPERS           = 4,
    SAY_ZEPIK_KILLED_UNIT                   = 5,
    SAY_ZEPIK_NETTED_CHICKEN                = 6,
    SAY_ZEPIK_MISTWHISPER_REFUGE            = 7,

    SAY_JALOOT_ON_JUST_SUMMONED             = 1,
    SAY_JALOOT_CANOPY_AREA                  = 3,
    SAY_JALOOT_OWNER_WENT_AFK               = 4,
    SAY_JALOOT_APPROACH_ZEPIK               = 9,
    SAY_JALOOT_KILLED_UNIT                  = 10,
    SAY_JALOOT_KILLED_TARTEK                = 11,
    SAY_JALOOT_ENTER_COMBAT_TARTEK          = 12,

    SAY_MOODLE_OWNER_WENT_AFK               = 1,
    SAY_MOODLE_KILLED_UNIT                  = 4,
    SAY_MOODLE_ENTER_COMBAT                 = 9,
    SAY_MOODLE_CANOPY_AREA                  = 10,
    SAY_MOODLE_MOSSWALKER_VILLAGE           = 11,
    SAY_MOODLE_JUST_SUMMONED                = 12,

    SAY_LAFOO_OWNER_WENT_AFK                = 1,
    SAY_LAFOO_KILLED_UNIT                   = 4,
    SAY_LAFOO_CANOPY_AREA                   = 9,
    SAY_LAFOO_MOSSWALKER_VILLAGE            = 10,
    SAY_LAFOO_JUST_SUMMONED                 = 11,

    SAY_ARTRUIS_ENTER_COMBAT                = 0,
    SAY_ARTRUIS_REACHED_EIGHTY_PERCENT_HP   = 1,
    SAY_ARTRUIS_REACHED_SIXTY_PERCENT_HP    = 2,
    SAY_ARTRUIS_REACHED_THIRTY_PERCENT_HP   = 3,
    SAY_ATRUIS_SHIELDED                     = 4,
    SAY_ARTRUIS_REACHED_FIVE_PERCENT_HP     = 5,

    SAY_ZEPIK_JALOOT_SWITCH_FREED           = 0,
};

class zepik_cast_barbed_net_on_aggro : public BasicEvent
{
    public:
        zepik_cast_barbed_net_on_aggro(Creature* zepik) : _zepik(zepik) { }

        bool Execute(uint64 /*time*/, uint32 /*diff*/)
        {
            if (_zepik && _zepik->getVictim())
                _zepik->AI()->DoCast(_zepik->getVictim(), SPELL_BARBED_NET);

            return true;
        }

    private:
        Creature* _zepik;
};

class npc_artruis_the_heartless : public CreatureScript
{
    public:
        npc_artruis_the_heartless() : CreatureScript("npc_artruis_the_heartless") { }

        struct npc_artruisAI : public ScriptedAI
        {
            npc_artruisAI(Creature* creature) : ScriptedAI(creature) { }

            uint8 talkPhase;
            bool wasImmune;
            bool isBound;

            void Reset()
            {
                _events.Reset();
                me->SetReactState(REACT_AGGRESSIVE);
                wasImmune = false;
                isBound = false;
                talkPhase = 0;

                if (GameObject* phylactery = me->FindNearestGameObject(GAMEOBJECT_ARTRUISS_PHYLACTERY, 20.0f))
                    phylactery->Delete();
            }

            void JustRespawned()
            {
                if (Creature* zepik = me->FindNearestCreature(NPC_ZEPIK_SWITCH, 60.0f, false))
                    zepik->Respawn();
                else if (Creature* zepik = me->FindNearestCreature(NPC_ZEPIK_SWITCH, 60.0f, true))
                    zepik->AI()->Reset();

                if (Creature* jaloot = me->FindNearestCreature(NPC_JALOOT_SWITCH, 60.0f, false))
                    jaloot->Respawn();
                else if (Creature* jaloot = me->FindNearestCreature(NPC_JALOOT_SWITCH, 60.0f, true))
                    jaloot->AI()->Reset();
            }

            void EnterCombat(Unit* /*who*/)
            {
                _events.ScheduleEvent(EVENT_ARTRUIS_FROSTBOLT, 1000);
                _events.ScheduleEvent(EVENT_ARTRUIS_ICE_LANCE, 200);
                _events.ScheduleEvent(EVENT_ARTRUIS_FROST_NOVA, urand(100, 3000));
                _events.ScheduleEvent(EVENT_ARTRUIS_ICY_VEINS, 4500);
            }

            void JustDied(Unit* /*killer*/)
            {
                //Talk(SAY_ARTRUIS_DIED);
                DoCast(me, SPELL_SUMMON_ARTRUIS_URN, true);

                if (Creature* zepik = me->FindNearestCreature(NPC_ZEPIK_SWITCH, 100.0f, true))
                    zepik->AI()->DoAction(ACTION_ARTRUIS_SET_QUESTGIVER);
                else if (Creature* jaloot = me->FindNearestCreature(NPC_JALOOT_SWITCH, 100.0f, true))
                    jaloot->AI()->DoAction(ACTION_ARTRUIS_SET_QUESTGIVER);
            }

            void DoAction(int32 const action)
            {
                if (action == ACTION_ARTRUIS_UNBIND)
                {
                    me->RemoveAurasDueToSpell(SPELL_ARTRUIS_BINDINGS);
                    isBound = false;
                }
            }

            void UpdateAI(uint32 const diff)
            {
                _events.Update(diff);

                if (!UpdateVictim())
                    return;

                Unit* victim = me->getVictim();

                if (talkPhase == SAY_ARTRUIS_ENTER_COMBAT)
                {
                    Talk(SAY_ARTRUIS_ENTER_COMBAT);
                    talkPhase++;
                }
                else if (me->GetHealthPct() <= 80.0f && talkPhase++ == SAY_ARTRUIS_REACHED_EIGHTY_PERCENT_HP)
                    Talk(SAY_ARTRUIS_REACHED_EIGHTY_PERCENT_HP);
                else if (me->GetHealthPct() <= 60.0f && talkPhase++ == SAY_ARTRUIS_REACHED_SIXTY_PERCENT_HP)
                    Talk(SAY_ARTRUIS_REACHED_SIXTY_PERCENT_HP);
                else if (me->GetHealthPct() <= 5.0f && talkPhase++ == SAY_ARTRUIS_REACHED_FIVE_PERCENT_HP)
                    Talk(SAY_ARTRUIS_REACHED_FIVE_PERCENT_HP);

                if (HealthBelowPct(30) && !wasImmune)
                {
                    me->InterruptNonMeleeSpells(true);
                    //Talk(SAY_ARTRUIS_USED_BINDINGS);
                    DoCast(SPELL_ARTRUIS_BINDINGS);
                    Talk(SAY_ARTRUIS_REACHED_THIRTY_PERCENT_HP);
                    Talk(SAY_ATRUIS_SHIELDED);
                    talkPhase += 2;

                    if (Creature* zepik = me->FindNearestCreature(NPC_ZEPIK_SWITCH, 60.0f, true))
                        zepik->AI()->DoAction(ACTION_ARTRUIS_BIND);

                    if (Creature* jaloot = me->FindNearestCreature(NPC_JALOOT_SWITCH, 60.0f, true))
                        jaloot->AI()->DoAction(ACTION_ARTRUIS_BIND);

                    wasImmune = true;
                    isBound = true;
                }

                if (isBound || me->HasUnitState(UNIT_STATE_CASTING))
                    return;

                while (uint8 eventId = _events.ExecuteEvent())
                {
                    switch (eventId)
                    {
                        case EVENT_ARTRUIS_FROSTBOLT:
                            DoCastVictim(SPELL_ARTRUIS_FROSTBOLT);
                            _events.ScheduleEvent(EVENT_ARTRUIS_FROSTBOLT, 4000);
                            break;
                        case EVENT_ARTRUIS_ICE_LANCE:
                            DoCastVictim(SPELL_ARTRUIS_ICE_LANCE);
                            _events.ScheduleEvent(EVENT_ARTRUIS_ICE_LANCE, 7000);
                            break;
                        case EVENT_ARTRUIS_FROST_NOVA:
                            DoCastVictim(SPELL_ARTRUIS_FROST_NOVA);
                            _events.ScheduleEvent(EVENT_ARTRUIS_FROST_NOVA, 14000);
                            break;
                        case EVENT_ARTRUIS_ICY_VEINS:
                            DoCast(me, SPELL_ARTRUIS_ICY_VEINS);
                            _events.ScheduleEvent(EVENT_ARTRUIS_ICY_VEINS, 25000);
                            break;
                    }
                }

                DoMeleeAttackIfReady();
            }

        private:
            EventMap _events;
        };

        CreatureAI* GetAI(Creature* creature) const
        {
            return new npc_artruisAI(creature);
        }
};

class npc_zepik_jaloot_switch : public CreatureScript
{
    public:
        npc_zepik_jaloot_switch() : CreatureScript("npc_zepik_jaloot_switch") { }

        struct npc_zepik_jaloot_switchAI : public ScriptedAI
        {
            npc_zepik_jaloot_switchAI(Creature* creature) : ScriptedAI(creature)
            {
                //! Only on initialize
                isQuestGiver = false;
                me->RemoveFlag(UNIT_NPC_FLAGS, UNIT_NPC_FLAG_QUESTGIVER);
            }

            bool isQuestGiver;

            void Reset()
            {
                _events.Reset();
                me->GetMotionMaster()->MoveTargetedHome();

                // If Artruis is dead, don't execute the code below
                if (me->FindNearestCreature(NPC_ARTRUIS_THE_HEARTLESS, 60.0f, false))
                    return;

                //me->setFaction(35);
				 if (isQuestGiver == true)
				{
		          me->RemoveAurasDueToSpell(SPELL_ARTRUIS_BINDINGS);
				}
				else
				{
				  me->AddAura(SPELL_ARTRUIS_TOMB_OF_HEARTLESS, me);
				}
            }

            void JustDied(Unit* /*killer*/)
            {
                if (Creature* artruis = me->FindNearestCreature(NPC_ARTRUIS_THE_HEARTLESS, 100.0f, true))
                {
                    uint32 assistantEntry = me->GetEntry() == NPC_ZEPIK_SWITCH ? NPC_JALOOT_SWITCH : NPC_ZEPIK_SWITCH;

                    if (Creature* assistant = me->FindNearestCreature(assistantEntry, 60.0f, true))
                    {
                        artruis->AI()->DoAction(ACTION_ARTRUIS_UNBIND);
                        assistant->RemoveAurasDueToSpell(SPELL_ARTRUIS_BINDINGS);
                        assistant->AddThreat(artruis, 25000.0f);
                        assistant->GetMotionMaster()->MoveChase(artruis);
                        assistant->setFaction(35);
                        assistant->AI()->AttackStart(artruis);
                        assistant->AI()->Talk(SAY_ZEPIK_JALOOT_SWITCH_FREED);
                    }
                }
            }

            void EnterCombat(Unit* /*who*/)
            {
                switch (me->GetEntry())
                {
                    case NPC_ZEPIK_SWITCH:
                        _events.ScheduleEvent(EVENT_ZEPIK_OPEN_WOUND, 2000);
                        _events.ScheduleEvent(EVENT_ZEPIK_PLACE_SPIKE_TRAP, 4000);
                        break;
                    case NPC_JALOOT_SWITCH:
                        _events.ScheduleEvent(EVENT_JALOOT_LIGHTNING_WHIRL, 2000);
                        _events.ScheduleEvent(EVENT_JALOOT_SPARK_FRENZY, 4000);
                        break;
                    default:
                        break;
                }
            }

            void DoAction(int32 const action)
            {
                switch (action)
                {
                    case ACTION_ARTRUIS_BIND:
                        if (Creature* artruis = me->FindNearestCreature(NPC_ARTRUIS_THE_HEARTLESS, 120.0f, true))
                        {
                            if (Unit* victim = artruis->getVictim())
                            {
                                me->RemoveAurasDueToSpell(SPELL_ARTRUIS_TOMB_OF_HEARTLESS);
                                me->setFaction(FACTION_ZEPIK_JALOOT_OFFENSIVE);
                                me->GetMotionMaster()->MoveChase(victim);
                                AttackStart(victim);
                            }
                        }
                        break;
                    case ACTION_ARTRUIS_SET_QUESTGIVER:
                        isQuestGiver = true;
                        me->setFaction(35);
                        me->SetFlag(UNIT_NPC_FLAGS, UNIT_NPC_FLAG_QUESTGIVER);
                        me->RemoveAurasDueToSpell(SPELL_ARTRUIS_TOMB_OF_HEARTLESS); // Just in case
                        break;
                    default:
                        break;
                }
            }

            void UpdateAI(uint32 const diff)
            {
                _events.Update(diff);

                if (!UpdateVictim())
                    return;

                if (me->HasUnitState(UNIT_STATE_CASTING))
                    return;

                while (uint8 eventId = _events.ExecuteEvent())
                {
                    switch (eventId)
                    {
                        case EVENT_ZEPIK_OPEN_WOUND:
                            DoCastVictim(SPELL_OPEN_WOUND);
                            _events.ScheduleEvent(EVENT_ZEPIK_OPEN_WOUND, 4000);
                            break;
                        case EVENT_ZEPIK_PLACE_SPIKE_TRAP:
                            DoCastVictim(SPELL_PLACE_SPIKE_TRAP);
                            _events.ScheduleEvent(EVENT_ZEPIK_PLACE_SPIKE_TRAP, 4000);
                            break;
                        case EVENT_JALOOT_LIGHTNING_WHIRL:
                            DoCastVictim(SPELL_LIGHTNING_WHIRL);
                            _events.ScheduleEvent(EVENT_JALOOT_LIGHTNING_WHIRL, 4000);
                            break;
                        case EVENT_JALOOT_SPARK_FRENZY:
                            DoCastVictim(SPELL_SPARK_FRENZY);
                            _events.ScheduleEvent(EVENT_JALOOT_SPARK_FRENZY, 14000);
                            break;
                    }
                }

                DoMeleeAttackIfReady();
            }

        private:
            EventMap _events;
        };

        CreatureAI* GetAI(Creature* creature) const
        {
            return new npc_zepik_jaloot_switchAI(creature);
        }
};

class npc_hardknuckle_charger_proxy : public CreatureScript
{
    public:
        npc_hardknuckle_charger_proxy() : CreatureScript("npc_hardknuckle_charger_proxy") { }

        struct npc_hardknuckle_charger_proxyAI : public ScriptedAI
        {
            npc_hardknuckle_charger_proxyAI(Creature* creature) : ScriptedAI(creature) { }

            std::list<Creature*> softknuckles;
            uint8 fearedSoftknuckles;
            uint32 CheckTimer;

            void Reset()
            {
                softknuckles.clear();
                fearedSoftknuckles = 0;
                CheckTimer = 3000;
            }

            void UpdateAI(uint32 const diff)
            {
                if (CheckTimer <= diff)
                {
                    softknuckles.clear();

                    if (me->FindNearestCreature(NPC_HARDKNUCKLE_MATRIARCH, 50.0f, true))
                    {
                        CheckTimer = 3000;
                        return;
                    }

                    me->GetCreatureListWithEntryInGrid(softknuckles, NPC_SOFTKNUCKLE, 100.0f);
                    for (std::list<Creature*>::iterator itr = softknuckles.begin(); itr != softknuckles.end(); ++itr)
                        if ((*itr)->HasAura(SPELL_AURA_SCARED_SOFTKNUCKLE))
                            fearedSoftknuckles++;

                    if (fearedSoftknuckles >= 5)
                    {
                        if (Creature* goregek = me->FindNearestCreature(NPC_GOREGEK, 30.0f))
                            if (goregek->isSummon())
                                goregek->AI()->Talk(SAY_GOREGEK_OWNER_SCARED_SOFTKNUCKLE);

                        me->SummonCreature(NPC_HARDKNUCKLE_MATRIARCH, 5123.732910f, 3985.970947f, -61.940193f, 0.809257f, TEMPSUMMON_TIMED_OR_DEAD_DESPAWN, 60000);
                        fearedSoftknuckles = 0;
                    }

                    CheckTimer = 3000;
                }
                else
                    CheckTimer -= diff;
            }
        };

        CreatureAI* GetAI(Creature* creature) const
        {
            return new npc_hardknuckle_charger_proxyAI(creature);
        }
};

class npc_softknuckle : public CreatureScript
{
    public:
        npc_softknuckle() : CreatureScript("npc_softknuckle") { }

        struct npc_softknuckleAI : public ScriptedAI
        {
            npc_softknuckleAI(Creature* creature) : ScriptedAI(creature) { }

            void SpellHit(Unit* caster, const SpellInfo* spell)
            {
                if (spell->Id == SPELL_AURA_SCARED_SOFTKNUCKLE)
                    if (Creature* goregek = caster->FindNearestCreature(NPC_GOREGEK, 5.0f))
                        if (goregek->IsControlledByPlayer() && goregek->GetOwnerGUID() == caster->GetGUID())
                            if (urand(0, 2) == 0)
                                goregek->AI()->Talk(SAY_GOREGEK_OWNER_SCARED_SOFTKNUCKLE);
            }
        };

        CreatureAI* GetAI(Creature* creature) const
        {
            return new npc_softknuckleAI(creature);
        }
};


class npc_frenzyheart_oracle_companions : public CreatureScript
{
    public:
        npc_frenzyheart_oracle_companions() : CreatureScript("npc_frenzyheart_oracle_companions") { }

        struct npc_frenzyheart_oracle_companionsAI : public ScriptedAI
        {
            npc_frenzyheart_oracle_companionsAI(Creature* creature) : ScriptedAI(creature) { }
            
            bool mentionedKnucklesArea;
            bool movingToTreasureTrigger;
            bool reachedTreasureTrigger;
            bool mentionedDiggingUpLine;
            bool recentlyMentionedAFK;
            bool recentlyReachedArea;
            uint8 afkSecondsCount;
            uint32 TreasureSpell;
            uint64 OracleTriggerGUID;

            void Reset()
            {
                _events.Reset();
                _events.ScheduleEvent(EVENT_ALL_COMPANIONS_RANDOM_SAY, urand(20000, 140000));
                _events.ScheduleEvent(EVENT_ALL_CHECK_OWNER_HAS_SUMMONER_ITEM, 1000);
                _events.ScheduleEvent(EVENT_ALL_CHECK_ZONE_ID, 5000);
                me->SetReactState(REACT_AGGRESSIVE);
                mentionedKnucklesArea = false;
                reachedTreasureTrigger = false;
                movingToTreasureTrigger = false;
                mentionedDiggingUpLine = false;
                recentlyReachedArea = false;
                TreasureSpell = 0;
                OracleTriggerGUID = 0;

                if (me->isSummon() && me->ToTempSummon() && me->GetOwner())
                {
                    if (Unit* owner = me->ToTempSummon()->GetSummoner())
                    {
                        switch (me->GetEntry())
                        {
                            case NPC_DAJIK:
                                _events.ScheduleEvent(EVENT_ALL_CHECK_AREA_ID, 3000);
                                _events.ScheduleEvent(EVENT_ALL_CAST_NET_ON_CHICKENS, urand(5000, 10000));
                                _events.ScheduleEvent(EVENT_ALL_CHECK_OWNERS_HEALTH, urand(30000, 180000));
                                break;
                            case NPC_ZEPIK:
                                _events.ScheduleEvent(EVENT_ALL_CAST_NET_ON_CHICKENS, urand(5000, 10000));
                                _events.ScheduleEvent(EVENT_ALL_CHECK_OWNERS_HEALTH, urand(30000, 180000));
                                break;
                            case NPC_GOREGEK:
                                _events.ScheduleEvent(EVENT_ALL_CHECK_AREA_ID, 3000);
                                _events.ScheduleEvent(EVENT_ALL_CAST_NET_ON_CHICKENS, urand(5000, 10000));
                                break;
                            case NPC_JALOOT:
                                _events.ScheduleEvent(EVENT_ALL_CHECK_OWNER_AFK, 1000);
                                _events.ScheduleEvent(EVENT_ALL_SEARCH_FOR_TREASURE, 250);
                                break;
                            case NPC_MOODLE:
                                _events.ScheduleEvent(EVENT_ALL_CHECK_OWNER_AFK, 1000);
                                _events.ScheduleEvent(EVENT_ALL_SEARCH_FOR_TREASURE, 250);
                                break;
                            case NPC_LAFOO:
                                _events.ScheduleEvent(EVENT_LAFOO_CHECK_HEALTH, 5000);
                                _events.ScheduleEvent(EVENT_ALL_SEARCH_FOR_TREASURE, 250);
                                break;
                            default:
                                break;
                        }
                    }
                }
            }

            void JustRespawned()
            {
                StartFollowingOwner();
            }

            void IsSummonedBy(Unit* summoner)
            {
                switch (me->GetEntry())
                {
                    case NPC_ZEPIK:
                        if (urand(0, 2) == 0)
                            Talk(SAY_ZEPIK_ON_JUST_SUMMONED);
                        break;
                    case NPC_JALOOT:
                        if (urand(0, 2) == 0)
                            Talk(SAY_JALOOT_ON_JUST_SUMMONED);
                        break;
                    case NPC_MOODLE:
                        if (urand(0, 2) == 0)
                            Talk(SAY_MOODLE_JUST_SUMMONED);
                        break;
                    case NPC_LAFOO:
                        if (urand(0, 2) == 0)
                            Talk(SAY_LAFOO_JUST_SUMMONED);
                        break;
                    default:
                        break;
                }
            }

            void StartFollowingOwner()
            {
                if (!me->isSummon() || !me->GetOwner() || !me->ToTempSummon())
                    return;

                if (Unit* owner = me->ToTempSummon()->GetSummoner())
                {
                    if (Player* player = owner->ToPlayer())
                    {
                        me->HandleEmoteCommand(EMOTE_ONESHOT_NONE); //! Stop emoting in case
                        me->SetControlled(false, UNIT_STATE_ROOT); //! Removes UNIT_STATE_ROOT, else there's no way of removing it...
                        me->GetMotionMaster()->MoveFollow(player, PET_FOLLOW_DIST, PET_FOLLOW_ANGLE);
                    }
                }
            }

            void StopFollowingOwner()
            {
                if (!me->isSummon() || !me->GetOwner() || !me->ToTempSummon())
                    return;

                me->StopMoving();
                me->GetMotionMaster()->Clear();
                me->SetControlled(true, UNIT_STATE_ROOT);
                me->GetMotionMaster()->MoveIdle();
            }

            //! Overwriting original version as it is called after EnterCombat, making the MoveChase not 15.0f but 0.0f.
            //! This took a long time to figure out, so in order to never forget I've also created a SAI action called
            //! SMART_ACTION_RANGED_MOVEMENT. :)
            void AttackStart(Unit* victim)
            {
                if (!me->isSummon() || !me->GetOwner() || !me->ToTempSummon())
                    return;

                if (me->GetEntry() == NPC_ZEPIK)
                {
                    if (me->GetSheath() != SHEATH_STATE_RANGED && !me->IsWithinMeleeRange(victim))
                        me->SetSheath(SHEATH_STATE_RANGED);
                    else if (me->GetSheath() != SHEATH_STATE_MELEE && me->IsWithinMeleeRange(victim))
                        me->SetSheath(SHEATH_STATE_MELEE);

                    me->IsInRange(victim, 0.0f, 3.0f) ? me->GetMotionMaster()->MoveChase(victim) : me->GetMotionMaster()->MoveChase(victim, 15.0f, 0.0f);

                    _events.ScheduleEvent(EVENT_ZEPIK_OPEN_WOUND, urand(1000, 5000));
                    _events.ScheduleEvent(EVENT_ZEPIK_PIERCING_ARROW, 100);
                    //DoCast(victim, SPELL_PIERCING_ARROW);

                    if (urand(0, 2) == 0)
                        me->m_Events.AddEvent(new zepik_cast_barbed_net_on_aggro(me), me->m_Events.CalculateTime(500));
                }
                else
                    ScriptedAI::AttackStart(victim); //! If we are not Zepik, don't overwrite anything
            }

            void EnterCombat(Unit* who)
            {
                if (who->GetTypeId() != TYPEID_UNIT || !me->isSummon() || !me->GetOwner() || !me->ToTempSummon())
                    return;

                switch (me->GetEntry())
                {
                    case NPC_DAJIK:
                        _events.ScheduleEvent(EVENT_DAJIK_EARTHS_FURY, urand(15000, 5000));
                        _events.ScheduleEvent(EVENT_DAJIK_EARTHS_WRATH, urand(1000, 7000));

                        if (who->GetEntry() == NPC_SERFEX_THE_REAVER)
                            Talk(SAY_DAJIK_AGGRO_SERFEX);
                        break;
                    case NPC_GOREGEK:
                        _events.ScheduleEvent(EVENT_GOREGEK_DISARM, urand(5000, 15000));
                        _events.ScheduleEvent(EVENT_GOREGEK_SUNDER_ARMOR, urand(500, 5000));
                        _events.ScheduleEvent(EVENT_GOREGEK_PIERCING_HOWL, urand(1000, 19000));

                        switch (who->GetEntry())
                        {
                            case NPC_HARDKNUCKLE_CHARGER:
                            case NPC_HARDKNUCKLE_FORAGER:
                                Talk(SAY_GOREGEK_ENTER_COMBAT_HARDKNUCKLE);
                                break;
                            default:
                                if (urand(0, 1) == 0)
                                    Talk(SAY_GOREGEK_ENTER_COMBAT);
                                break;
                        }
                        break;
                    case NPC_JALOOT:
                        _events.ScheduleEvent(EVENT_JALOOT_LIGHTNING_WHIRL, 2000);
                        _events.ScheduleEvent(EVENT_JALOOT_SPARK_FRENZY, 4000);

                        if (who->ToCreature() && who->GetEntry() == NPC_WARLORD_TARTEK)
                            if (urand(0, 1) == 0)
                                Talk(SAY_JALOOT_ENTER_COMBAT_TARTEK);
                        break;
                    case NPC_MOODLE:
                        _events.ScheduleEvent(EVENT_MOODLE_FROST_BREATH, urand(4000, 18000));
                        _events.ScheduleEvent(EVENT_MOODLE_FRIGID_ABSORBTION, urand(1000, 28000));
                        _events.ScheduleEvent(EVENT_MOODLE_VICIOUS_ROAR, urand(1500, 8000));
                        _events.ScheduleEvent(EVENT_MOODLE_WHIRLWIND, urand(2000, 20000));

                        if (urand(0, 1) == 0)
                            DoCast(me, SPELL_DISPERSAL);

                        if (urand(0, 1) == 0)
                            Talk(SAY_MOODLE_ENTER_COMBAT);
                        break;
                    case NPC_ZEPIK:
                        _events.ScheduleEvent(EVENT_ZEPIK_OPEN_WOUND, 2000);
                        _events.ScheduleEvent(EVENT_ZEPIK_PLACE_SPIKE_TRAP, 4000);
                        break;
                    case NPC_LAFOO:
                        _events.ScheduleEvent(EVENT_LAFOO_TOXIC_SPIT, urand(1000, 13000));
                        break;
                    default:
                        break;
                }
            }

            void JustDied(Unit* killer)
            {
                if (!me->isSummon() || !me->GetOwner() || !me->ToTempSummon())
                    return;

                me->RemoveAllAuras();

                switch (me->GetEntry())
                {
                    case NPC_GOREGEK:
                        switch (killer->GetEntry())
                        {
                            case NPC_HARDKNUCKLE_CHARGER:
                            case NPC_HARDKNUCKLE_FORAGER:
                                Talk(SAY_GOREGEK_KILLED_BY_HARDKNUCKLE);
                                break;
                        }
                        break;
                    case NPC_DAJIK:
                        if (urand(0, 1) == 0)
                            Talk(SAY_DAJIK_JUST_DIED);
                        break;
                    default:
                        break;
                }
            }

            void KilledUnit(Unit* victim)
            {
                if (!victim || !me->isSummon() || !me->GetOwner() || !me->ToTempSummon())
                    return;

                switch (me->GetEntry())
                {
                    case NPC_DAJIK:
                        switch (victim->GetEntry())
                        {
                            case NPC_SERFEX_THE_REAVER:
                                Talk(SAY_DAJIK_KILLED_SERFEX);
                                break;
                            case NPC_WASP_QUEEN:
                                Talk(SAY_DAJIK_KILLED_WASP_QUEEN);
                                break;
                            case NPC_MISTWHISPER_ORACLE:
                            case NPC_MISTWHISPER_WARRIOR:
                                if (urand(0, 1) == 0)
                                    Talk(SAY_DAJIK_KILLED_MISTWHISPERS);
                                break;
                            default:
                                if (urand(0, 2) == 0)
                                    Talk(SAY_DAJIK_KILLED_UNIT);
                                break;
                        }
                        break;
                    case NPC_ZEPIK:
                        switch (victim->GetEntry())
                        {
                            case NPC_MISTWHISPER_WARRIOR:
                            case NPC_MISTWHISPER_ORACLE:
                                if (urand(0, 1) == 0)
                                    Talk(SAY_ZEPIK_KILLED_MISTWHISPERS);
                                break;
                            default:
                                if (urand(0, 2) == 0)
                                    Talk(SAY_ZEPIK_KILLED_UNIT);
                                break;
                        }
                        break;
                    case NPC_GOREGEK:
                        switch (victim->GetEntry())
                        {
                            case NPC_PITCH:
                                Talk(SAY_GOREGEK_KILLED_PITCH);
                                break;
                            default:
                                if (urand(0, 2) == 0)
                                    Talk(SAY_GOREGEK_KILLED_UNIT);
                                break;
                        }

                        /*if (HealthBelowPct(65) && me->IsWithinDist(victim, 3.0f))
                        {
                            //if (!me->isInCombat() && me->GetOwner() && !me->GetOwner()->isInCombat())
                            //{
                                if (victim->GetCreatureTypeMask() & CREATURE_TYPEMASK_HUMANOID_OR_UNDEAD)
                                {
                                    me->SetControlled(true, UNIT_STATE_ROOT);
                                    DoCast(me, SPELL_VORACIOUS_APPETITE, false);
                                    _events.ScheduleEvent(EVENT_ALL_FOLLOW_AGAIN, 6500);
                                }
                            //}
                        }*/
                        break;
                    case NPC_JALOOT:
                        switch (victim->GetEntry())
                        {
                            case NPC_WARLORD_TARTEK:
                                Talk(SAY_JALOOT_KILLED_TARTEK);
                                break;
                            default:
                                if (urand(0, 2) == 0)
                                    Talk(SAY_JALOOT_KILLED_UNIT);
                                break;
                        }
                        break;
                    case NPC_MOODLE:
                        if (urand(0, 2) == 0)
                            Talk(SAY_MOODLE_KILLED_UNIT);
                        break;
                    case NPC_LAFOO:
                        if (urand(0, 2) == 0)
                            Talk(SAY_LAFOO_KILLED_UNIT);
                        break;
                    default:
                        break;
                }
            }

            void SpellHit(Unit* /*caster*/, SpellInfo const* spell)
            {
                if (recentlyReachedArea || !me->isSummon() || !me->GetOwner() || !me->ToTempSummon())
                    return;

                if (Unit* owner = me->ToTempSummon()->GetSummoner())
                {
                    if (Player* player = owner->ToPlayer())
                    {
                        switch (spell->Id)
                        {
                            case SPELL_REACHED_FRENZYHEART_HILL:
                                if (!recentlyReachedArea)
                                    _events.ScheduleEvent(EVENT_ALL_RESET_RECENTLY_REACHED_BOOL, 60000);

                                recentlyReachedArea = true;

                                switch (me->GetEntry())
                                {
                                    case NPC_DAJIK:
                                        Talk(SAY_DAJIK_FRENZYHEART_HILL);
                                        break;
                                    case NPC_ZEPIK:
                                        Talk(SAY_ZEPIK_FRENZYHEART_HILL);
                                        break;
                                    case NPC_GOREGEK:
                                        Talk(SAY_GOREGEK_FRENZYHEART_HILL);
                                        break;
                                    default:
                                        break;
                                }
                                break;
                                // Can't find a spell for these..
                            /*case SPELL_REACHED_HARDKNUCKLE_CLEARING:
                                if (!recentlyReachedArea)
                                    _events.ScheduleEvent(EVENT_ALL_RESET_RECENTLY_REACHED_BOOL, 60000);

                                recentlyReachedArea = true;

                                switch (me->GetEntry())
                                {
                                    case NPC_DAJIK:
                                        Talk(SAY_DAJIK_POKE_KNUCKLES);
                                        break;
                                    case NPC_GOREGEK:
                                        Talk(SAY_GOREGEK_HARDKNUCKLES_CLEARING);
                                        break;
                                    default:
                                        break;
                                }
                                break;*/
                            case SPELL_REACHED_MOSSWALKER_VILLAGE:
                                if (!recentlyReachedArea)
                                    _events.ScheduleEvent(EVENT_ALL_RESET_RECENTLY_REACHED_BOOL, 30000);

                                recentlyReachedArea = true;

                                if (Player* owner = me->GetOwner()->ToPlayer())
                                {
                                    if (owner->GetQuestStatus(QUEST_THE_ANGRY_GORLOC) == QUEST_STATUS_INCOMPLETE)
                                        owner->GroupEventHappens(QUEST_THE_ANGRY_GORLOC, me);

                                    switch (me->GetEntry())
                                    {
                                        case NPC_MOODLE:
                                            Talk(SAY_MOODLE_MOSSWALKER_VILLAGE);
                                            break;
                                        case NPC_LAFOO:
                                            Talk(SAY_LAFOO_MOSSWALKER_VILLAGE);
                                            break;
                                    }
                                }
                                break;
                            case SPELL_REACHED_MISTWHISPER_REFUGE:
                                if (!recentlyReachedArea)
                                    _events.ScheduleEvent(EVENT_ALL_RESET_RECENTLY_REACHED_BOOL, 60000);

                                recentlyReachedArea = true;

                                if (me->GetEntry() == NPC_ZEPIK)
                                    Talk(SAY_ZEPIK_MISTWHISPER_REFUGE);
                                break;
                            case SPELL_REACHED_RAINSPEAKER_CANOPY:
                                if (!recentlyReachedArea)
                                    _events.ScheduleEvent(EVENT_ALL_RESET_RECENTLY_REACHED_BOOL, 60000);

                                recentlyReachedArea = true;

                                switch (me->GetEntry())
                                {
                                    case NPC_JALOOT:
                                        Talk(SAY_JALOOT_CANOPY_AREA);
                                        break;
                                    case NPC_MOODLE:
                                        Talk(SAY_MOODLE_CANOPY_AREA);
                                        break;
                                    case NPC_LAFOO:
                                        Talk(SAY_LAFOO_CANOPY_AREA);
                                        break;
                                    default:
                                        break;
                                }
                                break;
                            case SPELL_REACHED_SKYREACH_PILLAR:
                            case SPELL_REACHED_LIFEBLOOD_PILLAR:
                                if (!recentlyReachedArea)
                                    _events.ScheduleEvent(EVENT_ALL_RESET_RECENTLY_REACHED_BOOL, 60000);

                                recentlyReachedArea = true;

                                if (me->GetEntry() == NPC_DAJIK)
                                    Talk(SAY_DAJIK_SKYREACH_LIFEBLOOD_PILLAR);
                                break;
                            case SPELL_REACHED_KARTAKS_HOLD:
                                recentlyReachedArea = true;

                                if (me->GetEntry() == NPC_GOREGEK)
                                    Talk(SAY_GOREGEK_KARTAKS_HOLD);
                                break;
                            //! Can't find proper texts for these four areas that
                            //! should be said by the companions... =/
                            case SPELL_REACHED_SPEARBORN_ENCAMPMENT:
                            case SPELL_REACHED_SAPPHIRE_HIVE:
                            case SPELL_REACHED_SPARKTOUCHED_HAVEN:
                            case SPELL_REACHED_RIVERS_HEART:
                                if (!recentlyReachedArea)
                                    _events.ScheduleEvent(EVENT_ALL_RESET_RECENTLY_REACHED_BOOL, 60000);

                                recentlyReachedArea = true;
                                break;
                            default:
                                break;
                        }
                    }
                }
            }

            void MovementInform(uint32 type, uint32 point)
            {
                if (type != POINT_MOTION_TYPE)
                    return;

                switch (point)
                {
                    case POINT_MOVE_TO_ORACLE_TRIGGER:
                        movingToTreasureTrigger = false;

                        if (!reachedTreasureTrigger)
                            reachedTreasureTrigger = true;
                        break;
                    /*case POINT_MOVE_TO_UNDEAD_HUMAN_BODY:
                        sLog->outString("POINT_MOVE_TO_UNDEAD_HUMAN_BODY");
                        //me->SetControlled(true, UNIT_STATE_ROOT);
                        DoCast(me, SPELL_VORACIOUS_APPETITE, false);
                        _events.ScheduleEvent(EVENT_ALL_FOLLOW_AGAIN, 6500);
                        break;*/
                    default:
                        break;
                }
            }

            void UpdateAI(uint32 const diff)
            {
                _events.Update(diff);

              /*sLog->outString("OWNER - Walk: %f Run: %f", me->GetOwner()->GetSpeed(MOVE_WALK), me->GetOwner()->GetSpeed(MOVE_RUN));
                sLog->outString("ME    - Walk: %f Run: %f", me->GetSpeed(MOVE_WALK), me->GetSpeed(MOVE_RUN));
                if (me->isSummon() && me->GetOwner())
                {
                    me->SetSpeed(MOVE_WALK, me->GetOwner()->GetSpeed(MOVE_WALK), false);
                    me->SetSpeed(MOVE_RUN, me->GetOwner()->GetSpeed(MOVE_RUN), false);
                    me->SetSpeed(MOVE_SWIM, me->GetOwner()->GetSpeed(MOVE_SWIM), false);
                }*/

                if (!me->isSummon() || !me->GetOwner() || !me->ToTempSummon())
                    return;

                //! Once our owner uses his/her flying mount, we despawn.
                //! This isn't how they did it on retail, because on retail
                //! they follow their owner at the speed of their owner's
                //! mount while walking on the ground, but this looks really
                //! ugly using the methods we have in the current sourcecode
                //! and setting speed is completely fucked up (might be
                //! partly caused by the increased speed of players).
                if (Unit* owner = me->GetOwner())
                    if (owner->HasAuraType(SPELL_AURA_FLY) || owner->HasAuraType(SPELL_AURA_MOD_INCREASE_MOUNTED_FLIGHT_SPEED))
                        me->DespawnOrUnsummon();

                if (UpdateVictim())
                    DoMeleeAttackIfReady();

                if (me->GetOwner() && !me->IsWithinDist(me->GetOwner(), 50.0f))
                    me->DespawnOrUnsummon();

                /*if (zepikUsingRanged && me->GetEntry() == NPC_ZEPIK)
                {
                    if (!UpdateVictim() || !me->IsInRange(me->getVictim(), 10.0f, 30.0f))
                    {
                        me->SetSheath(SHEATH_STATE_MELEE);
                        zepikUsingRanged = false;
                        StartFollowingOwner();
                    }
                }*/

                if (me->GetEntry() == NPC_ZEPIK && me->getVictim())
                    me->IsInRange(me->getVictim(), 0.0f, 3.0f) ? me->GetMotionMaster()->MoveChase(me->getVictim()) : me->GetMotionMaster()->MoveChase(me->getVictim(), 15.0f, 0.0f);

                if (reachedTreasureTrigger && OracleTriggerGUID != 0)
                {
                    TreasureSpell = RAND(SPELL_DIG_UP_GLINTING_ARMOR, SPELL_DIG_UP_SPARKLING_HARE, SPELL_DIG_UP_GLOWING_GEM, SPELL_DIG_UP_POLISHED_PLATTER, SPELL_DIG_UP_SPARKLING_TREASURE);

                    //! We only say something when digging up shit if we
                    //! didn't already do this when moving towards it. This
                    //! bool is set to true when a 50/50 chance urand if-check
                    //! returns true and makes the companion say something.
                    if (!mentionedDiggingUpLine)
                    {
                        Talk(RAND(SAY_ALL_DUG_RANDOM_STUFF, SAY_ALL_DONE_DIGGING));
                        mentionedDiggingUpLine = true;
                    }

                    DoCast(me, TreasureSpell);
                    me->HandleEmoteCommand(EMOTE_ONESHOT_WORK_CHOPWOOD);

                    //! According to videos on retail a Glinting Bug is nearly always
                    //! dug up, but the amount of bugs spawned is different from time
                    //! to time, hence the for-loop.
                    if (urand(0, 3) != 0) //! 75% chance to succeed.
                    {
                        uint8 random = urand(0, 3);
                        uint32 rareSpell = RAND(SPELL_DIG_UP_GLINTING_BUG, SPELL_DIG_UP_SPARKLING_HARE);
                        for (uint8 i = 1; i < random; i++)
                            DoCast(me, rareSpell);

                        if (urand(0, 2) == 0)
                            rareSpell == SPELL_DIG_UP_GLINTING_BUG ? Talk(SAY_ALL_DUG_UP_BUG) : Talk(SAY_ALL_DUG_UP_RABBIT);
                    }

                    if (Creature* oracleTrigger = Unit::GetCreature(*me, OracleTriggerGUID))
                    {
                        if (oracleTrigger->IsAIEnabled)
                        {
                            oracleTrigger->AI()->DoAction(ACTION_ORACLE_TRIGGER_DUG_UP);
                            OracleTriggerGUID = 0;
                        }
                    }

                    //! We shouldn't start combat while digging up...
                    me->SetReactState(REACT_PASSIVE);
                    mentionedDiggingUpLine = false;
                    reachedTreasureTrigger = false;
                    _events.ScheduleEvent(EVENT_ALL_FOLLOW_AGAIN, 2500);
                    _events.ScheduleEvent(EVENT_ALL_BECOME_AGGRESSIVE, 2500);
                }

                while (uint8 eventId = _events.ExecuteEvent())
                {
                    switch (eventId)
                    {
                        case EVENT_ALL_CHECK_AREA_ID:
                            if (Player* player = me->ToTempSummon()->GetSummoner()->ToPlayer())
                            {
                                switch (me->GetEntry())
                                {
                                    case NPC_DAJIK:
                                        if (me->GetAreaId() == AREA_HARDKNUCKLE_CLEARING && player->GetQuestStatus(QUEST_TORMENTING_THE_SOFTKNUCKLES) == QUEST_STATUS_REWARDED)
                                            Talk(SAY_DAJIK_POKE_KNUCKLES);
                                        break;
                                    case NPC_GOREGEK:
                                        if (me->GetAreaId() == AREA_HARDKNUCKLE_CLEARING)
                                            Talk(SAY_GOREGEK_HARDKNUCKLES_CLEARING);
                                        break;
                                    default:
                                        break;
                                }
                            }

                            _events.ScheduleEvent(EVENT_ALL_CHECK_AREA_ID, 30000);
                            break;
                        case EVENT_ALL_CHECK_ZONE_ID:
                            if (Unit* owner = me->ToTempSummon()->GetSummoner())
                                if (Player* player = owner->ToPlayer())
                                    if (me->GetZoneId() != ZONE_SHOLAZAR_BASIN)
                                        me->DespawnOrUnsummon();

                            _events.ScheduleEvent(EVENT_ALL_CHECK_ZONE_ID, 5000);
                            break;
                        case EVENT_DAJIK_EARTHS_FURY:
                             DoCastVictim(SPELL_EARTHS_FURY);
                            _events.ScheduleEvent(EVENT_DAJIK_EARTHS_FURY, urand(5000, 15000));
                            break;
                        case EVENT_DAJIK_EARTHS_WRATH:
                            DoCastVictim(SPELL_EARTHS_WRATH);
                            _events.ScheduleEvent(EVENT_DAJIK_EARTHS_WRATH, urand(15000, 45000));
                            break;
                        case EVENT_ALL_COMPANIONS_RANDOM_SAY:
                            if (!me->isInCombat() && me->isAlive() && me->GetOwner() && me->IsWithinDist(me->GetOwner(), 20.0f) && me->GetOwner()->isAlive())
                                Talk(SAY_RANDOM_LINE_OOC);

                            _events.ScheduleEvent(EVENT_ALL_COMPANIONS_RANDOM_SAY, urand(20000, 320000));
                            break;
                        case EVENT_ALL_CHECK_OWNERS_HEALTH:
                            if (Player* player = me->GetOwner()->ToPlayer())
                            {
                                if (player->HealthBelowPct(80))
                                {
                                    //? All heal-owner-spells seem to use TARGET_UNIT_TARGET_CHAINHEAL_ALLY - why?
                                    switch (me->GetEntry())
                                    {
                                        case NPC_DAJIK:
                                            me->CastSpell(player, SPELL_DAJIK_HEAL_OWNER, false);
                                            break;
                                        case NPC_ZEPIK:
                                            if (!player->HasAura(SPELL_AURA_RECENTLY_BANDAGED))
                                            {
                                                me->SetFacingToObject(player);
                                                me->CastSpell(player, SPELL_ZEPIK_HEAL_OWNER, false);
                                            }
                                            break;
                                        default:
                                            break;
                                    }
                                }
                            }

                            _events.ScheduleEvent(EVENT_ALL_CHECK_OWNERS_HEALTH, urand(60000, 120000));
                            break;
                        case EVENT_ALL_CHECK_OWNER_AFK:
                            //! As the event is scheduled every second, it'll increase this counter every second,
                            //! meaning that it will only mention that the player is AFK every 60 seconds. We are
                            //! not resetting either the bool or variable in the Reset hook for obvious reasons.
                            if (afkSecondsCount++ == 60)
                            {
                                recentlyMentionedAFK = false;
                                afkSecondsCount = 0;
                            }

                            if (Player* player = me->GetOwner()->ToPlayer())
                            {
                                if (!recentlyMentionedAFK && player->isAFK())
                                {
                                    switch (me->GetEntry())
                                    {
                                        case NPC_JALOOT:
                                            Talk(SAY_JALOOT_OWNER_WENT_AFK);
                                            break;
                                        case NPC_MOODLE:
                                            Talk(SAY_MOODLE_OWNER_WENT_AFK);
                                            break;
                                        case NPC_LAFOO:
                                            Talk(SAY_LAFOO_OWNER_WENT_AFK);
                                            break;
                                        default:
                                            break;
                                    }

                                    recentlyMentionedAFK = true;
                                }
                            }

                            _events.ScheduleEvent(EVENT_ALL_CHECK_OWNER_AFK, 1000);
                            break;
                        case EVENT_ALL_SEARCH_FOR_TREASURE:
                            if (Unit* owner = me->ToTempSummon()->GetSummoner())
                            {
                                if (Player* player = owner->ToPlayer())
                                {
                                    if (player->HasAura(SPELL_AURA_DETECT_INVIS_TREASURE) && (player->GetQuestStatus(QUEST_APPEASING_THE_GREAT_RAIN_STONE) == QUEST_STATUS_INCOMPLETE || player->GetQuestStatus(QUEST_GODS_LIKE_SHINEY_THINGS) == QUEST_STATUS_INCOMPLETE))
                                    {
                                        //! The reason we are selecting all triggers within 10
                                        //! yards instead of simply using FindNearestCreature
                                        //! is because if there are two triggers close to the
                                        //! companion and one of them would shine while the
                                        //! companion (thus owner) wouldn't move, he wouldn't
                                        //! be 'detected' by the companion as its focusing on another
                                        //! creature/trigger at that moment.
                                        Creature* oracleTrigger = NULL;
                                        std::list<Creature*> treasures;
                                        std::list<Creature*> oracleTriggers;
                                        me->GetCreatureListWithEntryInGrid(oracleTriggers, NPC_ORACLE_TRIGGER, 10.0f);
                                        for (std::list<Creature*>::iterator itr = oracleTriggers.begin(); itr != oracleTriggers.end(); ++itr)
                                        {
                                            if ((*itr)->HasAura(SPELL_TREASURE_SPARKLE))
                                            {
                                                oracleTrigger = (*itr)->ToCreature();
                                                break;
                                            }
                                        }

                                        if (oracleTrigger && oracleTrigger->IsAIEnabled && oracleTrigger->IsWithinDist(me, 12.0f)) //! 12 yards in case we moved in the meantime
                                        {
                                            if (!movingToTreasureTrigger)
                                            {
                                                me->GetMotionMaster()->MovePoint(POINT_MOVE_TO_ORACLE_TRIGGER, oracleTrigger->GetPositionX(), oracleTrigger->GetPositionY(), oracleTrigger->GetPositionZ());
                                                OracleTriggerGUID = oracleTrigger->GetGUID();
                                                movingToTreasureTrigger = true;

                                                if (urand(0, 1) == 0)
                                                {
                                                    Talk(SAY_ALL_SEE_TREASURE);
                                                    mentionedDiggingUpLine = true; //! We shouldn't say something twice while digging up a single treasure/target, so we control this by a bool.
                                                }
                                            }
                                        }
                                    }
                                }
                            }

                            _events.ScheduleEvent(EVENT_ALL_SEARCH_FOR_TREASURE, 250);
                            break;
                        case EVENT_ALL_FOLLOW_AGAIN:
                            StartFollowingOwner();
                            break;
                        case EVENT_GOREGEK_SUNDER_ARMOR:
                            DoCastVictim(SPELL_SUNDER_ARMOR);
                            _events.ScheduleEvent(EVENT_GOREGEK_SUNDER_ARMOR, urand(5000, 13000));
                            break;
                        case EVENT_GOREGEK_DISARM:
                            DoCastVictim(SPELL_DISARM);
                            _events.ScheduleEvent(EVENT_GOREGEK_DISARM, urand(15000, 25000));
                            break;
                        case EVENT_GOREGEK_PIERCING_HOWL:
                            DoCastVictim(SPELL_PIERCING_HOWL);
                            _events.ScheduleEvent(EVENT_GOREGEK_PIERCING_HOWL, urand(15000, 25000));
                            break;
                        case EVENT_ALL_CAST_NET_ON_CHICKENS:
                            if (Player* player = me->ToTempSummon()->GetSummoner()->ToPlayer())
                            {
                                if (player->GetAreaId() == AREA_FRENZYHEART_HILL && me->GetAreaId() == AREA_FRENZYHEART_HILL && (player->GetQuestStatus(QUEST_FLOWN_THE_COOP) == QUEST_STATUS_INCOMPLETE || player->GetQuestStatus(QUEST_CHICKEN_PARTY) == QUEST_STATUS_INCOMPLETE))
                                {
                                    if (Creature* chicken = me->FindNearestCreature(NPC_CHICKEN_ESCAPEE, 15.0f))
                                    {
                                        if (!chicken->HasAura(SPELL_NET_CHICKEN) && chicken->isAlive() && player->HasAura(SPELL_SEE_INVISIBLE_CHICKEN))
                                        {
                                            if (urand(0, 3) != 0) // 75% chance to succeed
                                            {
                                                switch (me->GetEntry())
                                                {
                                                    case NPC_DAJIK:
                                                        Talk(SAY_DAJIK_NETTED_CHICKEN);
                                                        break;
                                                    case NPC_ZEPIK:
                                                        Talk(SAY_ZEPIK_NETTED_CHICKEN);
                                                        break;
                                                    case NPC_GOREGEK:
                                                        Talk(SAY_GOREGEK_NETTED_CHICKEN);
                                                        break;
                                                    default:
                                                        break;
                                                }
                                            }

                                            me->SetFacingToObject(chicken); // Better visual
                                            DoCast(chicken, SPELL_NET_CHICKEN, false);
                                            _events.ScheduleEvent(EVENT_ALL_CAST_NET_ON_CHICKENS, urand(10000, 22000));
                                            break;
                                        }
                                    }
                                }
                            }

                            _events.ScheduleEvent(EVENT_ALL_CAST_NET_ON_CHICKENS, urand(8000, 18000));
                            break;
                        case EVENT_ZEPIK_PLACE_SPIKE_TRAP:
                            if (UpdateVictim() && me->IsWithinMeleeRange(me->getVictim()))
                                DoCastVictim(SPELL_PLACE_SPIKE_TRAP);

                            _events.ScheduleEvent(EVENT_ZEPIK_PLACE_SPIKE_TRAP, urand(2000, 12000));
                            break;
                        case EVENT_ZEPIK_OPEN_WOUND:
                            if (UpdateVictim() && me->IsWithinMeleeRange(me->getVictim()))
                                DoCastVictim(SPELL_OPEN_WOUND);

                            _events.ScheduleEvent(EVENT_ZEPIK_OPEN_WOUND, urand(18000, 25000));
                            break;
                        case EVENT_ALL_BECOME_AGGRESSIVE:
                            me->SetReactState(REACT_AGGRESSIVE);
                            break;
                        case EVENT_ZEPIK_PIERCING_ARROW:
                            if (UpdateVictim() && !me->IsInRange(me->getVictim(), 5.0f, 30.0f))
                                DoCastVictim(SPELL_PIERCING_ARROW);

                            _events.ScheduleEvent(EVENT_ZEPIK_PIERCING_ARROW, urand(2500, 3000));
                            break;
                        case EVENT_MOODLE_FROST_BREATH:
                            if (UpdateVictim() && me->isInFront(me->getVictim()))
                                DoCastVictim(SPELL_FROST_BREATH);

                            _events.ScheduleEvent(EVENT_MOODLE_FROST_BREATH, urand(4000, 18000));
                            break;
                        case EVENT_MOODLE_FRIGID_ABSORBTION:
                            if (UpdateVictim() && HealthBelowPct(75))
                            {
                                me->InterruptNonMeleeSpells(true);
                                DoCast(me, SPELL_FRIGID_ABSORBTION);
                                _events.ScheduleEvent(EVENT_MOODLE_FRIGID_ABSORBTION, 2000);
                                break;
                            }

                            _events.ScheduleEvent(EVENT_MOODLE_FRIGID_ABSORBTION, urand(18000, 29000));
                            break;
                        case EVENT_MOODLE_VICIOUS_ROAR:
                            if (UpdateVictim() && me->IsWithinMeleeRange(me->getVictim()))
                                DoCast(me, SPELL_VICIOUS_ROAR);

                            _events.ScheduleEvent(EVENT_MOODLE_VICIOUS_ROAR, urand(4000, 18000));
                            break;
                        case EVENT_MOODLE_WHIRLWIND:
                            if (UpdateVictim() && me->IsWithinMeleeRange(me->getVictim()))
                                DoCast(me, SPELL_WHIRLWIND);

                            _events.ScheduleEvent(EVENT_MOODLE_WHIRLWIND, urand(4000, 18000));
                            break;
                        case EVENT_JALOOT_LIGHTNING_WHIRL:
                            DoCastVictim(SPELL_LIGHTNING_WHIRL);
                            _events.ScheduleEvent(EVENT_JALOOT_LIGHTNING_WHIRL, 4000);
                            break;
                        case EVENT_JALOOT_SPARK_FRENZY:
                            DoCastVictim(SPELL_SPARK_FRENZY);
                            _events.ScheduleEvent(EVENT_JALOOT_SPARK_FRENZY, 14000);
                            break;
                        case EVENT_LAFOO_CHECK_HEALTH:
                            if (!UpdateVictim())
                            {
                                if (HealthBelowPct(75))
                                {
                                    me->InterruptNonMeleeSpells(true);
                                    DoCast(SPELL_SLEEPY_TIME);
                                    _events.ScheduleEvent(EVENT_LAFOO_TOXIC_SPIT, 60000); //! If we DID heal, we add a little cooldown this way
                                    break;
                                }
                            }
                            else
                            {
                                //! If we're in combat, we won't cast the healing spell
                                //! and re-schedule the event much faster than we would
                                //! if we weren't in combat.
                                _events.ScheduleEvent(EVENT_LAFOO_CHECK_HEALTH, 500);
                                break;
                            }

                            _events.ScheduleEvent(EVENT_LAFOO_TOXIC_SPIT, 5000);
                            break;
                        case EVENT_LAFOO_TOXIC_SPIT:
                            DoCastVictim(SPELL_TOXIC_SPIT);
                            _events.ScheduleEvent(EVENT_LAFOO_TOXIC_SPIT, urand(11000, 14000));
                            break;
                        case EVENT_ALL_RESET_RECENTLY_REACHED_BOOL:
                            recentlyReachedArea = false;
                            break;
                        case EVENT_ALL_CHECK_OWNER_HAS_SUMMONER_ITEM:
                            if (me->isSummon() && me->ToTempSummon() && me->ToTempSummon()->GetSummoner() && me->ToTempSummon()->GetSummoner()->ToPlayer())
                            {
                                if (Player* owner = me->ToTempSummon()->GetSummoner()->ToPlayer())
                                {
                                    switch (me->GetEntry())
                                    {
                                        case NPC_DAJIK:
                                            if (!owner->HasItemCount(ITEM_DAJIKS_SUMMONER, 1, true))
                                                me->DespawnOrUnsummon();
                                            break;
                                        case NPC_ZEPIK:
                                            if (!owner->HasItemCount(ITEM_ZEPIKS_SUMMONER, 1, true))
                                                me->DespawnOrUnsummon();
                                            break;
                                        case NPC_GOREGEK:
                                            if (!owner->HasItemCount(ITEM_GOREGEKS_SUMMONER, 1, true))
                                                me->DespawnOrUnsummon();
                                            break;
                                        case NPC_LAFOO:
                                            if (!owner->HasItemCount(ITEM_LAFOOS_SUMMONER, 1, true))
                                                me->DespawnOrUnsummon();
                                            break;
                                        case NPC_MOODLE:
                                            if (!owner->HasItemCount(ITEM_MOODLES_SUMMONER, 1, true))
                                                me->DespawnOrUnsummon();
                                            break;
                                        case NPC_JALOOT:
                                            if (!owner->HasItemCount(ITEM_JALOOTS_SUMMONER, 1, true))
                                                me->DespawnOrUnsummon();
                                            break;
                                        default:
                                            break;
                                    }
                                }
                            }

                            _events.ScheduleEvent(EVENT_ALL_CHECK_OWNER_HAS_SUMMONER_ITEM, 1000);
                            break;
                        default:
                            break;
                    }
                }
            }

        private:
            EventMap _events;
        };

        CreatureAI* GetAI(Creature* creature) const
        {
            return new npc_frenzyheart_oracle_companionsAI(creature);
        }
};

class npc_rainspeaker_canopy_treasure : public CreatureScript
{
    public:
        npc_rainspeaker_canopy_treasure() : CreatureScript("npc_rainspeaker_canopy_treasure") { }

        struct npc_rainspeaker_canopy_treasureAI : public ScriptedAI
        {
            npc_rainspeaker_canopy_treasureAI(Creature* creature) : ScriptedAI (creature) { }

            bool recentlyDugUp;

            void Reset()
            {
                _events.Reset();
                recentlyDugUp = false;
                _events.ScheduleEvent(EVENT_RAINSPEAKER_TREASURE_SPARKLE, urand(1000, 20000));
            }

            void DoAction(int32 const action)
            {
                if (action == ACTION_ORACLE_TRIGGER_DUG_UP)
                {
                    recentlyDugUp = true;
                    me->RemoveAurasDueToSpell(SPELL_TREASURE_SPARKLE);
                    _events.ScheduleEvent(EVENT_RAINSPEAKER_TREASURE_RESET_BOOL, urand(15000, 40000));
                }
            }

            void UpdateAI(uint32 const diff)
            {
                _events.Update(diff);

                while (uint8 eventId = _events.ExecuteEvent())
                {
                    switch (eventId)
                    {
                        case EVENT_RAINSPEAKER_TREASURE_SPARKLE:
                            if (!recentlyDugUp)
                            {
                                if (!me->HasAura(SPELL_TREASURE_SPARKLE)) //! Only add if we don't have it, but run the removal event anyway
                                    me->AddAura(SPELL_TREASURE_SPARKLE, me);

                                //! Two seconds gives the player an average time if they
                                //! saw a sparkle and try to move towards it.
                                _events.ScheduleEvent(EVENT_RAINSPEAKER_TREASURE_REMOVE_SHINE_EFFECT, 3500);
                            }

                            _events.ScheduleEvent(EVENT_RAINSPEAKER_TREASURE_SPARKLE, urand(5000, 65000));
                            break;
                        case EVENT_RAINSPEAKER_TREASURE_RESET_BOOL:
                            recentlyDugUp = false;
                            break;
                        case EVENT_RAINSPEAKER_TREASURE_REMOVE_SHINE_EFFECT:
                            if (me->HasAura(SPELL_TREASURE_SPARKLE))
                                me->RemoveAurasDueToSpell(SPELL_TREASURE_SPARKLE);
                            break;
                        default:
                            break;
                    }
                }
            }

        private:
            EventMap _events;
        };

        CreatureAI* GetAI(Creature* creature) const
        {
            return new npc_rainspeaker_canopy_treasureAI(creature);
        }
};

/*class spell_gen_oracle_wolvar_reputation : public SpellScriptLoader
{
    public:
        spell_gen_oracle_wolvar_reputation() : SpellScriptLoader("spell_gen_oracle_wolvar_reputation") { }

        class spell_gen_oracle_wolvar_reputation_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_gen_oracle_wolvar_reputation_SpellScript);

            bool Load()
            {
                return GetCaster()->GetTypeId() == TYPEID_PLAYER;
            }

            void HandleDummy(SpellEffIndex effIndex)
            {
                Player* player = GetCaster()->ToPlayer();
                uint32 factionId = GetSpellInfo()->Effects[effIndex].CalcValue();
                int32 repChange = GetSpellInfo()->Effects[EFFECT_1].CalcValue();

                FactionEntry const* factionEntry = sFactionStore.LookupEntry(factionId);

                if (!factionEntry)
                    return;

                // Set rep to baserep + basepoints (expecting spillover for oposite faction -> become hated)
                // Not when player already has equal or higher rep with this faction
                if (player->GetReputationMgr().GetBaseReputation(factionEntry) < repChange)
                    player->GetReputationMgr().SetReputation(factionEntry, repChange);

                // EFFECT_INDEX_2 most likely update at war state, we already handle this in SetReputation
            }

            void Register()
            {
                OnEffectHit += SpellEffectFn(spell_gen_oracle_wolvar_reputation_SpellScript::HandleDummy, EFFECT_0, SPELL_EFFECT_DUMMY);
            }
        };

        SpellScript* GetSpellScript() const
        {
            return new spell_gen_oracle_wolvar_reputation_SpellScript();
        }
};
*/
class spell_q12737_horn_of_fecundity : public SpellScriptLoader
{
    public:
        spell_q12737_horn_of_fecundity() : SpellScriptLoader("spell_q12737_horn_of_fecundity") { }

        class spell_q12737_horn_of_fecundity_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_q12737_horn_of_fecundity_SpellScript);

            SpellCastResult CheckIfSoakedFertielDirtNear()
            {
                if (Unit* caster = GetCaster())
                {
                    if (caster->ToPlayer() && caster->ToPlayer()->GetQuestStatus(QUEST_SONG_OF_FECUNDITY) != QUEST_STATUS_INCOMPLETE)
                        return SPELL_FAILED_CANT_DO_THAT_RIGHT_NOW;

                    GameObject* fertileDirt = caster->FindNearestGameObject(GAMEOBJECT_SOAKED_FERTILE_DIRT, 5.0f);

                    if (!fertileDirt || (fertileDirt && fertileDirt->GetGoState() == GO_STATE_ACTIVE))
                        return SPELL_FAILED_NO_VALID_TARGETS;
                    else if (fertileDirt)
                        fertileDirt->DestroyForNearbyPlayers();
                }

                return SPELL_CAST_OK;
            }

            void Register()
            {
                OnCheckCast += SpellCheckCastFn(spell_q12737_horn_of_fecundity_SpellScript::CheckIfSoakedFertielDirtNear);
            }
        };

        SpellScript* GetSpellScript() const
        {
            return new spell_q12737_horn_of_fecundity_SpellScript();
        }
};

class spell_q12736_didgeridoo_of_contemplation : public SpellScriptLoader
{
    public:
        spell_q12736_didgeridoo_of_contemplation() : SpellScriptLoader("spell_q12736_didgeridoo_of_contemplation") { }

        class spell_q12736_didgeridoo_of_contemplation_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_q12736_didgeridoo_of_contemplation_SpellScript)

            void HandleDummy(SpellEffIndex /*effIndex*/)
            {
                if (Player* player = GetCaster()->ToPlayer())
                {
                    if (player->GetQuestStatus(QUEST_SONG_OF_REFLECTION) == QUEST_STATUS_INCOMPLETE)
                    {
                        uint32 killCredit = 0;

                        switch (player->GetAreaId())
                        {
                            case AREA_GLIMMERING_PILLAR:
                                killCredit = NPC_GLIMMERING_PILLAR_CREDIT;
                                break;
                            case AREA_MOSSLIGHT_PILLAR:
                                killCredit = NPC_MOSSLIGHT_PILLAR_CREDIT;
                                break;
                            case AREA_SKYREACH_PILLAR:
                                killCredit = NPC_SKYREACH_PILLAR_CREDIT;
                                break;
                            case AREA_SUNTOUCHED_PILLAR:
                                killCredit = NPC_SUNTOUCHED_PILLAR_CREDIT;
                                break;
                            default:
                                break;
                        }

                        player->KilledMonsterCredit(killCredit, 0);
                    }
                }
            }

            void Register()
            {
                OnEffectHit += SpellEffectFn(spell_q12736_didgeridoo_of_contemplation_SpellScript::HandleDummy, EFFECT_ALL, SPELL_EFFECT_ANY);
            }
        };

        SpellScript* GetSpellScript() const
        {
            return new spell_q12736_didgeridoo_of_contemplation_SpellScript();
        }
};

/*class npc_haiphoon_vehicle : public CreatureScript
{
    public:
        npc_haiphoon_vehicle() : CreatureScript("npc_haiphoon_vehicle") { }

        struct npc_haiphoon_vehicleAI : public VehicleAI
        {
            npc_haiphoon_vehicleAI(Creature* creature) : VehicleAI(creature) { }

            void IsSummonedBy(Unit* summoner)
            {
                if (summoner->ToPlayer() && summoner->ToPlayer()->GetQuestStatus(QUEST_SONG_OF_WIND_AND_WATER) == QUEST_STATUS_INCOMPLETE)
                {
                    summoner->EnterVehicle(me);
                    me->setFaction(summoner->getFaction());
                }
                else
                    me->DespawnOrUnsummon();
            }

            void SpellHitTarget(Unit* target, SpellInfo const* spell)
            {
                if (!target || target == me)
                    return;
        
                if (spell->Id == SPELL_DEVOUR_WIND)
                {
                    if (Player* player = me->GetCharmerOrOwnerPlayerOrPlayerItself())
                    {
                        player->KilledMonsterCredit(NPC_STORM_REVENANT_CREDIT, 0);
                        me->UpdateEntry(NPC_HAIPHOON_AIR);                    
                        player->VehicleSpellInitialize();
                        me->setFaction(player->getFaction());
                    }
                }
                else if (spell->Id == SPELL_DEVOUR_WATER)
                {
                    if (Player* player = me->GetCharmerOrOwnerPlayerOrPlayerItself())
                    {
                        player->KilledMonsterCredit(NPC_MONSOON_REVENANT_CREDIT, 0);                    
                        me->UpdateEntry(NPC_HAIPHOON_WATER);
                        player->VehicleSpellInitialize();
                        me->setFaction(player->getFaction());
                    }
                }
            }
        };

        CreatureAI* GetAI(Creature* creature) const
        {
            return new npc_haiphoon_vehicleAI(creature);
        }
};*/

class AreaTrigger_at_spearborn_encampment : public AreaTriggerScript
{
    public:
        AreaTrigger_at_spearborn_encampment() : AreaTriggerScript("at_spearborn_encampment") { }

        bool OnTrigger(Player* player, AreaTriggerEntry const* /*trigger*/)
        {
            if (!player->isAlive() || !player->FindNearestCreature(NPC_JALOOT, 30.0f))
                return true;

            if (Creature* jaloot = player->FindNearestCreature(NPC_JALOOT, 30.0f))
                if (jaloot->IsControlledByPlayer() && jaloot->GetOwnerGUID() == player->GetGUID())
                    if (player->GetQuestStatus(QUEST_THE_LOST_MISTWHISPER_TREASURE) == QUEST_STATUS_INCOMPLETE)
                        if (!player->FindNearestCreature(NPC_WARLORD_TARTEK, 50.0f))
                            player->SummonCreature(NPC_WARLORD_TARTEK, 6708.375000f, 5136.280762f, -19.425825f, 5.154010f, TEMPSUMMON_TIMED_OR_DEAD_DESPAWN, 300000);
            return true;
        }
};

class npc_tools_of_war_trap_helper : public CreatureScript
{
    public:
        npc_tools_of_war_trap_helper() : CreatureScript("npc_tools_of_war_trap_helper") { }

        struct npc_tools_of_war_trap_helperAI : public ScriptedAI
        {
            npc_tools_of_war_trap_helperAI(Creature* creature) : ScriptedAI(creature) { }

            Player* player;
            bool exploded;

            void Reset()
            {
                exploded = false;

                // Fucking hacky but I don't give a shit anymore, spend enough hours on this crap.
                if (Player* _player = me->SelectNearestPlayer(10.0f))
                    player = _player;
                    //me->setFaction(player->getFaction());
            }

            void MoveInLineOfSight(Unit* who)
            {
                if (!player || exploded || !who || !me->IsWithinDist(who, 2.5f) || !who->ToCreature())
                    return;

                if (who->GetEntry() == NPC_FRENZYHEART_BERSERKER || who->GetEntry() == NPC_FRENZYHEART_RAVAGER || who->GetEntry() == NPC_FRENZYHEART_RAVAGER || who->GetEntry() == NPC_SPARKTOUCHED_ORACLE || who->GetEntry() == NPC_FRENZYHEART_HUNTER || who->GetEntry() == NPC_FRENZYHEART_SPEARBEARER || who->GetEntry() == NPC_FRENZYHEART_SCAVENGER)
                {
                    switch (me->GetEntry())
                    {
                        case NPC_VOLATILE_TRAP_BUNNY:
                            if (GameObject* volatileTrap = me->FindNearestGameObject(GAMEOBJECT_VOLATILE_TRAP, 1.0f))
                                volatileTrap->Delete();

                            player->CastSpell(player, SPELL_VOLATILE_TRAP, false);
                            exploded = true;
                            me->DespawnOrUnsummon(3000);
                            break;
                        case NPC_SPIKE_TRAP_BUNNY:
                            if (GameObject* spikeTrap = me->FindNearestGameObject(GAMEOBJECT_SPIKE_TRAP, 1.0f))
                                spikeTrap->Delete();

                            player->CastSpell(player, SPELL_SPIKE_TRAP, false);
                            exploded = true;
                            me->DespawnOrUnsummon(3000);
                            break;
                        case NPC_ENSNARING_TRAP_BUNNY:
                            if (GameObject* ensnaringTrap = me->FindNearestGameObject(GAMEOBJECT_ENSNARING_TRAP, 1.0f))
                                ensnaringTrap->Delete();

                            player->CastSpell(player, SPELL_ENSNARING_TRAP, false);
                            exploded = true;
                            me->DespawnOrUnsummon(3000);
                            break;
                        default:
                            break;
                    }
                }
            }
        };

        CreatureAI* GetAI(Creature* creature) const
        {
            return new npc_tools_of_war_trap_helperAI(creature);
        }
};

class npc_shrine_of_the_tempest : public CreatureScript
{
    public:
        npc_shrine_of_the_tempest() : CreatureScript("npc_shrine_of_the_tempest") { }

        struct npc_shrine_of_the_tempestAI : public ScriptedAI
        {
            npc_shrine_of_the_tempestAI(Creature* creature) : ScriptedAI(creature) { }

            void MoveInLineOfSight(Unit* who)
            {
                if (!who || !me->IsWithinDist(who, 70.0f))
                    return;

                if (Player* player = who->ToPlayer())
                    if (player->GetQuestStatus(QUEST_STRENGTH_OF_THE_TEMPEST) == QUEST_STATUS_INCOMPLETE)
                        if (player->HasItemCount(ITEM_ESSENCE_OF_THE_STORM, 3) && player->HasItemCount(ITEM_ESSENCE_OF_THE_MOONSOON, 3))
                            player->CastSpell(player, SPELL_CREATE_TRUE_POWER_OF_THE_TEMPEST, false);
            }
        };

        CreatureAI* GetAI(Creature* creature) const
        {
            return new npc_shrine_of_the_tempestAI(creature);
        }
};

class npc_stormwatcher : public CreatureScript
{
    public:
        npc_stormwatcher() : CreatureScript("npc_stormwatcher") { }

        struct npc_stormwatcherAI : public ScriptedAI
        {
            npc_stormwatcherAI(Creature* creature) : ScriptedAI (creature) { }

            uint32 CallLightning_Timer;

            void Reset()
            {
                CallLightning_Timer = urand (3000,5000);
            }

            void SpellHit (Unit* /*caster*/, SpellInfo const* spell)
            {
                if (spell->Id == SPELL_THROW_VENTURE_CO_EXPLOSIVES)
                {
                    DoCast(me, SPELL_SUMMON_STORMWATCHERS_HEAD, true);
                    me->DespawnOrUnsummon();
                }
            }

            void UpdateAI(uint32 const diff)
            {
                if (!UpdateVictim())
                    return;

                if (CallLightning_Timer <= diff)
                {
                    DoCastVictim(SPELL_CALL_LIGHTNING);
                    CallLightning_Timer = urand (3000, 5000);
                }
                else CallLightning_Timer -= diff;

                DoMeleeAttackIfReady();
            }
        };

        CreatureAI* GetAI(Creature* creature) const
        {
            return new npc_stormwatcherAI(creature);
        }
};

class npc_q12761_crystal_totems : public CreatureScript
{
    public:
        npc_q12761_crystal_totems() : CreatureScript("npc_q12761_crystal_totems") { }

        struct npc_q12761_crystal_totemsAI : public ScriptedAI
        {
            npc_q12761_crystal_totemsAI(Creature* creature) : ScriptedAI(creature) { }

            void Reset()
            {
                me->SetReactState(REACT_PASSIVE);
                me->SetUnitMovementFlags(MOVEMENTFLAG_ROOT);
                me->SetExtraUnitMovementFlags(MOVEMENTFLAG2_NO_STRAFE | MOVEMENTFLAG2_NO_JUMPING | MOVEMENTFLAG2_ALWAYS_ALLOW_PITCHING);
            }

            void IsSummonedBy(Unit* /*summoner*/)
            {
                if (me->GetEntry() == NPC_CRYSTAL_OF_THE_FROZEN_GRIP)
                    DoCast(me, SPELL_FROZEN_GRIP, false);
            }

            void KilledUnit(Unit* victim)
            {
                //! BECAUSE FUCK YOU, LOGIC!
                if (victim->ToCreature())// && me->isSummon() && me->ToTempSummon() && me->ToTempSummon()->GetSummoner() && me->ToTempSummon()->GetSummoner()->ToPlayer())
                {
                    if (Player* owner = me->SelectNearestPlayer(25.0f))
                    {
                        owner->KilledMonsterCredit(28078, 0);//victim->GetEntry(), victim->GetGUID(), true);
                        owner->KilledMonsterCredit(28111, 0);//victim->GetEntry(), victim->GetGUID(), true);
                    }
                }
            }
        };

        CreatureAI* GetAI(Creature* creature) const
        {
            return new npc_q12761_crystal_totemsAI(creature);
        }
};

class spell_q12734_blood_rejeks_sword : public SpellScriptLoader
{
    public:
        spell_q12734_blood_rejeks_sword() : SpellScriptLoader("spell_q12734_blood_rejeks_sword") { }

        class spell_q12734_blood_rejeks_sword_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_q12734_blood_rejeks_sword_SpellScript);

            void CheckTarget(SpellEffIndex /*effIndex*/)
            {
                if (GetCaster()->ToPlayer())
                {
                    if (Player* caster = GetCaster()->ToPlayer())
                    {
                        if (Unit* hitUnit = GetHitUnit())
                        {
                            if (hitUnit->ToCreature() && !hitUnit->isAlive() && caster->GetQuestStatus(QUEST_REJEK_FIRST_BLOOD) == QUEST_STATUS_INCOMPLETE)// && !hitUnit->FindNearestGameObject(GAMEOBJECT_REJEKS_SWORD, 1.0f))
                            {
                                switch (hitUnit->GetEntry())
                                {
                                    case NPC_SAPPHIRE_HIVE_WASP:
                                        caster->KilledMonsterCredit(NPC_FIRST_BLOOD_CREDIT_1, 0);
                                        break;
                                    case NPC_HARDKNUCKLE_CHARGER:
                                        caster->KilledMonsterCredit(NPC_FIRST_BLOOD_CREDIT_2, 0);
                                        break;
                                    case NPC_MISTWHISPER_ORACLE: case NPC_MISTWHISPER_WARRIOR:
                                        caster->KilledMonsterCredit(NPC_FIRST_BLOOD_CREDIT_3, 0);
                                        break;
                                    default:
                                        break;
                                }

                                hitUnit->ToCreature()->DespawnOrUnsummon(1000);
                            }
                        }
                    }
                }
            }

            void Register()
            {
                OnEffectHitTarget += SpellEffectFn(spell_q12734_blood_rejeks_sword_SpellScript::CheckTarget, EFFECT_0, SPELL_EFFECT_TRIGGER_MISSILE);
            }
        };

        SpellScript* GetSpellScript() const
        {
            return new spell_q12734_blood_rejeks_sword_SpellScript();
        }
};

enum StillEvents
{
    EVENT_BANANA_CLICK,
    EVENT_ORANGE_CLICK,
    EVENT_PAPAYA_CLICK,
    EVENT_VALVE_CLICK,
    EVENT_BRAZIER_CLICK
};

class go_still_tools : public GameObjectScript
{
public:
    go_still_tools(): GameObjectScript("go_still_tools") { }

        bool OnGossipHello(Player* player, GameObject* go)
        {
            if (Creature* tipsy = go->FindNearestCreature(28566, 15.0f))
            {
                switch (go->GetEntry())
                {
                    case 190639: // Barrel of Papayas
                        tipsy->AI()->DoAction(EVENT_PAPAYA_CLICK);
                        break;
                    case 190638: // Barrel of Bananas
                        tipsy->AI()->DoAction(EVENT_BANANA_CLICK);
                        break;
                    case 190637: //Barrel of Oranges
                        tipsy->AI()->DoAction(EVENT_ORANGE_CLICK);
                        break;
                    case 190636: // Brazier
                        tipsy->AI()->DoAction(EVENT_BRAZIER_CLICK);
                        break;
                    case 190635: // Preassure Valve
                        tipsy->AI()->DoAction(EVENT_VALVE_CLICK);
                        break;
                }
            }
            return true;
        }
};

/*######
## Quest: Song of Wind and Water ID: 12726
######*/
/*This quest precisly needs core script since battle vehicles are not well integrated with SAI,
may be easily converted to SAI when they get.*/
enum SongOfWindAndWater
{
        // Spells
        SPELL_DEVOUR_WIND = 52862,
        SPELL_DEVOUR_WATER = 52864,
        // NPCs
        NPC_HAIPHOON_WATER = 28999,
        NPC_HAIPHOON_AIR = 28985
};
 
class npc_haiphoon : public CreatureScript
{
public:
        npc_haiphoon() : CreatureScript("npc_haiphoon") { }

        struct npc_haiphoonAI : public VehicleAI
        {
                npc_haiphoonAI(Creature* creature) : VehicleAI(creature) { }

                void SpellHitTarget(Unit* target, SpellInfo const* spell)
                {
                        if (target == me)
                                return;

                        if (spell->Id == SPELL_DEVOUR_WIND && me->GetCharmerOrOwnerPlayerOrPlayerItself())
                        {
							me->UpdateEntry(NPC_HAIPHOON_AIR);
                        }
						else if (spell->Id == SPELL_DEVOUR_WATER && me->GetCharmerOrOwnerPlayerOrPlayerItself())
                        {
							me->UpdateEntry(NPC_HAIPHOON_WATER);
                        }
                }
        };

        CreatureAI* GetAI(Creature* creature) const
        {
                return new npc_haiphoonAI(creature);
        }
};

/*######
## Quest: Reconnaissance Flight (12671)
######*/
enum ReconnaissanceFlight
{
    NPC_PLANE       = 28710, // Vic's Flying Machine
    NPC_PILOT       = 28646,

    VIC_SAY_0       = 0,
    VIC_SAY_1       = 1,
    VIC_SAY_2       = 2,
    VIC_SAY_3       = 3,
    VIC_SAY_4       = 4,
    VIC_SAY_5       = 5,
    VIC_SAY_6       = 6,
    PLANE_EMOTE     = 0,

    AURA_ENGINE     = 52255, // Engine on Fire

    SPELL_LAND      = 52226, // Land Flying Machine
    SPELL_CREDIT    = 53328 // Land Flying Machine Credit
};

class npc_vics_flying_machine : public CreatureScript
{
public:
    npc_vics_flying_machine() : CreatureScript("npc_vics_flying_machine") { }

    struct npc_vics_flying_machineAI : public VehicleAI
    {
        npc_vics_flying_machineAI(Creature* creature) : VehicleAI(creature) {}

        void PassengerBoarded(Unit* passenger, int8 /*seatId*/, bool apply)
        {
            if (apply && passenger->GetTypeId() == TYPEID_PLAYER)
                me->GetMotionMaster()->MovePath(NPC_PLANE, false);
        }

        void MovementInform(uint32 type, uint32 id)
        {
            if (type != WAYPOINT_MOTION_TYPE)
                return;

            if (Creature* pilot = GetClosestCreatureWithEntry(me, NPC_PILOT, 10))
                switch (id)
                {
                    case 5:
                        pilot->AI()->Talk(VIC_SAY_0);
                        break;
                    case 11:
                        pilot->AI()->Talk(VIC_SAY_1);
                        break;
                    case 12:
                        pilot->AI()->Talk(VIC_SAY_2);
                        break;
                    case 14:
                        pilot->AI()->Talk(VIC_SAY_3);
                        break;
                    case 15:
                        pilot->AI()->Talk(VIC_SAY_4);
                        break;
                    case 17:
                        pilot->AI()->Talk(VIC_SAY_5);
                        break;
                    case 21:
                        pilot->AI()->Talk(VIC_SAY_6);
                        break;
                    case 25:
                        me->AI()->Talk(PLANE_EMOTE);
                        me->AI()->DoCast(AURA_ENGINE);
                        break;
                }
        }

        void SpellHit(Unit* /*caster*/, SpellInfo const* spell)
        {
            if (spell->Id == SPELL_LAND)
            {
                Unit* passenger = me->GetVehicleKit()->GetPassenger(1); // player should be on seat 1
                if (passenger && passenger->GetTypeId() == TYPEID_PLAYER)
                {
                    passenger->CastSpell(passenger, SPELL_CREDIT, true);
                    passenger->ExitVehicle();
                }
            }
        }

        void UpdateAI(const uint32 /*diff*/) {}
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_vics_flying_machineAI(creature);
    }
};

void AddSC_sholazar_basin()
{
    new spell_q12737_horn_of_fecundity();
    new spell_q12736_didgeridoo_of_contemplation();
    new npc_zepik_jaloot_switch();
    new npc_artruis_the_heartless();
    new npc_hardknuckle_charger_proxy();
    new npc_softknuckle();
    //new npc_haiphoon_vehicle();    
    new npc_frenzyheart_oracle_companions();
    new npc_rainspeaker_canopy_treasure();
    new AreaTrigger_at_spearborn_encampment();
    new npc_tools_of_war_trap_helper();
    new npc_shrine_of_the_tempest();
    new npc_stormwatcher();
    new npc_q12761_crystal_totems();
    new spell_q12734_blood_rejeks_sword();
    new npc_injured_rainspeaker_oracle();
    new npc_vekjik();
    new npc_avatar_of_freya();
    new npc_bushwhacker();
    new npc_engineer_helice();
    new npc_adventurous_dwarf();
    new npc_jungle_punch_target();
    new spell_q12620_the_lifewarden_wrath();
    new spell_q12589_shoot_rjr();
    new npc_haiphoon();
	new go_still_tools();
	new npc_vics_flying_machine();
}
