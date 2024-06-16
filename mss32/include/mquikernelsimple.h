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

#ifndef MQUIKERNELSIMPLE_H
#define MQUIKERNELSIMPLE_H

#include "d2list.h"
#include "d2pair.h"
#include "mqpoint.h"
#include "mqrect.h"
#include "mquikernel.h"

namespace game {

struct TimerData
{
    std::uint32_t timerId;
    int creationTime;
    std::uint32_t timeoutMs;
};

assert_size(TimerData, 12);

struct CMqUIKernelSimpleData
{
    int unknown;
    Pair<HWND, bool>* childWndHandle;
    WNDPROC oldWndProc;
    HINSTANCE instance;
    HINSTANCE instanceCopy;
    char* windowTitle;
    bool hasFocus;
    bool minimized;
    char padding[2];
    List<TimerData> timers;
    std::uint32_t lastTimerId;
    int systemTime;
    CMqPoint mousePosClient;
    CMqPoint mousePosScreen;
    CMqRect clientArea;
    bool unknown17;
    char padding2[3];
    IMqUIController* uiController;
};

assert_size(CMqUIKernelSimpleData, 92);

struct CMqUIKernelSimple : public IMqUIKernel
{
    CMqUIKernelSimpleData* data;
};

assert_size(CMqUIKernelSimple, 8);

} // namespace game

#endif // MQUIKERNELSIMPLE_H
