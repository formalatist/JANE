#define SDL_MAIN_HANDLED
#include "GUI.h"
#include <SDL_image.h>
//#include "../../Util//FSM/FSM.hpp"

#undef main
//#include "ImGui/imgui_impl_sdl.h"
#include <stdio.h>


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
	std::string pathToROMS("C:/Users/Oivind/Documents/GitHub/oemulator/roms");
	std::string pathToThumbnails("C:/Users/Oivind/Documents/GitHub/oemulator/resources/");
	std::vector<ROMInfo> ROMInfos = std::vector<ROMInfo>();
	
	for (auto& p : std::experimental::filesystem::recursive_directory_iterator(pathToROMS))
	{
		if (p.path().extension() == ".nes") {
			ROMInfo info = {p.path().string(), pathToThumbnails + p.path().filename().replace_extension(".png").string() , p.path().filename().string()};
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
	
	//SDL_Surface *s = IMG_Load("C:\\Users\\Oivind\\Documents\\GitHub\\oemulator\\resources\\GUI\\MainMenuBackground.png");
	//SDL_Texture *tex = SDL_CreateTextureFromSurface(display.getRenderer(), s);

	//TEST of new UI
	FSM<UI::UIState> *fsm = new FSM<UI::UIState>();

	UI::MainMenuState *mms = new UI::MainMenuState(fsm);
	UI::ROMLibraryState *rls = new UI::ROMLibraryState(fsm, gui.ROMInfos, nes, loader, display.getRenderer());
	UI::GameActiveState *gas = new UI::GameActiveState(fsm);

	fsm->setTransitions({
		{ "MainMenu", mms },
		{ "ROMLibrary", rls},
		{ "Game", gas},
	});

	fsm->changeState("MainMenu");

	FILE *out = _popen("ffmpeg -y -f  rawvideo -vcodec rawvideo -pix_fmt bgra -s 256x240 -r 60 -i - -vf scale=1024:-1 -sws_flags neighbor -f mp4 -q:v 10 -an -vcodec mpeg4 qual10test2.mp4", "w");

	char* dir;
	while (run) {

		auto IO = &UI::input;
		IO->LMBPressed = false;
		IO->RMBPressed = false;
		IO->scrollwheelY = 0;
		while (SDL_PollEvent(&event)) {
			if (event.type == SDL_QUIT) {
				run = false;
			}
			else if (event.type == SDL_KEYDOWN) {
				controller.onKeyDown(event.key.keysym.sym);
				if (event.key.keysym.sym == SDLK_q) {
					//emulatorRunning = false;
					run = false;
				}
			}
			else if (event.type == SDL_KEYUP) {
				controller.onKeyUp(event.key.keysym.sym);
			}
			else if (event.type == SDL_MOUSEBUTTONDOWN) {
				IO->LMBPressed = event.button.button == SDL_BUTTON_LEFT;
				IO->RMBPressed = event.button.button == SDL_BUTTON_RIGHT;
			}
			else if (event.type == SDL_MOUSEWHEEL)
			{
				//TODO: handle mousewheel
				IO->scrollwheelY = event.wheel.y;
			}
			else if (event.type == SDL_DROPFILE) { // a file was droppen on the window
				dir = event.drop.file;
				SDL_free(dir);
			}
		}

		//update mouseposition and LMB and RMB
		int buttons = SDL_GetMouseState(&(IO->mouseX), &(IO->mouseY));
		IO->mouseX /= SCALE;
		IO->mouseY /= SCALE;
		IO->LMBDown = (buttons & (SDL_BUTTON(SDL_BUTTON_LEFT))) == SDL_BUTTON(SDL_BUTTON_LEFT);
		IO->RMBDown = (buttons & (SDL_BUTTON(SDL_BUTTON_RIGHT))) == SDL_BUTTON(SDL_BUTTON_RIGHT);

		double duration = clock();
		if (emulatorRunning | true) {
			nes.stepSeconds(0.016667f);
		}
		
		//gui.draw();
		//TEST for new ui system
		fsm->getState()->update(0.016f);
		fsm->getState()->draw(display.getRenderer(), SCALE);

		SDL_RenderPresent(display.getRenderer());

		fwrite(nes.ppu->pixels, 1, 256 * 240*4, out);

		duration = (clock() - duration) / ((double)CLOCKS_PER_SEC) * 1000;
		if (duration < 16.6667) {
			SDL_Delay(16.6667 - duration);
		}
		frame++;
	}
	fflush(out);
	_pclose(out);

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
