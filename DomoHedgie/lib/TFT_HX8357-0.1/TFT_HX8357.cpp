/***************************************************
  Arduino TFT graphics library targetted at the UNO
  and Mega boards.

  This library has been derived from the Adafruit_GFX
  library and the associated driver library. See text
  at the end of this file.

  This is a standalone library that contains the
  hardware driver, the graphics funtions and the
  proportional fonts.

  The larger fonts are Run Length Encoded to reduce their
  size.

 ****************************************************/

#include "TFT_HX8357.h"

#include <avr/pgmspace.h>

/***************************************************************************************
** Function name:           TFT_HX8357
** Description:             Constructor , we must use hardware SPI pins
***************************************************************************************/
TFT_HX8357::TFT_HX8357(int16_t w, int16_t h)
{
  _cs   = 40; //PORT G bit _BV(1)

#ifdef FAST_RS
  _rs   =  4; //PORT G bit _BV(5)
#else
  _rs   = 38; //PORT D bit _BV(7)
#endif

  _rst  = 41; //PORT G bit _BV(0)
  _wr   = 39; //PORT G bit _BV(2)
  _fcs  = 44; //FLASH chip select?

  pinMode(_rst, OUTPUT);
  digitalWrite(_rst, HIGH);

  pinMode(_rs, OUTPUT);
  pinMode(_cs, OUTPUT);
  pinMode(_wr, OUTPUT);
  digitalWrite(_rs, HIGH);

#ifndef KEEP_CS_LOW
  digitalWrite(_cs, HIGH);
#else
  digitalWrite(_cs, LOW);
#endif

  digitalWrite(_wr, HIGH);

  pinMode(_fcs, OUTPUT);
  digitalWrite(_fcs, HIGH); // Stop line floating

  DDRA = 0xFF; // Set direction for the 2 8 bit data ports
  DDRC = 0xFF;

  _width    = w;
  _height   = h;
  rotation  = 0;
  cursor_y  = cursor_x    = 0;
  textfont  = 1;
  textsize  = 1;
  textcolor   = 0xFFFF;
  textbgcolor = 0x0000;
  padX = 0;
  textwrap  = true;
  textdatum = TL_DATUM; // Top left text datum is default
  fontsloaded = 0;

#ifdef LOAD_GLCD
  fontsloaded = 0x0002; // Bit 1 set
#endif

#ifdef LOAD_FONT2
  fontsloaded |= 0x0004; // Bit 2 set
#endif

#ifdef LOAD_FONT4
  fontsloaded |= 0x0010; // Bit 4 set
#endif

#ifdef LOAD_FONT6
  fontsloaded |= 0x0040; // Bit 6 set
#endif

#ifdef LOAD_FONT7
  fontsloaded |= 0x0080; // Bit 7 set
#endif

#ifdef LOAD_FONT8
  fontsloaded |= 0x0100; // Bit 8 set
#endif

}

/***************************************************************************************
** Function name:           writecommand
** Description:             Send an 8 bit command to the TFT
***************************************************************************************/
void TFT_HX8357::writecommand(uint8_t c)
{
SETUP_CS_L;
RS_L;
PORTA = 0;
PORTC = c;
WR_STB;
RS_H;
SETUP_CS_H;
}

/***************************************************************************************
** Function name:           writedata
** Description:             Send a 8 bit data value to the TFT
***************************************************************************************/
void TFT_HX8357::writedata(uint8_t c)
{
SETUP_CS_L;
PORTA = c>>8;
PORTC = c;
WR_STB;
SETUP_CS_H;
}

/***************************************************************************************
** Function name:           begin
** Description:             Included for backwards compatibility
***************************************************************************************/
void TFT_HX8357::begin(void)
{
 init();
}

/***************************************************************************************
** Function name:           init
** Description:             Reset, then initialise the TFT display registers
***************************************************************************************/
void TFT_HX8357::init(void)
{
  // toggle RST low to reset
    digitalWrite(_rst, HIGH);
    delay(10);
    digitalWrite(_rst, LOW);
    delay(10);
    digitalWrite(_rst, HIGH);
    delay(10);

#ifndef HX8357C

// Configure HX8357-B display
    writecommand(0x11);
    delay(20);
    writecommand(0xD0);
    writedata(0x07);
    writedata(0x42);
    writedata(0x18);

    writecommand(0xD1);
    writedata(0x00);
    writedata(0x07);
    writedata(0x10);

    writecommand(0xD2);
    writedata(0x01);
    writedata(0x02);

    writecommand(0xC0);
    writedata(0x10);
    writedata(0x3B);
    writedata(0x00);
    writedata(0x02);
    writedata(0x11);

    writecommand(0xC5);
    writedata(0x08);

    writecommand(0xC8);
    writedata(0x00);
    writedata(0x32);
    writedata(0x36);
    writedata(0x45);
    writedata(0x06);
    writedata(0x16);
    writedata(0x37);
    writedata(0x75);
    writedata(0x77);
    writedata(0x54);
    writedata(0x0C);
    writedata(0x00);

    writecommand(0x36);
    writedata(0x0a);

    writecommand(0x3A);
    writedata(0x55);

    writecommand(0x2A);
    writedata(0x00);
    writedata(0x00);
    writedata(0x01);
    writedata(0x3F);

    writecommand(0x2B);
    writedata(0x00);
    writedata(0x00);
    writedata(0x01);
    writedata(0xDF);

    delay(120);
    writecommand(0x29);

    delay(25);
// End of HX8357-B display configuration

#else

// HX8357-C display initialisation

	writecommand(0xB9); // Enable extension command
	writedata(0xFF);
	writedata(0x83);
	writedata(0x57);
	delay(50);
	
	writecommand(0xB6); //Set VCOM voltage
	writedata(0x2C);    //0x52 for HSD 3.0"
	
	writecommand(0x11); // Sleep off
	delay(200);
	
	writecommand(0x35); // Tearing effect on
	writedata(0x00);    // Added parameter

	writecommand(0x3A); // Interface pixel format
	writedata(0x55);    // 16 bits per pixel

	//writecommand(0xCC); // Set panel characteristic
	//writedata(0x09);    // S960>S1, G1>G480, R-G-B, normally black

	//writecommand(0xB3); // RGB interface
	//writedata(0x43);
	//writedata(0x00);
	//writedata(0x06);
	//writedata(0x06);

	writecommand(0xB1); // Power control
	writedata(0x00);
	writedata(0x15);
	writedata(0x0D);
	writedata(0x0D);
	writedata(0x83);
	writedata(0x48);
	
	
	writecommand(0xC0); // Does this do anything?
	writedata(0x24);
	writedata(0x24);
	writedata(0x01);
	writedata(0x3C);
	writedata(0xC8);
	writedata(0x08);
	
	writecommand(0xB4); // Display cycle
	writedata(0x02);
	writedata(0x40);
	writedata(0x00);
	writedata(0x2A);
	writedata(0x2A);
	writedata(0x0D);
	writedata(0x4F);
	
	writecommand(0xE0); // Gamma curve
	writedata(0x00);
	writedata(0x15);
	writedata(0x1D);
	writedata(0x2A);
	writedata(0x31);
	writedata(0x42);
	writedata(0x4C);
	writedata(0x53);
	writedata(0x45);
	writedata(0x40);
	writedata(0x3B);
	writedata(0x32);
	writedata(0x2E);
	writedata(0x28);
	
	writedata(0x24);
	writedata(0x03);
	writedata(0x00);
	writedata(0x15);
	writedata(0x1D);
	writedata(0x2A);
	writedata(0x31);
	writedata(0x42);
	writedata(0x4C);
	writedata(0x53);
	writedata(0x45);
	writedata(0x40);
	writedata(0x3B);
	writedata(0x32);
	
	writedata(0x2E);
	writedata(0x28);
	writedata(0x24);
	writedata(0x03);
	writedata(0x00);
	writedata(0x01);

	writecommand(0x36); // MADCTL Memory access control
	writedata(0x48);
	delay(20);

	writecommand(0x21); //Display inversion on
	delay(20);

	writecommand(0x29); // Display on
	
	delay(120);
#endif

#ifdef KEEP_CS_LOW
  SETUP_CS_L;
#endif
}

