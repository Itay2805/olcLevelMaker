#include "Panel.hpp"

#include "../LevelEditor.hpp"

namespace olcLevelEditor {

	namespace ui {

		void Panel::Draw(int x, int y, wchar_t c, short col) {
			if (x < 0 || y < 0 || x >= width || y >= height) return;

			x += this->x;
			y += this->y;

			LevelEditor::Get()->Draw(x, y, c, col);
		}

		void Panel::DrawLine(int x1, int y1, int x2, int y2, wchar_t c, short col) {
			if (x1 < 0) x1 = 0;
			if (x2 < 0) x2 = 0;
			if (y1 < 0) y1 = 0;
			if (y2 < 0) y2 = 0;

			if (x1 >= width) x1 = width - 1;
			if (x2 >= width) x2 = width - 1;
			if (y1 >= height) y1 = height - 1;
			if (y2 >= height) y2 = height - 1;

			x1 += x;
			x2 += x;
			y1 += y;
			y2 += y;

			LevelEditor::Get()->DrawLine(x1, y1, x2, y2, c, col);
		}

		void Panel::Fill(int x1, int y1, int x2, int y2, wchar_t c, short col) {
			if (x1 < 0) x1 = 0;
			if (x2 < 0) x2 = 0;
			if (y1 < 0) y1 = 0;
			if (y2 < 0) y2 = 0;

			if (x1 >= width) x1 = width - 1;
			if (x2 >= width) x2 = width - 1;
			if (y1 >= height) y1 = height - 1;
			if (y2 >= height) y2 = height - 1;

			x1 += x;
			x2 += x;
			y1 += y;
			y2 += y;

			LevelEditor::Get()->Fill(x1, y1, x2, y2, c, col);
		}

		bool Panel::Button(int x, int y, const std::string& text, bool centered, Font* font) {
			TranslateRef(x, y);
			return LevelEditor::Get()->Button(x, y, text, centered, font, true);
		}

		void Panel::DrawStringFont(int x, int y, const std::string& text, Font* font) {
			TranslateRef(x, y);
			LevelEditor::Get()->DrawStringFont(x, y, text, font);
		}

		void Panel::DrawStringFontCenter(int x, int y, const std::string& text, Font* font) {
			TranslateRef(x, y);
			LevelEditor::Get()->DrawStringFontCenter(x, y, text, font);
		}
		
		void Panel::DrawRect(int x, int y, int width, int height, wchar_t c, short col) {
			TranslateRef(x, y);
			auto coord = Translate(width + x, height + y);
			LevelEditor::Get()->DrawRect(x, y, coord.first, coord.second, c, col);
		}


	}

}