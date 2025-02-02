#include "GameActiveState.h"


UI::GameActiveState::GameActiveState(UIFSM * UIFSM_, SDL_Renderer *renderer_, int *pixels_) : UIState(UIFSM_) 
{
	std::string pathToExe = std::experimental::filesystem::current_path().string();
	std::string pathToRecordingSymbol(pathToExe + "\\resources\\GUI\\RecordingSymbol.png"); //TODO this should be a global variable or atleast stored somewhere else
	SDL_Surface *s = IMG_Load(pathToRecordingSymbol.c_str());
	SDL_Texture *tex = SDL_CreateTextureFromSurface(renderer_, s);
	recordingSymbol = new Image(SDL_Rect{256-12,4,8,8}, tex);

	UIElements.push_back(new Button(SDL_Rect{96,0,64,8}, SDL_Color{ 0,0,0,0 }, 
		[this]() {
		this->FSM->changeState("ROMLibrary");
		}));
	pixels = pixels_;
}

void UI::GameActiveState::update(const Input::InputState& input, float d)
{
	//stop start recording
	if (input.isKeyPressed(SDLK_r) && !recordingKeyPressed) { //TODO, r for recording should not be hardcoded
		recordingKeyPressed = true; //cache key
		if (isRecording) {
			stopVideoRecording();
		}
		else {
			startVideoRecording();
		}
	}
	else if(!input.isKeyPressed(SDLK_r)) {
		recordingKeyPressed = false;
	}
	//if we are recording, push pixels to ffmpeg
	if (isRecording) {
		fwrite(pixels, 1, 256 * 240 * 4, videoOut);
	}

	for (auto e : UIElements) {
		e->update(input, d);
	}
}

void UI::GameActiveState::draw(SDL_Renderer * renderer, int scale)
{
	for (auto e : UIElements) {
		e->draw(renderer, scale);
	}
	if (isRecording) {
		recordingSymbol->draw(renderer, scale);
	}
}

void UI::GameActiveState::startVideoRecording()
{
	//							-hide_banner -loglevel panic are there to prevent ffmpeg from spamming the console
	std::string command("ffmpeg -hide_banner -loglevel panic -y -f  rawvideo -vcodec rawvideo -pix_fmt bgra -s 256x240 -r 60 -i - -vf scale=1024:-1 -sws_flags neighbor -f mp4 -q:v 10 -an -vcodec mpeg4 videos\\");
	//TODO: change this, this might technically cause conflicts since ticks starts at 0 when sdl inits
	//multiple runs of the emulator can overwrite videos
	command += std::to_string(SDL_GetTicks()) + ".mp4";
	videoOut = _popen(command.c_str(), "w");
	isRecording = true;
}

void UI::GameActiveState::stopVideoRecording()
{
	fflush(videoOut);
	_pclose(videoOut);
	isRecording = false;
}
