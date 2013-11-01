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
SDComment: Quest support: 11253, 11241.
SDCategory: howling_fjord
EndScriptData */

/* ContentData
npc_plaguehound_tracker
npc_apothecary_hanes
EndContentData */

#include "ScriptMgr.h"
#include "ScriptedCreature.h"
#include "ScriptedGossip.h"
#include "ScriptedEscortAI.h"

/*######
## npc_apothecary_hanes
######*/
enum Entries
{
    NPC_APOTHECARY_HANES         = 23784,
    FACTION_ESCORTEE_A           = 774,
    FACTION_ESCORTEE_H           = 775,
    NPC_HANES_FIRE_TRIGGER       = 23968,
    QUEST_TRAIL_OF_FIRE          = 11241,
    SPELL_COSMETIC_LOW_POLY_FIRE = 56274
};

class npc_apothecary_hanes : public CreatureScript
{
public:
    npc_apothecary_hanes() : CreatureScript("npc_apothecary_hanes") { }

    bool OnQuestAccept(Player* player, Creature* creature, Quest const* quest)
    {
        if (quest->GetQuestId() == QUEST_TRAIL_OF_FIRE)
        {
            switch (player->GetTeam())
            {
                case ALLIANCE:
                    creature->setFaction(FACTION_ESCORTEE_A);
                    break;
                case HORDE:
                    creature->setFaction(FACTION_ESCORTEE_H);
                    break;
            }
            CAST_AI(npc_escortAI, (creature->AI()))->Start(true, false, player->GetGUID());
        }
        return true;
    }

    struct npc_Apothecary_HanesAI : public npc_escortAI
    {
        npc_Apothecary_HanesAI(Creature* creature) : npc_escortAI(creature){}
        uint32 PotTimer;

        void Reset()
        {
            SetDespawnAtFar(false);
            PotTimer = 10000; //10 sec cooldown on potion
        }

        void JustDied(Unit* /*killer*/)
        {
            if (Player* player = GetPlayerForEscort())
                player->FailQuest(QUEST_TRAIL_OF_FIRE);
        }

        void UpdateEscortAI(const uint32 diff)
        {
            if (HealthBelowPct(75))
            {
                if (PotTimer <= diff)
                {
                    DoCast(me, 17534, true);
                    PotTimer = 10000;
                } else PotTimer -= diff;
            }
            if (GetAttack() && UpdateVictim())
                DoMeleeAttackIfReady();
        }

        void WaypointReached(uint32 waypointId)
        {
            Player* player = GetPlayerForEscort();
            if (!player)
                return;

            switch (waypointId)
            {
                case 1:
                    me->SetReactState(REACT_AGGRESSIVE);
                    SetRun(true);
                    break;
                case 23:
                    player->GroupEventHappens(QUEST_TRAIL_OF_FIRE, me);
                    me->DespawnOrUnsummon();
                    break;
                case 5:
                    if (Unit* Trigger = me->FindNearestCreature(NPC_HANES_FIRE_TRIGGER, 10.0f))
                        Trigger->CastSpell(Trigger, SPELL_COSMETIC_LOW_POLY_FIRE, false);
                    SetRun(false);
                    break;
                case 6:
                    if (Unit* Trigger = me->FindNearestCreature(NPC_HANES_FIRE_TRIGGER, 10.0f))
                        Trigger->CastSpell(Trigger, SPELL_COSMETIC_LOW_POLY_FIRE, false);
                    SetRun(true);
                    break;
                case 8:
                    if (Unit* Trigger = me->FindNearestCreature(NPC_HANES_FIRE_TRIGGER, 10.0f))
                        Trigger->CastSpell(Trigger, SPELL_COSMETIC_LOW_POLY_FIRE, false);
                    SetRun(false);
                    break;
                case 9:
                    if (Unit* Trigger = me->FindNearestCreature(NPC_HANES_FIRE_TRIGGER, 10.0f))
                        Trigger->CastSpell(Trigger, SPELL_COSMETIC_LOW_POLY_FIRE, false);
                    break;
                case 10:
                    SetRun(true);
                    break;
                case 13:
                    SetRun(false);
                    break;
                case 14:
                    if (Unit* Trigger = me->FindNearestCreature(NPC_HANES_FIRE_TRIGGER, 10.0f))
                        Trigger->CastSpell(Trigger, SPELL_COSMETIC_LOW_POLY_FIRE, false);
                    SetRun(true);
                    break;
            }
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_Apothecary_HanesAI(creature);
    }
};

/*######
## npc_plaguehound_tracker
######*/

enum ePlaguehound
{
    QUEST_SNIFF_OUT_ENEMY        = 11253
};

class npc_plaguehound_tracker : public CreatureScript
{
public:
    npc_plaguehound_tracker() : CreatureScript("npc_plaguehound_tracker") { }

