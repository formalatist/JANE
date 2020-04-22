#include "GameActiveState.h"

#include <iostream>

UI::GameActiveState::GameActiveState(UIFSM * UIFSM_, int *pixels_) : UIState(UIFSM_) 
{
	UIElements.push_back(new Button(SDL_Rect{96,0,64,8}, SDL_Color{ 0,0,0,0 }, 
		[this]() {
		this->FSM->changeState("ROMLibrary");
		}));
	pixels = pixels_;
}

void UI::GameActiveState::update(float d)
{
	auto &IO = input;
	//stop start recording
	if (IO.keyboardState[SDLK_r] && !recordingKeyPressed) { //TODO, r for recording should not be hardcoded
		recordingKeyPressed = true; //cache key
		if (isRecording) {
			stopVideoRecording();
		}
		else {
			startVideoRecording();
		}
	}
	else if(!IO.keyboardState[SDLK_r]) {
		recordingKeyPressed = false;
	}
	//if we are recording, push pixels to ffmpeg
	if (isRecording) {
		fwrite(pixels, 1, 256 * 240 * 4, videoOut);
	}

	for (auto e : UIElements) {
		e->update(d);
	}
}

void UI::GameActiveState::draw(SDL_Renderer * renderer, int scale)
{
	for (auto e : UIElements) {
		e->draw(renderer, scale);
	}
}

void UI::GameActiveState::startVideoRecording()
{
	std::string command("ffmpeg -hide_banner -loglevel panic -y -f  rawvideo -vcodec rawvideo -pix_fmt bgra -s 256x240 -r 60 -i - -vf scale=1024:-1 -sws_flags neighbor -f mp4 -q:v 10 -an -vcodec mpeg4 ");
	command += "testFilename.mp4";
	videoOut = _popen(command.c_str(), "w");
	isRecording = true;
	std::cout << "STARTED RECORDING" << std::endl;
}

void UI::GameActiveState::stopVideoRecording()
{
	fflush(videoOut);
	_pclose(videoOut);
	isRecording = false;
	std::cout << "STOPPED RECORDING" << std::endl;
}
