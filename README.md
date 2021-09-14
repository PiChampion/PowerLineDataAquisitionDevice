# PowerLineDataAquisitionDevice

Framatome Datalogger for ECE 485

Once the device is powered up, the File System on the SD Card will be initialized and any previous contents on the SD Card will be lost. After initial setup, the device will await a voltage signal higher than 0.200V(raw), 1.700V(from analog front end) on channel 1. Once that signal is detected, data collection on all 8 channels will occur. After the channel 1 signal goes below 0.200V(raw), 1.700V(from analog front end), data collection will cease for that cycle and the next cycle will be prepared. Data collection is designed to handle up to 9 cycles during each power cycle. Once 9 cycles have completed, it is recommended that the user cycles power to the device to reinitialize.

LED UI:

The first few seconds at power up are spent setting up the device. This will only happen once at start-up.

Once the setup is complete, the green light starts blinking. The device will now be waiting on the starting signal.

Once detected the device will start processing data immediately and yellow light will now be on.

Once the stopping signal is detected the yellow light turns off and the green light will be blinking, meaning the device is ready for the next cycle.

The device supports up to 9 cycles.

Pressing the LED button once will check for low battery. If the device is low on battery, the red LED will turn on, otherwise it won't.

To get ready to extract data, press the button twice to enable USB hosting. This mode has the green and yellow LEDs on simultaneously.

A Windows 10 computer can now recognize the device via USB.

The device can be put back into read/write mode by pressing the button twice again.

The yellow light will turn off and the green LED will then be blinking again.

ADC functionality:

There exists two modes of operation for the ADC, single-mode and cycle-mode. While in single mode channel 1, which corresponds to header J4 and pin 1 on the FRDMK-64F board, is being monitored in order to detect a signal higher than 0.200V(raw), 1.700V(from analog front end). Once this signal is detected we switch to cycle-mode. The driving force behind these modes is the Periodic Interrupt Timer (PIT), which is set to run at 96 kHz. Once in cycle-mode, each of the 8 channels coming in are cycled through at 96kHz, each channel being sampled at 12 kHz. While all channels are being sampled, channel 1 is still being monitored, but this time for the signal to go below 0.200V(raw), 1.700V(from analog front end), marking the end of a cycle and switching back to single-mode.