    struct npc_plaguehound_trackerAI : public npc_escortAI
    {
        npc_plaguehound_trackerAI(Creature* creature) : npc_escortAI(creature) { }

        void Reset()
        {
            uint64 summonerGUID = 0;

            if (me->isSummon())
                if (Unit* summoner = me->ToTempSummon()->GetSummoner())
                    if (summoner->GetTypeId() == TYPEID_PLAYER)
                        summonerGUID = summoner->GetGUID();

            if (!summonerGUID)
                return;

            me->SetUnitMovementFlags(MOVEMENTFLAG_WALKING);
            Start(false, false, summonerGUID);
        }

        void WaypointReached(uint32 waypointId)
        {
            if (waypointId != 26)
                return;

            me->DespawnOrUnsummon();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_plaguehound_trackerAI(creature);
    }
};

/*######
## npc_razael_and_lyana
######*/

#define GOSSIP_RAZAEL_REPORT "High Executor Anselm wants a report on the situation."
#define GOSSIP_LYANA_REPORT "High Executor Anselm requests your report."

enum eRazael
{
    QUEST_REPORTS_FROM_THE_FIELD = 11221,
    NPC_RAZAEL = 23998,
    NPC_LYANA = 23778,
    GOSSIP_TEXTID_RAZAEL1 = 11562,
    GOSSIP_TEXTID_RAZAEL2 = 11564,
    GOSSIP_TEXTID_LYANA1 = 11586,
    GOSSIP_TEXTID_LYANA2 = 11588
};

class npc_razael_and_lyana : public CreatureScript
{
public:
    npc_razael_and_lyana() : CreatureScript("npc_razael_and_lyana") { }

    bool OnGossipHello(Player* player, Creature* creature)
    {
        if (creature->isQuestGiver())
            player->PrepareQuestMenu(creature->GetGUID());

        if (player->GetQuestStatus(QUEST_REPORTS_FROM_THE_FIELD) == QUEST_STATUS_INCOMPLETE)
            switch (creature->GetEntry())
            {
                case NPC_RAZAEL:
                    if (!player->GetReqKillOrCastCurrentCount(QUEST_REPORTS_FROM_THE_FIELD, NPC_RAZAEL))
                    {
                        player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, GOSSIP_RAZAEL_REPORT, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 1);
                        player->SEND_GOSSIP_MENU(GOSSIP_TEXTID_RAZAEL1, creature->GetGUID());
                        return true;
                    }
                break;
                case NPC_LYANA:
                    if (!player->GetReqKillOrCastCurrentCount(QUEST_REPORTS_FROM_THE_FIELD, NPC_LYANA))
                    {
                        player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, GOSSIP_LYANA_REPORT, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 2);
                        player->SEND_GOSSIP_MENU(GOSSIP_TEXTID_LYANA1, creature->GetGUID());
                        return true;
                    }
                break;
            }
        player->SEND_GOSSIP_MENU(player->GetGossipTextId(creature), creature->GetGUID());
        return true;
    }

