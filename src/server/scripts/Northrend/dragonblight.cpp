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

/* ScriptData
SDName: Dragonblight
SD%Complete: 100
SDComment:
SDCategory: Dragonblight
EndScriptData */

/* ContentData
npc_alexstrasza_wr_gate
EndContentData */

#include "ScriptMgr.h"
#include "ScriptedCreature.h"
#include "ScriptedGossip.h"
#include "SpellScript.h"
#include "SpellAuraEffects.h"
#include "ScriptedEscortAI.h"
#include "Vehicle.h"
#include "CombatAI.h"

enum eEnums
{
    QUEST_RETURN_TO_AG_A    = 12499,
    QUEST_RETURN_TO_AG_H    = 12500,
    MOVIE_ID_GATES          = 14
};

#define GOSSIP_ITEM_WHAT_HAPPENED   "Alexstrasza, can you show me what happened here?"

class npc_alexstrasza_wr_gate : public CreatureScript
{
public:
    npc_alexstrasza_wr_gate() : CreatureScript("npc_alexstrasza_wr_gate") { }

    bool OnGossipHello(Player* player, Creature* creature)
    {
        if (creature->isQuestGiver())
            player->PrepareQuestMenu(creature->GetGUID());

        if (player->GetQuestRewardStatus(QUEST_RETURN_TO_AG_A) || player->GetQuestRewardStatus(QUEST_RETURN_TO_AG_H))
            player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, GOSSIP_ITEM_WHAT_HAPPENED, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF+1);

        player->SEND_GOSSIP_MENU(player->GetGossipTextId(creature), creature->GetGUID());
        return true;
    }

    bool OnGossipSelect(Player* player, Creature* /*creature*/, uint32 /*sender*/, uint32 action)
    {
        player->PlayerTalkClass->ClearMenus();
        if (action == GOSSIP_ACTION_INFO_DEF+1)
        {
            player->CLOSE_GOSSIP_MENU();
            player->SendMovieStart(MOVIE_ID_GATES);
        }

        return true;
    }
};

/*######
## Quest Strengthen the Ancients (12096|12092)
######*/

enum StrengthenAncientsMisc
{
    SAY_WALKER_FRIENDLY         = 0,
    SAY_WALKER_ENEMY            = 1,
    SAY_LOTHALOR                = 0,

    SPELL_CREATE_ITEM_BARK      = 47550,
    SPELL_CONFUSED              = 47044,

    NPC_LOTHALOR                = 26321,

    FACTION_WALKER_ENEMY        = 14,
};

class spell_q12096_q12092_dummy : public SpellScriptLoader // Strengthen the Ancients: On Interact Dummy to Woodlands Walker
{
public:
    spell_q12096_q12092_dummy() : SpellScriptLoader("spell_q12096_q12092_dummy") { }

    class spell_q12096_q12092_dummy_SpellScript : public SpellScript
    {
        PrepareSpellScript(spell_q12096_q12092_dummy_SpellScript);

        void HandleDummy(SpellEffIndex /*effIndex*/)
        {
            uint32 roll = rand() % 2;

            Creature* tree = GetHitCreature();
            Player* player = GetCaster()->ToPlayer();

            if (!tree || !player)
                return;

            tree->RemoveFlag(UNIT_NPC_FLAGS, UNIT_NPC_FLAG_SPELLCLICK);

            if (roll == 1) // friendly version
            {
                tree->CastSpell(player, SPELL_CREATE_ITEM_BARK);
                tree->AI()->Talk(SAY_WALKER_FRIENDLY, player->GetGUID());
                tree->DespawnOrUnsummon(1000);
            }
            else if (roll == 0) // enemy version
            {
                tree->AI()->Talk(SAY_WALKER_ENEMY, player->GetGUID());
                tree->setFaction(FACTION_WALKER_ENEMY);
                tree->Attack(player, true);
            }
        }

        void Register()
        {
            OnEffectHitTarget += SpellEffectFn(spell_q12096_q12092_dummy_SpellScript::HandleDummy, EFFECT_0, SPELL_EFFECT_DUMMY);
        }
    };

