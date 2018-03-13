#include "SpriteSheet.h"

void SpriteSheet::Load(std::wstring file, int tileWidth, int tileHeight) {
	this->tileHeight = tileHeight;
	this->tileWidth = tileWidth;

	spritesheet = new olcSprite(file);

	int tileCountX = spritesheet->nWidth / tileWidth;
	int tileCountY = spritesheet->nHeight / tileHeight;
	spriteCount = tileCountX * tileCountY;

	delete[] sprites;
	sprites = new olcSprite[spriteCount];

	for (size_t i = 0; i < spriteCount; i++) {
		sprites[i] = olcSprite(tileWidth, tileHeight);
		int baseX = (i % tileCountX);
		int baseY = ((i - baseX) / tileCountX);
		baseX *= tileWidth;
		baseY *= tileHeight;
		for (size_t y = 0; y < tileHeight; y++) {
			for (size_t x = 0; x < tileWidth; x++) {
				sprites[i].SetColour(x, y, spritesheet->GetColour(baseX + x, baseY + y));
				sprites[i].SetGlyph(x, y, spritesheet->GetGlyph(baseX + x, baseY + y));
			}
		}
	}

	delete spritesheet;
	spritesheet = nullptr;
}