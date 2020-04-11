#define _CRT_SECURE_NO_DEPRECATE
#define SDL_MAIN_HANDLED
#include "GUI.h"
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


long getFileSize(FILE *file)
{
	long lCurPos, lEndPos;
	lCurPos = ftell(file);
	fseek(file, 0, 2);
	lEndPos = ftell(file);
	fseek(file, lCurPos, 0);
	return lEndPos;
}

ROMInfo *getROMInfos() {
	std::string path("C:/Users/Oivind/Documents/GitHub/oemulator/roms");
	std::string ext(".nes");
	
	for (auto& p : std::experimental::filesystem::recursive_directory_iterator(path))
	{
		if (p.path().extension() == ext) {
			std::cout << p << '\n';
		}
	}
	return 0;
}


int main(int argc, char** argv) {

	std::string game = "Super mario bros";
	std::string filePath = "C:\\Users\\Oivind\\Documents\\GitHub\\oemulator\\roms\\" + game + ".nes";

	unsigned char* fileBuffer;
	FILE* file = NULL;

	if ((file = fopen(filePath.c_str(), "rb")) == NULL) {
		std::cout << "Could not open file at path: " << filePath << std::endl;
		std::cin.get();
	}
	long fileSize = getFileSize(file);
	fileBuffer = new unsigned char[fileSize];
	fread(fileBuffer, fileSize, 1, file);
	fclose(file);

	//parse the iNES header
	std::cout << fileBuffer[0] << fileBuffer[1] << fileBuffer[2] << std::endl;
	std::cout << "Size of PRG ROM: " << static_cast<unsigned>(fileBuffer[4])
		<< "*16KB = " << static_cast<unsigned>(fileBuffer[4]) * 16 << "KB" << std::endl;
	std::cout << "Size of CHR ROM: " << static_cast<unsigned>(fileBuffer[5])
		<< "*8KB = " << static_cast<unsigned>(fileBuffer[5]) * 8 << "KB" << std::endl;
	std::cout << "Size of PRG RAM: " << static_cast<unsigned>(fileBuffer[8])
		<< "*8KB = " << static_cast<unsigned>(fileBuffer[8]) * 8 << "KB" << std::endl;



	std::cout << (static_cast<int>(fileBuffer[7]) & 0x4) <<
		(static_cast<int>(fileBuffer[7]) & 0x8) << std::endl;


	SDL_Init(SDL_INIT_EVERYTHING);
	//create the NES
	NES nes = NES();
	//create a controller
	Controller controller = Controller();
	nes.connectController(&controller); //connect it
	std::string keysFilePath = "C:\\Users\\Oivind\\Documents\\GitHub\\oemulator\\keyBindingsTest.txt";
	controller.setKeyMap(keysFilePath);
	//load a rom
	ROMLoader loader = ROMLoader(nes.memory, nes.ppuMemory);
	loader.loadROM(fileBuffer, fileSize, (*nes.cpu));
	//create the screen
	Display display = Display("NES Emulator", 256, 240);
	nes.setDisplay(&display);
	display.setScale(SCALE);

	//create the GUI
	GUI gui = GUI(display.getRenderer(), 256 * SCALE, 240 * SCALE);

	getROMInfos();

	tickCounter = SDL_GetTicks();
	int frame = 0;
	int frameTime = 0;
	bool run = true;
	SDL_Event event;
	byte input = 0;

	char* dir;
	while (run) {
		while (SDL_PollEvent(&event)) {
			if (event.type == SDL_QUIT) {
				run = false;
			}
			else if (event.type == SDL_KEYDOWN) {
				controller.onKeyDown(event.key.keysym.sym);
			}
			else if (event.type == SDL_KEYUP) {
				controller.onKeyUp(event.key.keysym.sym);
			}
			else if (event.type == SDL_DROPFILE) { // a file was droppen on the window
				dir = event.drop.file;
				SDL_free(dir);
			}
		}

		int mouseX;
		int mouseY;
		const int buttons = SDL_GetMouseState(&mouseX, &mouseY);
		ImGuiIO& io = ImGui::GetIO();
		io.DeltaTime = 0.016f;
		io.MousePos = ImVec2(static_cast<float>(mouseX), static_cast<float>(mouseY));
		io.MouseDown[0] = buttons & SDL_BUTTON(SDL_BUTTON_LEFT);
		io.MouseDown[1] = buttons & SDL_BUTTON(SDL_BUTTON_RIGHT);
		

		double duration = clock();
		if (emulatorRunning) {
			nes.stepSeconds(0.016667f);
		}
		gui.draw();
		duration = (clock() - duration) / ((double)CLOCKS_PER_SEC) * 1000;
		if (duration < 16.6667) {
			SDL_Delay(16.6667 - duration);
		}
		frame++;
	}

	SDL_Quit();
	std::cin.get();

	delete[] fileBuffer;
	return 0;
}

void GUI::showMainMenu()
{
	ImGui::SetNextWindowPos(ImVec2(0, 0));
	ImGui::SetNextWindowSize(ImVec2(width, height));
	if (!ImGui::Begin("Main menu")) {
		ImGui::End();
		return;
	}

	ImGui::Columns(4, NULL);

	char * text = "test0";
	for (int i = 0; i < 16; i++) {
		if (i % 4 == 0) {
			ImGui::Separator();
		}
		ImGui::Button("Button", ImVec2(width/4, height/4));
		ImGui::Text(text);
		ImGui::NextColumn();
	}

	ImGui::Text("This is a text");
	ImGui::End();
}
