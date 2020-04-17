#define SDL_MAIN_HANDLED
#include "GUI.h"
#include <SDL_image.h>
#include "../../Util//FSM/FSM.h"

#undef main
//#include "ImGui/imgui_impl_sdl.h"

GUI::GUI(SDL_Renderer * renderer_, int width_, int height_)
{
	renderer = renderer_;
	width = width_;
	height = height_;
}

void GUI::draw()
{
	SDL_RenderPresent(renderer);
}



bool emulatorRunning = false;
const int FPS = 30;
int tickCounter = 0;
const int SCALE = 4;


std::vector<ROMInfo> getROMInfos() {
	std::string path("C:/Users/Oivind/Documents/GitHub/oemulator/roms");
	std::vector<ROMInfo> ROMInfos = std::vector<ROMInfo>();
	
	for (auto& p : std::experimental::filesystem::recursive_directory_iterator(path))
	{
		if (p.path().extension() == ".nes") {
			ROMInfo info = {p.path().string(), std::string(""), p.path().filename().string()};
			ROMInfos.push_back(info);
		}
	}
	return ROMInfos;
}


int main(int argc, char** argv) {

	std::string game = "Super mario bros";
	std::string filePath = "C:\\Users\\Oivind\\Documents\\GitHub\\oemulator\\roms\\" + game + ".nes";


	SDL_Init(SDL_INIT_EVERYTHING);
	IMG_Init(IMG_INIT_PNG);
	
	//create the NES
	NES nes = NES();
	//create a controller
	Controller controller = Controller();
	nes.connectController(&controller); //connect it
	std::string keysFilePath = "C:\\Users\\Oivind\\Documents\\GitHub\\oemulator\\keyBindingsTest.txt";
	controller.setKeyMap(keysFilePath);
	//load a rom
	ROMLoader loader = ROMLoader(nes.memory, nes.ppuMemory);
	loader.loadROMFromFile("C:\\Users\\Oivind\\Documents\\GitHub\\oemulator\\roms\\super mario bros.nes", (*nes.cpu));
	//loader.loadROM(fileBuffer, fileSize, (*nes.cpu));
	//create the screen
	Display display = Display("NES Emulator", 256, 240);
	nes.setDisplay(&display);
	display.setScale(SCALE);

	//create the GUI
	GUI gui = GUI(display.getRenderer(), 256 * SCALE, 240 * SCALE);
	gui.nes = nes;
	
	gui.ROMInfos = getROMInfos();

	tickCounter = SDL_GetTicks();
	int frame = 0;
	int frameTime = 0;
	bool run = true;
	SDL_Event event;
	byte input = 0;

	SDL_Surface *s = IMG_Load("C:\\Users\\Oivind\\Documents\\GitHub\\oemulator\\resources\\GUI\\MainMenuBackground.png");
	SDL_Texture *tex = SDL_CreateTextureFromSurface(display.getRenderer(), s);

	//TEST of new UI
	FSM<UI::UIState> *fsm = new FSM<UI::UIState>(std::map<std::string, UI::UIState>{
		{"MainMenu", UI::MainMenuState(fsm)}
	});



	char* dir;
	while (run) {

		while (SDL_PollEvent(&event)) {
			if (event.type == SDL_QUIT) {
				run = false;
			}
			else if (event.type == SDL_KEYDOWN) {
				controller.onKeyDown(event.key.keysym.sym);
				if (event.key.keysym.sym == SDLK_q) {
					emulatorRunning = false;
				}
			}
			else if (event.type == SDL_KEYUP) {
				controller.onKeyUp(event.key.keysym.sym);
			}
			else if (event.type == SDL_MOUSEWHEEL)
			{
				//TODO: handle mousewheel
			}
			else if (event.type == SDL_DROPFILE) { // a file was droppen on the window
				dir = event.drop.file;
				SDL_free(dir);
			}
		}

		double duration = clock();
		if (emulatorRunning | true) {
			nes.stepSeconds(0.016667f);
		}

		//gui.draw();
		SDL_RenderCopy(display.getRenderer(), tex, NULL, NULL);
		SDL_RenderPresent(display.getRenderer());

		duration = (clock() - duration) / ((double)CLOCKS_PER_SEC) * 1000;
		if (duration < 16.6667) {
			SDL_Delay(16.6667 - duration);
		}
		frame++;
	}

	SDL_Quit();
	std::cin.get();

	return 0;
}

void GUI::showMainMenu()
{
}

void GUI::showRebindKeys()
{
	
}