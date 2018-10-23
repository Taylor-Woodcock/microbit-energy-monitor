/*
The MIT License (MIT)

Copyright (c) 2016 British Broadcasting Corporation.
This software is provided by Lancaster University by arrangement with the BBC.

Permission is hereby granted, free of charge, to any person obtaining a
copy of this software and associated documentation files (the "Software"),
to deal in the Software without restriction, including without limitation
the rights to use, copy, modify, merge, publish, distribute, sublicense,
and/or sell copies of the Software, and to permit persons to whom the
Software is furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
DEALINGS IN THE SOFTWARE.
*/

#include "MicroBit.h"
#include "EnergyMonitor.h"

MicroBit uBit;
EnergyMonitor monitor(uBit.compass);

void onPowerChange(MicroBitEvent e)
{
    uBit.serial.send("Power state change: ");
    if (e.value == MICROBIT_ELECTRICAL_POWER_EVT_ON)
        uBit.serial.send("Off->On");
    
    if (e.value == MICROBIT_ELECTRICAL_POWER_EVT_OFF)
        uBit.serial.send("On->Off");
    
    uBit.serial.send("\n");
}

int main()
{
    // Initialise the micro:bit runtime.
    uBit.init();
    
    //uBit.messageBus.listen(MICROBIT_ID_ELECTRICAL_POWER, MICROBIT_ELECTRICAL_POWER_EVT_ON, onPowerChange);
    //uBit.messageBus.listen(MICROBIT_ID_ELECTRICAL_POWER, MICROBIT_ELECTRICAL_POWER_EVT_OFF, onPowerChange);
    
    uBit.display.scroll("u watt?");

    while(1)
    {
        uBit.serial.send("Usage: ");
        uBit.serial.send(monitor.getEnergyUsage());
        
        uBit.serial.send("\nAmplitude: ");
        uBit.serial.send(monitor.getAmplitude());
        
        uBit.serial.send("\nStatus: ");
        uBit.serial.send(monitor.isElectricalPowerOn() == 1 ? "On" : "Off");
        
        uBit.serial.send("\n");
        uBit.sleep(250);
    }
    
    // If main exits, there may still be other fibers running or registered event handlers etc.
    // Simply release this fiber, which will mean we enter the scheduler. Worse case, we then
    // sit in the idle task forever, in a power efficient sleep.
    release_fiber();
}