/*
 * This file is part of the modding toolset for Disciples 2.
 * (https://github.com/VladimirMakeev/D2ModdingToolset)
 * Copyright (C) 2022 Vladimir Makeev.
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

#ifndef MIDGARDPLAN_H
#define MIDGARDPLAN_H

#include "d2vector.h"
#include "midgardid.h"
#include "midscenarioobject.h"
#include "mqpoint.h"
#include <cstdint>

namespace game {

struct MidgardPlanElement
{
    std::uint16_t y;
    std::uint16_t x;
    CMidgardID elementId;
};

assert_size(MidgardPlanElement, 8);

using PlanElements = Vector<MidgardPlanElement>;

struct CMidgardPlan : public IMidScenarioObject
{
    CMidgardID unknownId;
    int mapSize;
    char data[5184]; /**< 144 * 36. Accessed as: 36 * posY + (posX >> 2) */
    PlanElements elements;
    PlanElements elements2;
};

assert_size(CMidgardPlan, 5232);

namespace CMidgardPlanApi {

struct Api
{
    /**
     * Searches for map object with specified type at position.
     * @returns id of found object or nullptr if nothing found.
     */
    using GetObjectId = const CMidgardID*(__thiscall*)(const CMidgardPlan* thisptr,
                                                       const CMqPoint* mapPosition,
                                                       const IdType* type);
    GetObjectId getObjectId;

    /** Returns true if plan contains object of one of the specified types at position. */
    using IsPositionContainsObjects = bool(__thiscall*)(const CMidgardPlan* thisptr,
                                                        const CMqPoint* mapPosition,
                                                        const IdType* objectTypes,
                                                        std::uint32_t typesTotal);
    IsPositionContainsObjects isPositionContainsObjects;
};

Api& get();

} // namespace CMidgardPlanApi

} // namespace game

#endif // MIDGARDPLAN_H
