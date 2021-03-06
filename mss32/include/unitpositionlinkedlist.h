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

#ifndef UNITPOSITIONLINKEDLIST_H
#define UNITPOSITIONLINKEDLIST_H

#include "d2pair.h"
#include "linkedlist.h"
#include "unitpositionpair.h"

namespace game {

using UnitPositionLinkedList = LinkedList<UnitPositionPair>;
using UnitPositionLinkedListNode = LinkedListNode<UnitPositionPair>;
using UnitPositionLinkedListIterator = LinkedListIterator<UnitPositionPair>;

namespace UnitPositionLinkedListApi {

struct Api
{
    using Constructor = UnitPositionLinkedList*(__thiscall*)(UnitPositionLinkedList* thisptr);
    Constructor constructor;

    using Destructor = void(__thiscall*)(UnitPositionLinkedList* thisptr);
    Destructor destructor;

    using GetIterator = UnitPositionLinkedListIterator*(
        __thiscall*)(UnitPositionLinkedList* thisptr, UnitPositionLinkedListIterator* iterator);
    GetIterator begin;
    GetIterator end;

    using Dereference = UnitPositionPair*(__thiscall*)(UnitPositionLinkedListIterator* thisptr);
    Dereference dereference;

    using Equals = bool(__thiscall*)(UnitPositionLinkedListIterator* thisptr,
                                     const UnitPositionLinkedListIterator* value);
    Equals equals;

    using Preincrement =
        UnitPositionLinkedListIterator*(__thiscall*)(UnitPositionLinkedListIterator* thisptr);
    Preincrement preinc;
};

Api& get();

} // namespace UnitPositionLinkedListApi

} // namespace game

#endif // UNITPOSITIONLINKEDLIST_H
