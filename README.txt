Things to be done:

1. Materials
2. Code
3. Spark Misc.


1. Materials
a. Small Box for MC
b. Batteries
c. Non-Conductive padding

2.
a. ADC in code. Set up for 3.3 volts, with an arbitrary value or greater being 1, and everything else zero
b. Timer set up to trigger an event every .01 second to check the adc, and every .25 seconds to update the display
c. SPI setup so data sent to the display every time the timer interupt is triggered. 
d. SPI interupt(the actual interupts code)
e. ADC interupt. This should check to see if the past 7 or so values have been equal and different and if they have, update a global variable
f. Power supply. Timer for setup for capture compare for the duty cycle on the transitor in the boost converter.


3. We need a poster
