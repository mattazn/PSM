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
SDName: Boss Mr.Smite
SD%Complete:
SDComment: Timers and say taken from acid script
EndScriptData */

#include "ScriptMgr.h"
#include "ScriptedCreature.h"
#include "deadmines.h"

enum eSpells
{
    SPELL_THRASH            = 3391,
    SPELL_SMITE_STOMP       = 6432,
    SPELL_SMITE_SLAM        = 6435,
    SPELL_NIMBLE_REFLEXES   = 6264,
    SPELL_SMITE_HAMMER      = 6436,

	EQUIP_ID_SWORD          = 2179,
    EQUIP_ID_AXE            = 2183,
    EQUIP_ID_HAMMER         = 10756,

    SAY_PHASE_1             = -1036001,
    SAY_PHASE_2             = -1036002,
    SAY_PHASE_3             = -1036003,
};

class boss_mr_smite : public CreatureScript
{
public:
    boss_mr_smite() : CreatureScript("boss_mr_smite") { }

    CreatureAI* GetAI(Creature* creature) const
    {
        return new boss_mr_smiteAI (creature);
    }

    struct boss_mr_smiteAI : public ScriptedAI
    {
        boss_mr_smiteAI(Creature* creature) : ScriptedAI(creature)
        {
            instance = creature->GetInstanceScript();
        }

        InstanceScript* instance;

        uint32 uiThrashTimer;
        uint32 uiSlamTimer;
        uint32 uiNimbleReflexesTimer;

        uint8 uiHealth;

        uint32 uiPhase;
        uint32 uiTimer;

        void Reset()
        {
            uiThrashTimer = urand(5000, 9000);
            uiSlamTimer = 9000;
            uiNimbleReflexesTimer = urand(15500, 31600);

            uiHealth = 0;

            uiPhase = 0;
            uiTimer = 0;

            SetEquipmentSlots(false, EQUIP_ID_SWORD, EQUIP_UNEQUIP, EQUIP_NO_CHANGE);
        }

        bool bCheckChances()
        {
            uint32 uiChances = urand(0, 99);
            if (uiChances <= 15)
                return false;
            else
                return true;
        }

        void UpdateAI(const uint32 uiDiff)
        {
            if (!UpdateVictim())
                return;

        /*START ACID-AI*/
            if (uiThrashTimer <= uiDiff)
            {
                if (bCheckChances())
                    DoCast(me, SPELL_THRASH);
                uiThrashTimer = urand(6000, 15500);
            } else uiThrashTimer -= uiDiff;

            if (uiSlamTimer <= uiDiff)
            {
                if (bCheckChances())
                    DoCast(me->getVictim(), SPELL_SMITE_SLAM);
                uiSlamTimer = 11000;
            } else uiSlamTimer -= uiDiff;

            if (uiNimbleReflexesTimer <= uiDiff)
            {
                if (bCheckChances())
                    DoCast(me, SPELL_NIMBLE_REFLEXES);
                uiNimbleReflexesTimer = urand(27300, 60100);
            } else uiNimbleReflexesTimer -= uiDiff;
        /*END ACID-AI*/

            if ((uiHealth == 0 && !HealthAbovePct(66)) || (uiHealth == 1 && !HealthAbovePct(33)))
            {
                ++uiHealth;

				if(uiHealth == 1)
					DoScriptText(SAY_PHASE_2, me);
				if(uiHealth == 2)
					DoScriptText(SAY_PHASE_3, me);

                DoCastAOE(SPELL_SMITE_STOMP, false);
                SetCombatMovement(false);
                if (instance)
                    if (GameObject* go = GameObject::GetGameObject(*me, instance->GetData64(DATA_SMITE_CHEST)))
                    {
                        me->GetMotionMaster()->Clear();
                        me->GetMotionMaster()->MovePoint(1, 1.1508f, -780.1147f, go->GetPositionZ());
                    }
            }

            if (uiPhase)
            {
                if (uiTimer <= uiDiff)
                {
                    switch (uiPhase)
                    {
                        case 1:
							me->AttackStop();
                            me->SetStandState(UNIT_STAND_STATE_KNEEL);
                            uiTimer = 1500;
                            uiPhase = 2;
							uiThrashTimer = urand(5000, 9000);
							uiSlamTimer = 9000;
							uiNimbleReflexesTimer = urand(15500, 31600);
                            break;
                        case 2:
                            if (uiHealth == 1)
                                SetEquipmentSlots(false, EQUIP_ID_AXE, EQUIP_ID_AXE, EQUIP_NO_CHANGE);
                            else
                                SetEquipmentSlots(false, EQUIP_ID_HAMMER, EQUIP_UNEQUIP, EQUIP_NO_CHANGE);
                            uiTimer = 1500;
                            uiPhase = 3;
                            break;
                        case 3:
							me->SetStandState(UNIT_STAND_STATE_STAND);
                            SetCombatMovement(true);
                            me->GetMotionMaster()->MoveChase(me->getVictim(), me->m_CombatDistance);
                            uiPhase = 0;
                            break;
                    }
                } else uiTimer -= uiDiff;
            }

            DoMeleeAttackIfReady();
        }

        void MovementInform(uint32 uiType, uint32 /*uiId*/)
        {
            if (uiType != POINT_MOTION_TYPE)
                return;

            uiTimer = 1500;
            uiPhase = 1;
        }
    };
};

void AddSC_boss_mr_smite()
{
    new boss_mr_smite();
}
