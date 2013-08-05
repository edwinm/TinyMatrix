///////////////////////////////////////////////////////////////////
// TinyMatrix.c                                                  //
// Copyright 2012 Tim Toner (tigeruppp/at/gmail.com)             //
// Modifications by Edwin Martin <edwin@bitstorm.org>            //
// Licensed under the GNU GPL v2 or newer                        //
///////////////////////////////////////////////////////////////////

#include <inttypes.h>
#include <stdlib.h>
#include <stdarg.h>
#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/sleep.h>
#include <util/delay.h>
#include <avr/pgmspace.h>


// 2D LED buffer
#define ROWS		5
#define COLS		7
char bitmap[ROWS][COLS];	
char need_refresh_line;
char need_render_frame;
unsigned char current_row;		// current lit row


// globals
int mode;						// current display mode
int t, frame, frame_delay;		// animation timing variables
int b1, last_b1, b2, last_b2;	// button states


// LED refresh interrupt, called 390 times per second
ISR(TIMER0_COMPA_vect)
{
	need_refresh_line = 1;	// flag to display new row of LEDs

	if (++current_row >= ROWS) 
		current_row = 0;

	if ( (t++%frame_delay) == 0 )
		need_render_frame = 1;

}

// attach a piezo to PORTD6 and GND.
void beep()
{
	int i;

	return;
	//	This routine is disabled.
	//	I'm using PORTD6 as a redundant switch input right now.
	//	It's at the bottom corner of the chip opposite GND.
	//	Perfect place to dead-bug a pushbutton.

	for(i=0; i<100; i++)
	{
		PORTD = _BV(0) | _BV(5) | _BV(6);		_delay_us(100);
		PORTD = 0;		_delay_us(100);
	}
	
}


/////////////////////////////////////////////////////////////////////
// ATTENTION:                                                      //
// The following 3 functions serve as the LED "driver":            //
// reset_led(), set_row(r), and set_column(c).
// Adjust these functions to match your LED module.                //
/////////////////////////////////////////////////////////////////////


// Two drivers are provided here.
// For Lite-on LTP-747 & LTP-757 families.

//#define LTP	747
#define LTP	757



#if LTP == 747

// Set port pins high and low in such order as to turn off the LED.
// This depends on the LED's cathode / anode arrangement etc.  
// At the same time, preserve pull-up resistors for switches.
void reset_led()
{
	// keep pull-up resistors active	
	PORTD = _BV(0) | _BV(5) | _BV(6);
		
	// hi/low the port pins to power off LED. (see datasheets)
	PORTA = _BV(1);
	PORTB = _BV(3) | _BV(5) | _BV(6);
	PORTD |= _BV(2) | _BV(3) | _BV(4);
}

// energize row r (call once)
void set_row(int r) 
{ 
	switch(r)
	{
		case 0:	PORTD |= _BV(1);	break;
		case 1:	PORTA |= _BV(0);	break;
		case 2:	PORTB |= _BV(4);	break;
		case 3:	PORTB |= _BV(1);	break;
		case 4:	PORTB |= _BV(2);	break;
	}
}

// energize col c (call once for each lit pixel in column)
void set_column(int c)
{
	switch(c)
	{
		case 6: PORTB &= ~_BV(6);		break;
		case 1: PORTD &= ~_BV(3);		break;
		case 2: PORTD &= ~_BV(2);		break;
		case 4: PORTA &= ~_BV(1);		break;
		case 3: PORTB &= ~_BV(3);		break;	
		case 5: PORTB &= ~_BV(5);		break;
		case 0: PORTD &= ~_BV(4);		break;
	}	
}

#endif

#if LTP == 757

// Set port pins high and low in such order as to turn off the LED.
// This depends on the LED's cathode / anode arrangement etc.  
// At the same time, preserve pull-up resistors for switches.
void reset_led()
{
	// keep pull-up resistors active	
	PORTD = _BV(0) | _BV(5) | _BV(6);
		
	// hi/low the port pins to power off LED. (see datasheets)
	PORTA = _BV(0);
	PORTB = _BV(1) | _BV(2) | _BV(4);
	PORTD |= _BV(1);
}