    SpellScript* GetSpellScript() const
    {
        return new spell_q12096_q12092_dummy_SpellScript();
    }
};

class spell_q12096_q12092_bark : public SpellScriptLoader // Bark of the Walkers
{
public:
    spell_q12096_q12092_bark() : SpellScriptLoader("spell_q12096_q12092_bark") { }

    class spell_q12096_q12092_bark_SpellScript : public SpellScript
    {
        PrepareSpellScript(spell_q12096_q12092_bark_SpellScript);

        void HandleDummy(SpellEffIndex /*effIndex*/)
        {
            Creature* lothalor = GetHitCreature();
            if (!lothalor || lothalor->GetEntry() != NPC_LOTHALOR)
                return;

            lothalor->AI()->Talk(SAY_LOTHALOR);
            lothalor->RemoveAura(SPELL_CONFUSED);
            lothalor->DespawnOrUnsummon(4000);
        }

        void Register()
        {
            OnEffectHitTarget += SpellEffectFn(spell_q12096_q12092_bark_SpellScript::HandleDummy, EFFECT_0, SPELL_EFFECT_DUMMY);
        }
    };

    SpellScript* GetSpellScript() const
    {
        return new spell_q12096_q12092_bark_SpellScript();
    }
};

/*######
## Quest: Defending Wyrmrest Temple ID: 12372
######*/

enum WyrmDefenderEnum
{
    // Quest data
    QUEST_DEFENDING_WYRMREST_TEMPLE          = 12372,
    GOSSIP_TEXTID_DEF1                       = 12899,
    
    // Gossip data
    GOSSIP_TEXTID_DEF2                       = 12900,

    // Spells data
    SPELL_CHARACTER_SCRIPT                   = 49213, 
    SPELL_DEFENDER_ON_LOW_HEALTH_EMOTE       = 52421, // ID - 52421 Wyrmrest Defender: On Low Health Boss Emote to Controller - Random /self/
    SPELL_RENEW                              = 49263, // casted to heal drakes
    SPELL_WYRMREST_DEFENDER_MOUNT            = 49256,

    // Texts data
    WHISPER_MOUNTED                        = 0,
    BOSS_EMOTE_ON_LOW_HEALTH               = 2
};

#define GOSSIP_ITEM_1      "We need to get into the fight. Are you ready?"

class npc_wyrmrest_defender : public CreatureScript
{
    public:
        npc_wyrmrest_defender() : CreatureScript("npc_wyrmrest_defender") { }

        bool OnGossipHello(Player* player, Creature* creature)
        {
            if (player->GetQuestStatus(QUEST_DEFENDING_WYRMREST_TEMPLE) == QUEST_STATUS_INCOMPLETE)
            {
                player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, GOSSIP_ITEM_1, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF+1);
                player->SEND_GOSSIP_MENU(GOSSIP_TEXTID_DEF1, creature->GetGUID());
            }
            else
                player->SEND_GOSSIP_MENU(player->GetGossipTextId(creature), creature->GetGUID());

