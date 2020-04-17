#pragma once
#include <string>

namespace UI {
	/*TODO: add SDL_TTF to project to make this happen
	using gradients we can make interesting font renderings
	this will be needed since we cant hardcode text as .png
	because some of the menu text will be dynamic, such as
	when rebinding keys*/
	class TextBox {

	public:
		TextBox(std::string text);
	};
}