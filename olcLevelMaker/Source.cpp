#include "olcConsoleGameEngine.h"

#include <string>
#include <queue>
#include <sstream>

#include "SpriteSheet.h"
#include "Level.h"

#define TILE_WIDTH 16
#define FONT_SPRITESHEET L"javidx9_nesfont8x8.spr"
#define TILE_SPRITESHEET L"toml_spritesheetdark.spr"
#define MAP_WIDTH 10
#define MAP_HEIGHT 10
#define LEVEL_FILE_NAME L"level.lvl"
#define DEFAULT_TILE 14

enum class Tool {
	TILES,
	META,
	EXPORT_IMPORT,
	LAST
};

string fillSpriteData = "8 8 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 15 9608 0 0 0 0 0 0 0 0 12 9608 12 9608 12 9608 12 9608 15 9608 0 0 0 0 0 0 12 9608 15 9608 12 9608 12 9608 12 9608 15 9608 0 0 0 0 12 9608 0 9608 15 9608 12 9608 12 9608 12 9608 15 9608 0 0 12 9608 0 9608 0 9608 15 9608 12 9608 15 9608 0 0 0 0 12 9608 0 0 0 9608 0 9608 15 9608 0 9608 0 0 0 0 0 0 0 0 0 0 0 9608 0 9608 0 0 0 0";

class olcLevelMaker : public olcConsoleGameEngine {

	float mapMoveX, mapMoveY;
	SpriteSheet font;
	SpriteSheet* tiles;
	wstring characters = L" !\"#$%&'()*+,-./0123456789:;<=>?@ABCDEFGHIJKLMNOPQRSTUVWXYZ[\\]^_`abcdefgijklmnopqrstuvwxyz{|}~";
	
	olcSprite fillIcon;

	Tool tool = Tool::TILES;
	Level level;
	int page = 0;
	int selectedSprite = 0;
	int pageCount = 1;
	int uiBase = 300, tilesPerRow, tilesPerColumn, tilesPerPage, uiWidth;
	int worldOffsetX = 0, worldOffsetY = 0;
	bool grid = false;
	bool floodMode = false;
	bool inMenu = false;

	wstring file = L"";

	void DrawStringFont(int x, int y, wstring characters) {
		// will use ascii
		for (wchar_t c : characters) {
			// int index = this->characters.find(c);
			DrawSprite(x, y, font[c - ' ']);
			x += font.GetTileWidth();
		}
	}

	virtual bool OnUserCreate() {
		font.Load(FONT_SPRITESHEET, 8, 8);
		// tiles.Load(TILE_SPRITESHEET, 16, 16);
		
		istringstream fillSprite(fillSpriteData);
		int fillWidth, fillHeight;
		fillSprite >> fillWidth >> fillHeight;
		fillIcon = olcSprite(fillWidth, fillHeight);
		for (int sy = 0; sy < fillIcon.nHeight; sy++) {
			for (int sx = 0; sx < fillIcon.nWidth; sx++) {
				short color, glyph;
				fillSprite >> color >> glyph;
				fillIcon.SetColour(sx, sy, color);
				fillIcon.SetGlyph(sx, sy, glyph);
			}
		}

		level.Create(MAP_WIDTH, MAP_HEIGHT);
		for (int i = 0; i < level.GetWidth() * level.GetHeight(); i++) {
			level[i].SetLevel(&level);
			level[i].SetSpriteId(DEFAULT_TILE);
		}

		level.LoadSpriteSheet(TILE_SPRITESHEET, TILE_WIDTH);
		tiles = level.GetSpriteSheet();

		uiWidth = 400 - uiBase;
		tilesPerRow = uiWidth / tiles->GetTileWidth();
		tilesPerColumn = (200 - 23) / tiles->GetTileHeight();
		tilesPerPage = tilesPerColumn * tilesPerRow;
		pageCount = tiles->GetTileCount() / tilesPerPage;

		return true;
	}