            return true;
        }

        bool OnGossipSelect(Player* player, Creature* creature, uint32 /*sender*/, uint32 action)
        {
            player->PlayerTalkClass->ClearMenus();
            if (action == GOSSIP_ACTION_INFO_DEF+1)
            {
                player->SEND_GOSSIP_MENU(GOSSIP_TEXTID_DEF2, creature->GetGUID());
                // Makes player cast trigger spell for 49207 on self
                player->CastSpell(player, SPELL_CHARACTER_SCRIPT, true);
                // The gossip should not auto close
            }

            return true;
        }

        struct npc_wyrmrest_defenderAI : public VehicleAI
        {
            npc_wyrmrest_defenderAI(Creature* creature) : VehicleAI(creature) { }

            bool hpWarningReady;
            bool renewRecoveryCanCheck;

            uint32 RenewRecoveryChecker;

            void Reset()
            {
                hpWarningReady = true;
                renewRecoveryCanCheck = false;

                RenewRecoveryChecker = 0;
            }

            void UpdateAI(uint32 const diff)
            {
                // Check system for Health Warning should happen first time whenever get under 30%,
                // after it should be able to happen only after recovery of last renew is fully done (20 sec),
                // next one used won't interfere
                if (hpWarningReady && me->GetHealthPct() <= 30.0f)
                {
                    me->CastSpell(me, SPELL_DEFENDER_ON_LOW_HEALTH_EMOTE);
                    hpWarningReady = false;
                }
                
                if (renewRecoveryCanCheck)
                {
                    if (RenewRecoveryChecker <= diff)
                    {
                        renewRecoveryCanCheck = false;
                        hpWarningReady = true;
                    }
                    else RenewRecoveryChecker -= diff;
                }
            }

            void SpellHit(Unit* /*caster*/, SpellInfo const* spell)
            {
                switch (spell->Id)
                {
                    case SPELL_WYRMREST_DEFENDER_MOUNT:
                        Talk(WHISPER_MOUNTED, me->GetCharmerOrOwnerGUID());
                        me->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_IMMUNE_TO_PC | UNIT_FLAG_IMMUNE_TO_NPC);
                        me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_PVP_ATTACKABLE);
                        break;
                    // Both below are for checking low hp warning
                    case SPELL_DEFENDER_ON_LOW_HEALTH_EMOTE:
                        Talk(BOSS_EMOTE_ON_LOW_HEALTH, me->GetCharmerOrOwnerGUID());
                        break;
                    case SPELL_RENEW:
                        if (!hpWarningReady && RenewRecoveryChecker <= 100)
                        {
                            RenewRecoveryChecker = 20000;
                        }
                        renewRecoveryCanCheck = true;
						break;
                }
            }
        };

        CreatureAI* GetAI(Creature* creature) const
        {
            return new npc_wyrmrest_defenderAI(creature);
        }
};

/*class spell_destabilize_dragonshire : public SpellScriptLoader
{
public:
    spell_destabilize_dragonshire() : SpellScriptLoader("spell_destabilize_dragonshire") {}

    class spell_destabilize_dragonshire_SpellScript : public SpellScript
    {
        PrepareSpellScript(spell_destabilize_dragonshire_SpellScript)

        bool Validate(SpellInfo const* /*spell*//*)
        {
            if (!sSpellMgr->GetSpellInfo(SPELL_DESTABILIZE_DRAGONSHIRE))
                return false;
            return true;
        }

               SpellCastResult CheckIfInZone()
        {
            Unit* caster = GetCaster();

                       if(caster->GetAreaId()==ZONE_AZURE_DRAGONSHIRE)
                               return SPELL_CAST_OK;
            else
                return SPELL_FAILED_CANT_DO_THAT_RIGHT_NOW;
        }

        void HandleBombEffect()
        {
            Unit* caster = GetCaster();
                       Player* player = caster->GetCharmerOrOwnerPlayerOrPlayerItself();

                       if (player && (player->GetQuestStatus(QUEST_DEFENDING_WYRMREST_TEMPLE)==QUEST_STATUS_INCOMPLETE))
                       {
                                        player->KilledMonsterCredit(NPC_DEFENDING_WRM_KILL_CREDIT,0);      
                       }
               }
        void Register()
        {
            OnCheckCast += SpellCheckCastFn(spell_destabilize_dragonshire_SpellScript::CheckIfInZone);
            OnCast += SpellCastFn(spell_destabilize_dragonshire_SpellScript::HandleBombEffect);//, EFFECT_0, SPELL_EFFECT_DUMMY);     
        }
                                              
    };

    SpellScript* GetSpellScript() const
    {
        return new spell_destabilize_dragonshire_SpellScript();
    }
};*/


/*######
## vehicle_alliance_steamtank
######*/

#define AREA_CARRION_FIELDS         4188
#define AREA_WINTERGARD_MAUSOLEUM   4246
#define AREA_THORSONS_POINT         4190

class vehicle_alliance_steamtank : public CreatureScript
{
public:
    vehicle_alliance_steamtank() : CreatureScript("vehicle_alliance_steamtank") { }

