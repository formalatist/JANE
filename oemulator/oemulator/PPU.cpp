#include "PPU.h"

PPU::PPU(PPUMemory* memory_)
{
	memory = memory_;
}

void PPU::setNES(NES * nes_)
{
	nes = nes_;
}

void PPU::step()
{
	advanceCounters();

	bool isPreLine = scanLine == 261;
	bool isVisibleLine = scanLine < 240; //lines are the Y of the pixel
	bool isRenderedLine = isPreLine || isVisibleLine;
	bool isPrefetchCycle = cycle >= 321 && cycle <= 336;
	bool isVisibleCycle = cycle >= 1 && cycle <= 256; //cycles gives the X
	bool isFetchCycle = isPrefetchCycle || isVisibleCycle;

	if (true /*rendering is enabled*/) {
		if (isVisibleLine && isVisibleCycle) { //Both X and Y are within their visible areas
			blitPixel();
		}
		if (isRenderedLine && isFetchCycle) {
			switch (cycle % 8) {
			case 1:
				setNametableByte();
			case 3:
				setAttributeTableByte();
			case 5:
				setTileBitmapLow();
			case 7:
				setTileBitmapHigh();
			case 0:
				createTileBitmap();
			}
		}
		if (isPreLine && cycle >= 280 && cycle <= 304) {
			/*If rendering is enabled, at the end of vblank, shortly after 
			the horizontal bits are copied from t to v at dot 257, the PPU 
			will repeatedly copy the vertical bits from t to v from 
			dots 280 to 304, completing the full initialization of v from t:*/
			copyVerticalBits();
		}
		if (isRenderedLine) {
			if (isFetchCycle && cycle % 8 == 0) {
				incrementX();
			}
			if (cycle == 256) {
				incrementY();
			}
			if (cycle == 257) {
				copyHorizontalBits();
			}
		}

		if (cycle == 257) {
			if (isVisibleLine) { //we are at the end of a visible line. evaluate sprites for the next line
				spriteEvaluation();
			} else {
				numberOfSpritesOnScanline = 0;
			}
		}
	}

	if(scanLine == 241 && cycle == 1) { //vertical blank
		
	}
	if (isPreLine && cycle == 1) {
		leaveVerticalBlank();
	}
}

void PPU::advanceCounters()
{
	bool showSprites = (MASK & MASKShowSprites) == MASKShowSprites;
	bool showBackground = (MASK & MASKShowBackground) == MASKShowBackground;
	
	//generate NMI
	if (((CTRL & CTRLNMI) == CTRLNMI) && NMIOccured) {
		memory->cpu->triggerNMI();
	}

	if(showBackground || showSprites) { //if we are rendering anything
		//TODO: 
	}

	cycle++;
	if (cycle > 340) { //we are done with this scanline so we go to the next
		cycle = 0;
		scanLine++;
		if(scanLine > 261) { //we are done with the screen
			scanLine = 0;
			nes->updateScreen();
		}
	}
}

byte PPU::readRegister(int addr)	
{
	if (addr == 0x2002) { // PPUSTATUS
		writeToggle = false;
		byte val = STATUS & (STATUSVBlankStarted | STATUSSpriteZeroHit | STATUSSpriteOverflow);
		val = val | (registerBuffer & 0x1F);
		return val;
	} else if (addr == 0x2004) { // OAMDATA
		return OAM[OAMADDR];
	} else if (addr == 0x2007) { // PPUDATA
		byte val = memory->read(v);
		/*When reading while the VRAM address is in the range 0-$3EFF 
		(i.e., before the palettes), the read will return the contents of an 
		internal read buffer. This internal buffer is updated only when reading PPUDATA, 
		and so is preserved across frames*/
		if(v < 0x3F00) { // we need to do a buffered read (see above)
			byte oldDATABuffer = DATABuffer;
			DATABuffer = val;
			val = oldDATABuffer;
		} else {
			//TODO check: v - 0x1000 should put us at nametable 3 and not the mirror region
			//which would instead have been nametable 0 (me thinks). This is probably not
			//used by any games anyway and should not matter
			DATABuffer = memory->read(v - 0x1000);
		}

		//remember to increment v
		if ((CTRL & CTRLIncrement) == CTRLIncrement) {
			v += 32;
		}
		else {
			v++;
		}
		return val;
	} else {
		std::cout << "Tried reading register that doesnt exist: " << addr << std::endl;
		return 0;
	}
}

