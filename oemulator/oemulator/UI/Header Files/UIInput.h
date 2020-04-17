#pragma once

namespace UI {
	typedef struct UIInput {
		int mouseX;
		int mouseY;
	} UIInput;

	/*const UIInput getInput() {
		static UIInput input{ 0,0 };
		return input;
	}*/

	extern UIInput input;
}

