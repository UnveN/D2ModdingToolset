﻿/*
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

#pragma comment(lib, "detours.lib")

#include "autodialog.h"
#include "buildingbranch.h"
#include "buildingcat.h"
#include "game.h"
#include "hooks.h"
#include "log.h"
#include "menunewskirmishsingle.h"
#include "settings.h"
#include "unitsforhire.h"
#include "version.h"
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#include <detours.h>
#include <filesystem>
#include <fmt/format.h>
#include <string>

static HMODULE library{};
static void* registerInterface{};
static void* unregisterInterface{};

extern "C" __declspec(naked) void __stdcall RIB_register_interface(void)
{
    __asm {
        jmp registerInterface;
    }
}

extern "C" __declspec(naked) void __stdcall RIB_unregister_interface(void)
{
    __asm {
        jmp unregisterInterface;
    }
}

template <typename T>
static void writeProtectedMemory(T* address, T value)
{
    DWORD oldProtection{};
    if (VirtualProtect(address, sizeof(T), PAGE_EXECUTE_READWRITE, &oldProtection)) {
        *address = value;
        VirtualProtect(address, sizeof(T), oldProtection, &oldProtection);
        return;
    }

    hooks::logError("mssProxyError.log",
                    fmt::format("Failed to change memory protection for {:p}", (void*)address));
}

static void adjustGameRestrictions()
{
    auto& variables = game::gameVariables();
    // Allow game to load and scenario editor to create scenarios with maximum allowed spells level
    // set to zero, disabling usage of magic in scenario
    writeProtectedMemory(variables.spellMinLevel, 0);
    // Allow using units with tier higher than 5
    writeProtectedMemory(variables.unitMaxLevel, 10);
}

/** Hooks that used only in game. */
static void setupGameHooks()
{
    auto& fn = game::gameFunctions();

    DetourAttach((PVOID*)&fn.respopupInit, (PVOID)hooks::respopupInitHooked);
    DetourAttach((PVOID*)&fn.toggleShowBannersInit, (PVOID)hooks::toggleShowBannersInitHooked);
    // Fix game crash in battles with summoners
    DetourAttach((PVOID*)&fn.processUnitModifiers, (PVOID)hooks::processUnitModifiersHooked);

    DetourAttach((PVOID*)&game::CBuildingBranchApi::get().constructor,
                 (PVOID)hooks::buildingBranchCtorHooked);

    DetourAttach((PVOID*)&fn.chooseUnitLane, (PVOID)hooks::chooseUnitLaneHooked);

    // Random map generation
    /*DetourAttach((PVOID*)&game::CMenuNewSkirmishSingleApi::get().constructor,
                 (PVOID)hooks::menuNewSkirmishSingleCtorHooked);*/

    if (!hooks::unitsForHire().empty()) {
        DetourAttach((PVOID*)&fn.addPlayerUnitsToHireList,
                     (PVOID)hooks::addPlayerUnitsToHireListHooked);
    }
}

/** Hooks that used only in editor. */
static void setupEditorHooks()
{
    /** Returns true if tiles are suitable for site or ruin. */
    using CanPlace = bool(__stdcall*)(int, int, int);
    CanPlace canPlaceSite = (CanPlace)0x511142;
    CanPlace canPlaceRuin = (CanPlace)0x512376;
    // Check sites placement the same way as ruins, allowing them to be placed on water
    DetourAttach((PVOID*)&canPlaceSite, (PVOID)canPlaceRuin);
}

static void setupHooks()
{
    if (hooks::executableIsGame()) {
        setupGameHooks();
    } else {
        setupEditorHooks();
    }

    auto& fn = game::gameFunctions();

    // Support custom building branch category
    DetourAttach((PVOID*)&fn.createBuildingType, (PVOID)hooks::createBuildingTypeHooked);
    DetourAttach((PVOID*)&game::LBuildingCategoryTableApi::get().constructor,
                 (PVOID)hooks::buildingCategoryTableCtorHooked);

    DetourAttach((PVOID*)&fn.isTurnValid, (PVOID)hooks::isTurnValidHooked);
}

BOOL APIENTRY DllMain(HMODULE hDll, DWORD reason, LPVOID reserved)
{
    if (reason == DLL_PROCESS_DETACH) {
        FreeLibrary(library);
        return TRUE;
    }

    if (reason != DLL_PROCESS_ATTACH) {
        return TRUE;
    }

    library = LoadLibrary("Mss23.dll");
    if (!library) {
        MessageBox(NULL, "Failed to load Mss23.dll", "mss32.dll proxy", MB_OK);
        return FALSE;
    }

    registerInterface = GetProcAddress(library, "RIB_register_interface");
    unregisterInterface = GetProcAddress(library, "RIB_unregister_interface");
    if (!registerInterface || !unregisterInterface) {
        MessageBox(NULL, "Could not load Mss23.dll addresses", "mss32.dll proxy", MB_OK);
        return FALSE;
    }

    DisableThreadLibraryCalls(hDll);

    HMODULE module = GetModuleHandle(NULL);
    std::string moduleName(MAX_PATH, '\0');
    GetModuleFileName(module, &moduleName[0], MAX_PATH - 1);

    std::filesystem::path exeFilePath{moduleName};

    const std::error_code error = hooks::determineGameVersion(exeFilePath);
    if (error || hooks::gameVersion() == hooks::GameVersion::Unknown) {
        const std::string msg{
            fmt::format("Failed to determine target exe type.\nReason: {:s}.", error.message())};

        hooks::logError("mssProxyError.log", msg);
        MessageBox(NULL, msg.c_str(), "mss32.dll proxy", MB_OK);
        return FALSE;
    }

    exeFilePath.remove_filename();
    hooks::readUserSettings(exeFilePath / "disciple.ini");

    DetourTransactionBegin();
    DetourUpdateThread(GetCurrentThread());

    if (hooks::executableIsGame() && !hooks::loadUnitsForHire(exeFilePath)) {
        MessageBox(NULL, "Failed to load new units. Check error log for details.",
                   "mss32.dll proxy", MB_OK);
        return FALSE;
    }

    adjustGameRestrictions();
    setupHooks();

    const auto result = DetourTransactionCommit();
    if (result != NO_ERROR) {
        const std::string msg{
            fmt::format("Failed to hook game functions. Error code: {:d}.", result)};

        hooks::logError("mssProxyError.log", msg);
        MessageBox(NULL, msg.c_str(), "mss32.dll proxy", MB_OK);
        return FALSE;
    }

    return TRUE;
}