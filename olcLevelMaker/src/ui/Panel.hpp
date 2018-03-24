#pragma once

#include "Font.hpp"

#include <vector>

namespace olcLevelEditor {

	namespace ui {

		class Panel {
		private:
			int x, y;
			int width, height;

		public:
			Panel(int x, int y, int width, int height)
				: x(x)
				, y(y)
				, width(width)
				, height(height)
			{
			}

			// maybe add some more stuff
			void Draw(int x, int y, wchar_t c = 0x2588, short col = 0x000F);
			void DrawLine(int x1, int y1, int x2, int y2, wchar_t c = 0x2588, short col = 0x000F);
			void Fill(int x1, int y1, int x2, int y2, wchar_t c = 0x2588, short col = 0x000F);

			bool Button(int x, int y, const std::string& text, bool centered = false, Font* font = nullptr);

			void DrawStringFont(int x, int y, const std::string& text, Font* font = nullptr);
			void DrawStringFontCenter(int x, int y, const std::string& text, Font* font = nullptr);
			void DrawRect(int x, int y, int width, int height, wchar_t c = 0x2588, short col = 0x000F);

			inline std::pair<int, int> Translate(int x, int y) {
				if (x < 0) x = 0;
				if (y < 0) y = 0;

				if (x >= width) x = width - 1;
				if (y >= height) y = height - 1;

				x += this->x;
				y += this->y;

				return std::make_pair(x, y);
			}

			inline void TranslateRef(int& x, int& y) {
				if (x < 0) x = 0;
				if (y < 0) y = 0;

				if (x >= width) x = width - 1;
				if (y >= height) y = height - 1;

				x += this->x;
				y += this->y;
			}

			inline int GetX() const { return x; }
			inline int GetY() const { return y; }
			inline int GetWidth() const { return width; }
			inline int GetHeight() const { return height; }
		};

	}

}
