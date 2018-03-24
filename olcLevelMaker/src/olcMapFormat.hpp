#pragma once

#include <cstdint>
#include <string>
#include <vector>
#include <unordered_map>
#include <fstream>

namespace olcMapFormat {

	inline bool EndsWith(const std::wstring& value, const std::string& ending);

	enum class Format {
		AUTO,

		// simple text based
		LVL,
		
		// binary 
		MAP_V1,
		MAP_V2,
		MAP_V2_COMPRESSED,	// compressed
		MAP_V2_JSON,		// json

		MAP = MAP_V1,		// the default map format
	};

	class Map;

	class Tile {
	protected:
		int x, y;
		int tileId;
		Map* map;
		std::unordered_map<std::string, std::string> params;

		// cahced params
		bool solid;

		friend class Map;

	public:
		
		Tile() {}

		const char* Read(Format format, std::istream& input);
		const char* Write(Format format, std::ostream& output);

		inline int GetX() const { return x; }
		inline int GetY() const { return y; }
		inline int GetID() const { return tileId; }
		inline void SetID(int id) { tileId = id; }

		void SetParam(const std::string& key, const std::string& value);
		inline const std::string& GetParam(const std::string& key) { return params[key]; }

		inline Map* GetMap() const { return map; }
		inline void SetMap(Map* map) { this->map = map; }

		inline bool IsSolid() const { return solid; }
		inline void SetSolid(bool value) { 
			if (value != solid) {
				solid = value;
				params["solid"] = value ? "true" : "false";
			}
		}

	};

	class Map {
	private:
		std::string name;
		int width, height;
		std::vector<Tile> tiles;

	public:
		Map() {}

		Map(int width, int height, const std::string& name = "Unnamed") 
			: width(width)
			, height(height)
			, name(name)
		{
			tiles.reserve(width * height);
		}

		const char* LoadFromFile(const std::wstring& file, Format format = Format::AUTO);
		const char* SaveToFile(const std::wstring& file, Format format = Format::AUTO);

		const char* Load(Format format, std::istream& input);
		const char* Save(Format format, std::ostream& output);

		void EmptyMap();

		Tile* GetTile(int x, int y) { return &tiles[x + y * width]; }
		Tile* operator[](int index) { return &tiles[index]; }
		bool TileExists(int x, int y) { return x >= 0 && y >= 0 && x < width && y < height; }

		inline const std::string& GetName() { return name; }
		inline void SetName(const std::string& name) { this->name = name; }
		
		inline int GetWidth() { return width; }
		inline int GetHeight() { return height; }

	};

	
}