/***************************************************************************************
** Function name:           drawCircle
** Description:             Draw a circle outline
***************************************************************************************/
void TFT_HX8357::drawCircle(int16_t x0, int16_t y0, int16_t r, uint16_t color)
{
  int16_t f = 1 - r;
  int16_t ddF_x = 1;
  int16_t ddF_y = - r - r;
  int16_t x = 0;

  drawPixel(x0  , y0 + r, color);
  drawPixel(x0  , y0 - r, color);
  drawPixel(x0 + r, y0  , color);
  drawPixel(x0 - r, y0  , color);

  while (x < r) {
    if (f >= 0) {
      r--;
      ddF_y += 2;
      f += ddF_y;
    }
    x++;
    ddF_x += 2;
    f += ddF_x;

    drawPixel(x0 + x, y0 + r, color);
    drawPixel(x0 - x, y0 + r, color);
    drawPixel(x0 + x, y0 - r, color);
    drawPixel(x0 - x, y0 - r, color);
    drawPixel(x0 + r, y0 + x, color);
    drawPixel(x0 - r, y0 + x, color);
    drawPixel(x0 + r, y0 - x, color);
    drawPixel(x0 - r, y0 - x, color);
  }
}

/***************************************************************************************
** Function name:           drawCircleHelper
** Description:             Support function for circle drawing
***************************************************************************************/
void TFT_HX8357::drawCircleHelper( int16_t x0, int16_t y0, int16_t r, uint8_t cornername, uint16_t color)
{
  int16_t f     = 1 - r;
  int16_t ddF_x = 1;
  int16_t ddF_y = -r -r;
  int16_t x     = 0;

  while (x < r) {
    if (f >= 0) {
      r--;
      ddF_y += 2;
      f     += ddF_y;
    }
    x++;
    ddF_x += 2;
    f     += ddF_x;
    if (cornername & 0x4) {
      drawPixel(x0 + x, y0 + r, color);
      drawPixel(x0 + r, y0 + x, color);
    }
    if (cornername & 0x2) {
      drawPixel(x0 + x, y0 - r, color);
      drawPixel(x0 + r, y0 - x, color);
    }
    if (cornername & 0x8) {
      drawPixel(x0 - r, y0 + x, color);
      drawPixel(x0 - x, y0 + r, color);
    }
    if (cornername & 0x1) {
      drawPixel(x0 - r, y0 - x, color);
      drawPixel(x0 - x, y0 - r, color);
    }
  }
}

/***************************************************************************************
** Function name:           fillCircle
** Description:             draw a filled circle
***************************************************************************************/
void TFT_HX8357::fillCircle(int16_t x0, int16_t y0, int16_t r, uint16_t color)
{
  drawFastVLine(x0, y0 - r, r + r + 1, color);
  fillCircleHelper(x0, y0, r, 3, 0, color);
}

/***************************************************************************************
** Function name:           fillCircleHelper
** Description:             Support function for filled circle drawing
***************************************************************************************/
// Used to do circles and roundrects
void TFT_HX8357::fillCircleHelper(int16_t x0, int16_t y0, int16_t r, uint8_t cornername, int16_t delta, uint16_t color)
{
  int16_t f     = 1 - r;
  int16_t ddF_x = 1;
  int16_t ddF_y = -r - r;
  int16_t x     = 0;

  delta++;
  while (x < r) {
    if (f >= 0) {
      r--;
      ddF_y += 2;
      f     += ddF_y;
    }
    x++;
    ddF_x += 2;
    f     += ddF_x;

    if (cornername & 0x1) {
      drawFastVLine(x0 + x, y0 - r, r + r + delta, color);
      drawFastVLine(x0 + r, y0 - x, x + x + delta, color);
    }
    if (cornername & 0x2) {
      drawFastVLine(x0 - x, y0 - r, r + r + delta, color);
      drawFastVLine(x0 - r, y0 - x, x + x + delta, color);
    }
  }
}

/***************************************************************************************
** Function name:           fillScreen
** Description:             Clear the screen to defined colour
***************************************************************************************/
void TFT_HX8357::fillScreen(uint16_t color)
{
  fillRect(0, 0, _width, _height, color);
}

/***************************************************************************************
** Function name:           drawRect
** Description:             Draw a rectangle outline
***************************************************************************************/
// Draw a rectangle
void TFT_HX8357::drawRect(int16_t x, int16_t y, int16_t w, int16_t h, uint16_t color)
{
  drawFastHLine(x, y, w, color);
  drawFastHLine(x, y + h - 1, w, color);
  drawFastVLine(x, y, h, color);
  drawFastVLine(x + w - 1, y, h, color);
}

/***************************************************************************************
** Function name:           drawRoundRect
** Description:             Draw a rounded corner rectangle outline
***************************************************************************************/
// Draw a rounded rectangle
void TFT_HX8357::drawRoundRect(int16_t x, int16_t y, int16_t w, int16_t h, int16_t r, uint16_t color)
{
  // smarter version
  drawFastHLine(x + r  , y    , w - r - r, color); // Top
  drawFastHLine(x + r  , y + h - 1, w - r - r, color); // Bottom
  drawFastVLine(x    , y + r  , h - r - r, color); // Left
  drawFastVLine(x + w - 1, y + r  , h - r - r, color); // Right
  // draw four corners
  drawCircleHelper(x + r    , y + r    , r, 1, color);
  drawCircleHelper(x + w - r - 1, y + r    , r, 2, color);
  drawCircleHelper(x + w - r - 1, y + h - r - 1, r, 4, color);
  drawCircleHelper(x + r    , y + h - r - 1, r, 8, color);
}

/***************************************************************************************
** Function name:           fillRoundRect
** Description:             Draw a rounded corner filled rectangle
***************************************************************************************/
// Fill a rounded rectangle
void TFT_HX8357::fillRoundRect(int16_t x, int16_t y, int16_t w, int16_t h, int16_t r, uint16_t color)
{
  // smarter version
  fillRect(x + r, y, w - r - r, h, color);

  // draw four corners
  fillCircleHelper(x + w - r - 1, y + r, r, 1, h - r - r - 1, color);
  fillCircleHelper(x + r    , y + r, r, 2, h - r - r - 1, color);
}

/***************************************************************************************
** Function name:           drawTriangle
** Description:             Draw a triangle outline using 3 arbitrary points
***************************************************************************************/
// Draw a triangle
void TFT_HX8357::drawTriangle(int16_t x0, int16_t y0, int16_t x1, int16_t y1, int16_t x2, int16_t y2, uint16_t color)
{
  drawLine(x0, y0, x1, y1, color);
  drawLine(x1, y1, x2, y2, color);
  drawLine(x2, y2, x0, y0, color);
}

