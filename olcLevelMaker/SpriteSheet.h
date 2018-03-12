#pragma once

#include "olcConsoleGameEngine.h"

#include <string>

class SpriteSheet {
private:
	olcSprite* spritesheet;

	olcSprite* sprites;
	size_t spriteCount;
	size_t tileWidth, tileHeight;

public:

	~SpriteSheet() {
		delete[] sprites;
	}


	SpriteSheet() {}

	SpriteSheet(std::wstring file, int tileWidth, int tileHeight = -1) {
		Load(file, tileWidth, tileHeight);
	}

	void Load(std::wstring file, int tileWidth, int tileHeight);

	olcSprite* operator[](size_t index) const {
		return &sprites[index];
	}

	inline size_t GetTileWidth() const { return this->tileWidth; }
	inline size_t GetTileHeight() const { return this->tileHeight; }
	inline size_t GetTileCount() const { return this->spriteCount; }

public:

};

