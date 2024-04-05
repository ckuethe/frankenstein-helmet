# frankenstein-helmet

Once upon a time there was a production of Young Frankenstein in which there was a "brain transfer" machine.
This is the code that runs that prop.

The brain transfer effect runs in approximately 12 seconds. To use it:
1. Apply 5V to all of the USB connectors. This device needs a lot of power to illuminate the LEDs
2. Observe the blinking light on the control board. This indicates that the controller is ready.
3. Toggle the control switch a couple of times. This triggers the effect.
4. Enjoy the blinkylights.

The controller must be reset by removing all power or by pressing the small reset button in order to run the effect again.

This effect was built with an ESP8266 (Wemos D1 Mini), and two strips of WS2818B LEDs (300 pixels each) connected
to `D1` and `D2`. There is a "go" button on D5 which starts the effect. Adjust the values of `DTIM` and `BLKSIZ`
to change the speed with which the effect runs. Modifying this is left as an exercise to the reader.

It's a royal pain to try feed 10ft of LED tape through 1/2" ID vinyl tubing, and the LEDs are prone to breaking off. Handle with care...

![Schematic](https://github.com/ckuethe/frankenstein-helnet/raw/main/schematic.svg)
