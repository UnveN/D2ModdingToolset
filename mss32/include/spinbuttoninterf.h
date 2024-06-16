/*
 * This file is part of the modding toolset for Disciples 2.
 * (https://github.com/VladimirMakeev/D2ModdingToolset)
 * Copyright (C) 2021 Vladimir Makeev.
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

#ifndef SPINBUTTONINTERF_H
#define SPINBUTTONINTERF_H

#include "d2string.h"
#include "functordispatch1.h"
#include "interface.h"
#include "mqpoint.h"

namespace game {

struct CButtonInterf;
struct CImage2TextBackground;
struct CSpinButtonInterf;
struct CDialogInterf;

struct CSpinButtonInterfData
{
    int textBoxChildIndex;
    SmartPtr<CBFunctorDispatch1<CSpinButtonInterf*>> onOptionChanged;
    Vector<String> options;
    int selectedOption;
    /**
     * Increments selected option.
     * Button on the right side of the spin box, first button in the .dlg SPIN description.
     */
    CButtonInterf* increment;
    /**
     * Decrements selected option.
     * Button on the left side of the spin box, second button in the .dlg SPIN description.
     */
    CButtonInterf* decrement;
    CImage2TextBackground* image2TextBgnd;
    CMqPoint bgndImagePos;
};

assert_size(CSpinButtonInterfData, 52);
assert_offset(CSpinButtonInterfData, increment, 32);

/**
 * Spin button ui element.
 * Represents SPIN from Interf.dlg or ScenEdit.dlg files.
 */
struct CSpinButtonInterf : public CInterface
{
    CSpinButtonInterfData* data;
};

assert_size(CSpinButtonInterf, 12);

namespace CSpinButtonInterfApi {

struct Api
{
    /** Sets up specified integer values as spin button options. */
    using SetValues = void(__thiscall*)(CSpinButtonInterf* thisptr, const Vector<int>* values);
    SetValues setValues;

    /** Sets up specified string as spin button options. */
    using SetOptions = void(__thiscall*)(CSpinButtonInterf* thisptr, const Vector<String>* options);
    SetOptions setOptions;

    /** Sets up option with specified index as currently selected. */
    using SetSelectedOption = void(__thiscall*)(CSpinButtonInterf* thisptr, int option);
    SetSelectedOption setSelectedOption;

    /**
     * Assigns functor that is called each time selected spin button option is changed.
     * Searches for spin button by its name in the provided dialog.
     * @returns found spin button with bound callback set or nullptr if not found.
     */
    using AssignFunctor = CSpinButtonInterf*(__stdcall*)(CDialogInterf* dialog,
                                                         const char* buttonName,
                                                         const char* dialogName,
                                                         SmartPointer* functor);
    AssignFunctor assignFunctor;
};

Api& get();

} // namespace CSpinButtonInterfApi

} // namespace game

#endif // SPINBUTTONINTERF_H
