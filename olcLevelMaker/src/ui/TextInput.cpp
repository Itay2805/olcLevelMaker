#include "TextInput.hpp"
#include "../LevelEditor.hpp"


namespace olcLevelEditor {
	namespace ui {

		TextInput::TextInput(int x, int y, int width, Panel* panel, Font* font)
			: x(x)
			, y(y)
			, width(width)
			, panel(panel)
			, font(font)
		{
			if (this->panel == nullptr) {
				this->panel = new Panel(0, 0, 400, 200);
				panelIsMine = true;
			}
		}

		TextInput::~TextInput() {
			if (panelIsMine) {
				delete panel;
			}
		}

		void TextInput::Draw() {
			panel->Fill(x, y, x + 2 + width * font->GetWidth(), y + 2 + font->GetWidth(), PIXEL_SOLID, (focused ? FG_WHITE : FG_DARK_GREY) | BG_WHITE);
			panel->DrawStringFont(x + 2, y + 2, str, font);
			if (focused) {
				if (str.length() < width) {
					if (numbers) {
						for (char c : strNumbers) {
							if (LevelEditor::Get()->GetKey(c).bPressed || LevelEditor::Get()->GetKey(VK_NUMPAD0 + (c - '0')).bPressed) {
								str += c;
							}
						}
					}
					if (characters) {
						for (char c : strCharacters) {
							if (LevelEditor::Get()->GetKey(c).bPressed) {
								if (!LevelEditor::Get()->GetKey(VK_SHIFT).bHeld) {
									c -= 'A' - 'a';
								}
								str += c;
							}
						}
					}
					if (special) {
						if (LevelEditor::Get()->GetKey(VK_SPACE).bPressed) str += ' ';
					}
				}
				if (LevelEditor::Get()->GetKey(VK_BACK).bPressed && str.length() > 0) {
					str = str.substr(0, str.size() - 1);
				}
			}
			if (LevelEditor::Get()->GetMouse(0).bPressed) {
				int mouseX = LevelEditor::Get()->GetMouseX();
				int mouseY = LevelEditor::Get()->GetMouseY();
				std::pair<int, int> start = panel->Translate(x, y);
				std::pair<int, int> end = panel->Translate(x + 2 + width * font->GetWidth(), y + 2 + font->GetWidth());
				focused = mouseX >= start.first && mouseX <= end.first && mouseY >= start.second && mouseY <= end.second;
			}
		}

	}
}