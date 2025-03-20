#ifndef ControllerFACTORY_H
#define ControllerFACTORY_H

#include "cacheflush.h"
#include "controller.h"
#include "initcontroller.h"
#include "messagecontroller.h"
#include "publishjottingcontroller.h"
#include "scanandcheckjottingcontroller.h"

class ControllerFactory
{
public: 
    static ControllerFactory *getInstance();
    ~ControllerFactory();

    ScanAndCheckJottingController* createScanAndCheckJottingController();
    InitController *createInitController();
    PublishJottingController *createPublishJottingController();
    MessageController *createMessageController();
    CacheFlush *createCacheFlush();
private:
    ControllerFactory();
    static ControllerFactory* m_instance;

};

#endif // ControllerFACTORY_H
