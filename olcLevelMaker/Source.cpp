#include "olcConsoleGameEngine.h"

#include <string>
#include <queue>

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
	LAST
};

class olcLevelMaker : public olcConsoleGameEngine {

	float mapMoveX, mapMoveY;
	SpriteSheet font;
	SpriteSheet tiles;
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
	
	void DrawStringFont(int x, int y, wstring characters) {
		for (wchar_t c : characters) {
			int index = this->characters.find(c);
			DrawSprite(x, y, font[index]);
			x += font.GetTileWidth();
		}
	}

	virtual bool OnUserCreate() {
		font.Load(FONT_SPRITESHEET, 8, 8);
		tiles.Load(TILE_SPRITESHEET, 16, 16);
		fillIcon.Load(L"fill.spr");

		level.Create(MAP_WIDTH, MAP_HEIGHT);
		for (int i = 0; i < level.GetWidth() * level.GetHeight(); i++) {
			level[i].SetSpriteId(DEFAULT_TILE);
		}

		uiWidth = 400 - uiBase;
		tilesPerRow = uiWidth / tiles.GetTileWidth();
		tilesPerColumn = (200 - 23) / tiles.GetTileHeight();
		tilesPerPage = tilesPerColumn * tilesPerRow;
		pageCount = tiles.GetTileCount() / tilesPerPage;

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
				DrawSprite(screenX, screenY, level[i].GetSprite(tiles));
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
		if (floodMode) {
			DrawSprite(2, 190, &fillIcon);
			iconOffset += 10;
		}

		// world movement
		if (m_keys[VK_UP].bPressed) {
			moved = true;
			worldOffsetY -= 16;
		}
		else if (m_keys[VK_DOWN].bPressed) {
			moved = true;
			worldOffsetY += 16;
		}
		else if (m_keys[VK_LEFT].bPressed) {
			moved = true;
			worldOffsetX -= 16;
		}
		else if (m_keys[VK_RIGHT].bPressed) {
			moved = true;
			worldOffsetX += 16;
		} else if (m_keys[L'S'].bPressed) {
			level.Save(LEVEL_FILE_NAME);
		}
		else if (m_keys[L'L'].bPressed) {
			level.Load(LEVEL_FILE_NAME);
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
		if (tileX >= 0 && tileY >= 0 && tileX < level.GetWidth() && tileY < level.GetHeight()) {
			// change the tile
			if (m_mouse[0].bPressed) {
				if (floodMode) {
					FloorFillSolid(tileX, tileY);
				}
				else {
					level[tileX + tileY * level.GetWidth()].SetSolid(!level[tileX + tileY * level.GetWidth()].IsSolid());
				}
			}
		}
	}

	void tilesTool(int tileX, int tileY) {

		// draw page
		wstring pageText(L"TILES:");
		if (pageCount == 0) {
			pageText.append(std::to_wstring(tiles.GetTileCount()));
		}
		else {
			pageText.append(std::to_wstring(page));
			pageText.append(L"/");
			pageText.append(std::to_wstring(pageCount));
			pageText.append(L"/");
			pageText.append(std::to_wstring(tiles.GetTileCount()));
		}
		DrawStringFont(uiBase + 5, 5, pageText);

		// draw sprites in menu
		int drawn = 0;
		int toDraw = min(tilesPerPage, tiles.GetTileCount() - tilesPerPage * page);
		for (int row = 0; row < tilesPerColumn; row++) {
			if (drawn >= toDraw) break;
			int y = 23 + row * tiles.GetTileHeight();
			for (int col = 0; col < tilesPerRow; col++) {
				if (drawn >= toDraw) break;
				int x = uiBase + col * tiles.GetTileWidth();
				DrawSprite(x, y, tiles[(col + row * tilesPerRow) + tilesPerPage * page]);
				drawn++;
			}
		}

		// draw selected sprite thing
		if (selectedSprite >= tilesPerPage * page && selectedSprite < tilesPerPage * page + tilesPerPage) {
			int col = selectedSprite % tilesPerRow;
			int row = (selectedSprite - col) / tilesPerRow;
			int y = 23 + row * tiles.GetTileHeight();
			int x = uiBase + col * tiles.GetTileWidth();
			DrawLine(x, y, x + 16, y, PIXEL_SOLID, BG_RED | FG_RED);
			DrawLine(x, y, x, y + 16, PIXEL_SOLID, BG_RED | FG_RED);
			DrawLine(x + 16, y, x + 16, y + 16, PIXEL_SOLID, BG_RED | FG_RED);
			DrawLine(x, y + 16, x + 16, y + 16, PIXEL_SOLID, BG_RED | FG_RED);
		}

		// are we in the selection menu
		if (m_mousePosX >= uiBase && m_mousePosX < uiBase + tilesPerRow * tiles.GetTileWidth() && m_mousePosY > 28) {
			int menuX = m_mousePosX - uiBase;
			int menuY = m_mousePosY - 28;
			int col = menuX / tiles.GetTileWidth();
			int row = menuY / tiles.GetTileHeight();
			int index = col + row * tilesPerRow;
			if (index < toDraw) {
				int y = 23 + row * tiles.GetTileHeight();
				int x = uiBase + col * tiles.GetTileWidth();
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
				if (floodMode) {
					FloodFillTile(tileX, tileY);
				}
				else {
					level[tileX + tileY * level.GetWidth()].SetSpriteId(selectedSprite);
				}
			}
		}

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