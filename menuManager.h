#pragma once

#include "Polygons.h"
#include "font.h"
#include "font2.h"
#include "font3.h"

class MenuManager
{
private:
    int lastUpdatedParam;
    int lastUpdateTime;
public:
    uint8_t pageCount;
    int SelectedPage;
    void (*getPageName)(int page, char* dest);
    void (*getParameterShortName)(int page, int index, char* dest);
    void (*getParameterLongName)(int page, int index, char* dest);
    void (*getParameterDisplay)(int page, int index, char* dest);
    
    // for displaying custom messages
    char message[32];
    uint messageTimestamp;
    int messageExpiryMillis;
    bool showMessage;

    inline MenuManager()
    {
        showMessage = false;
        lastUpdatedParam = 255;
        lastUpdateTime = 0;

        SelectedPage = 0;
        pageCount = 8;

        getPageName = [](int page, char* dest)
        {
            sprintf(dest, "PAGE%d", page);
        };

        getParameterShortName = [](int page, int index, char* dest) 
        {
            sprintf(dest, "PAR%d", (page * 8 + index));
        };

        getParameterLongName = getParameterShortName;

        getParameterDisplay = [](int page, int index, char* dest)
        {
            strcpy(dest, "");
        };
    }

    inline void setUpdated(int paramId = -1)
    {
        if (paramId == -1)
        {
            lastUpdateTime = 0;
            lastUpdatedParam = -1;
        }
        else
        {
            lastUpdateTime = millis();
            lastUpdatedParam = paramId;
        }
    }

    inline void setMessage(const char* msg, int expireMillis = -1)
    {
        strcpy(message, msg);
        messageExpiryMillis = expireMillis;
        messageTimestamp = millis();
        showMessage = true;
    }

    inline void clearMessage()
    {
        showMessage = false;
    }

    inline bool ShouldShowMessage()
    {
        if (messageExpiryMillis == -1)
            return showMessage;
        else
            return (millis() - messageTimestamp) < (uint)messageExpiryMillis;
    }

    inline void Draw()
    {
        char buffer[32];
        auto canvas = getCanvas();
        canvas->fillRect(0, 0, 128, 64, 0);
        canvas->setTextWrap(false);
        canvas->setTextSize(1);
        canvas->setFont(&AtlantisInternational_jen08pt7b);
        
        // draw pages
        for (int i = 0; i < 8; i++)
        {
            int y = i < 4 ? 7 : 61;
            int ySel = i < 4 ? 0 : 54;
            int x = (i % 4) * 32;
            if (i == SelectedPage)
            {
                canvas->setTextColor(0);
                canvas->fillRect(x, ySel, 32, 10, 1);
            }
            else
            {
                canvas->setTextColor(1);
            }

            canvas->setCursor(x+1,y);
            getPageName(i, buffer);
            canvas->println(buffer);
        }

        // Draw lines
        canvas->setTextColor(1);
        canvas->fillRect(0, 10, 128, 1, 1);
        canvas->fillRect(0, 53, 128, 1, 1);
                
        // Draw parameter names
        for (size_t i = 0; i < 8; i++)
        {
            int y = i < 4 ? 19 : 40;
            int x = (i % 4) * 32;

            canvas->setCursor(x+1,y);
            getParameterShortName(SelectedPage, i, buffer);
            canvas->println(buffer);
        }

        // Draw parameter values
        for (size_t i = 0; i < 8; i++)
        {
            int y = i < 4 ? 28 : 50;
            int x = (i % 4) * 32;

            canvas->setCursor(x+1,y);
            getParameterDisplay(SelectedPage, i, buffer);
            canvas->println(buffer);
        }

        // show overlay window
        if (ShouldShowMessage())
        {
            canvas->fillRect(10, 10, 128-20, 64-20, 0);
            canvas->drawRect(10, 10, 128-20, 64-20, 1);
            canvas->setFont(NULL);
            canvas->cp437(true);
            int16_t  x1, y1;
            uint16_t w, h;      
            
            canvas->getTextBounds(message, 0, 0, &x1, &y1, &w, &h);
            canvas->setCursor(64 - w/2, 19);
            canvas->println(message);
        }
        else if (millis() - lastUpdateTime < 1000)
        {
            getParameterLongName(SelectedPage, lastUpdatedParam, buffer);
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

                getParameterDisplay(SelectedPage, lastUpdatedParam, buffer);
                canvas->getTextBounds(buffer, 0, 0, &x1, &y1, &w, &h);
                canvas->setCursor(64 - w/2, 34);
                canvas->println(buffer);
            }
        }
        
    }
};