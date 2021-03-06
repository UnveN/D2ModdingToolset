/*
 * This file is part of the modding toolset for Disciples 2.
 * (https://github.com/VladimirMakeev/D2ModdingToolset)
 * Copyright (C) 2021 Stanislav Egorov.
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

#include "gameutils.h"
#include "game.h"

namespace hooks {

bool isGreaterPickRandomIfEqual(int first, int second)
{
    using namespace game;

    const auto& fn = gameFunctions();

    return first > second || (first == second && fn.generateRandomNumber(2) == 1);
}

game::CMidUnitGroup* getAllyOrEnemyGroup(const game::IMidgardObjectMap* objectMap,
                                         const game::BattleMsgData* battleMsgData,
                                         const game::CMidgardID* unitId,
                                         bool ally)
{
    using namespace game;

    const auto& fn = gameFunctions();

    CMidgardID groupId{};
    fn.getAllyOrEnemyGroupId(&groupId, battleMsgData, unitId, ally);

    void* tmp{};
    return fn.getStackFortRuinGroup(tmp, objectMap, &groupId);
}

} // namespace hooks
