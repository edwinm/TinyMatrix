#TinyMatrix workshop

A TinyMatrix is a small LED matrix soldered on a AVR microcontroller that has been programmed with figures and animations.

I learned about TinyMatrix [on this site by TigerUp](https://sites.google.com/site/tinymatrix/).

Just getting started in AVR programming, I bought the hardware and [made TinyMatrices with my doughters (Dutch)](http://www.bitstorm.org/journaal/2013-5/TinyMatrix_maken_met_mijn_dochters.html) (then 7 and 10 years old).

In August 2013, I held a workshop with 10 kids at [OHM2013](https://ohm2013.org/site/) (the largest camping festival for hackers and makers in Europe). Here are the files I used so you can use them and improve them. Also take a look at the [TinyMatrix workshop webpage](https://ohm2013.org/wiki/Workshop:TinyMatrix).

## Setup

If you have never programmed a (AVR) microcontroller before, you need two things:

- A compiler. I used [AVRDUDE](http://www.nongnu.org/avrdude/), a free compiler for AVR microcontrollers.
- Hardware that can be used to burn the microcontroller. I used [Bus Pirate](http://dangerousprototypes.com/bus-pirate-manual/) you can buy it for US$30 at [SparkFun](https://www.sparkfun.com/products/9544).
- Something to connect the Bus Pirate to the AVR microcontroller. I soldered something together with the help of the [Bus Pirate AVR Programming documentation](http://dangerousprototypes.com/docs/Bus_Pirate_AVR_Programming) and [this image](http://dangerousprototypes.com/docs/images/1/1b/Bp-pin-cable-color.png). There are also programmers in which you can plug in an AVR microcontroller directly, but I have no experience with those.

## Compiling

After you have drawn some figures and calculated the numbers, you can enter them in TinyMatrix.c at around line 217. Remember to enter the numbers from right to left.

If AVRDUDE and your programming hardware is setup correctly, you just have to type

`make fuse`

`make flash`

## Hardware

This is the hardware I bought for the TinyMatrix:

* |AVR Microprocessor |1,42|[#719-4082 at RS](https://nl.rs-online.com/web/p/products/719-4082/)|
* |LED Matrix         |2,42|[#719-2588 at RS](https://nl.rs-online.com/web/p/products/719-2588/)|
* |Resistor 100k      |0,02|[#131-491 at RS](https://nl.rs-online.com/web/p/products/131-491/)|
* |Switch             |0,75|[#702-3763 at RS](https://nl.rs-online.com/web/p/products/702-3763/)|
* |Button             |0,41|[#378-6303 at RS](https://nl.rs-online.com/web/p/products/378-6303/)|
* |Battery            |0,15|[DealExtreme](http://dx.com/p/2032-x-20-pcs-cell-batteries-751)|
* |Battery holder     |0,40|[yallstock at eBay](http://www.ebay.com/itm/5-pcs-New-CR2032-Half-Round-Battery-Coin-Button-Cell-Socket-Holder-Case-Black-/320924808105)|



Copyright 2013 Edwin Martin <edwin@bitstorm.org>

License: MIT