// energize row r (call once)
void set_row(int r) 
{ 
	switch(r)
	{
		case 0:	PORTD &= ~_BV(1);	break;
		case 1:	PORTA &= ~_BV(0);	break;
		case 2:	PORTB &= ~_BV(4);	break;
		case 3:	PORTB &= ~_BV(1);	break;
		case 4:	PORTB &= ~_BV(2);	break;
	}
}

// energize col c (call once for each lit pixel in column)
void set_column(int c)
{
	switch(c)
	{
		case 6: PORTB |= _BV(6);		break;
		case 1: PORTD |= _BV(3);		break;
		case 2: PORTD |= _BV(2);		break;
		case 4: PORTA |= _BV(1);		break;
		case 3: PORTB |= _BV(3);		break;	
		case 5: PORTB |= _BV(5);		break;
		case 0: PORTD |= _BV(4);		break;
	}	
}

#endif

/////////////////////////////////////////////////////////////////////
// end of LED specific hardware code                               //
/////////////////////////////////////////////////////////////////////



// render and energize the current row, based on bitmap array
void refresh_line()
{
	int c;

	reset_led();
	set_row(current_row);
	
	for (c=0; c<COLS; c++) 
		if (bitmap[current_row][c]) set_column(c);

	/* GARY - Tuesday Sept 26 11:44pm */

	need_refresh_line = 0;
}


// zero out the bitmap array
void clear_bitmap()
{
	int c,r;

	for (c=0; c<COLS; c++)
		for (r=0; r<ROWS; r++)
			bitmap[r][c] = 0 ;
}



/////////////////////////////////////////////////////////////////////
//                                   static 5x7 graphics / symbols //
/////////////////////////////////////////////////////////////////////
#define CHARS 6 // Number of graphics
unsigned const char charset[CHARS][5] PROGMEM = 
{
	// Enter the numbers from right to left!

	// Pair 1
	{ 16, 36, 32, 36, 16 }, { 16, 100, 32, 36, 16 },

	// Pair 2
	{ 127, 94, 62, 94, 127 }, { 127, 90, 62, 90, 127 },

	// Pair 3
	{ 0, 0, 127, 0, 0 }, { 8, 8, 8, 8, 8 }
};

// renders character c onto the bitmap
void render_frame(const unsigned char frame[])
{
	int x,y, byte;

	clear_bitmap();

	for (y=0; y<ROWS; y++)
	{
		byte = pgm_read_byte(&(frame[y]));

		for (x=0; x<COLS; x++)
		{
			if (byte & _BV(0)) bitmap[y][x] = 1;
			byte = byte >> 1;
		}		
	}	
}

/////////////////////////////////////////////////////////////////////
//                                                      animations //
/////////////////////////////////////////////////////////////////////

void render_rain()
{
	int y;

	frame_delay = 20;
	clear_bitmap();

	// this is a modulus based particle system

	y = frame%19;	
	if (y<COLS) bitmap[0][y] = 1;

	y = frame%11;	
	if (y<COLS) bitmap[2][y] = 1;

	y = frame%17;	
	if (y<COLS) bitmap[4][y] = 1;
}