    bool OnGossipSelect(Player* player, Creature* creature, uint32 /*sender*/, uint32 action)
    {
        player->PlayerTalkClass->ClearMenus();
        switch (action)
        {
            case GOSSIP_ACTION_INFO_DEF + 1:
                player->SEND_GOSSIP_MENU(GOSSIP_TEXTID_RAZAEL2, creature->GetGUID());
                player->TalkedToCreature(NPC_RAZAEL, creature->GetGUID());
                break;
            case GOSSIP_ACTION_INFO_DEF + 2:
                player->SEND_GOSSIP_MENU(GOSSIP_TEXTID_LYANA2, creature->GetGUID());
                player->TalkedToCreature(NPC_LYANA, creature->GetGUID());
                break;
        }
        return true;
    }
};

/*######
## npc_mcgoyver
######*/

#define GOSSIP_ITEM_MG_I  "Walt sent me to pick up some dark iron ingots."
#define GOSSIP_ITEM_MG_II "Yarp."

enum eMcGoyver
{
    QUEST_WE_CAN_REBUILD_IT             = 11483,

    SPELL_CREATURE_DARK_IRON_INGOTS     = 44512,
    SPELL_TAXI_EXPLORERS_LEAGUE         = 44280,

    GOSSIP_TEXTID_MCGOYVER              = 12193
};

class npc_mcgoyver : public CreatureScript
{
public:
    npc_mcgoyver() : CreatureScript("npc_mcgoyver") { }

    bool OnGossipHello(Player* player, Creature* creature)
    {
        if (player->GetQuestStatus(QUEST_WE_CAN_REBUILD_IT) == QUEST_STATUS_INCOMPLETE)
            player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, GOSSIP_ITEM_MG_I, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF+1);

        player->SEND_GOSSIP_MENU(player->GetGossipTextId(creature), creature->GetGUID());
        return true;
    }

    bool OnGossipSelect(Player* player, Creature* creature, uint32 /*sender*/, uint32 action)
    {
        player->PlayerTalkClass->ClearMenus();
        switch (action)
        {
            case GOSSIP_ACTION_INFO_DEF+1:
                player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, GOSSIP_ITEM_MG_II, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF+2);
                player->SEND_GOSSIP_MENU(GOSSIP_TEXTID_MCGOYVER, creature->GetGUID());
                player->CastSpell(player, SPELL_CREATURE_DARK_IRON_INGOTS, true);
                break;
            case GOSSIP_ACTION_INFO_DEF+2:
                player->CastSpell(player, SPELL_TAXI_EXPLORERS_LEAGUE, true);
                player->CLOSE_GOSSIP_MENU();
                break;
        }
        return true;
    }
};

/*######
## npc_daegarn
######*/

enum eDaegarnn
{
    QUEST_DEFEAT_AT_RING            = 11300,

    NPC_FIRJUS                      = 24213,
    NPC_JLARBORN                    = 24215,
    NPC_YOROS                       = 24214,
    NPC_OLUF                        = 23931,

    NPC_PRISONER_1                  = 24253,  // looks the same but has different abilities
    NPC_PRISONER_2                  = 24254,
    NPC_PRISONER_3                  = 24255,
};

static float afSummon[] = {838.81f, -4678.06f, -94.182f};
static float afCenter[] = {801.88f, -4721.87f, -96.143f};

class npc_daegarn : public CreatureScript
{
public:
    npc_daegarn() : CreatureScript("npc_daegarn") { }

    bool OnQuestAccept(Player* player, Creature* creature, const Quest* quest)
    {
        if (quest->GetQuestId() == QUEST_DEFEAT_AT_RING)
        {
            if (npc_daegarnAI* pDaegarnAI = CAST_AI(npc_daegarn::npc_daegarnAI, creature->AI()))
                pDaegarnAI->StartEvent(player->GetGUID());
        }

        return true;
    }

    // TODO: make prisoners help (unclear if summoned or using npc's from surrounding cages (summon inside small cages?))
    struct npc_daegarnAI : public ScriptedAI
    {
        npc_daegarnAI(Creature* creature) : ScriptedAI(creature) { }

        bool bEventInProgress;
        uint64 uiPlayerGUID;

