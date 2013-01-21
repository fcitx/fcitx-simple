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

#include <pthread.h>
#include <semaphore.h>
#include <fcntl.h>

#include "fcitx/addon.h"
#include "fcitx/module.h"
#include "fcitx/fcitx.h"
#include "fcitx/instance.h"

#include "simple.h"
#include "simple-common.h"
#include "fcitx-simple-frontend.h"
#include "fcitx-simple-ui.h"

typedef struct _FcitxSimpleModule {
    int selfPipe[2];
    sem_t sem;
    FcitxInstance* owner;
    FcitxSimpleEventHandler eventCallback;
    void* callbackArg;
    FcitxSimpleCallQueue* queue;
} FcitxSimpleModule;

static void* SimpleModuleCreate(FcitxInstance* instance);
static void SimpleModuleSetFD(void* arg);
static void SimpleModuleProcessEvent(void* arg);
static void SimpleModuleDestroy(void* arg);
static void SimpleModuleReloadConfig(void* arg);
DECLARE_ADDFUNCTIONS(Simple)

FCITX_DEFINE_PLUGIN(fcitx_simple_module, module, FcitxModule) = {
    SimpleModuleCreate,
    SimpleModuleSetFD,
    SimpleModuleProcessEvent,
    SimpleModuleDestroy,
    SimpleModuleReloadConfig
};

void* SimpleModuleCreate(FcitxInstance* instance)
{
    FcitxSimpleModule* simple = fcitx_utils_new(FcitxSimpleModule);
    simple->owner = instance;

    if (0 != pipe(simple->selfPipe)) {
        free(simple);
        return NULL;
    }

    fcntl(simple->selfPipe[0], F_SETFL, O_NONBLOCK);
    fcntl(simple->selfPipe[1], F_SETFL, O_NONBLOCK);

    simple->queue = FcitxSimpleCallQueueNew();

    FcitxSimpleAddFunctions(instance);
    return simple;
}

void SimpleModuleDestroy(void* arg)
{
    FcitxSimpleModule* simple = (FcitxSimpleModule*) arg;
    sem_destroy(&simple->sem);
    close(simple->selfPipe[0]);
    close(simple->selfPipe[1]);
    free(simple);
}


void SimpleModuleSetFD(void* arg)
{
    FcitxSimpleModule* simple = (FcitxSimpleModule*) arg;
    FcitxInstance* instance = simple->owner;
    int maxfd = simple->selfPipe[0];
    FD_SET(maxfd, FcitxInstanceGetReadFDSet(instance));
    if (maxfd > FcitxInstanceGetMaxFD(instance)) {
        FcitxInstanceSetMaxFD(instance, maxfd);
    }
}

void SimpleModuleProcessEvent(void* arg)
{
    FcitxSimpleModule* simple = (FcitxSimpleModule*) arg;
    FcitxInstance* instance = simple->owner;
    if (!FD_ISSET(simple->selfPipe[0], FcitxInstanceGetReadFDSet(instance)))
        return;

    char c = 0;

    while(read(simple->selfPipe[0], &c, sizeof(c)) > 0);

    FcitxSimpleCallQueueItem* item = NULL;
    while ((item = FcitxSimpleCallQueueDequeue(simple->queue)) != NULL) {
        switch(item->request->type) {
            case SE_KeyEventPress:
            case SE_KeyEventRelease:
                {
                    int *result = (int*) item->result;
                    *result = FcitxSimpleFrontendProcessKey(instance, item->request);
                    break;
                }
            case SE_SetCurrentIM:
                FcitxInstanceSwitchIMByName(instance, item->request->imname);
                break;
            case SE_TriggerMenuItem:
                FcitxSimpleUITriggerMenuItem(instance, item->request->menu.name, item->request->menu.index);
                break;
            case SE_TriggerStatus:
                FcitxSimpleUITriggerStatus(instance, item->request->statusName);
                break;
            case SE_End:
                FcitxInstanceEnd(instance);
                break;
        }
        sem_post(&item->sem);
    }
}

void SimpleModuleReloadConfig(void* arg)
{

}

static void
SimpleModuleSendEvent(FcitxSimpleModule *simple, FcitxSimpleEvent *event)
{
    if (simple->eventCallback) {
        simple->eventCallback(simple->callbackArg, event);
    }
}

static void
SimpleModuleSetEventCallback(FcitxSimpleModule *simple,
                             FcitxSimpleEventHandler eventCallback,
                             void *callbackArg)
{
    simple->eventCallback = eventCallback;
    simple->callbackArg = callbackArg;
}

#include "fcitx-simple-module-addfunctions.h"
