#pragma once

#include "Panel.hpp"
#include "Font.hpp"

namespace olcLevelEditor {
	namespace ui {
		
		class TextInput {
		private:
			Panel * panel;
			bool panelIsMine = false;
			Font* font = nullptr;
			int x, y, width;
			bool focused = false;
			std::string str = "";

			std::string strNumbers = "0123456789";
			std::string strCharacters = "QWERTYUIOPASDFGHJKLZXCVBNM";

			bool numbers = true;
			bool characters = true;
			bool special = true;

		public:
			TextInput(int x, int y, int width, Panel* panel = nullptr, Font* font = nullptr);
			~TextInput();

			void Draw();

			inline void AllowCharacters(bool allow) { this->characters = allow; }
			inline void AllowNumbers(bool allow) { this->numbers = allow; }
			inline void AllowSpecial(bool allow) { this->special = allow; }

			inline const std::string& GetText() { return this->str; }

		};

	}
}