#include "olcConsoleGameEngine.h"

#include <string>

#include "SpriteSheet.h"
#include "Level.h"

#define TILE_WIDTH 16
#define FONT_SPRITESHEET L"javidx9_nesfont8x8.spr"
#define TILE_SPRITESHEET L"toml_spritesheetdark.spr"
#define MAP_WIDTH 10
#define MAP_HEIGHT 10
#define LEVEL_FILE_NAME L"level.lvl"

class olcLevelMaker : public olcConsoleGameEngine {

	float mapMoveX, mapMoveY;
	SpriteSheet font;
	SpriteSheet tiles;
	wstring characters = L" !\"#$%&'()*+,-./0123456789:;<=>?@ABCDEFGHIJKLMNOPQRSTUVWXYZ[\\]^_`abcdefgijklmnopqrstuvwxyz{|}~";

	Level level;
	int page = 0;
	int selectedSprite = 0;
	int pageCount = 1;
	int uiBase = 300, tilesPerRow, tilesPerColumn, tilesPerPage, uiWidth;
	int worldOffsetX = 0, worldOffsetY = 0;
	
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

		level.Create(MAP_WIDTH, MAP_HEIGHT);
		for (int i = 0; i < level.GetWidth() * level.GetHeight(); i++) {
			level[i].SetSpriteId(14);
		}

		uiWidth = 400 - uiBase;
		tilesPerRow = uiWidth / tiles.GetTileWidth();
		tilesPerColumn = (200 - 23) / tiles.GetTileHeight();
		tilesPerPage = tilesPerColumn * tilesPerRow;
		pageCount = tiles.GetTileCount() / tilesPerPage;

		return true;
	}

	virtual bool OnUserUpdate(float fElapsedTime) {
		int tileX = m_mousePosX / 16;
		int tileY = m_mousePosY / 16;
		// simple
		tileX -= worldOffsetX / 16;
		tileY -= worldOffsetY / 16;

		Fill(0, 0, 400, 200, ' ', BG_WHITE | FG_WHITE);

		// DrawSprite(0, 0, font[16]);

		for (int i = 0; i < level.GetWidth() * level.GetHeight(); i++) {
			int x = i % level.GetWidth();
			int y = (i - x) / level.GetWidth();
			if (x < 0 || y < 0) {
				continue;
			}
			DrawSprite(x * TILE_WIDTH + worldOffsetX, y * TILE_WIDTH + worldOffsetY, level[i].GetSprite(tiles));
		}

		Fill(uiBase, 0, 400, 200, ' ', BG_GREY | FG_BLACK);
		
		// draw page
		wstring pageText(L"PAGE: ");
		pageText.append(std::to_wstring(page));
		pageText.append(L"/");
		pageText.append(std::to_wstring(pageCount));
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
		
		if (selectedSprite >= tilesPerPage * page && selectedSprite < tilesPerPage * page + tilesPerPage) {
			// draw selected sprite thing
			int col = selectedSprite % tilesPerRow;
			int row = (selectedSprite - col) / tilesPerRow;
			int y = 23 + row * tiles.GetTileHeight();
			int x = uiBase + col * tiles.GetTileWidth();
			DrawLine(x, y, x + 16, y, PIXEL_SOLID, BG_RED | FG_RED);
			DrawLine(x, y, x, y + 16, PIXEL_SOLID, BG_RED | FG_RED);
			DrawLine(x + 16,y, x + 16, y + 16, PIXEL_SOLID, BG_RED | FG_RED);
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
		// will need to change to if to work for moving world
		if (tileX >= 0 && tileY >= 0 && tileX < level.GetWidth() && tileY < level.GetHeight()) {
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
			DrawLine(tileX * 16 + worldOffsetX, tileY * 16 + worldOffsetY, tileX * 16 + 16 + worldOffsetX, tileY * 16 + worldOffsetY, PIXEL_SOLID, BG_BLACK | FG_WHITE);
			DrawLine(tileX * 16 + worldOffsetX, tileY * 16 + worldOffsetY, tileX * 16 + worldOffsetX, tileY * 16 + 16 + worldOffsetY, PIXEL_SOLID, BG_BLACK | FG_WHITE);
			DrawLine(tileX * 16 + 16 + worldOffsetX, tileY * 16 + worldOffsetY, tileX * 16 + 16 + worldOffsetX, tileY * 16 + 16 + worldOffsetY, PIXEL_SOLID, BG_BLACK | FG_WHITE);
			DrawLine(tileX * 16 + worldOffsetX, tileY * 16 + 16 + worldOffsetY, tileX * 16 + 16 + worldOffsetX, tileY * 16 + 16 + worldOffsetY, PIXEL_SOLID, BG_BLACK | FG_WHITE);
			
			// change the tile
			if (m_mouse[0].bHeld) {
				level[tileX + tileY * level.GetWidth()].SetSpriteId(selectedSprite);
			}
		}

		// world movement
		if (m_keys[VK_UP].bPressed) {
			worldOffsetY -= 16;
		}
		else if (m_keys[VK_DOWN].bPressed) {
			worldOffsetY += 16;
		}
		else if (m_keys[VK_LEFT].bPressed) {
			worldOffsetX -= 16;
		}
		else if (m_keys[VK_RIGHT].bPressed) {
			worldOffsetX += 16;
		} else if (m_keys[L'S'].bPressed) {
			level.Save(LEVEL_FILE_NAME);
		}
		else if (m_keys[L'L'].bPressed) {
			level.Load(LEVEL_FILE_NAME);
		}

		return true;
	}

};

int main() {
	olcLevelMaker levelMaker;
	levelMaker.ConstructConsole(400, 200, 4, 4);
	levelMaker.Start();
}