#pragma once

#include <iostream>
#include <fstream>

#include "olcConsoleGameEngine.h"

#include "SpriteSheet.h"

// TODO: maybe have some custom file instead of using this

class Tile {
private:
	int spriteId;
	bool solid = false;

public:
	Tile() {
		spriteId = 0;
	}

	Tile(int spriteId) 
		: spriteId(spriteId)
	{

	}

	inline void SetSpriteId(int spriteId) { this->spriteId = spriteId; }
	inline int GetSpriteId() const { return spriteId; }
	inline void SetSolid(bool solid) { this->solid = solid; }
	inline bool IsSolid() const { return solid; }
	inline olcSprite* GetSprite(const SpriteSheet& spritesheet) { return spritesheet[spriteId]; }

	friend istream& operator>>(istream& input, Tile& tile) {
		input >> tile.spriteId >> tile.solid;
		return input;
	}

	friend ostream& operator<<(ostream& output, const Tile& tile) {
		output << tile.spriteId << " " << tile.solid << " ";
		return output;
	}

};

class Level {
private:
	SpriteSheet spritesheet;
	Tile* tiles = nullptr;
	int mapWidth, mapHeight;

public:
	Level() 
	{
	}

	Level(int mapWidth, int mapHeight)
		:	mapWidth(mapWidth),
			mapHeight(mapHeight)
	{
		tiles = new Tile[mapWidth * mapHeight];
	}

	Level(wstring map)
	{
		Load(map);
	}

	Level(wstring map, wstring spriteSheet, int tileSize)
	{
		Load(map);
		LoadSpriteSheet(spriteSheet, tileSize);
	}

	void LoadSpriteSheet(wstring map, int tileSize) {
		spritesheet.Load(map, tileSize, tileSize);
	}

	void Create(int mapWidth, int mapHeight) {
		this->mapWidth = mapWidth;
		this->mapHeight = mapHeight;
		tiles = new Tile[mapWidth * mapHeight];
	}

	void Load(wstring mapFile) {

		if (tiles != nullptr) {
			delete[] tiles;
		}

		ifstream map(mapFile, ios::in | ios::binary);
		if (map.is_open()) {
			map >> mapWidth >> mapHeight;
			tiles = new Tile[mapWidth * mapHeight];
			for (int i = 0; i < mapWidth * mapHeight; i++) {
				map >> tiles[i];
			}
			map.close();
		}
	}

	void Save(wstring mapFile) {
		ofstream map(mapFile, ios::out | ios::binary);
		if (map.is_open()) {
			map << mapWidth << " " << mapHeight << "\n";
			for (int i = 0; i < mapWidth * mapHeight; i++) {
				map << tiles[i];
			}
			map.close();
		}
	}

	Tile& operator[](int index) const {
		return tiles[index];
	}

	inline int GetWidth() { return this->mapWidth; }
	inline int GetHeight() { return this->mapHeight; }

};
