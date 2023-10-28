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

#ifndef CUSTOMUNITDESCRIPTOR_H
#define CUSTOMUNITDESCRIPTOR_H

#include <optional>
#include <sol/sol.hpp>

namespace game {
struct CMidUnit;
} // namespace game

namespace hooks {

struct CustomUnitDescriptorFunctions;

class CustomUnitDescriptor
{
public:
    CustomUnitDescriptor();
    virtual ~CustomUnitDescriptor();

    int getAttackInitiative(const game::CMidUnit* unit, bool global, int boost, int def) const;
    int getAttackPower(const game::CMidUnit* unit, bool global, int def) const;
    int getAttack2Power(const game::CMidUnit* unit, bool global, int def) const;
    int getAttackDamage(const game::CMidUnit* unit, bool global, int boost, int def) const;
    int getAttack2Damage(const game::CMidUnit* unit, bool global, int boost, int def) const;
    int getAttackHeal(const game::CMidUnit* unit, bool global, int def) const;
    int getAttack2Heal(const game::CMidUnit* unit, bool global, int def) const;

protected:
    int getValue(std::optional<sol::function> function,
                 const game::CMidUnit* unit,
                 bool global,
                 int def) const;
    int getValue(std::optional<sol::function> function,
                 const game::CMidUnit* unit,
                 bool global,
                 int param,
                 int def) const;
    void showScriptErrorMessage(const char* reason) const;

private:
    const CustomUnitDescriptorFunctions& getFunctions() const;

    mutable CustomUnitDescriptorFunctions* mainThreadFunctions;
    mutable CustomUnitDescriptorFunctions* workerThreadFunctions;
};

CustomUnitDescriptor& getCustomUnitDescriptor();

} // namespace hooks

#endif // CUSTOMUNITDESCRIPTOR_H
