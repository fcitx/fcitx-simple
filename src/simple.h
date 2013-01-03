/***************************************************************************
 *   Copyright (C) 2012~2013 by CSSlayer                                   *
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

#ifndef _FCITX_MODULE_SIMPLE_H
#define _FCITX_MODULE_SIMPLE_H

#include <stdint.h>
#include <fcitx/instance.h>
#include <fcitx/module.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct _FcitxSimpleCallQueue FcitxSimpleCallQueue;

typedef struct _FcitxSimpleCallQueueItem FcitxSimpleCallQueueItem;

typedef enum _FcitxSimpleEventType {
    SE_KeyEventPress,
    SE_KeyEventRelease,
} FcitxSimpleEventType;

typedef enum _FcitxSimpleServerEventType {
    SSE_ShowInputWindow,
    SSE_CloseInputWindow,
    SSE_RegisterComplexStatus,
    SSE_RegisterStatus,
    SSE_UpdateComplexStatus,
    SSE_UpdateStatus,
    SSE_RegisterMenu,
    SSE_ShowMenu
} FcitxSimpleServerEventType;

typedef struct _FcitxSimpleEvent {
    uint32_t type;
    union {
        FcitxUIStatus* status;
        FcitxUIComplexStatus* compstatus;
        struct {
            FcitxUIMenu* menu;
            char* menuName;
        };
    };
} FcitxSimpleEvent;

typedef void (*FcitxSimpleEventHandler)(void* arg, FcitxSimpleEvent* event);

typedef struct _FcitxSimpleRequest {
    uint32_t type;
    union {
        struct {
            uint32_t key;
            uint32_t state;
            uint32_t keycode;
        };
    };
} FcitxSimpleRequest;

struct _FcitxSimpleCallQueueItem {
    FcitxSimpleRequest* request;
    void* result;
    sem_t sem;
    struct _FcitxSimpleCallQueueItem* next;
};

typedef struct _FcitxSimpleScheduler FcitxSimpleScheduler;

#ifdef __cplusplus
}
#endif

#endif // _FCITX_MODULE_SIMPLE_H