/***************************************************************************************
** Function name:           fillTriangle 
** Description:             Draw a filled triangle using 3 arbitrary points
***************************************************************************************/
// Fill a triangle - uses faster Bressenham method (original Adafruit function overflowed on big triangles!)
void TFT_HX8357::fillTriangle (int16_t x1,int16_t y1,int16_t x2,int16_t y2,int16_t x3,int16_t y3, uint16_t c)
{
	int16_t t1x,t2x,y,minx,maxx,t1xp,t2xp;
	bool changed1 = false;
	bool changed2 = false;
	int16_t signx1,signx2,dx1,dy1,dx2,dy2;
	uint16_t e1,e2;
     // Sort vertices
	if (y1>y2) { swap(y1,y2); swap(x1,x2); }
	if (y1>y3) { swap(y1,y3); swap(x1,x3); }
	if (y2>y3) { swap(y2,y3); swap(x2,x3); }

	t1x=t2x=x1; y=y1;   // Starting points

	dx1 = x2 - x1; if(dx1<0) { dx1=-dx1; signx1=-1; } else signx1=1;
	dy1 = y2 - y1;
 
	dx2 = x3 - x1; if(dx2<0) { dx2=-dx2; signx2=-1; } else signx2=1;
	dy2 = y3 - y1;
	
	if (dy1 > dx1) {   // swap values
        swap(dx1,dy1);
		changed1 = true;
	}
	if (dy2 > dx2) {   // swap values
        swap(dy2,dx2);
		changed2 = true;
	}
	
	e2 = dx2>>1;
    // Flat top, just process the second half
    if(y1==y2) goto next;
    e1 = dx1>>1;
	
	for (uint16_t i = 0; i < dx1;) {
		t1xp=0; t2xp=0;
		if(t1x<t2x) { minx=t1x; maxx=t2x; }
		else		{ minx=t2x; maxx=t1x; }
        // process first line until y value is about to change
		while(i<dx1) {
			i++;			
			e1 += dy1;
	   	   	while (e1 >= dx1) {
				e1 -= dx1;
   	   	   	   if (changed1) t1xp=signx1;//t1x += signx1;
				else          goto next1;
			}
			if (changed1) break;
			else t1x += signx1;
		}
	// Move line
	next1:
        // process second line until y value is about to change
		while (1) {
			e2 += dy2;		
			while (e2 >= dx2) {
				e2 -= dx2;
				if (changed2) t2xp=signx2;//t2x += signx2;
				else          goto next2;
			}
			if (changed2)     break;
			else              t2x += signx2;
		}
	next2:
		if(minx>t1x) minx=t1x; if(minx>t2x) minx=t2x;
		if(maxx<t1x) maxx=t1x; if(maxx<t2x) maxx=t2x;
	   	drawFastHLine(minx, y, maxx-minx, c);    // Draw line from min to max points found on the y
		// Now increase y
		if(!changed1) t1x += signx1;
		t1x+=t1xp;
		if(!changed2) t2x += signx2;
		t2x+=t2xp;
    	     y += 1;
		if(y==y2) break;
		
   }
	next:
	// Second half
	dx1 = x3 - x2; if(dx1<0) { dx1=-dx1; signx1=-1; } else signx1=1;
	dy1 = y3 - y2;
	t1x=x2;
 
	if (dy1 > dx1) {   // swap values
        swap(dy1,dx1);
		changed1 = true;
	} else changed1=false;
	
	e1 = dx1>>1;
	
	for (uint16_t i = 0; i<=dx1; i++) {
		t1xp=0; t2xp=0;
		if(t1x<t2x) { minx=t1x; maxx=t2x; }
		else		{ minx=t2x; maxx=t1x; }
	    // process first line until y value is about to change
		while(i<dx1) {
    		e1 += dy1;
	   	   	while (e1 >= dx1) {
				e1 -= dx1;
   	   	   	   	if (changed1) { t1xp=signx1; break; }//t1x += signx1;
				else          goto next3;
			}
			if (changed1) break;
			else   	   	  t1x += signx1;
			if(i<dx1) i++;
		}
	next3:
        // process second line until y value is about to change
		while (t2x!=x3) {
			e2 += dy2;
	   	   	while (e2 >= dx2) {
				e2 -= dx2;
				if(changed2) t2xp=signx2;
				else          goto next4;
			}
			if (changed2)     break;
			else              t2x += signx2;
		}	   	   
	next4:

		if(minx>t1x) minx=t1x; if(minx>t2x) minx=t2x;
		if(maxx<t1x) maxx=t1x; if(maxx<t2x) maxx=t2x;
	   	drawFastHLine(minx, y, maxx-minx, c);    // Draw line from min to max points found on the y
		// Now increase y
		if(!changed1) t1x += signx1;
		t1x+=t1xp;
		if(!changed2) t2x += signx2;
		t2x+=t2xp;
       	y += 1;
		if(y>y3) return;
	}
}

/***************************************************************************************
** Function name:           drawBitmap
** Description:             Draw an image stored in an array on the TFT
***************************************************************************************/
void TFT_HX8357::drawBitmap(int16_t x, int16_t y, const uint8_t *bitmap, int16_t w, int16_t h, uint16_t color) {

  int16_t i, j, byteWidth = (w + 7) / 8;

  for (j = 0; j < h; j++) {
    for (i = 0; i < w; i++ ) {
      if (pgm_read_byte(bitmap + j * byteWidth + i / 8) & (128 >> (i & 7))) {
        drawPixel(x + i, y + j, color);
      }
    }
  }
}

/***************************************************************************************
** Function name:           setCursor
** Description:             Set the text cursor x,y position
***************************************************************************************/
void TFT_HX8357::setCursor(int16_t x, int16_t y)
{
  cursor_x = x;
  cursor_y = y;
}

/***************************************************************************************
** Function name:           setCursor
** Description:             Set the text cursor x,y position and font
***************************************************************************************/
void TFT_HX8357::setCursor(int16_t x, int16_t y, uint8_t font)
{
  textfont = font;
  cursor_x = x;
  cursor_y = y;
}

/***************************************************************************************
** Function name:           setTextSize
** Description:             Set the text size multiplier
***************************************************************************************/
void TFT_HX8357::setTextSize(uint8_t s)
{
  if (s>7) s = 7; // Limit the maximum size multiplier so byte variables can be used for rendering
  textsize = (s > 0) ? s : 1; // Don't allow font size 0
}

/***************************************************************************************
** Function name:           setTextFont
** Description:             Set the font for the print stream
***************************************************************************************/
void TFT_HX8357::setTextFont(uint8_t f)
{
  textfont = (f > 0) ? f : 1; // Don't allow font 0
}

/***************************************************************************************
** Function name:           setTextColor
** Description:             Set the font foreground colour (background is transparent)
***************************************************************************************/
void TFT_HX8357::setTextColor(uint16_t c)
{
  // For 'transparent' background, we'll set the bg
  // to the same as fg instead of using a flag
  textcolor = textbgcolor = c;
}

/***************************************************************************************
** Function name:           setTextColor
** Description:             Set the font foreground and background colour
***************************************************************************************/
void TFT_HX8357::setTextColor(uint16_t c, uint16_t b)
{
  textcolor   = c;
  textbgcolor = b;
}

/***************************************************************************************
** Function name:           setTextWrap
** Description:             Define if text should wrap at end of line
***************************************************************************************/
void TFT_HX8357::setTextWrap(boolean w)
{
  textwrap = w;
}

/***************************************************************************************
** Function name:           setTextDatum
** Description:             Set the text position reference datum
***************************************************************************************/
void TFT_HX8357::setTextDatum(uint8_t d)
{
  textdatum = d;
}

/***************************************************************************************
** Function name:           setTextPadding
** Description:             Define padding width (aids erasing old text and numbers)
***************************************************************************************/
void TFT_HX8357::setTextPadding(uint16_t x_width)
{
  padX = x_width;
}

/***************************************************************************************
** Function name:           getRotation
** Description:             Return the rotation value (as used by setRotation())
***************************************************************************************/
uint8_t TFT_HX8357::getRotation(void)
{
  return rotation;
}

/***************************************************************************************
** Function name:           width
** Description:             Return the pixel width of display (per current rotation)
***************************************************************************************/
// Return the size of the display (per current rotation)
int16_t TFT_HX8357::width(void)
{
  return _width;
}

/***************************************************************************************
** Function name:           height
** Description:             Return the pixel height of display (per current rotation)
***************************************************************************************/
int16_t TFT_HX8357::height(void)
{
  return _height;
}

/***************************************************************************************
** Function name:           textWidth
** Description:             Return the width in pixels of a string in a given font
***************************************************************************************/
int16_t TFT_HX8357::textWidth(char *string, int font)
{
  unsigned int str_width  = 0;
  char uniCode;
  char *widthtable;

  if (font>1 && font<9)
  widthtable = (char *)pgm_read_word( &(fontdata[font].widthtbl ) ) - 32; //subtract the 32 outside the loop

  while (*string)
  {
    uniCode = *(string++);
#ifdef LOAD_GLCD
    if (font == 1) str_width += 6;
    else
#endif
    str_width += pgm_read_byte( widthtable + uniCode); // Normally we need to subract 32 from uniCode
  }
  return str_width * textsize;
}

