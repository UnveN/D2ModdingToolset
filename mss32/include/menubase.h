/*
 * This file is part of the modding toolset for Disciples 2.
 * (https://github.com/VladimirMakeev/D2ModdingToolset)
 * Copyright (C) 2020 Vladimir Makeev.
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

#ifndef MENUBASE_H
#define MENUBASE_H

#include "imagepointlist.h"
#include "interffullscreen.h"
#include "smartptr.h"
#include "taskmanagerholder.h"
#include <cstddef>

namespace game {

struct CDialogInterf;
struct CMenuPhase;
struct CMenuBaseVftable;

struct CMenuBaseData
{
    CMenuPhase* menuPhase;
    char unknown[44];
};

assert_size(CMenuBaseData, 48);

/** Base class for all menus. */
struct CMenuBase : public CInterfFullScreenT<CMenuBaseVftable>
{
    CMenuBaseData* menuBaseData;
};

assert_size(CMenuBase, 12);
assert_offset(CMenuBase, menuBaseData, 8);

struct CMenuBaseVftable : public CInterfaceVftable
{
    void* method35;
};
assert_vftable_size(CMenuBaseVftable, 35);

namespace CMenuBaseApi {

struct Api
{
    /**
     * Initializes CMenuBase object, sets vftable.
     * @param[in] thisptr pointer to existing CMenuBase object.
     * @param a1 meaning unknown.
     * @returns thisptr.
     */
    using Constructor = CMenuBase*(__thiscall*)(CMenuBase* thisptr, CMenuPhase* menuPhase);
    Constructor constructor;

    using Destructor = void(__thiscall*)(CMenuBase* thisptr);
    Destructor destructor;

    /** Creates ui elements of specified dialog to be used in menu. */
    using CreateMenu = int(__thiscall*)(CMenuBase* thisptr, const char* dialogName);
    CreateMenu createMenu;

    /** Returns CDialogInterf object. */
    using GetDialogInterface = CDialogInterf*(__thiscall*)(CMenuBase* thisptr);
    GetDialogInterface getDialogInterface;

    /** Returns back to main menu and clears network logic. */
    using ButtonCallback = void(__thiscall*)(CMenuBase* thisptr);
    ButtonCallback buttonBackCallback;

    /**
     * Creates functor for buttons of CMenuBase and its childs.
     * Reused from CMenuNewSkirmish.
     */
    using CreateButtonFunctor = SmartPointer*(__stdcall*)(SmartPointer* functor,
                                                          int a2,
                                                          CMenuBase* menu,
                                                          const ButtonCallback* callback);
    CreateButtonFunctor createButtonFunctor;

    using ListBoxDisplayCallback = void(__thiscall*)(CMenuBase* thisptr,
                                                     ImagePointList* contents,
                                                     const CMqRect* lineArea,
                                                     int index,
                                                     bool selected);

    using CreateListBoxDisplayFunctor = SmartPointer*(__stdcall*)(SmartPointer* functor,
                                                                  int a2,
                                                                  CMenuBase* menu,
                                                                  ListBoxDisplayCallback* callback);
    CreateListBoxDisplayFunctor createListBoxDisplayFunctor;
};

Api& get();

const CMenuBaseVftable* vftable();

} // namespace CMenuBaseApi

namespace editor {

struct CMenuBase : game::CInterfFullScreen
{
    game::ITaskManagerHolder taskManagerHolder;
    int unknown12;
    int unknown16; // constructor argument, probably CMenuPhase*
};

assert_size(CMenuBase, 20);
assert_offset(CMenuBase, unknown16, 16);

} // namespace editor

} // namespace game

#endif // MENUBASE_H
