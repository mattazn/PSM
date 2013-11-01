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
#include "violet_hold.h"

//Spells
enum Spells
{
    SPELL_CORROSIVE_SALIVA                     = 54527,
    SPELL_OPTIC_LINK                           = 54396,
	SPELL_RAY_OF_SUFFERING					   = 54417,//54442 Bugged,
	H_SPELL_RAY_OF_SUFFERING				   = 54417,//59524 Bugged,
	SPELL_RAY_OF_PAIN						   = 59525,//54438 Bugged,
	H_SPELL_RAY_OF_PAIN						   = 59525//59523 Bugged,
};

class boss_moragg : public CreatureScript
{
public:
    boss_moragg() : CreatureScript("boss_moragg") { }

    CreatureAI* GetAI(Creature* creature) const
    {
        return new boss_moraggAI (creature);
    }

    struct boss_moraggAI : public ScriptedAI
    {
        boss_moraggAI(Creature* creature) : ScriptedAI(creature)
        {
            instance = creature->GetInstanceScript();
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_INTERRUPT, true);
			me->ApplySpellImmune(1, IMMUNITY_MECHANIC, MECHANIC_STUN, true);
        }

        uint32 uiOpticLinkTimer;
        uint32 uiCorrosiveSalivaTimer;
		uint32 uiRayTimer;

        InstanceScript* instance;

        void Reset()
        {
            uiOpticLinkTimer = 6000+rand()%4000;
            uiCorrosiveSalivaTimer = 3000+rand()%3000;
			uiRayTimer = 2500+rand()%4000;

            if (instance)
            {
                if (instance->GetData(DATA_WAVE_COUNT) == 6)
                    instance->SetData(DATA_1ST_BOSS_EVENT, NOT_STARTED);
                else if (instance->GetData(DATA_WAVE_COUNT) == 12)
                    instance->SetData(DATA_2ND_BOSS_EVENT, NOT_STARTED);
            }
        }

        void EnterCombat(Unit* /*who*/)
        {
            if (instance)
            {
                if (GameObject* pDoor = instance->instance->GetGameObject(instance->GetData64(DATA_MORAGG_CELL)))
                    if (pDoor->GetGoState() == GO_STATE_READY)
                   {
                        EnterEvadeMode();
                        return;
                    }
                if (instance->GetData(DATA_WAVE_COUNT) == 6)
                    instance->SetData(DATA_1ST_BOSS_EVENT, IN_PROGRESS);
                else if (instance->GetData(DATA_WAVE_COUNT) == 12)
                    instance->SetData(DATA_2ND_BOSS_EVENT, IN_PROGRESS);
            }
        }

        void AttackStart(Unit* who)
        {
            if (me->HasFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_IMMUNE_TO_PC) || me->HasFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE))
                return;

            if (me->Attack(who, true))
            {
                me->AddThreat(who, 0.0f);
                me->SetInCombatWith(who);
                who->SetInCombatWith(me);
				me->SetInCombatWithZone();
                DoStartMovement(who);
            }
        }

        void MoveInLineOfSight(Unit* /*who*/) {}

        void UpdateAI(const uint32 diff)
        {
            //Return since we have no target
            if (!UpdateVictim())
                return;

            if (uiOpticLinkTimer <= diff)
            {
            // Hack per Optical Link (Hack from Loken script)
            Map* pMap = me->GetMap();
            if (pMap->IsDungeon())
            {
                Map::PlayerList const &PlayerList = pMap->GetPlayers();

                if (PlayerList.isEmpty())
                    return;

                for (Map::PlayerList::const_iterator i = PlayerList.begin(); i != PlayerList.end(); ++i)
                    if (i->getSource() && i->getSource()->isAlive() && i->getSource()->isTargetableForAttack())
                    {
                        int32 dmg;
                        float m_fDist = me->GetExactDist(i->getSource()->GetPositionX(), i->getSource()->GetPositionY(), i->getSource()->GetPositionZ());

                        dmg = 150; // need to correct damage
                        if (m_fDist > 1.0f) // Further from 1 yard
                            dmg += (m_fDist * 50);
                       
                        Unit* pTarget = SelectTarget(SELECT_TARGET_RANDOM, 0, 100, true);
                        me->CastCustomSpell(pTarget, SPELL_OPTIC_LINK, &dmg, 0, 0, false);
                    }
            }
               /* if (Unit* target = SelectTarget(SELECT_TARGET_RANDOM, 0, 100, true))
                    DoCast(target, SPELL_OPTIC_LINK);*/
                uiOpticLinkTimer = 25000;
            } else uiOpticLinkTimer -= diff;

            if (uiCorrosiveSalivaTimer <= diff)
            {
                DoCast(me->getVictim(), SPELL_CORROSIVE_SALIVA);
                uiCorrosiveSalivaTimer = 10000;
            } else uiCorrosiveSalivaTimer -= diff;

            if (uiRayTimer <= diff)
            {
                if (Unit* target = SelectTarget(SELECT_TARGET_RANDOM, 0, 100, true))
				{
					bool bChance = urand(0,1);
					if(bChance)
					{
						DoCast(target, IsHeroic() ? H_SPELL_RAY_OF_SUFFERING : SPELL_RAY_OF_SUFFERING);
					} else {
						DoCast(target, IsHeroic() ? H_SPELL_RAY_OF_PAIN : SPELL_RAY_OF_PAIN);
					}
				}
                uiRayTimer = 8000+rand()%2500;
            } else uiRayTimer -= diff;

            DoMeleeAttackIfReady();
        }

        void JustDied(Unit* /*killer*/)
        {
            if (instance)
            {
                if (instance->GetData(DATA_WAVE_COUNT) == 6)
                {
                    instance->SetData(DATA_1ST_BOSS_EVENT, DONE);
                    instance->SetData(DATA_WAVE_COUNT, 7);
                }
                else if (instance->GetData(DATA_WAVE_COUNT) == 12)
                {
                    instance->SetData(DATA_2ND_BOSS_EVENT, DONE);
                    instance->SetData(DATA_WAVE_COUNT, 13);
                }
            }
        }
    };

};

void AddSC_boss_moragg()
{
    new boss_moragg();
}
