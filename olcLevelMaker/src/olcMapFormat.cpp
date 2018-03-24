#include "olcMapFormat.hpp"


namespace olcMapFormat {

#pragma region Utils


	inline bool EndsWith(const std::wstring& value, const std::string& ending)
	{
		if (ending.size() > value.size()) return false;
		return std::equal(ending.rbegin(), ending.rend(), value.rbegin());
	}

	template<typename T>
	inline T ReadValue(std::istream& input) {
		T t;
		input.read((char*)&t, sizeof(T));
		return t;
	}

	template<typename T>
	inline void WriteValue(std::ostream& output, T value) {
		output.write((char*)&value, sizeof(T));
	}

	template<typename T>
	inline std::string ReadString(std::istream& input) {
		T size = ReadValue<T>(input);
		char* buffer = new char[size];
		input.read((char*)&size, sizeof(T));
		input.read(buffer, size);
		return std::string(buffer);
	}

	template<typename T>
	inline void WriteString(std::ostream& output, const std::string& str) {
		WriteValue<T>(output, (int32_t)str.size());
		output.write(str.c_str(), str.size());
	}

#pragma endregion


#pragma region Tile

	void Tile::SetParam(const std::string& key, const std::string& value) {
		// if the key is solid, cache the value
		if ("solid" == key) {
			solid = "true" == value;
		}
		params[key] = value;
	}

	const char* Tile::Read(Format format, std::istream& input) {
		switch (format) {

		case Format::LVL:
		{
			input >> tileId >> solid;
			params["solid"] = solid ? "true" : "false";
		}
		break;

		case Format::MAP_V1:
		{
			// read position
			int32_t position = ReadValue<int32_t>(input);
			this->x = position % map->GetWidth();
			this->y = position / map->GetWidth();

			this->tileId = ReadValue<int32_t>(input);

			// read params
			int32_t paramsCount = ReadValue<int32_t>(input);
			params.reserve(paramsCount);
			for (int i = 0; i < paramsCount; i++) {
				std::string key = ReadString<int32_t>(input);
				std::string value = ReadString<int32_t>(input);
				SetParam(key, value);
			}
		}
		break;

		default:
			return "Invalid map format";
		}
		return nullptr;
	}

	const char* Tile::Write(Format format, std::ostream& output) {
		switch (format) {

		case Format::LVL:
		{
			output << tileId << ' ' << solid << ' ';
		}
		break;

		case Format::MAP_V1:
		{
				WriteValue<int32_t>(output, this->x + this->y * map->GetWidth());
				WriteValue<int32_t>(output, this->tileId);
				WriteValue<int32_t>(output, (int32_t)params.size());
				for (const auto& element : params) {
					WriteString<int32_t>(output, element.first);
					WriteString<int32_t>(output, element.second);
				}
		}
		break;

		default: return "Invalid map format";
		}
		return nullptr;
	}

#pragma endregion

#pragma region Map

	Format Detect(const std::wstring& file) {
		if (EndsWith(file, ".map")) {
			return Format::MAP;
		}
		else if (EndsWith(file, ".lvl")) {
			return Format::LVL;
		}
		else {
			return Format::AUTO;
		}
	}

	const char* Map::LoadFromFile(const std::wstring& file, Format format) {
		if (format == Format::AUTO) {
			format = Detect(file);
		}
		if (format == Format::AUTO) {
			return "Could not auto detect format";
		}
		std::ifstream input(file, std::ios::in | std::ios::binary);
		return Load(format, input);
	}

	const char* Map::SaveToFile(const std::wstring& file, Format format) {
		if (format == Format::AUTO) {
			format = Detect(file);
		}
		if (format == Format::AUTO) {
			return "Could not auto detect format";
		}
		std::ofstream output(file, std::ios::out | std::ios::binary);
		return Save(format, output);
	}

	const char* Map::Save(Format format, std::ostream& output) {
		if (format == Format::AUTO) {
			// auto will set both
			format = Format::MAP;
		}
		switch (format) {

		case Format::LVL:
		{
			output << width << ' ' << height << '\n';
			for (auto& tile : tiles) {
				tile.Write(format, output);
			}
		}
		break;

		case Format::MAP_V1:
		{
			// header
			WriteString<int8_t>(output, "SPRMAP");
			WriteValue<int8_t>(output, 1);

			// map data
			WriteString<int32_t>(output, this->name);
			WriteValue<int32_t>(output, width);
			WriteValue<int32_t>(output, height);

			// tiles
			WriteValue<int32_t>(output, (int32_t)tiles.size());
			for (auto& tile : tiles) {
				tile.Write(format, output);
			}
		}
		break;
		default:
			return "Invalid format";
		}
		return nullptr;
	}

	const char* Map::Load(Format format, std::istream& input) {
		switch (format) {

		case Format::LVL:
		{
			input >> width >> height;
			tiles.reserve(width * height);
			for (int i = 0; i < width * height; i++) {
				tiles.emplace_back();
				tiles.back().Read(format, input);
			}
		}
		break;

		case Format::MAP:
		{
			bool compressed = false;
			// this will read the common map header first
			std::string type = ReadString<int8_t>(input);
			int version = ReadValue<int8_t>(input);

			if (type == "SPRMAPC") {
				// this is a compressed map
				compressed = true;
			}
			else if (type != "SPRMAP") {
				return ("Invalid map type: " + type).c_str();
			}
			
			// no we can actully parse the file
			switch (version) {
			case 1:
			{
				format = Format::MAP_V1;

				this->name = ReadString<int32_t>(input);
				this->width = ReadValue<int32_t>(input);
				this->height = ReadValue<int32_t>(input);

				tiles.resize(width * height);

				int tileCount = ReadValue<int32_t>(input);
				for (int i = 0; i < tileCount; i++) {
					Tile t;
					t.Read(format, input);
					tiles[t.GetX() + t.GetY() * width] = t;
				}
			}
			break;
			
			case 2:
			{
				format = compressed ? Format::MAP_V2_COMPRESSED : Format::MAP_V2;
				// TODO: implement
			}
			break;
			default:
				return ("Unsupported map version: " + std::to_string(version)).c_str();
			}
		}
		break;
		default:
			return "Invalid format";
		}
		return nullptr;
	}

	void Map::EmptyMap() {
		tiles.resize(width * height);
		for (int i = 0; i < width * height; i++) {
			tiles[i].SetID(0);
			tiles[i].x = i % width;
			tiles[i].y = i / width;
		}
	}

#pragma endregion


}