    struct vehicle_alliance_steamtankAI : public VehicleAI
    {
        vehicle_alliance_steamtankAI(Creature *c) : VehicleAI(c) { }

        uint32 check_Timer;
        bool isInUse;

        void Reset()
        {
            check_Timer = 5000;
        }

        void OnCharmed(bool apply)
        {
            isInUse = apply;

            if (!apply)
                check_Timer = 30000;
        }

        void UpdateAI(const uint32 diff)
        {
            if (!me->IsVehicle())
                return;

            if (isInUse)
            {
                if (check_Timer < diff)
                {
                    uint32 area = me->GetAreaId();
                    switch (area)
                    {
                    case AREA_CARRION_FIELDS:
                    case AREA_WINTERGARD_MAUSOLEUM:
                    case AREA_THORSONS_POINT:
                        break;
                    default:
                        me->DealDamage(me,me->GetHealth());
                        break;
                    }

                    check_Timer = 5000;
                }else check_Timer -= diff;
            }else
            {
                if (check_Timer < diff)
                {
                    uint32 area = me->GetAreaId();
                    if (area != AREA_THORSONS_POINT)
                    {
                        me->DealDamage(me,me->GetHealth());
                    }
                    check_Timer = 5000;
                }else check_Timer -= diff;
            }
        }

    };

    CreatureAI* GetAI(Creature *_Creature) const
    {
        return new  vehicle_alliance_steamtankAI(_Creature);
    }

};

enum TheChainGunAndYou
{
    NPC_WAYPOINT_01             = 27792,
    NPC_WAYPOINT_02             = 27793,
    NPC_WAYPOINT_03             = 27794,
    NPC_SUMMON_POINT            = 27795,
    NPC_CHAIN_GUN               = 27714,
    NPC_MINDLESS_GHOUL_ONE      = 27712,
    NPC_MINDLESS_GHOUL_TWO      = 27556,

    POINT_MOVE_TO_STAIRS        = 0,

    QUEST_THE_CHAIN_GUN_AND_YOU = 12457,
};

class npc_injured_7th_legion_soldier : public CreatureScript
{
    public:
        npc_injured_7th_legion_soldier() : CreatureScript("npc_injured_7th_legion_soldier") { }

        struct npc_injured_7th_legion_soldierAI : public ScriptedAI
        {
            npc_injured_7th_legion_soldierAI(Creature* creature) : ScriptedAI(creature)
            {
                me->SetReactState(REACT_PASSIVE);
                me->AddUnitMovementFlag(MOVEMENTFLAG_WALKING);
            }

            bool finished;
            bool gettingRescued;
            bool barricadeSide;
            bool summonedByGun;
            bool followingWaypoint;
            uint32 CheckTimer;
            uint64 PlayerGUID;

            void Reset()
            {
                finished = false;
                gettingRescued = false;
                barricadeSide = false;
                summonedByGun = false;
                followingWaypoint = false;
                CheckTimer = 500;
            }

            void IsSummonedBy(Unit* summoner)
            {
                //! Commenting this out because the
                //! spell that summons the soldiers
                //! is a triggered spell that makes
                //! the player (passenger) cast the
                //! summon spell (TARGET_UNIT_MASTER)
                //if (summoner->ToCreature() && summoner->ToCreature()->GetEntry() == NPC_CHAIN_GUN && summoner->IsVehicle() && summoner->GetVehicleKit())
                //    if (Unit* player = summoner->GetVehicleKit()->GetPassenger(0))
                //        if (player->GetTypeId() == TYPEID_PLAYER && player->ToPlayer()->GetQuestStatus(QUEST_THE_CHAIN_GUN_AND_YOU) == QUEST_STATUS_INCOMPLETE)
                PlayerGUID = summoner->GetGUID();
                summonedByGun = true;

                if (me->FindNearestCreature(NPC_MINDLESS_GHOUL_ONE, 10.0f, true) || me->FindNearestCreature(NPC_MINDLESS_GHOUL_ONE, 10.0f, false))
                    barricadeSide = false;
                else if (me->FindNearestCreature(NPC_MINDLESS_GHOUL_TWO, 10.0f, true) || me->FindNearestCreature(NPC_MINDLESS_GHOUL_TWO, 10.0f, false))
                    barricadeSide = true;
                else
                    me->DespawnOrUnsummon();
            }

