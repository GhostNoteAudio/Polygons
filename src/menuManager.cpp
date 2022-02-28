#include "menuManager.h"
#include "PolygonsBase.h"
#include "PolyOS.h"

namespace Polygons
{
    MenuManager::MenuManager(PolyOS* polyOsPtr)
    {
        this->polyOs = polyOsPtr;
        showMessage = false;
        _lastUpdatedParam = 255;
        _lastUpdateTime = 0;

        getPageName = [](int page, char* dest)
        {
            sprintf(dest, "Page %d", page);
        };

        getParameterName = [](int paramId, char* dest) 
        {
            sprintf(dest, "Param %d", paramId);
        };

        getParameterDisplay = [](int paramId, char* dest)
        {
            sprintf(dest, "%d", 0);
        };
    }

    void MenuManager::setUpdated(int paramId)
    {
        if (paramId == -1)
        {
            _lastUpdateTime = 0;
            _lastUpdatedParam = -1;
        }
        else
        {
            _lastUpdateTime = millis();
            _lastUpdatedParam = paramId;
        }
    }

    void MenuManager::setMessage(const char* msg, int expireMillis)
    {
        strcpy(message, msg);
        messageExpiryMillis = expireMillis;
        messageTimestamp = millis();
        showMessage = true;
    }

    void MenuManager::clearMessage()
    {
        showMessage = false;
    }

    bool MenuManager::ShouldShowMessage()
    {
        if (messageExpiryMillis == -1)
            return showMessage;
        else
            return (millis() - messageTimestamp) < (uint)messageExpiryMillis;
    }

    namespace MenuManagerDrawing
    {
        void DrawSigmaMenu(MenuManager* menu)
        {
            char buffer[32];
            auto canvas = getCanvas();
            canvas->fillRect(0, 0, 256, 64, 0);
            canvas->setTextWrap(false);

            if (millis() - menu->_lastUpdateTime < 1500)
            {
                int paramId = menu->polyOs->getParamEncoded(menu->polyOs->SelectedPage * 8 + menu->_lastUpdatedParam);
                menu->getParameterName(paramId, buffer);
                if (strlen(buffer) > 0)
                {
                    canvas->fillRect(10, 10, 128-20, 64-20, 0);
                    canvas->drawRect(10, 10, 128-20, 64-20, 1);
                    canvas->setFont(NULL);
                    canvas->cp437(true);
                    int16_t  x1, y1;
                    uint16_t w, h;      
                    
                    canvas->getTextBounds(buffer, 0, 0, &x1, &y1, &w, &h);
                    canvas->setCursor(64 - w/2, 19);
                    canvas->println(buffer);

                    menu->getParameterDisplay(paramId, buffer);
                    canvas->getTextBounds(buffer, 0, 0, &x1, &y1, &w, &h);
                    canvas->setCursor(64 - w/2, 34);
                    canvas->println(buffer);
                }
            }
        }

        void DrawAlphaMenu(MenuManager* menu)
        {
            char buffer[32];
            auto canvas = getCanvas();
            canvas->fillRect(0, 0, 256, 64, 0);
            canvas->setTextWrap(false);
            canvas->setTextSize(1);
            canvas->setFont(&AtlantisInternational_jen08pt7b);
            
            // draw pages
            for (int i = 0; i < 8; i++)
            {
                int y = i < 4 ? 7 : 61;
                int ySel = i < 4 ? 0 : 54;
                int x = (i % 4) * 64;
                if (i == menu->polyOs->SelectedPage)
                {
                    canvas->setTextColor(0);
                    canvas->fillRect(x, ySel, 64, 10, 1);
                }
                else
                {
                    canvas->setTextColor(1);
                }

                canvas->setCursor(x+1,y);
                menu->getPageName(i, buffer);
                canvas->println(buffer);
            }

            // Draw lines
            canvas->setTextColor(1);
            canvas->fillRect(0, 10, 256, 1, 1);
            canvas->fillRect(0, 53, 256, 1, 1);
                    
            // Draw parameter names
            for (size_t i = 0; i < 8; i++)
            {
                int y = i < 4 ? 19 : 40;
                int x = (i % 4) * 64;

                canvas->setCursor(x+1,y);
                int paramId = menu->polyOs->getParamEncoded(menu->polyOs->SelectedPage * 8 + i);
                if (paramId == -1)
                    continue;
                menu->getParameterName(paramId, buffer);
                canvas->println(buffer);
            }

            // Draw parameter values
            for (size_t i = 0; i < 8; i++)
            {
                int y = i < 4 ? 28 : 50;
                int x = (i % 4) * 64;

                canvas->setCursor(x+1,y);
                int paramId = menu->polyOs->getParamEncoded(menu->polyOs->SelectedPage * 8 + i);
                if (paramId == -1)
                    continue;
                menu->getParameterDisplay(paramId, buffer);
                canvas->println(buffer);
            }

            // show overlay window
            if (menu->ShouldShowMessage())
            {
                canvas->fillRect(10, 10, 256-20, 64-20, 0);
                canvas->drawRect(10, 10, 256-20, 64-20, 1);
                canvas->setFont(NULL);
                canvas->cp437(true);
                int16_t  x1, y1;
                uint16_t w, h;      
                
                canvas->getTextBounds(menu->message, 0, 0, &x1, &y1, &w, &h);
                canvas->setCursor(128 - w/2, 19);
                canvas->println(menu->message);
            }
            /*else if (millis() - menu->_lastUpdateTime < 1000)
            {
                int paramId = menu->polyOs->getParamPage(menu->SelectedPage, menu->_lastUpdatedParam);
                menu->getParameterName(paramId, buffer);
                if (strlen(buffer) > 0)
                {
                    canvas->fillRect(10, 10, 256-20, 64-20, 0);
                    canvas->drawRect(10, 10, 256-20, 64-20, 1);
                    canvas->setFont(NULL);
                    canvas->cp437(true);
                    int16_t  x1, y1;
                    uint16_t w, h;      
                    
                    canvas->getTextBounds(buffer, 0, 0, &x1, &y1, &w, &h);
                    canvas->setCursor(128 - w/2, 19);
                    canvas->println(buffer);

                    menu->getParameterDisplay(paramId, buffer);
                    canvas->getTextBounds(buffer, 0, 0, &x1, &y1, &w, &h);
                    canvas->setCursor(128 - w/2, 34);
                    canvas->println(buffer);
                }
            }*/
        }
    }
}