	bool moved = false;

	virtual bool OnUserUpdate(float fElapsedTime) {
		int tileX = m_mousePosX / 16;
		int tileY = m_mousePosY / 16;
		// simple
		tileX -= worldOffsetX / 16;
		tileY -= worldOffsetY / 16;

		Fill(0, 0, 300, 200, ' ', BG_BLACK | FG_BLACK);

		int topTileX = -(worldOffsetX / 16);
		int topTileY = -(worldOffsetY / 16);

		// draw map
		for (int y = topTileY; y < topTileY + ceil(200.0 / TILE_WIDTH); y++) {
			for (int x = topTileX; x < topTileX + ceil(300.0 / TILE_WIDTH); x++) {
				if (x < 0 || x >= level.GetWidth() || y < 0 || y >= level.GetHeight()) continue;
				int i = x + y * level.GetWidth();
				if (i < 0 || i >= level.GetWidth() * level.GetHeight()) continue;
				int screenX = x * TILE_WIDTH + worldOffsetX;
				int screenY = y * TILE_WIDTH + worldOffsetY;
				if (screenX < 0 || screenX >= 300 || screenY < 0 || screenY >= 200) continue;
				DrawSprite(screenX, screenY, level[i].GetSprite());
				if (grid) {
					DrawLine(screenX, screenY, screenX + TILE_WIDTH, screenY, PIXEL_SOLID, BG_BLACK | FG_BLACK);
					DrawLine(screenX, screenY, screenX, screenY + TILE_WIDTH, PIXEL_SOLID, BG_BLACK | FG_BLACK);
					DrawLine(screenX + TILE_WIDTH, screenY, screenX + TILE_WIDTH, screenY + TILE_WIDTH, PIXEL_SOLID, BG_BLACK | FG_BLACK);
					DrawLine(screenX, screenY + TILE_WIDTH, screenX + TILE_WIDTH, screenY + TILE_WIDTH, PIXEL_SOLID, BG_BLACK | FG_BLACK);
				}
				if (tool == Tool::META) {
					int offset = 0;
					if (level[i].IsSolid()) {
						Fill(screenX + offset, screenY + offset, screenX + 3 + offset, screenY + 3 + offset, PIXEL_SOLID, BG_BLACK | FG_RED);
						offset += 3;
					}
				}
			}
		}

		// fill the menu
		Fill(uiBase, 0, 400, 200, ' ', BG_GREY | FG_BLACK);

		if (tool == Tool::TILES) tilesTool(tileX, tileY);
		if (tool == Tool::META) metaTool(tileX, tileY);
		if (tool == Tool::EXPORT_IMPORT) exportAndImportTool();

		if (tileX >= 0 && tileY >= 0 && tileX < level.GetWidth() && tileY < level.GetHeight() && m_mousePosX <= 300) {
			// draw coords
			wstring str(L"<");
			str.append(std::to_wstring(tileX));
			str.append(L", ");
			str.append(std::to_wstring(tileY));
			str.append(L">");
			DrawStringFont(0, 0, str);

			int yoff = worldOffsetY;
			int xoff = worldOffsetX;

			// draw hovered tile rect
			DrawLine(tileX * 16 + worldOffsetX, tileY * 16 + worldOffsetY, tileX * 16 + 16 + worldOffsetX, tileY * 16 + worldOffsetY, PIXEL_SOLID, BG_BLACK | FG_GREY);
			DrawLine(tileX * 16 + worldOffsetX, tileY * 16 + worldOffsetY, tileX * 16 + worldOffsetX, tileY * 16 + 16 + worldOffsetY, PIXEL_SOLID, BG_BLACK | FG_GREY);
			DrawLine(tileX * 16 + 16 + worldOffsetX, tileY * 16 + worldOffsetY, tileX * 16 + 16 + worldOffsetX, tileY * 16 + 16 + worldOffsetY, PIXEL_SOLID, BG_BLACK | FG_GREY);
			DrawLine(tileX * 16 + worldOffsetX, tileY * 16 + 16 + worldOffsetY, tileX * 16 + 16 + worldOffsetX, tileY * 16 + 16 + worldOffsetY, PIXEL_SOLID, BG_BLACK | FG_GREY);
		}

		int iconOffset = 0;
		if (floodMode || m_keys[VK_CONTROL].bHeld) {
			DrawSprite(2, 190, &fillIcon);
			iconOffset += 10;
		}

		// world movement
		if (m_keys[L'W'].bPressed) {
			moved = true;
			worldOffsetY -= 16;
		}
		else if (m_keys[L'S'].bPressed) {
			moved = true;
			worldOffsetY += 16;
		}
		else if (m_keys[L'A'].bPressed) {
			moved = true;
			worldOffsetX -= 16;
		}
		else if (m_keys[L'D'].bPressed) {
			moved = true;
			worldOffsetX += 16;
		} else if (m_keys[VK_CONTROL].bHeld && m_keys[L'S'].bPressed) {
			if (file.length() == 0) {
				SaveLevel();
			}
			else {
				level.Save(file);
			}
		}
		else if (m_keys[VK_CONTROL].bHeld && m_keys[L'L'].bPressed) {
			LoadLevel();
		}
		else if (m_keys[L'T'].bPressed) {
			tool = (Tool)((int)tool + 1);
			if (tool == Tool::LAST) {
				tool = Tool::TILES;
			}
		}
		else if (m_keys[L'G'].bPressed) {
			grid = !grid;
		}
		else if (m_keys[L'F'].bPressed) {
			floodMode = !floodMode;
		}
		else if (m_keys[VK_LEFT].bPressed) {
			if (pageCount != 0) {
				page--;
				if (page < 0) {
					page = 0;
				}
			}
		}
		else if (m_keys[VK_RIGHT].bPressed) {
			if (pageCount != 0) {
				page++;
				if (page >= pageCount) {
					page = pageCount - 1;
				}
			}
		}

		return true;
	}

