#pragma once

#include "olcConsoleGameEngine.h"

#include <string>

class SpriteSheet {
private:
	olcSprite* spritesheet;

	olcSprite* sprites;
	int spriteCount;
	int tileWidth, tileHeight;

public:

	~SpriteSheet() {
		delete[] sprites;
	}


	SpriteSheet() {}

	SpriteSheet(const wstring& file, int tileWidth, int tileHeight = -1) {
		Load(file, tileWidth, tileHeight);
	}

	void Load(const wstring& file, int tileWidth, int tileHeight);

	olcSprite* operator[](int index) const {
		return &sprites[index];
	}

	inline int GetTileWidth() const { return this->tileWidth; }
	inline int GetTileHeight() const { return this->tileHeight; }
	inline int GetTileCount() const { return this->spriteCount; }

public:

};

