# frankenstein-helmet

Once upon a time there was a production of Young Frankenstein in which there was a "brain transfer" machine.
This is the code that runs that prop.

This effect was built with an ESP8266 (Wemos D1 Mini), and two strips of WS2818B LEDs (300 pixels each) connected
to `D1` and `D2`. There is a "go" button on D5 which starts the effect. Adjust the values of `DTIM` and `BLKSIZ`
to change the speed with which the effect runs.

It's a royal pain to try feed 10ft of LED tape through 1/2" ID vinyl tubing, and the LEDs are prone to breaking off. Handle with care...

![Schematic](https://github.com/ckuethe/frankenstein-helnet/raw/main/schematic.svg)