            void MovementInform(uint32 type, uint32 id)
            {
                if (type != POINT_MOTION_TYPE)
                    return;

                if (id == POINT_MOVE_TO_STAIRS)
                {
                    if (Player* player = ObjectAccessor::GetPlayer(*me, PlayerGUID))
                    {
                        if (player->GetQuestStatus(QUEST_THE_CHAIN_GUN_AND_YOU) == QUEST_STATUS_INCOMPLETE)
                        {
                            finished = true;
                            player->KilledMonsterCredit(me->GetEntry(), me->GetGUID());
                            me->DespawnOrUnsummon(5000);
                        }
                    }
                    else // Else we'd just stand there forever... lmao
                        me->DespawnOrUnsummon();
                }
            }

            void UpdateAI(uint32 const diff)
            {
                if (finished)
                    return;

                if (CheckTimer <= diff)
                {
                    if (!followingWaypoint && !summonedByGun)
                    {
                        switch (urand(0, 2))
                        {
                            case 0:
                                if (Creature* wp = me->FindNearestCreature(NPC_WAYPOINT_01, 50.0f))
                                    me->GetMotionMaster()->MoveFollow(wp, PET_FOLLOW_DIST, PET_FOLLOW_ANGLE);
                                break;
                            case 1:
                                if (Creature* wp = me->FindNearestCreature(NPC_WAYPOINT_02, 50.0f))
                                    me->GetMotionMaster()->MoveFollow(wp, PET_FOLLOW_DIST, PET_FOLLOW_ANGLE);
                                break;
                            case 2:
                                if (Creature* wp = me->FindNearestCreature(NPC_WAYPOINT_03, 50.0f))
                                    me->GetMotionMaster()->MoveFollow(wp, PET_FOLLOW_DIST, PET_FOLLOW_ANGLE);
                                break;
                        }

                        followingWaypoint = true;
                    }
                    else if (!followingWaypoint && summonedByGun)
                    {
                        if (!gettingRescued)
                        {
                            gettingRescued = true;

                            switch (urand(0, 1))
                            {
                                case 0:
                                    if (barricadeSide)
                                        me->GetMotionMaster()->MovePoint(POINT_MOVE_TO_STAIRS, 3670.129150f, -1219.104248f, 98.091438f);
                                    else
                                        me->GetMotionMaster()->MovePoint(POINT_MOVE_TO_STAIRS, 3672.639160f, -1165.750977f, 98.091187f);
                                    break;
                                case 1:
                                    if (barricadeSide)
                                        me->GetMotionMaster()->MovePoint(POINT_MOVE_TO_STAIRS, 3651.504639f, -1209.865601f, 98.091759f);
                                    else
                                        me->GetMotionMaster()->MovePoint(POINT_MOVE_TO_STAIRS, 3691.383789f, -1174.775879f, 98.091187f);
                                    break;
                            }
                        }
                    }
                }
                else
                    CheckTimer -= diff;
            }
        };

        CreatureAI* GetAI(Creature* creature) const
        {
            return new npc_injured_7th_legion_soldierAI(creature);
        }
};

/*######
<<<<<<< HEAD
## npc_hourglass_of_eternity
######*/

// Support for 'Future you' is currently missing
enum MysteryOftheInfiniteRedux
{
    QUEST_MYSTERY_OF_THE_INFINITE       = 12470,
    QUEST_MYSTERY_OF_THE_INFINITE_REDUX = 13343,

    SPELL_SUMMON_INFINITE_ASSAILANT     = 49900,
    SPELL_SUMMON_INFINITE_DESTROYER     = 49901,
    SPELL_SUMMON_INFINITE_TIMERENDER    = 49905,
    SPELL_SUMMON_INFINITE_CHRONO_MAGUS  = 49902,
};

