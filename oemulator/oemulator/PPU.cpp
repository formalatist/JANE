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

	bool isRendering = (MASK & (MASKShowBackground | MASKShowSprites))
		== (MASKShowBackground | MASKShowSprites);
	bool isPreLine = scanLine == 261;
	bool isVisibleLine = scanLine < 240; //lines are the Y of the pixel
	bool isRenderedLine = isPreLine || isVisibleLine;
	bool isPrefetchCycle = cycle >= 321 && cycle <= 336;
	bool isVisibleCycle = cycle >= 1 && cycle <= 256; //cycles gives the X
	bool isFetchCycle = isPrefetchCycle || isVisibleCycle;

	if (isRendering) {
		if (isVisibleLine && isVisibleCycle) { //Both X and Y are within their visible areas
			blitPixel();
		}
		if (isRenderedLine && isFetchCycle) {
			tileBitmap <<= 4;
			switch (cycle % 8) {
			case 1:
				setNametableByte();
				break;
			case 3:
				setAttributeTableByte();
				break;
			case 5:
				setTileBitmapLow();
				break;
			case 7:
				setTileBitmapHigh();
				break;
			case 0:
				createTileBitmap();
				break;
				//std::cout << "Created tileBitmap: " << tileBitmap << std::endl;
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
			if (isFetchCycle && (cycle % 8 == 0)) {
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
		enterVerticalBlank();
	}
	if (isPreLine && cycle == 1) {
		leaveVerticalBlank();
		STATUS &= ~STATUSSpriteZeroHit;
		STATUS &= ~STATUSSpriteOverflow;
	}
}

void PPU::advanceCounters()
{
	bool showSprites = (MASK & MASKShowSprites) == MASKShowSprites;
	bool showBackground = (MASK & MASKShowBackground) == MASKShowBackground;
	
	/*
	if (((CTRL & CTRLNMI) == CTRLNMI) 
		&& ((STATUS & STATUSVBlankStarted) == STATUSVBlankStarted)
		&& !nmiSent) {
		memory->cpu->triggerNMI();
		nmiSent = true;
		std::cout << "NMI CREATED#####################" << std::endl;
	}*/
	//generate NMI
	if (nmiDelay > 0) {
		nmiDelay--;
		if (nmiDelay == 0 && ((CTRL & CTRLNMI) == CTRLNMI)
			&& ((STATUS & STATUSVBlankStarted) == STATUSVBlankStarted)) {
			memory->cpu->triggerNMI();
			//std::cout << "NMI CREATED#####################" << std::endl;
		}
	}

	if(showBackground || showSprites) { //if we are rendering anything
		if (isOddFrame && scanLine == 261 && cycle == 339) {
			cycle = 0;
			scanLine = 0;
			isOddFrame = !isOddFrame;
			return;
		}
	}

	cycle++;
	if (cycle > 340) { //we are done with this scanline so we go to the next
		cycle = 0;
		scanLine++;
		if(scanLine > 261) { //we are done with the screen
			scanLine = 0;
			isOddFrame = !isOddFrame;
		}
	}
}

byte PPU::readRegister(int addr)	
{
	if (addr == 0x2002) { // PPUSTATUS
		writeToggle = false;
		byte val = STATUS;
		val = val | (registerBuffer & 0x1F);
		STATUS = (STATUS & ~STATUSVBlankStarted);
		NMIChange();
		return val;
	} else if (addr == 0x2004) { // OAMDATA
		return OAM[OAMADDR];
	} else if (addr == 0x2007) { // PPUDATA
		byte val = memory->read(v);
		/*When reading while the VRAM address is in the range 0-$3EFF 
		(i.e., before the palettes), the read will return the contents of an 
		internal read buffer. This internal buffer is updated only when reading PPUDATA, 
		and so is preserved across frames*/
		if((v%0x4000) < 0x3F00) { // we need to do a buffered read (see above)
			byte oldDATABuffer = DATABuffer;
			DATABuffer = val;
			val = oldDATABuffer;
		} else {
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
	//std::cout << "#####WRITE TO REGISTER: " << addr << std::endl;
	if (addr == 0x2000) { //PPUCTRL register
		//sets the PPUCTRL register to val
		CTRL = val;
		//In the following, d refers to the data written to the port, and A through H 
		//to individual bits of a value.
		//t: ...BA.. ........ = d: ......BA
		//clear bit 10 and 11 of t and OR t with bit 0 and 1 of val bitshifted 10 to the left
		t = (t & 0xF3FF) | ((val & 0x03) << 10);
		NMIChange();
	} else if (addr == 0x2001) { // PPUMASK
		MASK = val;
		//std::cout << "Write to MASK: " << (int)MASK << "  val:  " << (int)val << std::endl;
	} else if (addr == 0x2002) { // PPUSTATUS read only
		//std::cout << "ERROR: Writing to 0x2002 PPUSTATUS. Val: " << val << std::endl;
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
			t = (t & 0xFFE0) | (val >> 3);
			x = val & 7;
			//std::cout << "Write to  0x2005: val: " << (int)val << "  x: " << (int)x << std::endl;
			writeToggle = true;
		}
		else {
			/*	t: CBA..HG FED..... = d: HGFEDCBA
				w:                  = 0*/
			//t = (t & 0xC1F) | ((val & 7) << 12) | ((val & 0xF8) << 2);
			t = (t & 0x8FFF) | ((val & 7) << 12);
			t = (t & 0xFC1F) | ((val & 0xF8) << 2);
			writeToggle = false;
		}
	} else if (addr == 0x2006) { // PPUADDR
		if (!writeToggle) {
			/*	t: .FEDCBA ........ = d: ..FEDCBA
				t: X...... ........ = 0
   (writeToggle)w:                  = 1*/
			t = (t & 0x80FF) | ((val & 0x3F) << 8);
			writeToggle = true;
		}
		else {
			/*	t: ....... HGFEDCBA = d: HGFEDCBA
				v                   = t
   (writeToggle)w:                  = 0*/
			t = (t & 0xFF00) | val;
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
		uint16_t cpuMemoryAddress = val << 8;
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
		//std::cout << "ERROR: Tried writing to PPU register " << addr << std::endl;
	}
}

void PPU::spriteEvaluation()
{
	/*
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
				//std::cout << "Sprite eval: " << (int)spriteBitmapData[numberOfSpritesOnScanline] << std::endl;
			}
			numberOfSpritesOnScanline++;
		}
	}*/

	int spriteHeight;
	if ((CTRL & CTRLSpriteSize) == CTRLSpriteSize) {
		spriteHeight = 16;
	}
	else {
		spriteHeight = 8;
	}
	numberOfSpritesOnScanline = 0;
	for (int i = 0; i < 64; i++) {
		byte y = OAM[i * 4];
		byte a = OAM[i * 4 + 2];
		byte xPos = OAM[i * 4 + 3];
		int row = scanLine - y;
		if (row < 0 || row >= spriteHeight) {
			continue;
		}
		if (numberOfSpritesOnScanline < 8) {
			spritePatterns[numberOfSpritesOnScanline] = getSpriteBitmapData(i, row);
			spritePositions[numberOfSpritesOnScanline] = xPos;
			spritePriorities[numberOfSpritesOnScanline] = (a >> 5) & 1;
			spriteIndexes[numberOfSpritesOnScanline] = i;
		}
		numberOfSpritesOnScanline++;
	}

	//set sprite overflow flag
	if (numberOfSpritesOnScanline > 8) {
		numberOfSpritesOnScanline = 8;
		STATUS = STATUS | STATUSSpriteOverflow;
	}
	//std::cout << "SPRITES ON SCANLINE: " << (int)numberOfSpritesOnScanline << std::endl;
}

void PPU::NMIChange()
{
	bool nmi = ((CTRL & CTRLNMI) == CTRLNMI)
		&& ((STATUS & STATUSVBlankStarted) == STATUSVBlankStarted);
	if (nmi && !nmiSent) {
		nmiDelay = 15;
	}
	nmiSent = nmi;
}

void PPU::enterVerticalBlank()
{
	nmiSent = false;
	STATUS = STATUS | STATUSVBlankStarted;
	NMIChange();
	nes->updateScreen();
	//NMIOccured = true;
}

void PPU::leaveVerticalBlank()
{
	STATUS = STATUS & ~STATUSVBlankStarted;
	NMIChange();
	//NMIOccured = false;
}

uint32_t PPU::getSpriteBitmapData(byte index, byte row)
{
	/*
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
		spriteBitmapData = spriteBitmapData | (highBit << 1) | lowBit;
	}
	//std::cout << "SpriteBitmapData: " << (int)spriteBitmapData << std::endl;
	return spriteBitmapData;*/
	byte tile = OAM[index * 4 + 1];
	byte attributes = OAM[index * 4 + 2];
	int addr;
	if ((CTRL & CTRLSpriteSize) != CTRLSpriteSize) {
		if ((attributes & 0x80) == 0x80) {
			row = 7 - row;
		}
		int table = (CTRL & CTRLSpritePattern) == CTRLSpritePattern;
		addr = 0x1000 * table + tile * 16 + row;
	}
	else {
		if ((attributes & 0x80) == 0x80) {
			row = 15 - row;
		}
		int table = tile & 1;
		tile &= 0xFE;
		if (row > 7) {
			tile++;
			row -= 8;
		}
		addr = 0x1000 * table + tile * 16 + row;
	}
	byte a = (attributes & 3) << 2;
	byte lowSpriteByte = memory->read(addr);
	byte highSpriteByte = memory->read(addr + 8);
	uint32_t data = 0;
	for (int i = 0; i < 8; i++) {
		byte p1 = 0;
		byte p2 = 0;
		if ((attributes & 0x40) == 0x40) {
			p1 = (lowSpriteByte & 1) << 0;
			p2 = (highSpriteByte & 1) << 1;
			lowSpriteByte >>= 1;
			highSpriteByte >>= 1;
		}
		else {
			p1 = (lowSpriteByte & 0x80) >> 7;
			p2 = (highSpriteByte & 0x80) >> 6;
			lowSpriteByte <<= 1;
			highSpriteByte <<= 1;
		}
		data <<= 4;
		data |= (uint32_t)(a | p1 | p2);
	}
	return data;
}

void PPU::setNametableByte()
{
	nameTableByte = memory->read(0x2000 | (v & 0x0FFF));
}

void PPU::setAttributeTableByte()
{
	int addr = 0x23C0 | (v & 0x0C00) | ((v >> 4) & 0x38) | ((v >> 2) & 0x07);
	int shift = ((v >> 4) & 4) | (v & 2);
	attributeTableByte = ((memory->read(addr) >> shift) & 3) << 2;
}

void PPU::setTileBitmapLow()
{
	//get the fine Y scroll from V. This is in the 3 high bits. v is 15 bit
	int fineYScroll = (v >> 12) & 7;
	//if backgroundpattern bit is 1 we add 0x1000 to the address
	int table = (CTRL & CTRLBackgroundPattern) == CTRLBackgroundPattern;
	uint16_t tile = nameTableByte;
	uint16_t addr = 0x1000*table + tile * 16 + fineYScroll;
	tileBitmapLow = memory->read(addr);
	//std::cout << "bitmapLow addr: " << (int)addr << std::endl;
	//std::cout << "TileBitmapLow: " << (int)tileBitmapLow << std::endl;
}

//high byte is 8 bytes further in memory
void PPU::setTileBitmapHigh()
{
	//get the fine Y scroll from V. This is in the 3 high bits. v is 15 bit
	int fineYScroll = (v >> 12) & 7;
	int table = (CTRL & CTRLBackgroundPattern) == CTRLBackgroundPattern;
	uint16_t tile = nameTableByte;
	//if backgroundpattern bit is 1 we add 0x1000 to the address
	uint16_t addr = 0x1000 * table + tile * 16 + fineYScroll;
	tileBitmapHigh = memory->read(addr+8);
	//std::cout << "bitmapHigh addr: " << (int)addr << std::endl;
	//std::cout << "TileBitmapHigh: " << (int)tileBitmapHigh << std::endl;
}

void PPU::createTileBitmap()
{
	/*
	int low = tileBitmapLow;
	int high = tileBitmapHigh;
	int newTileBitmap = 0;
	for (int i = 0; i < 8; i++) {
		newTileBitmap <<= 2;
		newTileBitmap = newTileBitmap | (((tileBitmapLow & 0x80) >> 7)
			| ((tileBitmapHigh & 0x80) >> 6));
		arrBM[i] = (((tileBitmapLow & 0x80) >> 7)
			| ((tileBitmapHigh & 0x80) >> 6));
		tileBitmapLow <<= 1;
		tileBitmapHigh <<= 1;
	}
	tileBitmap = newTileBitmap;
	//std::cout << "created tileBitmap: " << (int)tileBitmap << "   low: " << (int)low << "   high: "
	//	<< (int)high << std::endl;*/
	uint32_t newData = 0;
	for (int i = 0; i < 8; i++) {
		byte a = attributeTableByte;
		byte low = (tileBitmapLow & 0x80) >> 7;
		byte high = (tileBitmapHigh & 0x80) >> 6;
		tileBitmapLow <<= 1;
		tileBitmapHigh <<= 1;
		newData <<= 4;
		newData |= (int)(a | low | high);
	}
	tileBitmap |= (uint64_t)newData;
}

void PPU::copyVerticalBits()
{
	//v: IHGF.ED CBA..... = t: IHGF.ED CBA.....
	v = (v & 0x841F) | (t & 0x7BE0);
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
		v &= 0x8FFF;// fine Y = 0
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
		}
		v = (v & 0xFC1F) | (y << 5);// put coarse Y back into v
	}
}

void PPU::copyHorizontalBits()
{
	//v: ....F.. ...EDCBA = t: ....F.. ...EDCBA
	v = (v & 0xFBE0) | (t & 0x41F);
}

int PPU::indexOfCurrentSprite()
{
	if ((MASK & MASKShowSprites) != MASKShowSprites) {
		return 0;
	}
	for (int i = 0; i < numberOfSpritesOnScanline; i++) {
		byte offset = (cycle - 1) - spritePositions[i];
		if (offset < 0 || offset > 7) {
			continue;
		}
		offset = 7 - offset;
		int color = (spritePatterns[i] >> (offset * 4)) & 0x0F;
		if (color % 4 == 0) {
			continue;
		}
		return i;
	}
	return 0;
}

int PPU::getPixelSpriteColor()
{ 
	/*	
	int col = 0;
	//loop over the sprites on the scanline and check if any of them are on this pixel
	for (int i = 0; i < numberOfSpritesOnScanline; i++) {
		int spriteColumn = cycle - 1 - spriteXPositions[i];
		if(spriteColumn >= 0 && spriteColumn <= 7) { //we hit this sprite
			col = (spriteBitmapData[i] >> (2 * spriteColumn)) & 3;
			//std::cout << "data: " << (int)spriteBitmapData[i] << "  spriteColumn: " 
				//<< spriteColumn << "  col: " << (int)col<< "   val: " <<
				//(int)((spriteBitmapData[i]) >> (2 * spriteColumn)) << std::endl;
			if(col != 0) { //pixel is not transparent so we stop searching and return it
				return col;
			}
		}
	}
	return col;*/
	if ((MASK & MASKShowSprites) != MASKShowSprites) {
		return 0;
	}
	for (int i = 0; i < numberOfSpritesOnScanline; i++) {
		int offset = (cycle - 1) - spritePositions[i];
		if ((offset < 0) || (offset > 7)) {
			continue;
		}
		offset = 7 - offset;
		int color = (spritePatterns[i] >> (offset * 4)) & 0x0F;
		if (color % 4 == 0) {
			continue;
		}
		return color;
	}
	return 0;
}

int PPU::getPixelBackgroundColor()
{
	//return (tileBitmap >> (2*(x&0b111))) & 0xf;
	//std::cout << "X pos: " << (int)x << "  arrBM: " << arrBM[x & 0b111] << std::endl;
	//return arrBM[x & 0b111];
	if ((MASK & MASKShowBackground) != MASKShowBackground) {
		return 0;
	}
	return ((tileBitmap >> 32) >> ((7 - x) * 4)) & 0x0F;
}

void PPU::blitPixel()
{
	int xPos = cycle - 1;
	int yPos = scanLine;
	int spritePixel = getPixelSpriteColor();
	int spriteIndex = indexOfCurrentSprite();
	int backgroundPixel = getPixelBackgroundColor();
	byte color = 0;
	if (x < 8 && ((MASK & MASKShowLeftBackground) != MASKShowLeftBackground)) {
		backgroundPixel = 0;
	}
	if (x < 8 && ((MASK & MASKShowLeftSprites) != MASKShowLeftSprites)) {
		spritePixel = 0;
	}

	bool b = backgroundPixel % 4 != 0;
	bool s = spritePixel % 4 != 0;
	if (!b && !s) {
		color = 0;
	}
	else if (!b && s) {
		color = spritePixel | 0x10;
	}
	else if (b && !s) {
		color = backgroundPixel;
	}
	else {
		if ((spriteIndexes[spriteIndex] == 0) && x < 255) {
			STATUS |= STATUSSpriteZeroHit;
		}
		if (spritePriorities[spriteIndex] == 0) {
			color = spritePixel | 0x10;
		}
		else {
			color = backgroundPixel;
		}
	}

	pixels[xPos + yPos * (512)] = PaletteLookup[memory->read((spritePixel | 0x10)+0x3F00)];
	
	pixels[xPos + 256 + yPos * 512] = PaletteLookup[memory->read(color + 0x3F00)];
}