        void Reset()
        {
            bEventInProgress = false;
            uiPlayerGUID = 0;
        }

        void StartEvent(uint64 uiGUID)
        {
            if (bEventInProgress)
                return;

            uiPlayerGUID = uiGUID;

            SummonGladiator(NPC_FIRJUS);
        }

        void JustSummoned(Creature* summon)
        {
            if (Player* player = me->GetPlayer(*me, uiPlayerGUID))
            {
                if (player->isAlive())
                {
                    summon->SetWalk(false);
                    summon->GetMotionMaster()->MovePoint(0, afCenter[0], afCenter[1], afCenter[2]);
                    summon->AI()->AttackStart(player);
                    return;
                }
            }

            Reset();
        }

        void SummonGladiator(uint32 uiEntry)
        {
            me->SummonCreature(uiEntry, afSummon[0], afSummon[1], afSummon[2], 0.0f, TEMPSUMMON_TIMED_DESPAWN_OUT_OF_COMBAT, 30*IN_MILLISECONDS);
        }

        void SummonedCreatureDies(Creature* summoned, Unit* /*killer*/)
        {
            uint32 uiEntry = 0;

            // will eventually reset the event if something goes wrong
            switch (summoned->GetEntry())
            {
                case NPC_FIRJUS:    uiEntry = NPC_JLARBORN; break;
                case NPC_JLARBORN:  uiEntry = NPC_YOROS;    break;
                case NPC_YOROS:     uiEntry = NPC_OLUF;     break;
                case NPC_OLUF:      Reset();                return;
            }

            SummonGladiator(uiEntry);
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_daegarnAI(creature);
    }
};

enum DropItThenRockIt
{
    QUEST_DROP_IT_ROCK_IT = 11429,
    NPC_WINTERSKORN_DEFENDER = 24015,
};

class npc_banner_drop_it_then_rock : public CreatureScript
{
    public:
        npc_banner_drop_it_then_rock() : CreatureScript("npc_banner_drop_it_then_rock") { }

        struct npc_banner_drop_it_then_rockAI : public ScriptedAI
        {
            npc_banner_drop_it_then_rockAI(Creature* creature) : ScriptedAI(creature) { }

            uint32 waveTimer;
            uint8 killCounter;

            void Reset()
            {
                killCounter = 0;
                waveTimer = 2000;
                me->SetReactState(REACT_PASSIVE);
            }

            void JustDied(Unit* /*killer*/)
            {
                if (me->isSummon() && me->GetOwner() && me->GetOwner()->ToPlayer())
                    me->GetOwner()->ToPlayer()->FailQuest(QUEST_DROP_IT_ROCK_IT);
            }

            void UpdateAI(const uint32 diff)
            {
                if (waveTimer < diff)
                {
                    if (Creature* vrykul = me->SummonCreature(NPC_WINTERSKORN_DEFENDER, (1476.85f + rand()%20), (-5327.56f + rand()%20), (194.8f  + rand()%2), 0.0f, TEMPSUMMON_CORPSE_DESPAWN))
                        if (vrykul->IsAIEnabled)
                            vrykul->AI()->AttackStart(me);

                    waveTimer = urand(8000, 16000);
                }
                else
                    waveTimer -= diff;
            }

            // We use SummonedCreatureDespawn instead of SummonedCreatureDies because of TEMPSUMMON_CORPSE_DESPAWN
            void SummonedCreatureDespawn(Creature* summon)
            {
                if (summon->GetEntry() == NPC_WINTERSKORN_DEFENDER)
                {
                    if (killCounter++ >= 3)
                    {
                        if (me->isSummon() && me->GetOwner() && me->GetOwner()->ToPlayer())
                            me->GetOwner()->ToPlayer()->GroupEventHappens(QUEST_DROP_IT_ROCK_IT, me);

                        me->DespawnOrUnsummon(2000);
                    }
                }
            }
        };

