/***************************************************************************
 *   Copyright (C) 2013~2013 by CSSlayer                                   *
 *   wengxt@gmail.com                                                      *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   51 Franklin St, Fifth Floor, Boston, MA 02110-1301, USA.              *
 ***************************************************************************/

#include "simple.h"
#include "fcitx-simple-module.h"
#include "fcitx/instance.h"

typedef struct _FcitxSimpleUI
{
    FcitxInstance* owner;
} FcitxSimpleUI;


static void* SimpleUICreate(FcitxInstance* instance);
static void SimpleUICloseInputWindow(void* arg);
static void SimpleUIShowInputWindow(void* arg);
static void SimpleUIRegisterMenu(void *arg, FcitxUIMenu* menu);
static void SimpleUIUpdateStatus(void *arg, FcitxUIStatus* status);
static void SimpleUIRegisterStatus(void *arg, FcitxUIStatus* status);
static void SimpleUIRegisterComplexStatus(void* arg, FcitxUIComplexStatus* status);
static void SimpleUIUpdateComplexStatus(void* arg, FcitxUIComplexStatus* status);
static void SimpleUIDestroy(void* arg);
static void* SimpleUITriggerStatus(FCITX_MODULE_FUNCTION_ARGS);
static void* SimpleUITriggerMenuItem(FCITX_MODULE_FUNCTION_ARGS);

FCITX_DEFINE_PLUGIN(fcitx_simple_ui, ui, FcitxUI) = {
    SimpleUICreate,
    SimpleUICloseInputWindow,
    SimpleUIShowInputWindow,
    NULL,
    SimpleUIUpdateStatus,
    SimpleUIRegisterStatus,
    SimpleUIRegisterMenu,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    SimpleUIDestroy,
    SimpleUIRegisterComplexStatus,
    SimpleUIUpdateComplexStatus,
    NULL
};

void* SimpleUICreate(FcitxInstance* instance)
{
    FcitxSimpleUI* ui = fcitx_utils_new(FcitxSimpleUI);
    UT_array *addons = FcitxInstanceGetAddons(instance);
    FcitxAddon *addon = FcitxAddonsGetAddonByName(addons, "fcitx-simple-module");

    FcitxModuleAddFunction(addon, SimpleUITriggerStatus);
    FcitxModuleAddFunction(addon, SimpleUITriggerMenuItem);

    ui->owner = instance;
    return ui;
}

void SimpleUIDestroy(void* arg)
{
    free(arg);
}

void SimpleUIShowInputWindow(void* arg)
{
    FcitxSimpleUI* ui = (FcitxSimpleUI*) arg;
    FcitxSimpleEvent event;
    event.type = SSE_ShowInputWindow;
    FcitxSimpleSendEvent(ui->owner, &event);
}

void SimpleUIRegisterStatus(void* arg, FcitxUIStatus* status)
{
    FcitxSimpleUI* ui = (FcitxSimpleUI*) arg;
    FcitxSimpleEvent event;
    event.type = SSE_RegisterStatus;
    event.status = status;
    FcitxSimpleSendEvent(ui->owner, &event);
}

void SimpleUICloseInputWindow(void* arg)
{
    FcitxSimpleUI* ui = (FcitxSimpleUI*) arg;
    FcitxSimpleEvent event;
    event.type = SSE_CloseInputWindow;
    FcitxSimpleSendEvent(ui->owner, &event);
}

void SimpleUIRegisterComplexStatus(void* arg, FcitxUIComplexStatus* status)
{
    FcitxSimpleUI* ui = (FcitxSimpleUI*) arg;
    FcitxSimpleEvent event;
    event.type = SSE_RegisterComplexStatus;
    event.compstatus = status;
    FcitxSimpleSendEvent(ui->owner, &event);
}

void SimpleUIUpdateComplexStatus(void* arg, FcitxUIComplexStatus* status)
{
    FcitxSimpleUI* ui = (FcitxSimpleUI*) arg;
    FcitxSimpleEvent event;
    event.type = SSE_UpdateComplexStatus;
    event.compstatus = status;
    FcitxSimpleSendEvent(ui->owner, &event);
}

void SimpleUIUpdateStatus(void* arg, FcitxUIStatus* status)
{
    FcitxSimpleUI* ui = (FcitxSimpleUI*) arg;
    FcitxSimpleEvent event;
    event.type = SSE_UpdateStatus;
    event.status = status;
    FcitxSimpleSendEvent(ui->owner, &event);
}

void SimpleUIRegisterMenu(void* arg, FcitxUIMenu* menu)
{
    FcitxSimpleUI* ui = (FcitxSimpleUI*) arg;
    FcitxSimpleEvent event;
    event.type = SSE_RegisterMenu;
    event.menu = menu;
    FcitxSimpleSendEvent(ui->owner, &event);
}

#define GetMenuItem(m, i) ((FcitxMenuItem*) utarray_eltptr(&(m)->shell, (i)))

void* SimpleUITriggerStatus(FCITX_MODULE_FUNCTION_ARGS)
{
    FCITX_MODULE_SELF(ui, FcitxSimpleUI);
    FCITX_MODULE_ARG(statusName, char*, 0);

    FcitxInstance* instance = ui->owner;
    FcitxUIMenu *menu = FcitxUIGetMenuByStatusName(instance, statusName);
    if (menu) {
        menu->UpdateMenu(menu);
        FcitxSimpleEvent event;
        event.type = SSE_ShowMenu;
        event.menuName = statusName;
        event.menu = menu;
        FcitxSimpleSendEvent(ui->owner, &event);
    } else {
        FcitxUIUpdateStatus(instance, statusName);
    }
    return NULL;
}

void* SimpleUITriggerMenuItem(FCITX_MODULE_FUNCTION_ARGS)
{
    FCITX_MODULE_SELF(ui, FcitxSimpleUI);
    FCITX_MODULE_ARG(statusName, char*, 0);
    FCITX_MODULE_ARG(index, int, 1);
    FcitxUIMenu* menup = FcitxUIGetMenuByStatusName(ui->owner, statusName);
    if (menup)
        menup->MenuAction(menup, index);
    return NULL;
}