	enum class MetaTools {
		SOLID_BRUSH
	};

	MetaTools selectedMetaTool;


	void metaTool(int tileX, int tileY) {
		wstring title(L"TILE META");
		DrawStringFont(uiBase + 5, 5, title);
		
		wstring solidBrushText(L"");
		if (selectedMetaTool == MetaTools::SOLID_BRUSH) {
			solidBrushText.append(L" * ");
		}
		solidBrushText.append(L"SOLID");
		DrawStringFont(uiBase + 10, 18, solidBrushText);
		Fill(uiBase + 7, 19, uiBase + 7 + 5, 19 + 5, PIXEL_SOLID, BG_BLACK | FG_RED);


		// are we in the world editor
		if (tileX >= 0 && tileY >= 0 && tileX < level.GetWidth() && tileY < level.GetHeight() && !inMenu) {
			// change the tile
			if (m_mouse[0].bHeld) {
				if (floodMode || m_keys[VK_CONTROL].bHeld) {
					FloorFillSolid(tileX, tileY);
				}
				else {
					level[tileX + tileY * level.GetWidth()].SetSolid(true);
				}
			}
			else if (m_mouse[1].bHeld) {
				if (floodMode || m_keys[VK_CONTROL].bHeld) {
					FloorFillSolid(tileX, tileY);
				}
				else {
					level[tileX + tileY * level.GetWidth()].SetSolid(false);
				}
			}
		}
	}

