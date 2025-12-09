/*
 * This file is part of the modding toolset for Disciples 2.
 * (https://github.com/VladimirMakeev/D2ModdingToolset)
 * Copyright (C) 2025 Alexey Voskresensky.
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "batattackhealhooks.h"
#include "batattackheal.h"
#include "batattackutils.h"
#include "battlemsgdata.h"
#include "game.h"
#include "midgardobjectmap.h"
#include "ussoldier.h"
#include "utils.h"
#include <spdlog/spdlog.h>

namespace hooks {

bool __fastcall healAttackCanPerformHooked(game::CBatAttackHeal* thisptr,
                                              int /*%edx*/,
                                              game::IMidgardObjectMap* objectMap,
                                              game::BattleMsgData* battleMsgData,
                                              game::CMidgardID* targetUnitId)
{
    using namespace game;

    if (*targetUnitId == emptyId || *targetUnitId == invalidId) {
        return false;
    }

    const auto& fn = gameFunctions();
    const auto& battle = BattleMsgDataApi::get();
    const auto& idApi = CMidgardIDApi::get();

    CMidgardID targetGroupId{};
    thisptr->vftable->getTargetGroupId(thisptr, &targetGroupId, battleMsgData);

    CMidgardID unitGroupId{};
    fn.getAllyOrEnemyGroupId(&unitGroupId, battleMsgData, targetUnitId, true);

    if (targetGroupId != unitGroupId) {
        return false;
    }

    CMidUnit* unit = fn.findUnitById(objectMap, targetUnitId);
    if (!unit) {
        return false;
    }

    auto soldier = fn.castUnitImplToSoldier(unit->unitImpl);

    int unitHp = unit->currentHp;
    int unitHpMax = soldier->vftable->getHitPoints(soldier);

    bool canHeal = (unitHp < unitHpMax) && (unitHp > 0);
    bool canCure = false;
    bool canRevive = false;
    bool unitDead = false;

    if (battle.getUnitStatus(battleMsgData, targetUnitId, BattleStatus::XpCounted)
        || battle.getUnitStatus(battleMsgData, targetUnitId, BattleStatus::Dead)) {
        unitDead = true;
    }

    if (!canHeal) {
        if (thisptr->unknown4 == -1) {
             thisptr->unknown4 = fn.getAttackImplMagic(objectMap, &thisptr->unitId2, 0);
        }
        if (thisptr->attackNumber == 1) {
            if (idApi.getType(&thisptr->unitId2) != IdType::Item) {
                if (!thisptr->attack2) {
                    thisptr->attack2 = fn.getAttackById(objectMap, &thisptr->unitId2,
                                                        thisptr->attackNumber + 1, true);
                }

                if (thisptr->attack2) {
                    auto attackClass = thisptr->attack2->vftable->getAttackClass(thisptr->attack2);
                    auto& categories = AttackClassCategories::get();

                    if (attackClass->id == categories.cure->id) {
                        canCure = battle.unitCanBeCured(battleMsgData, targetUnitId);
                    }

                    if (attackClass->id == categories.revive->id) {
                        canRevive = battle.unitCanBeRevived(battleMsgData, targetUnitId);
                    }
                }
            }
        }
    }

    if (unitDead) {
        return canRevive;
    } else if (canHeal) {
        return true;
    } else {
        return canCure;
    }

    return false;
}

} // namespace hooks