void PPU::writeRegiter(int addr, byte val)
{
	registerBuffer = val;
	if (addr == 0x2000) { //PPUCTRL register
		//sets the PPUCTRL register to val
		CTRL = val;
		//In the following, d refers to the data written to the port, and A through H 
		//to individual bits of a value.
		//t: ...BA.. ........ = d: ......BA
		//clear bit 10 and 11 of t and OR t with bit 0 and 1 of val bitshifted 10 to the left
		t = (t & 0xF3FF) | ((val & 0x03) << 10);
	} else if (addr == 0x2001) { // PPUMASK
		MASK = val;
	} else if (addr == 0x2002) { // PPUSTATUS read only
		std::cout << "ERROR: Writing to 0x2002 PPUSTATUS. Val: " << val << std::endl;
	} else if (addr == 0x2003) { // OAMADDR
		//sets the address for OAM
		OAMADDR = val;
	} else if (addr == 0x2004) { // OAMDATA
		/*Write OAM data here. Writes will increment OAMADDR after the write; 
		reads during vertical or forced blanking return the value from OAM at that 
		address but do not increment.*/
		OAM[OAMADDR] = val;
		OAMADDR++;
	} else if (addr == 0x2005) { // PPUSCROLL
		if (!writeToggle) {
			/*	t: ....... ...HGFED = d: HGFED...
				x:              CBA = d: .....CBA
   (writeToggle)w:                  = 1				*/
			t = (t & 0xFFE0) | ((val & 0xF8) >> 3);
			x = val & 0b111;
			writeToggle = true;
		}
		else {
			/*	t: CBA..HG FED..... = d: HGFEDCBA
				w:                  = 0*/
			t = (t & 0xC1F) | ((val & 7) << 12) | ((val & 0xF8) << 2);
			writeToggle = false;
		}
	} else if (addr == 0x2006) { // PPUADDR
		if (!writeToggle) {
			/*	t: .FEDCBA ........ = d: ..FEDCBA
				t: X...... ........ = 0
   (writeToggle)w:                  = 1*/
			t = (t & 0xFF) | ((val & 0x3F) << 8);
			writeToggle = true;
		}
		else {
			/*	t: ....... HGFEDCBA = d: HGFEDCBA
				v                   = t
   (writeToggle)w:                  = 0*/
			t = (t & 0x7F00) | val;
			v = t;
			writeToggle = false;
		}
	} else if (addr == 0x2007) { // PPUDATA
		memory->write(v, val);
		/*Outside of rendering, reads from or writes to $2007 will 
		add either 1 or 32 to v depending on the VRAM increment bit set via $2000.
		TODO: There might be more complicated behaviour here we need to emulate. check nesdev wiki*/
		if ((CTRL & CTRLIncrement) == CTRLIncrement) {
			v += 32;
		}
		else {
			v++;
		}
	} else if(addr == 0x4014) { //OAMDMA
		/*Writing $XX will upload 256 bytes of data from CPU page $XX00-$XXFF to 
		the internal PPU OAM*/
		int cpuMemoryAddress = val << 8;
		for (int i = 0; i < 256; i++) {
			OAM[OAMADDR] = memory->cpu->memory->read(cpuMemoryAddress);
			cpuMemoryAddress++;
			OAMADDR++;
		}
		//it takes the CPU 513 cycles to do this, so stall for 513
		memory->cpu->cyclesToStall += 513;
		//if the number of cycles the cpu has done is odd it takes an extra cycle
		if (memory->cpu->cycles % 2 == 1) {
			memory->cpu->cyclesToStall++;
		}
	} else {
		std::cout << "ERROR: Tried writing to PPU register " << addr << std::endl;
	}
}

void PPU::spriteEvaluation()
{
	//loop over all sprites in OAM and find the (up to) 8 on this scanline
	numberOfSpritesOnScanline = 0;
	for (int i = 0; i < 64; i++) {
		//check if this sprite is on our scanline
		byte yPos = OAM[i * 4];
		byte rowOfSprite = scanLine - yPos;
		if(rowOfSprite >= 0 && rowOfSprite <= 7) { //our scanline hits this sprite
			if (numberOfSpritesOnScanline < 8) { // theres still room for sprites on this scanline
				byte tile =		 OAM[i * 4 + 1];
				byte attribute = OAM[i * 4 + 2];
				byte xPos =		 OAM[i * 4 + 3];
				spriteAttributes[numberOfSpritesOnScanline] = attribute;
				spriteXPositions[numberOfSpritesOnScanline] = xPos;
				isSpriteZero[numberOfSpritesOnScanline] = i == 0;
				spriteBitmapData[numberOfSpritesOnScanline] = getSpriteBitmapData(rowOfSprite, tile, attribute);
			}
			numberOfSpritesOnScanline++;
		}
	}

	//set sprite overflow flag
	if (numberOfSpritesOnScanline > 8) {
		numberOfSpritesOnScanline = 8;
		STATUS = STATUS | STATUSSpriteOverflow;
	}
}

void PPU::enterVerticalBlank()
{
	NMIOccured = true;
}

void PPU::leaveVerticalBlank()
{
	NMIOccured = false;
}

