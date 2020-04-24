#include "NametableView.h"

NametableView::NametableView(const NES &nes_) : nes(nes_)
{
	window = SDL_CreateWindow("Nametableview", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
		width, height,
		SDL_WINDOW_SHOWN);

	nametableS = SDL_GetWindowSurface(window);
	
	draw();
}

void NametableView::draw()
{
	typedef unsigned char byte;
	byte *mem = nes.ppuMemory->memory;
	bool tableSide = (nes.ppu->CTRL & nes.ppu->CTRLBackgroundPattern) == nes.ppu->CTRLBackgroundPattern;

	int colors[4] = { 0x666666, 0xFFCCC5, 0x1412A7, 0xB53120 };
	for (int PTX = 0; PTX < 2; PTX++) {
		for (int PTY = 0; PTY < 2; PTY++) {
			for (int x = 0; x < numColumns; x++) {
				for (int y = 0; y < numRows; y++) {
					int nametableEntry = mem[0x2000 + x + y*numColumns + PTX*numColumns*numRows + PTY*numColumns*numRows*2];
					int attribEntryAddr = 0x1000 * tableSide + nametableEntry << 4;
					//iterate over the pattern
					for (int subY = 0; subY < 8; subY++) {
						byte patternRowTop = mem[attribEntryAddr + subY];
						byte patternRowBot = mem[attribEntryAddr + subY + 8];
						for (int subX = 0; subX < 8; subX++) {
							byte pixelValue = ((patternRowBot >> (subX - 1)) & 0b10)
								| ((patternRowTop >> (subX)) & 0b1);
							int colorValue = colors[pixelValue];

							Uint8 *targetPixel = (Uint8*)nametableS->pixels
								+ (y * 8 + subY)*nametableS->pitch
								+ (x * 8 + subX + PTX*256 + PTY*256*240*2) * 4;
							//std::cout << "X : " << x << " subX : " << subX << " Y : " << y << " subY : " << subY << std::endl;
							*(Uint32*)targetPixel = colorValue;
						}
					}
				}
			}
		}
	}

	SDL_UpdateWindowSurface(window);
}
