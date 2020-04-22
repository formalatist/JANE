#pragma once
#include <stdio.h>

#include "UIState.h"
#include "UIElement.h"
#include "Button.h"

namespace UI {
	class GameActiveState : public UIState {
	public:
		GameActiveState(UIFSM *UIFSM_, int *pixels_);

		void update(float d) override;
		void draw(SDL_Renderer *renderer, int scale) override;

	private:
		//ffmpeg outputstream when we are recording a video
		FILE *videoOut;
		bool isRecording = false;
		//pointer to the pixels shown on screen, this is needed for recording
		int *pixels;
		//cache of the key used for recording, to prevent recording from rapidly
		//starting and stopping when a key is held for more than a frame
		bool recordingKeyPressed = false;
		
		void startVideoRecording();
		void stopVideoRecording();
	};
}