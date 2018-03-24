#pragma once

#include "../olcConsoleGameEngine.hpp"

namespace olcLevelEditor {
	namespace ui {

		// TODO: Move to cpp file

		class Font {
		private:
			olcSprite* sprite;
			int charWidth;

		public:
			Font(const std::wstring& file, int charWidth)
				: charWidth(charWidth)
			{
				sprite = new olcSprite(file);
			}

			Font(olcSprite* sprite, int charWidth) 
				: charWidth(charWidth)
			{
				this->sprite = new olcSprite(sprite->nWidth, sprite->nHeight);
				for (int y = 0; y < sprite->nHeight; y++) {
					for (int x = 0; x < sprite->nWidth; x++) {
						this->sprite->SetColour(x, y, sprite->GetColour(x, y));
						this->sprite->SetGlyph(x, y, sprite->GetGlyph(x, y));
					}
				}
			}

			inline std::pair<int, int> GetSpriteCoords(char c) {
				int i = (c - ' ');
				int x = i % (sprite->nWidth / charWidth);
				int y = i / (sprite->nWidth / charWidth);
				return std::make_pair(x * charWidth, y * charWidth);
			}

			inline int GetWidth() const { return charWidth; }
			inline olcSprite* GetSprite() const { return this->sprite; }

			void Draw(olcConsoleGameEngine* engine, int x, int y, const std::string& str) {
				int i = 0;
				for (char c : str) {
					std::pair<int, int> coord = GetSpriteCoords(c);
					engine->DrawPartialSprite(x + i, y, sprite, coord.first, coord.second, charWidth, charWidth);
					i += charWidth;
				}
			}

		};

	}
}