/***************************************************************************************
** Function name:           fontsLoaded
** Description:             return an encoded 16 bit value showing the fonts loaded
***************************************************************************************/
// Returns a value showing which fonts are loaded (bit N set =  Font N loaded)

uint16_t TFT_HX8357::fontsLoaded(void)
{
  return fontsloaded;
}


/***************************************************************************************
** Function name:           fontHeight
** Description:             return the height of a font
***************************************************************************************/
int16_t TFT_HX8357::fontHeight(int font)
{
  return pgm_read_byte( &fontdata[font].height ) * textsize;
}

/***************************************************************************************
** Function name:           drawChar
** Description:             draw a single character in the Adafruit GLCD font
***************************************************************************************/
void TFT_HX8357::drawChar(int16_t x, int16_t y, unsigned char c, uint16_t color, uint16_t bg, uint8_t size)
{
#ifdef LOAD_GLCD
  if ((x >= _width)            || // Clip right
      (y >= _height)           || // Clip bottom
      ((x + 6 * size - 1) < 0) || // Clip left
      ((y + 8 * size - 1) < 0))   // Clip top
    return;
  boolean fillbg = (bg != color);

  if ((size==1) && fillbg)
  {
    setAddrWindow(x, y, x+5, y+8);

    byte column[5];
    char *flash_address = (char *)font + c * 5;
    column[0] = pgm_read_byte(flash_address++);
    column[1] = pgm_read_byte(flash_address++);
    column[2] = pgm_read_byte(flash_address++);
    column[3] = pgm_read_byte(flash_address++);
    column[4] = pgm_read_byte(flash_address++);

    byte mask = 0x1;

    for (int8_t j = 0; j < 8; j++) {
      if (column[0] & mask) {PORTC = color; PORTA = color>>8;}
      else {PORTC = bg; PORTA = bg>>8;} WR_STB;
      if (column[1] & mask) {PORTC = color; PORTA = color>>8;}
      else {PORTC = bg; PORTA = bg>>8;} WR_STB;
      if (column[2] & mask) {PORTC = color; PORTA = color>>8;}
      else {PORTC = bg; PORTA = bg>>8;} WR_STB;
      if (column[3] & mask) {PORTC = color; PORTA = color>>8;}
      else {PORTC = bg; PORTA = bg>>8;} WR_STB;
      if (column[4] & mask) {PORTC = color; PORTA = color>>8;}
      else {PORTC = bg; PORTA = bg>>8;} WR_STB;
      mask <<= 1;
      PORTC = bg; PORTA = bg>>8; WR_STB;
    }
    CS_H;
  }
  else
  {
    char *flash_address = (char *)font + c * 5; 
    for (int8_t i = 0; i < 6; i++ ) {
      uint8_t line;
      if (i == 5)
        line = 0x0;
      else
        line = pgm_read_byte(flash_address + i);

      if (size == 1) // default size
      {
        for (int8_t j = 0; j < 8; j++) {
          if (line & 0x1) drawPixel(x + i, y + j, color);
          line >>= 1;
        }
      }
      else {  // big size
        for (int8_t j = 0; j < 8; j++) {
          if (line & 0x1)  fillRect(x + i * size, y + j * size, size, size, color);
          else if (fillbg) fillRect(x + i * size, y + j * size, size, size, bg);
          line >>= 1;
        }
      }
    }
  }

#endif // LOAD_GLCD
}

/***************************************************************************************
** Function name:           setWindow
** Description:             define an area to receive a stream of pixels
***************************************************************************************/
// Chip select is high at the end of this function

void TFT_HX8357::setWindow(int16_t x0, int16_t y0, int16_t x1, int16_t y1)
{
  setAddrWindow(x0, y0, x1, y1);
  CS_H;
}

/***************************************************************************************
** Function name:           setAddrWindow
** Description:             define an area to rexeive a stream of pixels
***************************************************************************************/
// Chip select stays low, use setWindow() from sketches
#ifdef FAST_RS

void TFT_HX8357::setAddrWindow(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1)
{
  if (x0>x1) swap(x0,x1);
  if (y0>y1) swap(y0,y1);
  //if((x1 >= _width) || (y1 >= _height)) return;
  CSL_RSL_WRL;  PORTC = HX8357_CASET; WR_H; CSL_RSH_WRL;
  PORTC = x0>>8; WR_H;
  PORTC = x0; WR_STB;
  PORTC = x1>>8; WR_STB;
  PORTC = x1; WR_STB;
  CSL_RSL_WRL; PORTC = HX8357_PASET; WR_H; CSL_RSH_WRL;
  PORTC = y0>>8; WR_H;
  PORTC = y0; WR_STB;
  PORTC = y1>>8; WR_STB;
  PORTC = y1; WR_STB;
  CSL_RSL_WRL; PORTC = HX8357_RAMWR; WR_H; CSL_RSH_WRH;
}

#else

void TFT_HX8357::setAddrWindow(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1)
{
  if (x0>x1) swap(x0,x1);
  if (y0>y1) swap(y0,y1);
  //if((x1 >= _width) || (y1 >= _height)) return;
  CS_L;
  RS_L;  PORTC = HX8357_CASET; WR_STB; RS_H;
  PORTC = x0>>8; WR_STB;
  PORTC = x0; WR_STB;
  PORTC = x1>>8; WR_STB;
  PORTC = x1; WR_STB;
  RS_L; PORTC = HX8357_PASET; WR_STB; RS_H;
  PORTC = y0>>8; WR_STB;
  PORTC = y0; WR_STB;
  PORTC = y1>>8; WR_STB;
  PORTC = y1; WR_STB;
  RS_L; PORTC = HX8357_RAMWR; WR_STB; RS_H;
}
#endif

/***************************************************************************************
** Function name:           drawPixel
** Description:             push a single pixel at an arbitrary position
***************************************************************************************/
#ifdef FAST_RS

void TFT_HX8357::drawPixel(uint16_t x, uint16_t y, uint16_t color)
{
  // Faster range checking, possible because x and y are unsigned
  if ((x >= _width) || (y >= _height)) return;

  CSL_RSL_WRL;  PORTC = HX8357_CASET; WR_H; CSL_RSH_WRL;
  PORTC = x>>8; WR_H;
  PORTC = x; WR_STB;
  PORTC = x>>8; WR_STB;
  PORTC = x; WR_STB;
  CSL_RSL_WRL; PORTC = HX8357_PASET; WR_H; CSL_RSH_WRL;
  PORTC = y>>8; WR_H;
  PORTC = y; WR_STB;
  PORTC = y>>8; WR_STB;
  PORTC = y; WR_STB;
  CSL_RSL_WRL; PORTC = HX8357_RAMWR; WR_H; CSL_RSH_WRL;
  PORTC = color; PORTA = color>>8; WR_H; CS_H;
}

#else

void TFT_HX8357::drawPixel(uint16_t x, uint16_t y, uint16_t color)
{
  // Faster range checking, possible because x and y are unsigned
  if ((x >= _width) || (y >= _height)) return;

  CS_L;
  RS_L;  PORTC = HX8357_CASET; WR_STB; RS_H;
  PORTC = x>>8; WR_STB;
  PORTC = x; WR_STB;
  PORTC = x>>8; WR_STB;
  PORTC = x; WR_STB;
  RS_L; PORTC = HX8357_PASET; WR_STB; RS_H;
  PORTC = y>>8; WR_STB;
  PORTC = y; WR_STB;
  PORTC = y>>8; WR_STB;
  PORTC = y; WR_STB;
  RS_L; PORTC = HX8357_RAMWR; WR_STB; RS_H;
  PORTC = color; PORTA = color>>8; WR_STB;
  CS_H;
}
#endif
/***************************************************************************************
** Function name:           pushColor
** Description:             push a single pixel
***************************************************************************************/
void TFT_HX8357::pushColor(uint16_t color)
{
  CS_L;
  PORTA    = color;
  PORTC    = color >> 8;
  WR_STB;
  CS_H;
}

