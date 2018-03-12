#pragma once

#include <iostream>
#include <fstream>

#include "SpriteSheet.h"

// TODO: maybe have some custom file instead of using this

class Level;

class Tile {
private:
	Level * level = nullptr;
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
	inline void SetLevel(Level* level) { this->level = level; }
	inline Level* GetLevel() const { return level; }
	inline void SetSolid(bool solid) { this->solid = solid; }
	inline bool IsSolid() const { return solid; }
	
	olcSprite* GetSprite();

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
		for (int i = 0; i < mapWidth * mapHeight; i++) {
			tiles[i].SetLevel(this);
		}
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

	void Load(wstring mapFile);
	void Save(wstring mapFile);

	inline Tile& operator[](int index) const { return tiles[index]; }

	inline int GetWidth() const { return this->mapWidth; }
	inline int GetHeight() const { return this->mapHeight; }
	inline SpriteSheet* GetSpriteSheet() { return &this->spritesheet; }
};
