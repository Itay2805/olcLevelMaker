#pragma once

#include <iostream>
#include <fstream>

#include "SpriteSheet.h"

// TODO: maybe have some custom file instead of using this

class Level;

class Tile {
private:
	Level * level = nullptr;
	int spriteId = 0;
	bool solid = false;

public:
	Tile() {}

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

	~Level() {
		delete[] tiles;
	}

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

	Level(const wstring& map)
	{
		Load(map);
	}

	Level(const wstring& map, const wstring& spriteSheet, int tileSize)
	{
		Load(map);
		LoadSpriteSheet(spriteSheet, tileSize);
	}

	void LoadSpriteSheet(const wstring& map, int tileSize) {
		spritesheet.Load(map, tileSize, tileSize);
	}

	void Create(int mapWidth, int mapHeight) {
		this->mapWidth = mapWidth;
		this->mapHeight = mapHeight;
		delete[] tiles;
		tiles = new Tile[mapWidth * mapHeight];
	}

	void Load(const wstring& mapFile);
	void Save(const wstring& mapFile);

	inline Tile& operator[](int index) const { return tiles[index]; }

	inline int GetWidth() const { return this->mapWidth; }
	inline int GetHeight() const { return this->mapHeight; }
	inline SpriteSheet* GetSpriteSheet() { return &this->spritesheet; }
};