/***************************************************************************************
** Function name:           pushColor
** Description:             push a single colour to "len" pixels
***************************************************************************************/
void TFT_HX8357::pushColor(uint16_t color, uint16_t len)
{
  CS_L;
  PORTA    = color;
  PORTC    = color >> 8;
  while(len) { len--;WR_L;WR_H;}
  CS_H;
}

/***************************************************************************************
** Function name:           pushColors
** Description:             push an aray of pixels for BMP image drawing
***************************************************************************************/
// Sends an array of 16-bit color values to the TFT; used
// externally by BMP examples.  Assumes that setWindow() has
// previously been called to define the bounds.  Max 255 pixels at
// a time (BMP examples read in small chunks due to limited RAM).

void TFT_HX8357::pushColors(uint16_t *data, uint8_t len)
{
  CS_L;
  while(len--) {
    uint16_t color = *data++;
    PORTC    = color;
    PORTA    = color >> 8;
    WR_STB; 
  }
  CS_H;
}

/***************************************************************************************
** Function name:           pushColors
** Description:             push an aray of pixels for 16 bit raw image drawing
***************************************************************************************/
// Assumes that setWindow() has previously been called

void TFT_HX8357::pushColors(uint8_t *data, uint16_t len)
{
  CS_L;
  while(len--) {
    PORTA    = *data++;
    PORTC    = *data++;
    WR_STB; 
  }
  CS_H;
}

/***************************************************************************************
** Function name:           drawLine
** Description:             draw a line between 2 arbitrary points
***************************************************************************************/

// Bresenham's algorithm - thx wikipedia - speed enhanced by Bodmer to use
// an eficient FastH/V Line draw routine for line segments of 2 pixels or more
// enhanced further using code from Xark and Spellbuilder (116 byte penalty)

void TFT_HX8357::drawLine(int16_t x0, int16_t y0, int16_t x1, int16_t y1, uint16_t color)
{
  boolean steep = abs(y1 - y0) > abs(x1 - x0);
  if (steep) {
    swap(x0, y0);
    swap(x1, y1);
  }

  if (x0 > x1) {
    swap(x0, x1);
    swap(y0, y1);
  }

  int16_t dx = x1 - x0, dy = abs(y1 - y0);;


  int16_t err = dx>>1, ystep=-1, xs=x0, dlen=0;
  if (y0 < y1) ystep = 1;

  CS_L;
  // Split into steep and not steep for FastH/V separation
  if(steep) {
    for (; x0<=x1; x0++) {
      dlen++;
      err -= dy;
      if (err < 0) {
        err += dx;
#ifdef FAST_RS
        CSL_RSL_WRL;  PORTC = HX8357_CASET; WR_H; CSL_RSH_WRL;
        PORTC = y0>>8; WR_H;
        PORTC = y0; WR_STB;
        PORTC = y0>>8; WR_STB;
        PORTC = y0; WR_STB;
        CSL_RSL_WRL; PORTC = HX8357_PASET; WR_H; CSL_RSH_WRL;
        PORTC = xs>>8; WR_H;
        PORTC = xs; WR_STB;
        PORTC = x1>>8; WR_STB;
        PORTC = x1; WR_STB;
        CSL_RSL_WRL; PORTC = HX8357_RAMWR; WR_H; CSL_RSH_WRL;
#else
        RS_L;  PORTC = HX8357_CASET; WR_STB; RS_H;
        PORTC = y0>>8; WR_STB;
        PORTC = y0; WR_STB;
        PORTC = y0>>8; WR_STB;
        PORTC = y0; WR_STB;
        RS_L; PORTC = HX8357_PASET; WR_STB; RS_H;
        PORTC = xs>>8; WR_STB;
        PORTC = xs; WR_STB;
        PORTC = x1>>8; WR_STB;
        PORTC = x1; WR_STB;
        RS_L; PORTC = HX8357_RAMWR; WR_STB; RS_H;
#endif
        PORTC = color;
        PORTA = color>>8;

        while (dlen) { dlen--;WR_L;WR_H;}

        y0 += ystep; xs=x0+1;
      }
    }
    if(dlen) drawFastVLine(y0, xs, dlen, color);
  }
  else
  {
    for (; x0<=x1; x0++) {
      dlen++;
      err -= dy;
      if (err < 0) {
        err += dx;

#ifdef FAST_RS
        CSL_RSL_WRL;  PORTC = HX8357_CASET; WR_H; CSL_RSH_WRL;
        PORTC = xs>>8; WR_H;
        PORTC = xs; WR_STB;
        PORTC = x1>>8; WR_STB;
        PORTC = x1; WR_STB;
        CSL_RSL_WRL; PORTC = HX8357_PASET; WR_H; CSL_RSH_WRL;
        PORTC = y0>>8; WR_H;
        PORTC = y0; WR_STB;
        PORTC = y0>>8; WR_STB;
        PORTC = y0; WR_STB;
        CSL_RSL_WRL; PORTC = HX8357_RAMWR; WR_H; CSL_RSH_WRL;
#else
        RS_L;  PORTC = HX8357_CASET; WR_STB; RS_H;
        PORTC = xs>>8; WR_STB;
        PORTC = xs; WR_STB;
        PORTC = x1>>8; WR_STB;
        PORTC = x1; WR_STB;
        RS_L; PORTC = HX8357_PASET; WR_STB; RS_H;
        PORTC = y0>>8; WR_STB;
        PORTC = y0; WR_STB;
        PORTC = y0>>8; WR_STB;
        PORTC = y0; WR_STB;
        RS_L; PORTC = HX8357_RAMWR; WR_STB; RS_H;
#endif
        PORTC = color;
        PORTA = color>>8;

        while(dlen){ dlen--;WR_L;WR_H;}

        y0 += ystep; xs=x0+1;
      }
    }
    if(dlen) drawFastHLine(xs, y0, dlen, color);
  }
  CS_H;
}

/***************************************************************************************
** Function name:           drawFastVLine
** Description:             draw a vertical line
***************************************************************************************/
void TFT_HX8357::drawFastVLine(int16_t x, int16_t y, int16_t h, uint16_t color)
{
#ifdef CLIP_CHECK
  // Rudimentary clipping
  if ((x >= _width) || (y >= _height)) return;
  if ((y + h - 1) >= _height) h = _height - y;
#endif

#ifdef FAST_RS
  CSL_RSL_WRL;  PORTC = HX8357_CASET; WR_H; CSL_RSH_WRL;
  PORTC = x>>8; WR_H;
  PORTC = x; WR_STB;
  PORTC = x>>8; WR_STB;
  PORTC = x; WR_STB;
  CSL_RSL_WRL; PORTC = HX8357_PASET; WR_H; CSL_RSH_WRL;
  PORTC = y>>8; WR_H;
  PORTC = y; WR_STB;
  y+=h-1; PORTC = y>>8; WR_STB;
  PORTC = y; WR_STB;
  CSL_RSL_WRL; PORTC = HX8357_RAMWR; WR_H; CSL_RSH_WRH;
#else
  CS_L;
  RS_L;  PORTC = HX8357_CASET; WR_STB; RS_H;
  PORTC = x>>8; WR_STB;
  PORTC = x; WR_STB;
  PORTC = x>>8; WR_STB;
  PORTC = x; WR_STB;
  RS_L; PORTC = HX8357_PASET; WR_STB; RS_H;
  PORTC = y>>8; WR_STB;
  PORTC = y; WR_STB;
  y+=h-1; PORTC = y>>8; WR_STB;
  PORTC = y; WR_STB;
  RS_L; PORTC = HX8357_RAMWR; WR_STB; RS_H;
#endif

  PORTC = color;
  PORTA = color>>8;
  while (h>16) { h-=16;
    WR_STB;WR_STB;WR_STB;WR_STB;
    WR_STB;WR_STB;WR_STB;WR_STB;
    WR_STB;WR_STB;WR_STB;WR_STB;
    WR_STB;WR_STB;WR_STB;WR_STB;
  }
  while (h--) { WR_L;WR_H;}
  CS_H;
}

