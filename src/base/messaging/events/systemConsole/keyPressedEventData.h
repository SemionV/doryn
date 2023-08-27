#pragma once

namespace dory
{
    struct KeyPressedEventData
    {
        public:
            const char keyPressed;

        public:
            KeyPressedEventData(char keyPressed):
                keyPressed(keyPressed)
            {                
            }
    };
}