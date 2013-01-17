/***************************************************************************
 *   Copyright (C) 2012~2012 by CSSlayer                                   *
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

#include "fcitx-config/fcitx-config.h"
#include "fcitx/frontend.h"
#include "fcitx/instance.h"
#include "fcitx/module.h"

#include "simple.h"
#include "fcitx-simple-module.h"

typedef struct _FcitxSimpleFrontend
{
    FcitxInstance* owner;
    int frontendid;
    FcitxInputContext* ic;
} FcitxSimpleFrontend;

static void* SimpleFrontendCreate(FcitxInstance* instance, int frontendid);
static boolean SimpleFrontendDestroy(void* arg);
void SimpleFrontendCreateIC(void* arg, FcitxInputContext* context, void *priv);
boolean SimpleFrontendCheckIC(void* arg, FcitxInputContext* context, void* priv);
void SimpleFrontendDestroyIC(void* arg, FcitxInputContext* context);
static void SimpleFrontendEnableIM(void* arg, FcitxInputContext* ic);
static void SimpleFrontendCloseIM(void* arg, FcitxInputContext* ic);
static void SimpleFrontendCommitString(void* arg, FcitxInputContext* ic, const char* str);
static void SimpleFrontendForwardKey(void* arg, FcitxInputContext* ic, FcitxKeyEventType event, FcitxKeySym sym, unsigned int state);
static void SimpleFrontendSetWindowOffset(void* arg, FcitxInputContext* ic, int x, int y);
static void SimpleFrontendGetWindowRect(void* arg, FcitxInputContext* ic, int* x, int* y, int* w, int* h);
static void SimpleFrontendUpdatePreedit(void* arg, FcitxInputContext* ic);
static void SimpleFrontendDeleteSurroundingText(void* arg, FcitxInputContext* ic, int offset, unsigned int size);
static boolean SimpleFrontendGetSurroundingText(void* arg, FcitxInputContext* ic, char** str, unsigned int* cursor, unsigned int* anchor);
static void SimpleFrontendUpdateClientSideUI(void* arg, FcitxInputContext* ic);
static boolean SimpleFrontendCheckICFromSameApplication(void* arg, FcitxInputContext* icToCheck, FcitxInputContext* ic);
static pid_t SimpleFrontendGetPid(void* arg, FcitxInputContext* ic);
DECLARE_ADDFUNCTIONS(SimpleFrontend)

FCITX_DEFINE_PLUGIN(fcitx_simple_frontend, frontend, FcitxFrontend) = {
    SimpleFrontendCreate,
    SimpleFrontendDestroy,
    SimpleFrontendCreateIC,
    SimpleFrontendCheckIC,
    SimpleFrontendDestroyIC,
    SimpleFrontendEnableIM,
    SimpleFrontendCloseIM,
    SimpleFrontendCommitString,
    SimpleFrontendForwardKey,
    SimpleFrontendSetWindowOffset,
    SimpleFrontendGetWindowRect,
    SimpleFrontendUpdatePreedit,
    SimpleFrontendUpdateClientSideUI,
    NULL,
    SimpleFrontendCheckICFromSameApplication,
    SimpleFrontendGetPid,
    SimpleFrontendDeleteSurroundingText,
    SimpleFrontendGetSurroundingText
};

void*
SimpleFrontendCreate(FcitxInstance* instance, int frontendid)
{
    FcitxSimpleFrontend* simple = fcitx_utils_new(FcitxSimpleFrontend);
    simple->owner = instance;
    simple->frontendid = frontendid;
    FcitxSimpleFrontendAddFunctions(instance);
    return simple;
}

static void
SimpleFrontendInitIC(FcitxSimpleFrontend *simple)
{
    if (simple->ic == NULL)
        simple->ic = FcitxInstanceCreateIC(simple->owner,
                                           simple->frontendid, NULL);
    if (FcitxInstanceSetCurrentIC(simple->owner, simple->ic)) {
        FcitxUIOnInputFocus(simple->owner);
    }
}

static int
SimpleFrontendProcessKey(FcitxSimpleFrontend *simple,
                         FcitxSimpleRequest *request)
{
    FcitxInputState *input = FcitxInstanceGetInputState(simple->owner);

    if (simple->ic == NULL)
        simple->ic = FcitxInstanceCreateIC(simple->owner,
                                           simple->frontendid, NULL);

    FcitxInputContext* ic = FcitxInstanceGetCurrentIC(simple->owner);

    if (ic != simple->ic || ic->frontendid != simple->frontendid) {
        FcitxInstanceSetCurrentIC(simple->owner, simple->ic);
        FcitxUIOnInputFocus(simple->owner);
    }
    ic = simple->ic;
    FcitxKeySym sym;
    unsigned int state;

    state = request->state & FcitxKeyState_SimpleMask;
    state &= FcitxKeyState_UsedMask;
    FcitxHotkeyGetKey(request->key, state, &sym, &state);

    if (request->key == 0)
        return 0;

    //FcitxInstanceEnableIM(ipc->owner, ic, false);

    FcitxInputStateSetKeyCode(input, request->keycode);
    FcitxInputStateSetKeySym(input, request->key);
    FcitxInputStateSetKeyState(input, request->state);

    INPUT_RETURN_VALUE retVal = FcitxInstanceProcessKey(simple->owner,
                                                        request->type == SE_KeyEventPress ? FCITX_PRESS_KEY : FCITX_RELEASE_KEY,
                                                        0, sym, state);
    FcitxInputStateSetKeyCode(input, 0);
    FcitxInputStateSetKeySym(input, 0);
    FcitxInputStateSetKeyState(input, 0);

    if (retVal & IRV_FLAG_FORWARD_KEY || retVal == IRV_TO_PROCESS) {
        return 0;
    } else {
        return 1;
    }
}


boolean SimpleFrontendCheckIC(void* arg, FcitxInputContext* context, void* priv)
{
    return true;
}

boolean SimpleFrontendCheckICFromSameApplication(void* arg, FcitxInputContext* icToCheck, FcitxInputContext* ic)
{
    return false;
}

void SimpleFrontendCloseIM(void* arg, FcitxInputContext* ic)
{

}

void SimpleFrontendCommitString(void* arg, FcitxInputContext* ic, const char* str)
{
    FcitxSimpleFrontend* simple = (FcitxSimpleFrontend*) arg;
    FcitxSimpleEvent event;
    event.type = SSE_CommitString;
    event.commitString = str;
    FcitxSimpleSendEvent(simple->owner, &event);
}

void SimpleFrontendCreateIC(void* arg, FcitxInputContext* context, void* priv)
{
    // FcitxSimpleFrontend* simple = (FcitxSimpleFrontend*) arg;
    context->privateic = NULL;
    context->contextCaps = CAPACITY_PREEDIT;
}

void SimpleFrontendDeleteSurroundingText(void* arg, FcitxInputContext* ic, int offset, unsigned int size)
{

}

boolean SimpleFrontendDestroy(void* arg)
{
    return true;
}

void SimpleFrontendDestroyIC(void* arg, FcitxInputContext* context)
{

}

void SimpleFrontendEnableIM(void* arg, FcitxInputContext* ic)
{

}

void SimpleFrontendForwardKey(void* arg, FcitxInputContext* ic, FcitxKeyEventType event, FcitxKeySym sym, unsigned int state)
{

}

pid_t SimpleFrontendGetPid(void* arg, FcitxInputContext* ic)
{
    return 0;
}

boolean SimpleFrontendGetSurroundingText(void* arg, FcitxInputContext* ic, char** str, unsigned int* cursor, unsigned int* anchor)
{
    return false;
}

void SimpleFrontendGetWindowRect(void* arg, FcitxInputContext* ic, int* x, int* y, int* w, int* h)
{
    return;
}

void SimpleFrontendSetWindowOffset(void* arg, FcitxInputContext* ic, int x, int y)
{
    return;
}

void SimpleFrontendUpdateClientSideUI(void* arg, FcitxInputContext* ic)
{
    return;
}

void SimpleFrontendUpdatePreedit(void* arg, FcitxInputContext* ic)
{
    FcitxSimpleFrontend* simple = (FcitxSimpleFrontend*) arg;
    FcitxSimpleEvent event;
    event.type = SSE_UpdatePreedit;
    FcitxSimpleSendEvent(simple->owner, &event);
    return;
}

#include "fcitx-simple-frontend-addfunctions.h"