/***************************************************************************************
** Function name:           drawFastHLine
** Description:             draw a horizontal line
***************************************************************************************/
void TFT_HX8357::drawFastHLine(int16_t x, int16_t y, int16_t w, uint16_t color)
{
#ifdef CLIP_CHECK
  // Rudimentary clipping
  if ((x >= _width) || (y >= _height)) return;
  if ((x + w - 1) >= _width)  w = _width - x;
#endif

#ifdef FAST_RS
  CSL_RSL_WRL;  PORTC = HX8357_CASET; WR_H; CSL_RSH_WRL;
  PORTC = x>>8; WR_H;
  PORTC = x; WR_STB;
  x+=w-1; PORTC = x>>8; WR_STB;
  PORTC = x; WR_STB;
  CSL_RSL_WRL; PORTC = HX8357_PASET; WR_H; CSL_RSH_WRL;
  PORTC = y>>8; WR_H;
  PORTC = y; WR_STB;
  PORTC = y>>8; WR_STB;
  PORTC = y; WR_STB;
  CSL_RSL_WRL; PORTC = HX8357_RAMWR; WR_H; CSL_RSH_WRH;
#else
  CS_L;
  RS_L;  PORTC = HX8357_CASET; WR_STB; RS_H;
  PORTC = x>>8; WR_STB;
  PORTC = x; WR_STB;
  x+=w-1; PORTC = x>>8; WR_STB;
  PORTC = x; WR_STB;
  RS_L; PORTC = HX8357_PASET; WR_STB; RS_H;
  PORTC = y>>8; WR_STB;
  PORTC = y; WR_STB;
  PORTC = y>>8; WR_STB;
  PORTC = y; WR_STB;
  RS_L; PORTC = HX8357_RAMWR; WR_STB; RS_H;
#endif

  PORTC = color;
  PORTA = color>>8;
  while (w>16) { w-=16;
    WR_STB;WR_STB;WR_STB;WR_STB;
    WR_STB;WR_STB;WR_STB;WR_STB;
    WR_STB;WR_STB;WR_STB;WR_STB;
    WR_STB;WR_STB;WR_STB;WR_STB;
  }
  while(w--){ WR_L;WR_H;}
  CS_H;
}

/***************************************************************************************
** Function name:           fillRect
** Description:             draw a filled rectangle
***************************************************************************************/
void TFT_HX8357::fillRect(int16_t x, int16_t y, int16_t w, int16_t h, uint16_t color)
{
#ifdef CLIP_CHECK
  // rudimentary clipping (drawChar w/big text requires this)
  if ((x >= _width) || (y >= _height) || (w==0) || (h==0)) return;
  if ((x + w - 1) >= _width)  w = _width  - x;
  if ((y + h - 1) >= _height) h = _height - y;
#endif

  setAddrWindow(x, y, x+w-1, y+h-1);
  
  PORTA = color>>8;
  PORTC = color;
  if (h>w) swap(w,h);

  if (w>15){
    while(h--) {
      x=w;
      while (x>16) { x-=16;
        WR_STB;WR_STB;WR_STB;WR_STB;
        WR_STB;WR_STB;WR_STB;WR_STB;
        WR_STB;WR_STB;WR_STB;WR_STB;
        WR_STB;WR_STB;WR_STB;WR_STB;
      }
      while(x--) { WR_L;WR_H;}
    }
  }
  else {
    while(h--) {
      x=w;
      while(x--) { WR_L;WR_H;}
    }
  }
  CS_H;
}

/***************************************************************************************
** Function name:           color565
** Description:             convert three 8 bit RGB levels to a 16 bit colour value
***************************************************************************************/
uint16_t TFT_HX8357::color565(uint8_t r, uint8_t g, uint8_t b)
{
  return ((r & 0xF8) << 8) | ((g & 0xFC) << 3) | (b >> 3);
}

/***************************************************************************************
** Function name:           setRotation
** Description:             rotate the screen orientation m = 0-3 or 4-7 for BMP drawing
***************************************************************************************/

#define MADCTL_MY  0x80
#define MADCTL_MX  0x40
#define MADCTL_MV  0x20
#define MADCTL_ML  0x10
#define MADCTL_RGB 0x00
#define MADCTL_BGR 0x08
#define MADCTL_SS  0x02
#define MADCTL_GS  0x01

void TFT_HX8357::setRotation(uint8_t m)
{
  writecommand(HX8357_MADCTL);
  rotation = m % 8;
  switch (rotation) {
   case 0: // Portrait
     writedata(MADCTL_BGR | MADCTL_MX);
     _width  = HX8357_TFTWIDTH;
     _height = HX8357_TFTHEIGHT;
     break;
   case 1: // Landscape (Portrait + 90)
     writedata(MADCTL_BGR | MADCTL_MV);
     _width  = HX8357_TFTHEIGHT;
     _height = HX8357_TFTWIDTH;
     break;
  case 2: // Inverter portrait
    writedata( MADCTL_BGR | MADCTL_MY);
     _width  = HX8357_TFTWIDTH;
     _height = HX8357_TFTHEIGHT;
    break;
   case 3: // Inverted landscape
     writedata(MADCTL_BGR | MADCTL_MV | MADCTL_MX | MADCTL_MY);
     _width  = HX8357_TFTHEIGHT;
     _height = HX8357_TFTWIDTH;
     break;
   case 4: // Portrait
     writedata(MADCTL_BGR | MADCTL_MX | MADCTL_MY);
     _width  = HX8357_TFTWIDTH;
     _height = HX8357_TFTHEIGHT;
     break;
   case 5: // Landscape (Portrait + 90)
     writedata(MADCTL_BGR | MADCTL_MV | MADCTL_MX);
     _width  = HX8357_TFTHEIGHT;
     _height = HX8357_TFTWIDTH;
     break;
  case 6: // Inverter portrait
    writedata( MADCTL_BGR);
     _width  = HX8357_TFTWIDTH;
     _height = HX8357_TFTHEIGHT;
    break;
   case 7: // Inverted landscape
     writedata(MADCTL_BGR | MADCTL_MV | MADCTL_MY);
     _width  = HX8357_TFTHEIGHT;
     _height = HX8357_TFTWIDTH;
     break;
  }
}

/***************************************************************************************
** Function name:           invertDisplay
** Description:             invert the display colours i = 1 invert, i = 0 normal
***************************************************************************************/
void TFT_HX8357::invertDisplay(boolean i)
{
  writecommand(i ? HX8357_INVON : HX8357_INVOFF);
}

/***************************************************************************************
** Function name:           write
** Description:             draw characters piped through serial stream
***************************************************************************************/
size_t TFT_HX8357::write(uint8_t uniCode)
{
  if (uniCode == '\r') return 1;
  unsigned int width = 0;
  unsigned int height = 0;
  //Serial.print((char) uniCode); // Debug line sends all printed TFT text to serial port

#ifdef LOAD_FONT2
  if (textfont == 2)
  {
      // This is 20us faster than using the fontdata structure (0.443ms per character instead of 0.465ms)
      width = pgm_read_byte(widtbl_f16 + uniCode-32);
      height = chr_hgt_f16;
      // Font 2 is rendered in whole byte widths so we must allow for this
      width = (width + 6) / 8;  // Width in whole bytes for font 2, should be + 7 but must allow for font width change
      width = width * 8;        // Width converted back to pixles
  }
  #ifdef LOAD_RLE
  else
  #endif
#endif


#ifdef LOAD_RLE
  {
      // Uses the fontinfo struct array to avoid lots of 'if' or 'switch' statements
      // A tad slower than above but this is not significant and is more convenient for the RLE fonts
      // Yes, this code can be needlessly executed when textfont == 1...
      width = pgm_read_byte( pgm_read_word( &(fontdata[textfont].widthtbl ) ) + uniCode-32 );
      height= pgm_read_byte( &fontdata[textfont].height );
  }
#endif

#ifdef LOAD_GLCD
  if (textfont==1)
  {
      width =  6;
      height = 8;
  }
#else
  if (textfont==1) return 0;
#endif

  height = height * textsize;

  if (uniCode == '\n') {
    cursor_y += height;
    cursor_x  = 0;
  }
  else
  {
    if (textwrap && (cursor_x + width * textsize >= _width))
    {
      cursor_y += height;
      cursor_x = 0;
    }
    cursor_x += drawChar(uniCode, cursor_x, cursor_y, textfont);
  }
  return 1;
}

