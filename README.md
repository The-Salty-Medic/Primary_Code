# Primary_Code
Arduino sketch without dependencies

Written By Aaron Hope, MPH, MsDa
hope.aaron.c@gmail.com
November 2023

Currently, the FCC requires that repeaters self-identify.  The least invasive way to perform that action has proven to be using morse code also known as CW.  I found that repeater contollers require me to solder wiring ont the board of a radio, which I find to be risky given the prices.

This project is meant to use a common UV5R radio tuned to the repeater frequencies and tones to perform the action.

Necessary hardware includes an arduino board, 5v relay, a K1 plug and wire, and the UV5R.

I also intendo to incorporate an IC2 compliant OLED to display the status of the system.

Thought went into loggint the activity of the repeater, but this would require an RTC component and a microSD card to export the log to.  This would increase the size of the board necessary, the cost, the coding, and likely not gain anything substantive for the effort.
