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

#include "simple-common.h"
#include "simple.h"
#include <fcitx-utils/utils.h>
#include <pthread.h>
#include <assert.h>

struct _FcitxSimpleCallQueue {
    pthread_mutex_t mutex;
    FcitxSimpleCallQueueItem* head;
    FcitxSimpleCallQueueItem* tail;
};

FCITX_EXPORT_API
FcitxSimpleCallQueue* FcitxSimpleCallQueueNew() {
    FcitxSimpleCallQueue* queue = fcitx_utils_new(FcitxSimpleCallQueue);
    pthread_mutex_init(&queue->mutex, NULL);
    return queue;
}

FCITX_EXPORT_API
FcitxSimpleCallQueueItem* FcitxSimpleCallQueueEnqueue(FcitxSimpleCallQueue* queue, FcitxSimpleRequest* request, void* resultData)
{
    FcitxSimpleCallQueueItem *item = fcitx_utils_new(FcitxSimpleCallQueueItem);
    sem_init(&item->sem,0 ,0);
    item->request = request;
    item->result = resultData;
    pthread_mutex_lock(&queue->mutex);
    if (queue->head) {
        assert(queue->tail);
        queue->tail = item;
    }
    else {
        queue->head = item;
        queue->tail = item;
    }
    pthread_mutex_unlock(&queue->mutex);
    return item;
}

FCITX_EXPORT_API
FcitxSimpleCallQueueItem* FcitxSimpleCallQueueDequeue(FcitxSimpleCallQueue* queue)
{
    FcitxSimpleCallQueueItem *item = NULL;
    pthread_mutex_lock(&queue->mutex);
    if (queue->head) {
        item = queue->head;
        queue->head = item->next;
        if (!queue->head)
            queue->tail = NULL;
    }
    pthread_mutex_unlock(&queue->mutex);
    return item;
}