	void exportAndImportTool() {
		DrawStringFont(uiBase + 1, 15, L"IMPORT:");
		DrawStringFont(uiBase + 6, 25, L"LEVEL");
		DrawStringFont(uiBase + 6, 35, L"SPRITESHEET");
		DrawStringFont(uiBase + 1, 65, L"EXPORT:");
		DrawStringFont(uiBase + 6, 75, L"LEVEL");
		DrawStringFont(uiBase + 6, 85, L"SPRITE");
		if (m_mouse[0].bPressed) {
			
			// import level
			if (m_mousePosX > uiBase + 6 && m_mousePosX < 400 && m_mousePosY > 25 && m_mousePosY < 25 + 8) {
				LoadLevel();
			}
			// import spritesheet
			if (m_mousePosX > uiBase + 6 && m_mousePosX < 400 && m_mousePosY > 35 && m_mousePosY < 35 + 8) {
				ImportSpriteSheet();
			}
			// export level
			if (m_mousePosX > uiBase + 6 && m_mousePosX < 400 && m_mousePosY > 75 && m_mousePosY < 75 + 8) {
				SaveLevel();
			}
			// export level as sprite
			if (m_mousePosX > uiBase + 6 && m_mousePosX < 400 && m_mousePosY > 85 && m_mousePosY < 85 + 8) {
				ExportAsSprite();
			}
		}
	}

	void ImportSpriteSheet() {
		wchar_t filename[MAX_PATH];
		OPENFILENAME ofn;
		ZeroMemory(&filename, sizeof(filename));
		ZeroMemory(&ofn, sizeof(ofn));
		ofn.lStructSize = sizeof(ofn);
		ofn.hwndOwner = NULL;
		ofn.lpstrFile = filename;
		ofn.nMaxFile = MAX_PATH;
		ofn.Flags = OFN_FILEMUSTEXIST;
		ofn.lpstrFilter = L"olcSprite (*.spr)\0*.spr\0Any File\0*.*\0";
		ofn.lpstrTitle = L"Import Sprite Sheet";
		if (GetOpenFileName(&ofn)) {
			level.LoadSpriteSheet(filename, 16);
		}
	}

	void LoadLevel() {
		wchar_t filename[MAX_PATH];
		OPENFILENAME ofn;
		ZeroMemory(&filename, sizeof(filename));
		ZeroMemory(&ofn, sizeof(ofn));
		ofn.lStructSize = sizeof(ofn);
		ofn.hwndOwner = NULL;
		ofn.lpstrFile = filename;
		ofn.nMaxFile = MAX_PATH;
		ofn.Flags = OFN_FILEMUSTEXIST;
		ofn.lpstrFilter = L"Level File (*.lvl)\0*.lvl\0Any File\0*.*\0";
		ofn.lpstrTitle = L"Load Level";
		if (GetOpenFileName(&ofn)) {
			level.Load(filename);
		}
	}

	void SaveLevel() {
		wchar_t filename[MAX_PATH];
		OPENFILENAME ofn;
		ZeroMemory(&filename, sizeof(filename));
		ZeroMemory(&ofn, sizeof(ofn));
		ofn.lStructSize = sizeof(ofn);
		ofn.hwndOwner = NULL;
		ofn.lpstrFile = filename;
		ofn.nMaxFile = MAX_PATH;
		ofn.lpstrFilter = L"Level File (*.lvl)\0*.lvl\0Any File\0*.*\0";
		ofn.lpstrTitle = L"Save Level";
		if (GetSaveFileName(&ofn)) {
			wstring f = filename;
			if (!ends_with(filename, L".lvl")) {
				f.append(L".lvl");
			}
			level.Save(f);
			file = f;
		}
	}

