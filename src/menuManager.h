#pragma once

//#include "PolyOS.h"
#include <stdint.h>
#include "Arduino.h"
#include <Adafruit_GFX.h>

#include "fonts/font.h"
#include "fonts/font2.h"
#include "fonts/font3.h"

namespace Polygons
{
    class PolyOS;

    class MenuManager
    {
    public:
        PolyOS* polyOs;
        int _lastUpdatedParam;
        int _lastUpdateTime;
    
        void (*getPageName)(int page, char* dest);
        void (*getParameterName)(int paramId, char* dest);
        void (*getParameterDisplay)(int paramId, char* dest);
        
        // for displaying custom messages
        char message[32];
        uint messageTimestamp;
        int messageExpiryMillis;
        bool showMessage;

        MenuManager(PolyOS* polyOs);
        void setUpdated(int paramId = -1);
        void setMessage(const char* msg, int expireMillis = -1);
        void clearMessage();
        bool ShouldShowMessage();
    };

    namespace MenuManagerDrawing
    {
        void DrawSigmaMenu(MenuManager* menu);
        void DrawAlphaMenu(MenuManager* menu);
    }
}
