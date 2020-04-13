#define SDL_MAIN_HANDLED
#include "GUI.h"
#include <SDL_image.h>

#undef main
//#include "ImGui/imgui_impl_sdl.h"

GUI::GUI(SDL_Renderer * renderer_, int width_, int height_)
{
	renderer = renderer_;
	width = width_;
	height = height_;

	ImGui::CreateContext();
	ImGuiSDL::Initialize(renderer, width, height);
}

void GUI::draw()
{
	ImGui::NewFrame();

	//ImGui::ShowDemoWindow();
	showMainMenu();

	ImGui::Render();
	ImGuiSDL::Render(ImGui::GetDrawData());

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

	char* dir;
	while (run) {
		ImGuiIO& io = ImGui::GetIO();

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
				io.MouseWheel = static_cast<float>(event.wheel.y);
			}
			else if (event.type == SDL_DROPFILE) { // a file was droppen on the window
				dir = event.drop.file;
				SDL_free(dir);
			}
		}

		int mouseX;
		int mouseY;
		const int buttons = SDL_GetMouseState(&mouseX, &mouseY);
		io.DeltaTime = 0.016f;
		io.MousePos = ImVec2(static_cast<float>(mouseX), static_cast<float>(mouseY));
		io.MouseDown[0] = buttons & SDL_BUTTON(SDL_BUTTON_LEFT);
		io.MouseDown[1] = buttons & SDL_BUTTON(SDL_BUTTON_RIGHT);
		

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
	if (emulatorRunning) {
		return;
	}

	static bool rebindKeysVisible = false;
	//SDL_RenderF
	if (rebindKeysVisible) {
		//showRebindKeys();
	}

	ImGui::SetNextWindowPos(ImVec2(0, 0));
	ImGui::SetNextWindowSize(ImVec2(width, height));

	if (!ImGui::Begin("ROM Select", NULL, ImGuiWindowFlags_MenuBar
											| ImGuiWindowFlags_NoCollapse)) {
		ImGui::End();
		return;
	}

	if (ImGui::BeginMenuBar()) {
		if (ImGui::BeginMenu("Menu")) {

			ImGui::MenuItem("Rebind keys", NULL, &rebindKeysVisible);
			ImGui::EndMenu();
		}
		if (ImGui::BeginMenu("Tools")) {
			ImGui::MenuItem("Test", NULL);
			ImGui::EndMenu();
		}
		ImGui::EndMenuBar();
	}

	ImGui::Columns(4, NULL);

	//SDL_Texture *texture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_BGRA32, SDL_TEXTUREACCESS_TARGET, width, height);

	int numROMS = ROMInfos.size();
	for (int i = 0; i < numROMS; i++) {
		if (i % 4 == 0) {
			ImGui::Separator();
		}
		if (ImGui::Button(std::to_string(i).c_str(), ImVec2(width / 4, height / 4))) {
			emulatorRunning = true;
			ROMLoader l = ROMLoader(nes.memory, nes.ppuMemory);
			l.loadROMFromFile(ROMInfos[i].ROMPath, *(nes.cpu));
			std::cout << ROMInfos[i].ROMName << std::endl;
		}
		ImGui::Text(ROMInfos[i].ROMName.c_str());
		ImGui::NextColumn();
	}

	ImGui::Text("This is a text");
	ImGui::End();
}

void GUI::showRebindKeys()
{
	ImGui::SetNextWindowPos(ImVec2(0, 0));
	ImGui::SetNextWindowSize(ImVec2(width, height));

	
}