int PPU::getSpriteBitmapData(byte row, byte tile, byte attribute)
{
	int addr;
	int spriteBitmapData = 0;;
	//TODO: take into account weather or not we are in 8x8 or 8x16 sprite mode
	//for now only support 8x8
	if((attribute & 0x80) == 0x80) { //flip vertically
		row = 7 - row;
	}
	addr = 0x1000 * ((CTRL & CTRLSpritePattern) >> 3) + tile*16 + row;
	byte bitmapLow = memory->read(addr);
	byte bitmapHigh = memory->read(addr + 8);
	bool highBit;
	bool lowBit;
	bool flipHorizontal = attribute & 0x40;
	//loop over the 8 pixels and add them to the spriteBitmapData
	for (int i = 0; i < 8; i++) {
		if (flipHorizontal) { //if flipping hor we start at the right most bits, that way they end up at the end of bitmapData
			lowBit = bitmapLow & 1;
			highBit = bitmapHigh & 1;
			bitmapHigh = bitmapHigh >> 1;
			bitmapLow = bitmapLow >> 1;
		}
		else {
			lowBit = (bitmapLow & 0x80) >> 7;
			highBit = (bitmapHigh & 0x80) >> 7;
			bitmapHigh = bitmapHigh << 1;
			bitmapLow = bitmapLow << 1;
		}
		spriteBitmapData <<= 2;
		spriteBitmapData |= (highBit << 1) | lowBit;
	}

	return spriteBitmapData;
}

void PPU::setNametableByte()
{
	nameTableByte = memory->read(0x2000 | (v & 0x0FFF));
}

void PPU::setAttributeTableByte()
{
	attributeTableByte = memory->read(0x23C0 | (v & 0x0C00) | ((v >> 4) & 0x38) | ((v >> 2) & 0x07));
}

void PPU::setTileBitmapLow()
{
	//if backgroundpattern bit is 1 we add 0x1000 to the address
	int addr = (0x1000)*((CTRL & CTRLBackgroundPattern) == CTRLBackgroundPattern);
	//get the fine Y scroll from V. This is in the 3 high bits. v is 15 bit
	int fineYScroll = (v >> 12) & 7;
	addr += nameTableByte * 16 + fineYScroll;
	tileBitmapLow = memory->read(addr);
}

//high byte is 8 bytes further in memory
void PPU::setTileBitmapHigh()
{
	//if backgroundpattern bit is 1 we add 0x1000 to the address
	int addr = (0x1000)*((CTRL & CTRLBackgroundPattern) == CTRLBackgroundPattern);
	//get the fine Y scroll from V. This is in the 3 high bits. v is 15 bit
	int fineYScroll = (v >> 12) & 7;
	addr += nameTableByte * 16 + fineYScroll + 8;
	tileBitmapHigh = memory->read(addr);
}

void PPU::createTileBitmap()
{
	int newTileBitmap = 0;
	for (int i = 0; i < 8; i++) {
		newTileBitmap <<= 2;
		newTileBitmap |= ((tileBitmapLow & 0x80) >> 7) 
			| ((tileBitmapHigh & 0x80) >> 6);
		tileBitmapLow <<= 1;
		tileBitmapHigh <<= 1;
	}
	tileBitmap = newTileBitmap;
}

void PPU::copyVerticalBits()
{
	//v: IHGF.ED CBA..... = t: IHGF.ED CBA.....
	v = (v & 0x41F) || (t & 0x7BE0);
}

void PPU::incrementX()
{
	// increment hori(v)
	if ((v & 0x001F) == 31) { // if coarse X == 31
		v &= 0xFFE0; // coarse X = 0
		v ^= 0x0400; // switch horizontal nametable
	} else {// increment coarse X
		v++;
	}
}

void PPU::incrementY()
{
	if ((v & 0x7000) != 0x7000) {// if fine Y < 7
		v += 0x1000; // increment fine Y
	} 
	else {
		v &= ~0x7000;// fine Y = 0
		int y = (v & 0x03E0) >> 5;// let y = coarse Y
		if (y == 29) {
			y = 0;// coarse Y = 0
			v ^= 0x0800;
		}// switch vertical nametable
		else if (y == 31) {
			y = 0; // coarse Y = 0, nametable not switched
		}
		else {
			y += 1; // increment coarse Y
			v = (v & ~0x03E0) | (y << 5);// put coarse Y back into v
		}
	}
}

void PPU::copyHorizontalBits()
{
	//v: ....F.. ...EDCBA = t: ....F.. ...EDCBA
	v = (v & 0x7BE0) | (t & 0x41F);
}

int PPU::getPixelSpriteColor()
{
	int col = 0;
	//loop over the sprites on the scanline and check if any of them are on this pixel
	for (int i = 0; i < numberOfSpritesOnScanline; i++) {
		int spriteColumn = cycle - 1 - spriteXPositions[i];
		if(spriteColumn >= 0 && spriteColumn <= 7) { //we hit this sprite
			col = (spriteBitmapData[i] >> (2 * spriteColumn)) & 3;
			if(col != 0) { //pixel is not transparent so we stop searching and return it
				return col;
			}
		}
	}
	return col;
}

int PPU::getPixelBackgroundColor()
{
	return 0;
}

void PPU::blitPixel()
{
	int x = cycle - 1;
	int y = scanLine;
	const int tempPalette[4] = { 0xFF, 0x00FF, 0x0000FF, 0x00FFFF };
	int spritePixel = getPixelSpriteColor();
	int backgroundPixel = getPixelBackgroundColor();

	pixels[x + y * (512)] = tempPalette[spritePixel];
	pixels[x + 256 + y * 512] = tempPalette[backgroundPixel];
}