        CreatureAI* GetAI(Creature* creature) const
        {
            return new npc_banner_drop_it_then_rockAI(creature);
        }
};
// Sorlof's Booty - little hacky fix
enum SorlofsBooty
{
    SPELL_VISUAL_EXPLOSION  = 45008,
    ITEM_SORLOFS_BOOTY      = 34468,
    QUEST_SORLOFS_BOOTY     = 11529,
};

class npc_the_big_gun : public CreatureScript
{
    public:
        npc_the_big_gun() : CreatureScript("npc_the_big_gun") { }

        bool OnGossipHello(Player* player, Creature* creature)
        {
            if (player->GetQuestStatus(QUEST_SORLOFS_BOOTY) == QUEST_STATUS_INCOMPLETE)
            {
                creature->AI()->DoCast(SPELL_VISUAL_EXPLOSION);
                player->AddItem(ITEM_SORLOFS_BOOTY, 1);
            }

            player->PlayerTalkClass->SendCloseGossip();
            return true;
        }
};
/*######
## go_sprung_trap
######*/

enum eStationValve
{
    QUEST_PREYING_WEAK        = 11292,
    ENTRY_TRAPPED_ANIMAL      = 24196,
    ENTRY_PROWLING_WOLF       = 24206,
    GO_TRAP                   = 186616,
    GO_CHEST                  = 186619,
};

class go_sprung_trap : public GameObjectScript
{
    public:

        go_sprung_trap() : GameObjectScript("go_sprung_trap") { }

        bool OnGossipHello(Player* player, GameObject* go)
        {
            if (player->GetQuestStatus(QUEST_PREYING_WEAK) == QUEST_STATUS_INCOMPLETE)
            {
                switch (urand(0, 1))
                    {
                    case 0:
                        if (Creature* creature = go->FindNearestCreature(ENTRY_TRAPPED_ANIMAL, 2, true))
                        {
                            creature->setDeathState(JUST_DIED);
                            //player->SendLoot(creature->GetGUID(), LOOT_CORPSE);
                            go->SetEntry(GO_CHEST);
                        }
                        return true;
                        break;
                    case 1:
                        if (Creature* creature = go->FindNearestCreature(ENTRY_TRAPPED_ANIMAL, 2, true))
                        {
                            creature->setDeathState(JUST_DIED);
                            go->SummonCreature(ENTRY_PROWLING_WOLF, creature->GetPositionX(), creature->GetPositionY(), creature->GetPositionZ(), creature->GetOrientation(), TEMPSUMMON_TIMED_OR_DEAD_DESPAWN, 60000);
                            creature->MonsterWhisper("A Prowling Worg appears to defend its dinner.", player->GetGUID(), true);
                        }
                        return true;
                        break;
                    }
            }
            return true;
        }
};
enum eChillNymph
{
    QUEST_FALLEN_SISTERS        = 11314,
	
	FACTION_NEUTRAL				= 35,

	NPC_CREDIT					= 24117,

	SPELL_PENDANT				= 43340,
	SPELL_WRATH					= 9739,

	TEXT_EMOTE					= -1999945,
	TEXT_1						= -1999946,
	TEXT_2						= -1999947,
	TEXT_3						= -1999948
};
/*
INSERT INTO `script_texts` (`npc_entry`,`entry`, `content_default`, `type`, `language`, `comment`) VALUES
(23678, -236780, 'Chill Nymph appears weak!', 2, 0,'Chill Nymph'),
(23678, -236781, 'I knew Lurielle would send help! Thank you friend, and give Lurielle my thanks as well!', 0, 0, 'Chill Nymph'),
(23678, -236782, 'Where am I? What happened to me? You... you freed me?', 0, 0, 'Chill Nymph'),
(23678, -236783, 'Thank you. I thought I would die without seeing my sisters again!', 0, 0, 'Chill Nymph');
*/
class npc_chill_nymph : public CreatureScript
{
public:
    npc_chill_nymph() : CreatureScript("npc_chill_nymph") { }

    struct npc_chill_nymphAI : public ScriptedAI
    {
        npc_chill_nymphAI(Creature* c) : ScriptedAI(c) {}

