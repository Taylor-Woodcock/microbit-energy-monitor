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

#include "EventModel.h"
#include "MicroBitConfig.h"
#include "MicroBitEnergyMonitor.h"
#include "MicroBitEnergyMonitorCalibrator.h"

MicroBitEnergyMonitorCalibrator::MicroBitEnergyMonitorCalibrator(MicroBitEnergyMonitor& _monitor, MicroBitDisplay& _display) : monitor(_monitor), display(_display)
{
    if (EventModel::defaultEventBus)
    {
        EventModel::defaultEventBus->listen(MICROBIT_ID_ENERGY_MONITOR, MICROBIT_ENERGY_MONITOR_EVT_CALIBRATE, this, &MicroBitEnergyMonitorCalibrator::calibrateUX);
    }
}

void MicroBitEnergyMonitorCalibrator::calibrateUX(MicroBitEvent)
{
    // listen on A + B click event to end the calibration sequence
    if (EventModel::defaultEventBus)
        EventModel::defaultEventBus->listen(MICROBIT_ID_BUTTON_AB, MICROBIT_BUTTON_EVT_CLICK, this, &MicroBitEnergyMonitorCalibrator::stopCalibration, MESSAGE_BUS_LISTENER_IMMEDIATE);
    
    const int TIME_STEP = 100;
    
    MicroBitImage smiley("0,255,0,255,0\n0,255,0,255,0\n0,0,0,0,0\n255,0,0,0,255\n0,255,255,255,0\n");
    
    int displayBrightness = display.getBrightness();
    
    int minAmplitude = 2147483647;
    int maxAmplitude = -2147483646;
    int amplitude = 0;
    int strength = 0;
    
    wait_ms(1000);
    
    // firstly, we need to take over the display. Ensure all active animations are paused.
    display.stopAnimation();
    display.setBrightness(255); // max brightness

    while(monitor.isCalibrating())
    {
        amplitude = monitor.getAmplitude();        
        minAmplitude = min(minAmplitude, amplitude);
        maxAmplitude = max(maxAmplitude, amplitude);
        strength = monitor.map(amplitude, minAmplitude, maxAmplitude, 0, 4); // update the strength to 0-4

        SERIAL_DEBUG->printf("Calib: min %d, max: %d, amp: %d, str:%d\n", minAmplitude, maxAmplitude, amplitude, strength);
        
        display.clear();
        
        for(int x = 0; x < 5; x++)
            display.image.setPixelValue(x, 4 - strength, 255); // draw line accross display at current strength
            
        wait_ms(TIME_STEP);
    }

    display.clear();
    display.printAsync(smiley, 0, 0, 0, 1500);
    wait_ms(1000);
    display.clear();

    // retore the display brightness to the level it was at before this function was called.
    display.setBrightness(displayBrightness);
    
    // remove A + B event callback
    if (EventModel::defaultEventBus)
        EventModel::defaultEventBus->ignore(MICROBIT_ID_BUTTON_AB, MICROBIT_BUTTON_EVT_CLICK, this, &MicroBitEnergyMonitorCalibrator::stopCalibration);
}

void MicroBitEnergyMonitorCalibrator::stopCalibration(MicroBitEvent)
{
    monitor.stopCalibration();
}