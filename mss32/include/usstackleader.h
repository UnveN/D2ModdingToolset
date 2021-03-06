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

#ifndef USSTACKLEADER_H
#define USSTACKLEADER_H

#include "usunitextension.h"

namespace game {

struct IUsStackLeaderVftable;

struct IUsStackLeader : public IUsUnitExtensionT<IUsStackLeaderVftable>
{ };

struct IUsStackLeaderVftable : public IUsUnitExtensionVftable
{
    using GetMovement = int(__thiscall*)(IUsStackLeader* thisptr);
    GetMovement getMovement;

    void* methods[8];
};

static_assert(sizeof(IUsStackLeaderVftable) == 10 * sizeof(void*),
              "IUsStackLeader vftable must have exactly 10 methods");

} // namespace game

#endif // USSTACKLEADER_H