/***************************************************************************************
** Function name:           drawChar
** Description:             draw a unicode onto the screen
***************************************************************************************/
int TFT_HX8357::drawChar(unsigned int uniCode, int x, int y, int font)
{

  if (font==1)
  {
#ifdef LOAD_GLCD
      drawChar(x, y, uniCode, textcolor, textbgcolor, textsize);
      return 6 * textsize;
#else
      return 0;
#endif
  }

  unsigned int width  = 0;
  unsigned int height = 0;
  char *flash_address = 0;

  uniCode -= 32;

#ifdef LOAD_FONT2
  if (font == 2)
  {
      // This is 20us faster than using the fontdata structure
      flash_address = (char *)pgm_read_word(&chrtbl_f16[uniCode]);
      width = pgm_read_byte(widtbl_f16 + uniCode);
      height = chr_hgt_f16;
  }
  #ifdef LOAD_RLE
  else
  #endif
#endif

#ifdef LOAD_RLE
  {
      // This is slower than above but is more convenient for the RLE fonts
      flash_address = (char *)pgm_read_word( pgm_read_word( &(fontdata[font].chartbl ) ) + uniCode*sizeof(void *) );
      width = pgm_read_byte( pgm_read_word( &(fontdata[font].widthtbl ) ) + uniCode );
      height= pgm_read_byte( &fontdata[font].height );
  }
#endif

  int w = width;
  byte line = 0;

  byte tl = textcolor;
  byte th = textcolor >> 8;
  byte bl = textbgcolor;
  byte bh = textbgcolor >> 8;

#ifdef LOAD_FONT2 // chop out 962 bytes of code if we do not need it
  if (font == 2) {
    w = w + 6; // Should be + 7 but we need to compensate for width increment
    w = w / 8;
    if (x + width * textsize >= _width) return width * textsize ;

    if (textcolor == textbgcolor || textsize != 1) {
      int pX      = 0;
      int pY      = y;
      for (int i = 0; i < height; i++)
      {
        if (textcolor != textbgcolor) fillRect(x, pY, width * textsize, textsize, textbgcolor);

        for (int k = 0; k < w; k++)
        {
          line = pgm_read_byte(flash_address + w * i + k);
          if (line) {
            if (textsize == 1) {
              pX = x + k * 8;
              if (line & 0x80) drawPixel(pX, pY, textcolor);
              if (line & 0x40) drawPixel(pX + 1, pY, textcolor);
              if (line & 0x20) drawPixel(pX + 2, pY, textcolor);
              if (line & 0x10) drawPixel(pX + 3, pY, textcolor);
              if (line & 0x08) drawPixel(pX + 4, pY, textcolor);
              if (line & 0x04) drawPixel(pX + 5, pY, textcolor);
              if (line & 0x02) drawPixel(pX + 6, pY, textcolor);
              if (line & 0x01) drawPixel(pX + 7, pY, textcolor);
            }
            else {
              pX = x + k * 8 * textsize;
              if (line & 0x80) fillRect(pX, pY, textsize, textsize, textcolor);
              if (line & 0x40) fillRect(pX + textsize, pY, textsize, textsize, textcolor);
              if (line & 0x20) fillRect(pX + 2 * textsize, pY, textsize, textsize, textcolor);
              if (line & 0x10) fillRect(pX + 3 * textsize, pY, textsize, textsize, textcolor);
              if (line & 0x08) fillRect(pX + 4 * textsize, pY, textsize, textsize, textcolor);
              if (line & 0x04) fillRect(pX + 5 * textsize, pY, textsize, textsize, textcolor);
              if (line & 0x02) fillRect(pX + 6 * textsize, pY, textsize, textsize, textcolor);
              if (line & 0x01) fillRect(pX + 7 * textsize, pY, textsize, textsize, textcolor);
            }
          }
        }
        pY += textsize;
      }
    }
    else
      // Faster drawing of characters and background using block write
    {
      setAddrWindow(x, y, (x + w * 8) - 1, y + height - 1);

      for(int i=0; i<height; i++)
      {
        for (int k = 0;k < w; k++)
        { 
        line = pgm_read_byte(flash_address+w*i+k);
        if(line&0x80) {PORTA = th; PORTC = tl;}
        else {PORTA = bh; PORTC = bl;} WR_STB;
        if(line&0x40) {PORTA = th; PORTC = tl;}
        else {PORTA = bh; PORTC = bl;} WR_STB;
        if(line&0x20) {PORTA = th; PORTC = tl;}
        else {PORTA = bh; PORTC = bl;} WR_STB;
        if(line&0x10) {PORTA = th; PORTC = tl;}
        else {PORTA = bh; PORTC = bl;} WR_STB;
        if(line&0x8) {PORTA = th; PORTC = tl;}
        else {PORTA = bh; PORTC = bl;} WR_STB;
        if(line&0x4) {PORTA = th; PORTC = tl;}
        else {PORTA = bh; PORTC = bl;} WR_STB;
        if(line&0x2) {PORTA = th; PORTC = tl;}
        else {PORTA = bh; PORTC = bl;} WR_STB;
        if(line&0x1) {PORTA = th; PORTC = tl;}
        else {PORTA = bh; PORTC = bl;} WR_STB;
        }
      }
      CS_H;
    }
  }

  #ifdef LOAD_RLE
  else
  #endif
#endif  //FONT2

#ifdef LOAD_RLE  //674 bytes of code
  // Font is not 2 and hence is RLE encoded
  {

    w *= height; // Now w is total number of pixels in the character
    if ((textsize != 1) || (textcolor == textbgcolor)) {
      if (textcolor != textbgcolor) fillRect(x, y, width * textsize, textsize * height, textbgcolor);
      int px = 0, py = y, tpy = y; // To hold character block start and end column and row values
      int pc = 0; // Pixel count
      byte np = textsize * textsize; // Number of pixels in a drawn pixel

      byte tnp = 0; // Temporary copy of np for while loop
      byte ts = textsize - 1; // Temporary copy of textsize
      // 16 bit pixel count so maximum font size is equivalent to 180x180 pixels in area
      // w is total number of pixels to plot to fill character block
      while (pc < w)
      {
        line = pgm_read_byte(flash_address);
        flash_address++; // 20 bytes smaller by incrementing here
        if (line & 0x80) {
          line &= 0x7F;
          line++;
          if (ts) {
            px = x + textsize * (pc % width); // Keep these px and py calculations outside the loop as they are slow
            py = y + textsize * (pc / width);
          }
          else {
            px = x + pc % width; // Keep these px and py calculations outside the loop as they are slow
            py = y + pc / width;
          }
          while (line--) { // In this case the while(line--) is faster
            pc++; // This is faster than putting pc+=line before while()
            setAddrWindow(px, py, px + ts, py + ts);
            PORTA = th; PORTC = tl;
            if (ts) {
              tnp = np;
              // textsize > 1 so pixel count will be a multiple of 4
              while (tnp) { tnp-=4; WR_STB;WR_STB;WR_STB;WR_STB; }
            }
            else { WR_L; WR_H; }

            px += textsize;

            if (px >= (x + width * textsize))
            {
              px = x;
              py += textsize;
            }
          }
        }
        else {
          line++;
          pc += line;
        }
      }
      CS_H;
    }
    else // Text colour != background && textsize = 1
         // so use faster drawing of characters and background using block write
    {
      setAddrWindow(x, y, x + width - 1, y + height - 1);

      // Maximum font size is equivalent to 180x180 pixels in area
      while (w > 0)
      {
        line = pgm_read_byte(flash_address++);
        if (line & 0x80) {
          line &= 0x7F;
          PORTA = th;
          PORTC = tl;
        }
        else {
          PORTA = bh;
          PORTC = bl;
        }
        line++; w -= line;
        while(line>3){ line-=4; WR_STB; WR_STB; WR_STB; WR_STB; }
        while(line)  { line--;  WR_STB;}
      }
      CS_H;
    }
  }
  // End of RLE font rendering
#endif
  return width * textsize;    // x +
}