class npc_hourglass : public CreatureScript
{
    public:
        npc_hourglass() : CreatureScript("npc_hourglass") { }

        struct npc_hourglassAI : public ScriptedAI
        {
            npc_hourglassAI(Creature* creature) : ScriptedAI(creature) { }

            uint64 playerGUID;
            uint32 waveTimer;
            uint8 waveCounter;

            void Reset()
            {
                waveTimer = 5000;
                waveCounter = 0;
            }

            void IsSummonedBy(Unit* summoner)
            {
                playerGUID = summoner->GetGUID();
            }

            void JustSummoned(Creature* summoned)
            {
                summoned->AI()->AttackStart(me);
            }

            void JustDied()
            {
                if (playerGUID != 0)
                {
                    if (Player* player = Unit::GetPlayer(*me, playerGUID))
                    {
                        if (player->GetQuestStatus(QUEST_MYSTERY_OF_THE_INFINITE) == QUEST_STATUS_INCOMPLETE)
                            player->FailQuest(QUEST_MYSTERY_OF_THE_INFINITE);

                        if (player->GetQuestStatus(QUEST_MYSTERY_OF_THE_INFINITE_REDUX) == QUEST_STATUS_INCOMPLETE)
                            player->FailQuest(QUEST_MYSTERY_OF_THE_INFINITE_REDUX);
                    }
                }

                me->DespawnOrUnsummon();
            }

            void SummonWave()
            {
                switch (waveCounter)
                {
                    case 0:
                        DoCast(SPELL_SUMMON_INFINITE_ASSAILANT);
                        DoCast(SPELL_SUMMON_INFINITE_CHRONO_MAGUS);
                        break;
                    case 1:
                        DoCast(SPELL_SUMMON_INFINITE_CHRONO_MAGUS);
                        DoCast(SPELL_SUMMON_INFINITE_CHRONO_MAGUS);
                        break;
                    case 2:
                        DoCast(SPELL_SUMMON_INFINITE_ASSAILANT);
                        DoCast(SPELL_SUMMON_INFINITE_CHRONO_MAGUS);
                        DoCast(SPELL_SUMMON_INFINITE_DESTROYER);
                        break;
                    case 3:
                        DoCast(SPELL_SUMMON_INFINITE_ASSAILANT);
                        DoCast(SPELL_SUMMON_INFINITE_CHRONO_MAGUS);
                        DoCast(SPELL_SUMMON_INFINITE_DESTROYER);
                        break;
                    case 4:
                        DoCast(SPELL_SUMMON_INFINITE_TIMERENDER);
                        break;
                }
            }
    
            void UpdateAI(uint32 const diff)
            {
                if (waveTimer <= diff)
                {
                    if (waveCounter <= 4)
                    {
                        SummonWave();
                        waveTimer = 15000;
                        waveCounter++;
                    }
                    else
                    {
                        if (playerGUID != 0)
                        {
                            if (Player* player = Unit::GetPlayer(*me, playerGUID))
                            {
                                if (player->GetQuestStatus(QUEST_MYSTERY_OF_THE_INFINITE) == QUEST_STATUS_INCOMPLETE)
                                    player->GroupEventHappens(QUEST_MYSTERY_OF_THE_INFINITE, me);

                                if (player->GetQuestStatus(QUEST_MYSTERY_OF_THE_INFINITE_REDUX) == QUEST_STATUS_INCOMPLETE)
                                    player->GroupEventHappens(QUEST_MYSTERY_OF_THE_INFINITE_REDUX, me);
                            }
                        }
                    }

                }
                else waveTimer -= diff;
            }
        };

        CreatureAI* GetAI(Creature* creature) const
        {
            return new npc_hourglassAI(creature);
        }
};

void AddSC_dragonblight()
{
    new npc_alexstrasza_wr_gate;
    new spell_q12096_q12092_dummy;
    new spell_q12096_q12092_bark;
    new npc_wyrmrest_defender;
	//new spell_destabilize_dragonshire;
    new vehicle_alliance_steamtank();
    new npc_injured_7th_legion_soldier();
	new npc_hourglass();
}