void render_fire()
{
	int r;

	frame_delay = 40;
	clear_bitmap();

	// another modulus based particle system

	// fire body
	r = (frame+0)%3;	bitmap[0][6-r] = 1;
	r = (frame+1)%2;	bitmap[1][6-r] = 1;
	r = (frame+0)%2;	bitmap[2][6-r] = 1;
	r = (frame+1)%2;	bitmap[3][6-r] = 1;
	r = (frame+1)%3;	bitmap[4][6-r] = 1;

	r = (frame+1)%5;	bitmap[1][6-r] = 1;
	r = (frame+0)%3;	bitmap[2][6-r] = 1;
	r = (frame+2)%5;	bitmap[3][6-r] = 1;

	r = (frame+4)%4;	bitmap[0][6-r] = 1;
	r = (frame+1)%4;	bitmap[1][6-r] = 1;
	r = (frame+0)%4;	bitmap[2][6-r] = 1;
	r = (frame+3)%4;	bitmap[3][6-r] = 1;
	r = (frame+2)%4;	bitmap[4][6-r] = 1;

	// sparks
	r = (frame+0)%19;	if (r<COLS) bitmap[0][6-r] = 1;
	r = (frame+0)%6;	if (r<COLS) bitmap[1][6-r] = 1;
	r = (frame+0)%7;	if (r<COLS) bitmap[2][6-r] = 1;
	r = (frame+2)%6;	if (r<COLS) bitmap[3][6-r] = 1;
	r = (frame+0)%17;	if (r<COLS) bitmap[4][6-r] = 1;

}


void render_animation_frames(int n, ...)
{
	int i, f = 0;

	frame_delay = 300;

	va_list arguments;

	va_start(arguments, n);

	for ( i = 0; i <= frame % n; i++ )        
    {
        f = va_arg(arguments, int); 
    }
    render_frame(charset[f]);

    va_end ( arguments );

}

// renders the correct image / animation onto the bitmap
#define MODES 5
void render_buffer()
{
	frame++;
	need_render_frame = 0;
	
	// All animations
	switch(mode)
	{
		// Pair 1
		case 1:	render_animation_frames(2,  0, 1);	break;
		// Pair 2
		case 2:	render_animation_frames(2,  2, 3);	break;
		// Pair 3
		case 3:	render_animation_frames(2,  4, 5);	break;

		// Bonus rain and fire
		case 4:	render_rain();		break;
		case 5: render_fire();		break;
	}
}

// poll the pushbuttons, and record their states.
// increment/decrement 'mode' in response.
void check_inputs()
{
	// button 1 state (PORTD0 or PORTD6)
	if ((PIND & _BV(0)) == 0 || (PIND & _BV(6)) == 0) b1++;	else b1 = 0;

	// button 2 state (PORTD5)
	if ((PIND & _BV(5)) == 0) b2++;	else b2 = 0;

	// rudimentary de-bouncing
	if (b2 == 10) 		{ beep(); mode--; need_render_frame = 1; }
	if (b1 == 10) 		{ beep(); mode++; need_render_frame = 1; }

	// wraparound (optional)
	if (mode > MODES) mode = 1;
	if (mode < 1) mode = MODES;
}



////////////////////////////////////////////////////////////
//                                         initialization //
////////////////////////////////////////////////////////////
void init()
{
	// set output pins
	DDRA = _BV(0) | _BV(1);
	DDRB = _BV(1) | _BV(2) | _BV(3) | _BV(4) | _BV(5) | _BV(6);
	DDRD = _BV(1) | _BV(2) | _BV(3) | _BV(4);

	// setup Timer/Counter0 for LED refresh
	TCCR0A = _BV(WGM01);	// Set CTC mode
	TCCR0B = _BV(CS02);		// Set prescaler clk/256
	OCR0A = 40;				// ~ 390 interrupts/sec (by 5 cols = ~78fps)
	TIMSK = _BV(OCIE0A);	// Enable T/C 0A

	sei();

	mode = 1;	// Initial display pattern
}

//////////////////////////////////////////////////////////// 
//                                              main loop // 
//////////////////////////////////////////////////////////// 
void main_loop() 
{ 
	for (;;)
	{
		if (need_render_frame)
			render_buffer();		
		if (need_refresh_line)	
		{	
			refresh_line();
			check_inputs();
		}
	}	
}




////////////////////////////////////////////////////////////
//                                                  main  //
////////////////////////////////////////////////////////////
int main (void)
{
	init();
	main_loop();
	return (0);
}

