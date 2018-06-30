#include "PPU.h"

PPU::PPU(PPUMemory* memory_)
{
	memory = memory_;
}

void PPU::step()
{
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

	}
}
