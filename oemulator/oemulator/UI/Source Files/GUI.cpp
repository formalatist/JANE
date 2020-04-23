#define SDL_MAIN_HANDLED
#include "GUI.h"
#include <SDL_image.h>

#undef main


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
const int SCALE = 4;


std::vector<ROMInfo> getROMInfos() {
	std::string pathToExe = std::experimental::filesystem::current_path().string();
	std::string pathToROMS(pathToExe + "\\roms");
	std::string pathToThumbnails(pathToExe + "\\resources\\");
	std::vector<ROMInfo> ROMInfos = std::vector<ROMInfo>();
	std::cout << pathToROMS << std::endl;
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
	SDL_Init(SDL_INIT_EVERYTHING);
	IMG_Init(IMG_INIT_PNG);
	
	//create the NES
	NES nes = NES();
	//create a controller
	Controller controller = Controller();
	nes.connectController(&controller); //connect it
	std::string keysFilePath = std::experimental::filesystem::current_path().string() + "\\keyBindings.txt";
	controller.setKeyMap(keysFilePath);
	//load a rom
	ROMLoader loader = ROMLoader(nes.memory, nes.ppuMemory);
	//create the screen
	Display display = Display("NES Emulator", 256, 240);
	nes.setDisplay(&display);
	display.setScale(SCALE);

	//create the GUI
	GUI gui = GUI(display.getRenderer(), 256 * SCALE, 240 * SCALE);
	gui.nes = nes;
	
	gui.ROMInfos = getROMInfos();

	int frame = 0;
	int frameTime = 0;
	bool run = true;
	SDL_Event event;
	byte input = 0;

	FSM<UI::UIState> *fsm = new FSM<UI::UIState>();

	UI::MainMenuState *mms = new UI::MainMenuState(fsm);
	UI::ROMLibraryState *rls = new UI::ROMLibraryState(fsm, gui.ROMInfos, nes, loader, display.getRenderer(), emulatorRunning);
	UI::GameActiveState *gas = new UI::GameActiveState(fsm, display.getRenderer(), nes.ppu->pixels);

	fsm->setTransitions({
		{ "MainMenu", mms },
		{ "ROMLibrary", rls},
		{ "Game", gas},
	});

	fsm->changeState("MainMenu");

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
				IO->keyboardState[event.key.keysym.sym] = true;
				if (event.key.keysym.sym == SDLK_q) {
					emulatorRunning = false;
				}
			}
			else if (event.type == SDL_KEYUP) {
				controller.onKeyUp(event.key.keysym.sym);
				IO->keyboardState[event.key.keysym.sym] = false;
			}
			else if (event.type == SDL_MOUSEBUTTONDOWN) {
				IO->LMBPressed = event.button.button == SDL_BUTTON_LEFT;
				IO->RMBPressed = event.button.button == SDL_BUTTON_RIGHT;
			}
			else if (event.type == SDL_MOUSEWHEEL) {
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
		if (emulatorRunning) {
			nes.stepSeconds(0.016667f);
		}
		
		//update and draw ui
		fsm->getState()->update(0.016f);
		fsm->getState()->draw(display.getRenderer(), SCALE);

		SDL_RenderPresent(display.getRenderer());

		duration = (clock() - duration) / ((double)CLOCKS_PER_SEC) * 1000;
		if (duration < 16.6667) {
			SDL_Delay(16.6667 - duration);
		}
		frame++;
	}
	IMG_Quit();
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
