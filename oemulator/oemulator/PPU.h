#pragma once
#include "NES.h"

class PPUMemory;

typedef unsigned char byte;

class PPU {
public:
	PPU(PPUMemory* memory_);

	//performes one PPU step
	void step();

	//IO mapped registers at 0x2000-0x2007
	//0x2000 PPUCTRL CPU has write access
	byte CTRL;
	byte CTRLNametable = 0b0000'0011; //0: $2000; 1: $2400; 2: $2800; 3: $2C00
	//VRAM address increment per CPU read/write of PPUDATA (0: add 1, going across
	//1: add 32, going down)
	byte CTRLIncrement = 0b0000'0100;
	//Sprite pattern table address for 8x8 sprites (0: $0000; 1: $1000; ignored in 8x16 mode)
	byte CTRLSpritePattern = 0b0000'1000;
	//Background pattern table address(0: $0000; 1: $1000)
	byte CTRLBackgroundPattern = 0b0001'0000;
	//Sprite size (0: 8x8; 1: 8x16)
	byte CTRLSpriteSize = 0b0010'0000;
	//PPU master/slave select(0: read backdrop from EXT pins; 1: output color on EXT pins)
	byte CTRLMasterSlaveSelect = 0b0100'0000;
	//Generate an NMI at the start of the vertical blanking interval(0: off; 1: on)
	byte CTRLNMI = 0b1000'0000;

	//0x2001 PPUMASK CPU has write access
	byte MASK;
	byte MASKGreyscale = 0b0000'0001; //Greyscale (0: normal color, 1: produce a greyscale display)
	// 1: Show background in leftmost 8 pixels of screen, 0: Hide
	byte MASKShowLeftBackground = 0b0000'0010;
	// 1: Show sprites in leftmost 8 pixels of screen, 0 : Hide
	byte MASKShowLeftSprites = 0b0000'0100;
	// 1 : Show background
	byte MASKShowBackground = 0b0000'1000;
	// 1 : Show sprites
	byte MASKShowSprites = 0b0001'0000;
	// Emphasize red*
	byte MASKRed = 0b0010'0000;
	// Emphasize green*
	byte MASKGreen = 0b0100'0000;
	// Emphasize blue*
	byte MASKBlue = 0b1000'0000;

	//0x2002 PPUSTATUS access read
	byte STATUS;
	/*Sprite overflow. The intent was for this flag to be set
||         whenever more than eight sprites appear on a scanline, but a
||         hardware bug causes the actual behavior to be more complicated
||         and generate false positives as well as false negatives; see
||         PPU sprite evaluation. This flag is set during sprite
||         evaluation and cleared at dot 1 (the second dot) of the
||         pre-render line.
	*/
	byte STATUSSpriteOverflow = 0b0010'0000;
	/*Sprite 0 Hit.  Set when a nonzero pixel of sprite 0 overlaps
|          a nonzero background pixel; cleared at dot 1 of the pre-render
|          line.  Used for raster timing.*/
	byte STATUSSpriteZeroHit = 0b0100'0000;
	/*Vertical blank has started (0: not in vblank; 1: in vblank).
           Set at dot 1 of line 241 (the line *after* the post-render
           line); cleared after reading $2002 and at dot 1 of the
           pre-render line.*/
	byte STATUSVBlankStarted = 0b1000'0000;

	//0x2003 OAMADDR access write. (Object attribute memory address)
	byte OAMADDR; //this is just the address, the individual bits has no special significanse, so we dont need any bit-masks

	//0x2004-0x2006 does not need any register masks, they should be handeled in the
	//read/writeRegister functions

	//0x2007 PPUDATA read/write access	
	//this is also handeled like 0x2004-0x2006 but we need a buffer
	byte DATABuffer;

	byte readRegister(int addr); //read register at addr, addr should be 0x2000-0x2007
	void writeRegiter(int addr, byte val); //write register

	//Object attribute memory. PPU has 256 bytes of OAM
	byte OAM[256];

	//video buffer
	int pixels[256 * 240];


	//which cycle we are on (0-340). 341 cycles per scanline 
	int cycle;
	//which scanline are we on (0-261, where 0-239 are the visible ones, 240 is "post" 
	//241-260 is vblank (vertical blanking lines), 261 is some pre calculations for scanline 0
	int scanLine;

	//PPU registers
	int v; //current VRAM address (15bit)
	int t; //temporary VRAM address (15bit)
	byte x; //fine x scroll (3 bit)
	bool writeToggle = false; //some MM Registers behave differently if this is the first or second write. use this to keep track
	bool oddFrame; //is frame odd/even
	//we need to keep track of what was last written to a register, this is returned when reading
	//0x2007 for instance
	byte registerBuffer;

	//these are the values required to draw the next 8 pixels of background. these are fetched
	//every 8 ppu cycles
	byte nameTableByte;
	byte attributeTableByte;
	byte bitmapLow; 
	byte bitmapHigh;

	//OAM info about the (up to) 8 sprites on this scanline is kept in shift registers
	int spriteBitmapData[8]; //16bits of data per sprite (for 1 row) 1 byte upper and 1 byte lower
	byte spriteAttributes[8];
	byte ATTRIBUTEPALETTE = 3;
	byte ATTRIBUTEPRIORITY = 0x20;
	byte ATTRIBUTEFLIPHOR = 0x40;
	byte ATTRIBUTEFLIPVER = 0x80;
	byte spriteXPositions[8];
	byte numberOfSpritesOnScanline;

private:
	PPUMemory* memory;

	//sprite evaluation phase happens for the next scanline as cycles 1-256 are processed.
	//since we just do this instantly and we are not using shiftregisters we do it at cycle
	//257.if we evaluate sprite data for the next scanline in cycles1-256 we will
	//overwrite the data needed for this line
	void spriteEvaluation();

};