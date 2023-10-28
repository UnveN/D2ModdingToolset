/*
 * This file is part of the modding toolset for Disciples 2.
 * (https://github.com/VladimirMakeev/D2ModdingToolset)
 * Copyright (C) 2023 Stanislav Egorov.
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

#ifndef CUSTOMUNITDESCRIPTORFUNCTIONS_H
#define CUSTOMUNITDESCRIPTORFUNCTIONS_H

#include <lua.hpp>
#include <optional>
#include <sol/sol.hpp>
#include <string>

namespace hooks {

struct CustomUnitDescriptorFunctions
{
    CustomUnitDescriptorFunctions(const std::string& scriptFileName);

    std::optional<sol::environment> environment;
    std::optional<sol::function> getAttackInitiative;
    std::optional<sol::function> getAttackPower;
    std::optional<sol::function> getAttack2Power;
    std::optional<sol::function> getAttackDamage;
    std::optional<sol::function> getAttack2Damage;
    std::optional<sol::function> getAttackHeal;
    std::optional<sol::function> getAttack2Heal;
};

} // namespace hooks

#endif // CUSTOMUNITDESCRIPTORFUNCTIONS_H
