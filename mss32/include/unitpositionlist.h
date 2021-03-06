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

#ifndef UNITPOSITIONLIST_H
#define UNITPOSITIONLIST_H

#include "d2pair.h"
#include "sortedlist.h"
#include "unitpositionpair.h"

namespace game {

using UnitPositionList = SortedList<UnitPositionPair>;
using UnitPositionListNode = SortedListNode<UnitPositionPair>;
using UnitPositionListIterator = SortedListIterator<UnitPositionPair>;

namespace UnitPositionListApi {

struct Api
{
    using Constructor = UnitPositionList*(__thiscall*)(UnitPositionList* thisptr);
    Constructor constructor;

    using Destructor = void(__thiscall*)(UnitPositionList* thisptr);
    Destructor destructor;

    using CopyConstructor = UnitPositionList*(__thiscall*)(UnitPositionList* thisptr,
                                                           const UnitPositionList* src);
    CopyConstructor copyConstructor;

    using CopyAssignment = UnitPositionList*(__thiscall*)(UnitPositionList* thisptr,
                                                          const UnitPositionList* src);
    CopyAssignment copyAssignment;

    using GetIterator = UnitPositionListIterator*(__thiscall*)(UnitPositionList* thisptr,
                                                               UnitPositionListIterator* iterator);
    GetIterator begin;
    GetIterator end;

    using Dereference = UnitPositionPair*(__thiscall*)(UnitPositionListIterator* thisptr);
    Dereference dereference;

    using Equals = bool(__thiscall*)(UnitPositionListIterator* thisptr,
                                     const UnitPositionListIterator* value);
    Equals equals;

    using Preincrement = UnitPositionListIterator*(__thiscall*)(UnitPositionListIterator* thisptr);
    Preincrement preinc;

    using FindByPosition = UnitPositionPair*(__stdcall*)(UnitPositionPair* value,
                                                         const UnitPositionList* list,
                                                         int unitPosition);
    FindByPosition findByPosition;

    using HasNegativePosition = bool(__stdcall*)(const UnitPositionList* thisptr);
    HasNegativePosition hasNegativePosition;
};

Api& get();

} // namespace UnitPositionListApi

} // namespace game

#endif // UNITPOSITIONLIST_H
