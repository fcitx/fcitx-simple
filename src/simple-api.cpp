#include "simple-api.h"
#include "simple-common.h"
#include "fcitx-simple-module.h"
#include "fcitx-simple-ui.h"

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
    int fd = FcitxSimpleGetFD(instance);
    if (fd)
        return;
}

void FcitxSimpleTriggerMenuItem(FcitxInstance* instance, const char* name, int index)
{
    FcitxSimpleUITriggerMenuItem(instance, name, index);
}

void FcitxSimpleTriggerStatus(FcitxInstance* instance, const char* name)
{
    FcitxSimpleUITriggerStatus(instance, name);
}
