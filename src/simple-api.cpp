#include "simple-api.h"
#include "simple-common.h"
#include "fcitx-simple-module.h"
#include "fcitx-simple-ui.h"
#include "fcitx-simple-frontend.h"

template<typename T>
static inline bool FcitxSimpleSendRequest(FcitxInstance* instance, FcitxSimpleRequest* request, T* result) {
    int fd = FcitxSimpleGetFD(instance);
    FcitxSimpleCallQueue* queue = FcitxSimpleGetQueue(instance);
    FcitxSimpleCallQueueItem* item = FcitxSimpleCallQueueEnqueue(queue, request, result);
    if (fd < 0)
        return false;
    char c = 0;
    write(fd, &c, sizeof(c));
    sem_wait(&item->sem);
    free(item);
    return true;
}

FCITX_EXPORT_API
void FcitxSimpleInit(FcitxInstance* instance, FcitxSimpleEventHandler eventHandler, void* userData) {
    /* these function cache the result, hence we pre-cache before start */
    FcitxSimpleGetFD(instance);
    FcitxSimpleGetQueue(instance);
    FcitxSimpleFrontendInitIC(instance);
    FcitxSimpleSetEventCallback(instance, eventHandler, userData);
}

FCITX_EXPORT_API
int FcitxSimpleSendKeyEvent(FcitxInstance *instance, boolean release, FcitxKeySym key, unsigned int state, unsigned int keycode)
{
    FcitxSimpleRequest request;
    request.type = release ? SE_KeyEventRelease : SE_KeyEventPress;
    request.key = key;
    request.state = state;
    request.keycode = keycode;

    int result = 0;
    FcitxSimpleSendRequest<int>(instance, &request, &result);
    return result;
}

FCITX_EXPORT_API
void FcitxSimpleSetCurrentIM(FcitxInstance *instance, const char *name)
{
    FcitxSimpleRequest request;
    request.type = SE_SetCurrentIM;
    request.imname = name;

    FcitxSimpleSendRequest<void>(instance, &request, NULL);
}

void FcitxSimpleTriggerMenuItem(FcitxInstance* instance, const char* name, int index)
{
    FcitxSimpleRequest request;
    request.type = SE_TriggerMenuItem;
    request.menu.name = name;
    request.menu.index = index;

    FcitxSimpleSendRequest<void>(instance, &request, NULL);
}

void FcitxSimpleTriggerStatus(FcitxInstance* instance, const char* name)
{
    FcitxSimpleRequest request;
    request.type = SE_TriggerStatus;
    request.statusName = name;

    FcitxSimpleSendRequest<void>(instance, &request, NULL);
}