	void ExportAsSprite() {
		wchar_t filename[MAX_PATH];
		OPENFILENAME ofn;
		ZeroMemory(&filename, sizeof(filename));
		ZeroMemory(&ofn, sizeof(ofn));
		ofn.lStructSize = sizeof(ofn);
		ofn.hwndOwner = NULL;
		ofn.lpstrFile = filename;
		ofn.nMaxFile = MAX_PATH;
		ofn.lpstrFilter = L"olcSprite (*.spr)\0*.spr\0Any File\0*.*\0";
		ofn.lpstrTitle = L"Export Level As Sprite";
		ofn.lpstrDefExt = L"spr";
		if (GetSaveFileName(&ofn)) {
			wstring f = filename;
			if (!ends_with(filename, L".spr")) {
				f.append(L".spr");
			}
			olcSprite exportedSprite(level.GetWidth() * TILE_WIDTH, level.GetHeight() * TILE_WIDTH);
			for (int y = 0; y < level.GetHeight(); y++) {
				for (int x = 0; x < level.GetWidth(); x++) {
					olcSprite* sprite = level[x + y * level.GetWidth()].GetSprite();
					for (int sy = 0; sy < sprite->nHeight; sy++) {
						for (int sx = 0; sx < sprite->nWidth; sx++) {
							exportedSprite.SetColour(x * TILE_WIDTH + sx, y * TILE_WIDTH + sy, sprite->GetColour(sx, sy));
							exportedSprite.SetGlyph(x * TILE_WIDTH + sx, y * TILE_WIDTH + sy, sprite->GetGlyph(sx, sy));
						}
					}
				}
			}
			exportedSprite.Save(f);
		}
	}

	void tilesTool(int tileX, int tileY) {

		// draw page
		wstring pageText(L"TILES:");
		if (pageCount == 0) {
			pageText.append(std::to_wstring(tiles->GetTileCount()));
		}
		else {
			pageText.append(std::to_wstring(page));
			pageText.append(L"/");
			pageText.append(std::to_wstring(pageCount));
			pageText.append(L"/");
			pageText.append(std::to_wstring(tiles->GetTileCount()));
		}
		DrawStringFont(uiBase + 5, 5, pageText);

		// draw sprites in menu
		int drawn = 0;
		int toDraw = min(tilesPerPage, tiles->GetTileCount() - tilesPerPage * page);
		for (int row = 0; row < tilesPerColumn; row++) {
			if (drawn >= toDraw) break;
			int y = 23 + row * tiles->GetTileHeight();
			for (int col = 0; col < tilesPerRow; col++) {
				if (drawn >= toDraw) break;
				int x = uiBase + col * tiles->GetTileWidth();
				DrawSprite(x, y, tiles[0][(col + row * tilesPerRow) + tilesPerPage * page]);
				drawn++;
			}
		}

		// draw selected sprite thing
		if (selectedSprite >= tilesPerPage * page && selectedSprite < tilesPerPage * page + tilesPerPage) {
			int col = selectedSprite % tilesPerRow;
			int row = (selectedSprite - col) / tilesPerRow;
			int y = 23 + row * tiles->GetTileHeight();
			int x = uiBase + col * tiles->GetTileWidth();
			DrawLine(x, y, x + 16, y, PIXEL_SOLID, BG_RED | FG_RED);
			DrawLine(x, y, x, y + 16, PIXEL_SOLID, BG_RED | FG_RED);
			DrawLine(x + 16, y, x + 16, y + 16, PIXEL_SOLID, BG_RED | FG_RED);
			DrawLine(x, y + 16, x + 16, y + 16, PIXEL_SOLID, BG_RED | FG_RED);
		}

		// are we in the selection menu
		if (m_mousePosX >= uiBase && m_mousePosX < uiBase + tilesPerRow * tiles->GetTileWidth() && m_mousePosY > 28) {
			int menuX = m_mousePosX - uiBase;
			int menuY = m_mousePosY - 28;
			int col = menuX / tiles->GetTileWidth();
			int row = menuY / tiles->GetTileHeight();
			int index = col + row * tilesPerRow;
			if (index < toDraw) {
				int y = 23 + row * tiles->GetTileHeight();
				int x = uiBase + col * tiles->GetTileWidth();
				DrawLine(x, y, x + 16, y, PIXEL_SOLID, BG_RED | FG_DARK_RED);
				DrawLine(x, y, x, y + 16, PIXEL_SOLID, BG_RED | FG_DARK_RED);
				DrawLine(x + 16, y, x + 16, y + 16, PIXEL_SOLID, BG_RED | FG_DARK_RED);
				DrawLine(x, y + 16, x + 16, y + 16, PIXEL_SOLID, BG_RED | FG_DARK_RED);
			}
			if (m_mouse[0].bPressed) {
				selectedSprite = index;
			}
		}

		// are we in the world editor
		if (tileX >= 0 && tileY >= 0 && tileX < level.GetWidth() && tileY < level.GetHeight()) {
			// change the tile
			if (m_mouse[0].bHeld) {
				if (floodMode || m_keys[VK_CONTROL].bHeld) {
					FloodFillTile(tileX, tileY);
				}
				else {
					level[tileX + tileY * level.GetWidth()].SetSpriteId(selectedSprite);
				}
			}
		}

	}