        uint64 WrathTimer;
		uint64 DespawnTimer;
		bool doOnce;
		bool setDespawn;

        void Reset()
        {
           WrathTimer = 2000+rand()%1000;
		   doOnce = false;
		   setDespawn = false;
        }

        void SpellHit(Unit* caster, const SpellInfo* spell)
        {
            if (spell->Id == SPELL_PENDANT)
            {
				if(CAST_PLR(caster)->GetTypeId() == TYPEID_PLAYER)
				{
					if(CAST_PLR(caster)->GetQuestStatus(QUEST_FALLEN_SISTERS) == QUEST_STATUS_INCOMPLETE)
					{
						me->CombatStop();
						me->setFaction(FACTION_NEUTRAL);
						uint8 textChance = urand(1,3);
						switch(textChance)
						{
						case 1:
							DoScriptText(TEXT_1,me);
							break;
						case 2:
							DoScriptText(TEXT_2,me);
							break;
						case 3:
							DoScriptText(TEXT_3,me);
							break;
						}
						CAST_PLR(caster)->KilledMonsterCredit(NPC_CREDIT, 0);
						DespawnTimer = 1500;
					}
				}
			}
		}

        void UpdateAI(const uint32 diff)
        {
			if(me->getFaction() == 35 && DespawnTimer < diff)
			{
				me->DisappearAndDie();
			} else DespawnTimer -= diff;

            if (!UpdateVictim())
                return;
			
			if(WrathTimer < diff) {
				DoCast(me->getVictim(), SPELL_WRATH);
				WrathTimer = 2000+rand()%1500;
			} else WrathTimer -= diff;

	        if (HealthBelowPct(30) && !doOnce)
			{
	          DoScriptText(TEXT_EMOTE, me);
			  doOnce = true;
			}

            DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_chill_nymphAI(creature);
    }
};

enum TheWayToHisHeart
{
    NPC_FEMALE_REEF_COW         = 24797,

    SPELL_ANUNIAQS_NET          = 21014,
    SPELL_TASTY_REEF_FISH       = 44454,
    SPELL_LOVE_COSMETIC         = 52148,

    ITEM_TASTY_REEF_FISH        = 34127,

    QUEST_THE_WAY_TO_HIS_HEART  = 11472,
};

class npc_attracted_reef_bull : public CreatureScript
{
    public:
        npc_attracted_reef_bull() : CreatureScript("npc_attracted_reef_bull") { }

        struct npc_attracted_reef_bullAI : public ScriptedAI
        {
            npc_attracted_reef_bullAI(Creature* creature) : ScriptedAI(creature) { }

            bool completed;
            uint64 playerGUID;
            uint8 point;

            void Reset()
            {
                completed = false;
                playerGUID = 0;
                point = 0;
            }

            void SpellHit(Unit* caster, const SpellInfo* spell)
            {
                if (caster->GetTypeId() != TYPEID_PLAYER)
                    return;

                if (completed)
                    return;

                if (spell->Id == SPELL_TASTY_REEF_FISH && caster->ToPlayer()->GetQuestStatus(QUEST_THE_WAY_TO_HIS_HEART) == QUEST_STATUS_INCOMPLETE)
                {
                    if (playerGUID != caster->GetGUID())
                        playerGUID = caster->GetGUID();

                    me->GetMotionMaster()->MovePoint(point, caster->GetPositionX(), caster->GetPositionY(), caster->GetPositionZ());
                    ++point;
                }
            }

            void UpdateAI(uint32 const diff)
            {
                if (completed)
                    return;

                if (Creature* female = me->FindNearestCreature(NPC_FEMALE_REEF_COW, 5.0f, true))
                {
                    if (Player* player = me->GetPlayer(*me, playerGUID))
                    {
                        completed = true;
                        DoCast(me, SPELL_LOVE_COSMETIC);
                        female->AI()->DoCast(female, SPELL_LOVE_COSMETIC);
                        player->GroupEventHappens(QUEST_THE_WAY_TO_HIS_HEART, me);
                        me->DespawnOrUnsummon(5000);
                        female->DespawnOrUnsummon(5000);
                    }
                }
            }
        };

