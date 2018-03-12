#include "Level.h"

olcSprite* Tile::GetSprite() {
	int tileCount = level->GetSpriteSheet()->GetTileCount();
	SpriteSheet* spr = level->GetSpriteSheet();
	if (spriteId >= tileCount) {
		return spr[0][0];
	}
	return spr[0][spriteId];
}

void Level::Load(wstring mapFile) {
	if (tiles != nullptr) {
		delete[] tiles;
	}

	ifstream map(mapFile, ios::in | ios::binary);
	if (map.is_open()) {
		map >> mapWidth >> mapHeight;
		tiles = new Tile[mapWidth * mapHeight];
		for (int i = 0; i < mapWidth * mapHeight; i++) {
			map >> tiles[i];
			tiles[i].SetLevel(this);
		}
		map.close();
	}
}

void Level::Save(wstring mapFile) {
	ofstream map(mapFile, ios::out | ios::binary);
	if (map.is_open()) {
		map << mapWidth << " " << mapHeight << "\n";
		for (int i = 0; i < mapWidth * mapHeight; i++) {
			map << tiles[i];
		}
		map.close();
	}
}