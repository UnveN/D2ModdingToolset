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

#ifndef BATATTACKHEAL_H
#define BATATTACKHEAL_H

#include "batattack.h"
#include "midgardid.h"

namespace game {

struct IAttack;

struct CBatAttackHeal : public CBatAttackBase
{
    CMidgardID unitId;
    CMidgardID attackImplUnitId;
    int attackNumber; /**< 1 if this is a unit's primary attack, 2 for secondary. */
    IAttack* attackImpl;
    IAttack* attack2Impl;
    bool attack2Initialized;
    char padding[3];
    int attackImplMagic;
};

assert_size(CBatAttackHeal, 32);

namespace CBatAttackHealApi {

IBatAttackVftable* vftable();

} // namespace CBatAttackHealApi

} // namespace game

#endif // BATATTACKHEAL_H
