================
NoomioJS releases
================

Released
========

0.1.0 (2019-11-24)
------------------

* Initial alpha release

* Peripherals: DigitialOut, DigitalIn, DigitalInOut, InterruptIn and I2C

* Timers: clearInterval, setTimeout, setInterval

* Stdio: print

* Communication: At

* Implementation of malloc, calloc, realloc and free

* Most peripheral functions exposed but not fully tested


0.2.0 (2019-11-30)
------------------

* Added module load availability. Usage with require().

* Minor fixes


0.3.0 (2020-01-09)
------------------

* Added finaliser for Digital I/O functions to call native code to de-initialise the peripheral in use when the object is garbage collected.

* Enabled stdin input via serial.

* Added interactive console via stdin to test script on the fly without updating app.js. A \\r, \\n or EOF shall signal the end of the file.


Planned
=======

0.4.0 (XXXX-XX-XX)
------------------

* Add socket implementation and expose the necessary functions

* Extend the I2C peripheral functions as only transfer is implemented

* Tidy up the At functions and only load it with require.
