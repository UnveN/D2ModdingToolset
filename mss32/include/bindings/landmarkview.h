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

#ifndef LANDMARKVIEW_H
#define LANDMARKVIEW_H

#include "idview.h"
#include "point.h"
#include <string>

namespace sol {
class state;
}

namespace game {
struct CMidLandmark;
struct IMidgardObjectMap;
struct TLandmarkData;
struct CMidgardID;
} // namespace game

namespace bindings {

class LandmarkView
{
public:
    LandmarkView(const game::CMidLandmark* landmark, const game::IMidgardObjectMap* objectMap);

    static void bind(sol::state& lua);

    IdView getId() const;
    Point getPosition() const;
    Point getSize() const;
    std::string getDescription() const;
    IdView getTypeId() const;
    bool isMountain() const;

private:
    const game::CMidLandmark* landmark;
    const game::IMidgardObjectMap* objectMap;
};

} // namespace bindings

#endif // LANDMARKVIEW_H