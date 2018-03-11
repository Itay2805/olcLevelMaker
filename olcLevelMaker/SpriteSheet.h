#pragma once

#include "olcConsoleGameEngine.h"

class SpriteSheet {
private:
	olcSprite spritesheet;

	olcSprite* sprites;
	size_t spriteCount;
	size_t tileWidth, tileHeight;

public:

	~SpriteSheet() {
		delete[] sprites;
	}


	SpriteSheet() {

	}

	SpriteSheet(wstring file, int tileWidth, int tileHeight = -1) {
		Load(file, tileWidth, tileHeight);
	}

	void Load(wstring file, int tileWidth, int tileHeight) {
		this->tileHeight = tileHeight;
		this->tileWidth = tileWidth;

		spritesheet.Load(file);

		int tileCountX = spritesheet.nWidth / tileWidth;
		int tileCountY = spritesheet.nHeight / tileHeight;
		spriteCount = tileCountX * tileCountY;
		
		sprites = new olcSprite[spriteCount];

		for (size_t i = 0; i < spriteCount; i++) {
			sprites[i] = olcSprite(tileWidth, tileHeight);
			int baseX = (i % tileCountX);
			int baseY = ((i - baseX) / tileCountX);
			baseX *= tileWidth;
			baseY *= tileHeight;
			for (size_t y = 0; y < tileHeight; y++) {
				for (size_t x = 0; x < tileWidth; x++) {
					sprites[i].SetColour(x, y, spritesheet.GetColour(baseX + x, baseY + y));
					sprites[i].SetGlyph(x, y, spritesheet.GetGlyph(baseX + x, baseY + y));
				}
			}
		}
	}

	olcSprite* operator[](size_t index) const {
		return &sprites[index];
	}

	inline size_t GetTileWidth() const { return this->tileWidth; }
	inline size_t GetTileHeight() const { return this->tileHeight; }
	inline size_t GetTileCount() const { return this->spriteCount; }

public:

};