/***************************************************************************************
** Function name:           drawString
** Description :            draw string with padding if it is defined
***************************************************************************************/
int TFT_HX8357::drawString(char *string, int poX, int poY, int font)
{
  int16_t sumX = 0;
  uint8_t padding = 1;
  unsigned int cheight = 0;

  if (textdatum || padX)
  {
    // Find the pixel width of the string in the font
    unsigned int cwidth  = textWidth(string, font);

    // Get the pixel height of the font
    cheight = pgm_read_byte( &fontdata[font].height ) * textsize;

    switch(textdatum) {
      case TC_DATUM:
        poX -= cwidth/2;
        padding = 2;
        break;
      case TR_DATUM:
        poX -= cwidth;
        padding = 3;
        break;
      case ML_DATUM:
        poY -= cheight/2;
        padding = 1;
        break;
      case MC_DATUM:
        poX -= cwidth/2;
        poY -= cheight/2;
        padding = 2;
        break;
      case MR_DATUM:
        poX -= cwidth;
        poY -= cheight/2;
        padding = 3;
        break;
      case BL_DATUM:
        poY -= cheight;
        padding = 1;
        break;
      case BC_DATUM:
        poX -= cwidth/2;
        poY -= cheight;
        padding = 2;
        break;
      case BR_DATUM:
        poX -= cwidth;
        poY -= cheight;
        padding = 3;
        break;
    }
    // Check coordinates are OK, adjust if not
    if (poX < 0) poX = 0;
    if (poX+cwidth>_width)   poX = _width - cwidth;
    if (poY < 0) poY = 0;
    if (poY+cheight>_height) poY = _height - cheight;
  }

  while (*string) sumX += drawChar(*(string++), poX+sumX, poY, font);

//#define PADDING_DEBUG

#ifndef PADDING_DEBUG
  if((padX>sumX) && (textcolor!=textbgcolor))
  {
    int padXc = poX+sumX; // Maximum left side padding
    switch(padding) {
      case 1:
        fillRect(padXc,poY,padX-sumX,cheight, textbgcolor);
        break;
      case 2:
        fillRect(padXc,poY,(padX-sumX)>>1,cheight, textbgcolor);
        padXc = (padX-sumX)>>1;
        if (padXc>poX) padXc = poX;
        fillRect(poX - padXc,poY,(padX-sumX)>>1,cheight, textbgcolor);
        break;
      case 3:
        if (padXc>padX) padXc = padX;
        fillRect(poX + sumX - padXc,poY,padXc-sumX,cheight, textbgcolor);
        break;
    }
  }
#else

  // This is debug code to show text (green box) and blanked (white box) areas
  // to show that the padding areas are being correctly sized and positioned
  if((padX>sumX) && (textcolor!=textbgcolor))
  {
    int padXc = poX+sumX; // Maximum left side padding
    drawRect(poX,poY,sumX,cheight, TFT_GREEN);
    switch(padding) {
      case 1:
        drawRect(padXc,poY,padX-sumX,cheight, TFT_WHITE);
        break;
      case 2:
        drawRect(padXc,poY,(padX-sumX)>>1, cheight, TFT_WHITE);
        padXc = (padX-sumX)>>1;
        if (padXc>poX) padXc = poX;
        drawRect(poX - padXc,poY,(padX-sumX)>>1,cheight, TFT_WHITE);
        break;
      case 3:
        if (padXc>padX) padXc = padX;
        drawRect(poX + sumX - padXc,poY,padXc-sumX,cheight, TFT_WHITE);
        break;
    }
  }
#endif

return sumX;
}

/***************************************************************************************
** Function name:           drawCentreString
** Descriptions:            draw string centred on dX
***************************************************************************************/
int TFT_HX8357::drawCentreString(char *string, int dX, int poY, int font)
{
  byte tempdatum = textdatum;
  int sumX = 0;
  textdatum = TC_DATUM;
  sumX = drawString(string, dX, poY, font);
  textdatum = tempdatum;
  return sumX;
}

/***************************************************************************************
** Function name:           drawRightString
** Descriptions:            draw string right justified to dX
***************************************************************************************/
int TFT_HX8357::drawRightString(char *string, int dX, int poY, int font)
{
  byte tempdatum = textdatum;
  int sumX = 0;
  textdatum = TR_DATUM;
  sumX = drawString(string, dX, poY, font);
  textdatum = tempdatum;
  return sumX;
}

/***************************************************************************************
** Function name:           drawNumber
** Description:             draw a long integer
***************************************************************************************/
int TFT_HX8357::drawNumber(long long_num, int poX, int poY, int font)
{
  char str[12];
  ltoa(long_num, str, 10);
  return drawString(str, poX, poY, font);
}

/***************************************************************************************
** Function name:           drawFloat
** Descriptions:            drawFloat, prints 7 non zero digits maximum
***************************************************************************************/
// Adapted to assemble and print a string, this permits alignment relative to a datum
// looks complicated but much more compact and actually faster than using print class
int TFT_HX8357::drawFloat(float floatNumber, int dp, int poX, int poY, int font)
{
  char str[14];               // Array to contain decimal string
  uint8_t ptr = 0;            // Initialise pointer for array
  int8_t  digits = 1;         // Count the digits to avoid array overflow
  float rounding = 0.5;       // Round up down delta

  if (dp > 7) dp = 7; // Limit the size of decimal portion

  // Adjust the rounding value
  for (uint8_t i = 0; i < dp; ++i) rounding /= 10.0;

  if (floatNumber < -rounding)    // add sign, avoid adding - sign to 0.0!
  {
    str[ptr++] = '-'; // Negative number
    str[ptr] = 0; // Put a null in the array as a precaution
    digits = 0;   // Set digits to 0 to compensate so pointer value can be used later
    floatNumber = -floatNumber; // Make positive
  }

  floatNumber += rounding; // Round up or down

  // For error put ... in string and return (all TFT_HX8357 library fonts contain . character)
  if (floatNumber >= 2147483647) {
    strcpy(str, "...");
    return drawString(str, poX, poY, font);
  }
  // No chance of overflow from here on

  // Get integer part
  unsigned long temp = (unsigned long)floatNumber;

  // Put integer part into array
  ltoa(temp, str + ptr, 10);

  // Find out where the null is to get the digit count loaded
  while ((uint8_t)str[ptr] != 0) ptr++; // Move the pointer along
  digits += ptr;                  // Count the digits

  str[ptr++] = '.'; // Add decimal point
  str[ptr] = '0';   // Add a dummy zero
  str[ptr + 1] = 0; // Add a null but don't increment pointer so it can be overwritten

  // Get the decimal portion
  floatNumber = floatNumber - temp;

  // Get decimal digits one by one and put in array
  // Limit digit count so we don't get a false sense of resolution
  uint8_t i = 0;
  while ((i < dp) && (digits < 9)) // while (i < dp) for no limit but array size must be increased
  {
    i++;
    floatNumber *= 10;       // for the next decimal
    temp = floatNumber;      // get the decimal
    ltoa(temp, str + ptr, 10);
    ptr++; digits++;         // Increment pointer and digits count
    floatNumber -= temp;     // Remove that digit
  }
  
  // Finally we can plot the string and return pixel length
  return drawString(str, poX, poY, font);
}


/***************************************************

  ORIGINAL LIBRARY HEADER

  This is our library for the Adafruit  HX8357 Breakout and Shield
  ----> http://www.adafruit.com/products/1651

  Check out the links above for our tutorials and wiring diagrams
  These displays use SPI to communicate, 4 or 5 pins are required to
  interface (RST is optional)
  Adafruit invests time and resources providing this open source code,
  please support Adafruit and open-source hardware by purchasing
  products from Adafruit!

  Written by Limor Fried/Ladyada for Adafruit Industries.
  MIT license, all text above must be included in any redistribution

 ****************************************************/