	inline bool ends_with(std::wstring const & value, std::wstring const & ending)
	{
		if (ending.size() > value.size()) return false;
		return std::equal(ending.rbegin(), ending.rend(), value.rbegin());
	}

	int fillTileOfType = DEFAULT_TILE;
	bool solidStart = false;

	void FloorFillSolid(int x, int y) {
		fillTileOfType = level[x + y * level.GetWidth()].GetSpriteId();
		solidStart = !level[x + y * level.GetWidth()].IsSolid();
		queue<pair<int, int>> q;
		q.push(pair<int, int>(x, y));
		while (q.size() != 0) {
			pair<int, int> xy = q.front();
			q.pop();
			level[xy.first + xy.second * level.GetWidth()].SetSolid(solidStart);
			if (ShouldFillSolid(xy.first + 1, xy.second)) q.push(pair<int, int>(xy.first + 1, xy.second));
			if (ShouldFillSolid(xy.first - 1, xy.second)) q.push(pair<int, int>(xy.first - 1, xy.second));
			if (ShouldFillSolid(xy.first, xy.second + 1)) q.push(pair<int, int>(xy.first, xy.second + 1));
			if (ShouldFillSolid(xy.first, xy.second - 1)) q.push(pair<int, int>(xy.first, xy.second - 1));
		}
	}

	bool ShouldFillSolid(int x, int y) {
		if (x < 0 || y < 0 || x >= level.GetWidth() || y >= level.GetHeight()) return false;
		return level[x + y * level.GetWidth()].GetSpriteId() == fillTileOfType && level[x + y * level.GetWidth()].IsSolid() != solidStart;
	}

	void FloodFillTile(int x, int y) {
		fillTileOfType = level[x + y * level.GetWidth()].GetSpriteId();
		if (fillTileOfType == selectedSprite) return;
		queue<pair<int, int>> q;
		q.push(pair<int, int>(x, y));
		while (q.size() != 0) {
			pair<int, int> xy = q.front();
			q.pop();
			level[xy.first + xy.second * level.GetWidth()].SetSpriteId(selectedSprite);
			if (ShouldFillTile(xy.first + 1, xy.second)) q.push(pair<int, int>(xy.first + 1, xy.second));
			if (ShouldFillTile(xy.first - 1, xy.second)) q.push(pair<int, int>(xy.first - 1, xy.second));
			if (ShouldFillTile(xy.first, xy.second + 1)) q.push(pair<int, int>(xy.first, xy.second + 1));
			if (ShouldFillTile(xy.first, xy.second - 1)) q.push(pair<int, int>(xy.first, xy.second - 1));
		}
	}

	bool ShouldFillTile(int x,int y) {
		if (x < 0 || y < 0 || x >= level.GetWidth() || y >= level.GetHeight()) return false;
		return level[x + y * level.GetWidth()].GetSpriteId() == fillTileOfType;
	}

};

int main() {
	olcLevelMaker levelMaker;
	levelMaker.ConstructConsole(400, 200, 4, 4);
	levelMaker.Start();
}