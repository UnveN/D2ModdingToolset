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

#include "customunitdescriptor.h"
#include "customunitdescriptorfunctions.h"
#include "unitview.h"
#include <fmt/format.h>
#include <thread>

extern std::thread::id mainThreadId;

namespace hooks {

static const char* SCRIPT_FILE_NAME = "unitDescriptor.lua";

CustomUnitDescriptor::CustomUnitDescriptor()
    : mainThreadFunctions(nullptr)
    , workerThreadFunctions(nullptr)
{ }

CustomUnitDescriptor ::~CustomUnitDescriptor()
{
    delete mainThreadFunctions;
    delete workerThreadFunctions;
}

int CustomUnitDescriptor::getAttackInitiative(const game::CMidUnit* unit,
                                              bool global,
                                              int boost,
                                              int def) const
{
    return getValue(getFunctions().getAttackInitiative, unit, global, boost, def);
}

int CustomUnitDescriptor::getAttackPower(const game::CMidUnit* unit, bool global, int def) const
{
    return getValue(getFunctions().getAttackPower, unit, global, def);
}

int CustomUnitDescriptor::getAttack2Power(const game::CMidUnit* unit, bool global, int def) const
{
    return getValue(getFunctions().getAttack2Power, unit, global, def);
}

int CustomUnitDescriptor::getAttackDamage(const game::CMidUnit* unit,
                                          bool global,
                                          int boost,
                                          int def) const
{
    return getValue(getFunctions().getAttackDamage, unit, global, boost, def);
}

int CustomUnitDescriptor::getAttack2Damage(const game::CMidUnit* unit,
                                           bool global,
                                           int boost,
                                           int def) const
{
    return getValue(getFunctions().getAttack2Damage, unit, global, boost, def);
}

int CustomUnitDescriptor::getAttackHeal(const game::CMidUnit* unit, bool global, int def) const
{
    return getValue(getFunctions().getAttackHeal, unit, global, def);
}

int CustomUnitDescriptor::getAttack2Heal(const game::CMidUnit* unit, bool global, int def) const
{
    return getValue(getFunctions().getAttack2Heal, unit, global, def);
}

int CustomUnitDescriptor::getValue(std::optional<sol::function> function,
                                   const game::CMidUnit* unit,
                                   bool global,
                                   int def) const
{
    try {
        if (function) {
            bindings::UnitView unitView{unit};
            return (*function)(unitView, global, def);
        }
    } catch (const std::exception& e) {
        showScriptErrorMessage(e.what());
    }

    return def;
}

int CustomUnitDescriptor::getValue(std::optional<sol::function> function,
                                   const game::CMidUnit* unit,
                                   bool global,
                                   int param,
                                   int def) const
{
    try {
        if (function) {
            bindings::UnitView unitView{unit};
            return (*function)(unitView, global, param, def);
        }
    } catch (const std::exception& e) {
        showScriptErrorMessage(e.what());
    }

    return def;
}

void CustomUnitDescriptor::showScriptErrorMessage(const char* reason) const
{
    showErrorMessageBox(fmt::format("Failed to run '{:s}' script.\n"
                                    "Reason: '{:s}'",
                                    SCRIPT_FILE_NAME, reason));
}

const CustomUnitDescriptorFunctions& CustomUnitDescriptor::getFunctions() const
{
    auto& functions = std::this_thread::get_id() == mainThreadId ? mainThreadFunctions
                                                                 : workerThreadFunctions;
    if (functions == nullptr) {
        // Functions need to be initialized in a corresponding thread to use its own Lua instance
        functions = new CustomUnitDescriptorFunctions(SCRIPT_FILE_NAME);
    }

    return *functions;
}

CustomUnitDescriptor& getCustomUnitDescriptor()
{
    static CustomUnitDescriptor value{};

    return value;
}

} // namespace hooks