        CreatureAI* GetAI(Creature* creature) const
        {
            return new npc_attracted_reef_bullAI(creature);
        }
};

class spell_anuniaqs_net : public SpellScriptLoader
{
    public:
        spell_anuniaqs_net() : SpellScriptLoader("spell_anuniaqs_net") { }

        class spell_anuniaqs_net_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_anuniaqs_net_SpellScript);

            void HandleDummy(SpellEffIndex /*effIndex*/)
            {
                if (Unit* caster = GetCaster())
                    if (caster->GetTypeId() == TYPEID_PLAYER)
                        caster->ToPlayer()->AddItem(ITEM_TASTY_REEF_FISH, urand(1, 5));
            }

            void Register()
            {
                OnEffectHit += SpellEffectFn(spell_anuniaqs_net_SpellScript::HandleDummy, EFFECT_0, SPELL_EFFECT_DUMMY);
            }
        };

        SpellScript* GetSpellScript() const
        {
            return new spell_anuniaqs_net_SpellScript();
        }
};

enum WorgDisguise
{
    ITEM_WORG_DISGUISE          = 33618,
    
    QUEST_IN_WORGS_CLOTHING_A   = 11325,
    QUEST_IN_WORGS_CLOTHING_H   = 11323,
    QUEST_ALPHA_WORG_A          = 11326,
    QUEST_ALPHA_WORG_H          = 11324,

    NPC_WATCHER_MOONLEAF        = 24273,
    NPC_SAGE_MISTWALKER         = 24186,
};

class npcs_worg_disguise_retrieval : public CreatureScript
{
    public:
        npcs_worg_disguise_retrieval() : CreatureScript("npcs_worg_disguise_retrieval") { }

        bool OnGossipHello(Player* player, Creature* creature)
        {
            if (!player->HasItemCount(ITEM_WORG_DISGUISE, 1, true))
            {
                if (creature->GetEntry() == NPC_WATCHER_MOONLEAF)
                {
                    if (player->GetQuestStatus(QUEST_IN_WORGS_CLOTHING_A) == QUEST_STATUS_REWARDED || player->GetQuestStatus(QUEST_IN_WORGS_CLOTHING_A) == QUEST_STATUS_INCOMPLETE)
                        if (player->GetQuestStatus(QUEST_ALPHA_WORG_A) != QUEST_STATUS_REWARDED)
                            player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, "I've lost my Worg Disguise.", GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF);
                }
                else if (creature->GetEntry() == NPC_SAGE_MISTWALKER)
                {
                    if (player->GetQuestStatus(QUEST_IN_WORGS_CLOTHING_H) == QUEST_STATUS_REWARDED || player->GetQuestStatus(QUEST_IN_WORGS_CLOTHING_H) == QUEST_STATUS_INCOMPLETE)
                        if (player->GetQuestStatus(QUEST_ALPHA_WORG_H) != QUEST_STATUS_REWARDED)
                            player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, "I've lost my Worg Disguise.", GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF);
                }
            }

            player->SEND_GOSSIP_MENU(player->GetGossipTextId(creature), creature->GetGUID());
            return true;
        }

        bool OnGossipSelect(Player* player, Creature* creature, uint32 /*sender*/, uint32 /*action*/)
        {
            player->PlayerTalkClass->ClearMenus();
            player->PlayerTalkClass->SendCloseGossip();
            player->AddItem(ITEM_WORG_DISGUISE, 1);
            return true;
        }
};

void AddSC_howling_fjord()
{
    new npc_apothecary_hanes;
    new npc_plaguehound_tracker;
    new npc_razael_and_lyana;
    new npc_mcgoyver;
    new npc_daegarn;
    new npc_banner_drop_it_then_rock();
	new npc_the_big_gun();
    new go_sprung_trap();
	new npc_chill_nymph;
	new npcs_worg_disguise_retrieval();
    new npc_attracted_reef_bull();
    new spell_anuniaqs_net();
 }
