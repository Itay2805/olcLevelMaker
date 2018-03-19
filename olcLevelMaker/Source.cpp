#include "olcConsoleGameEngine.h"

#include <string>
#include <queue>
#include <sstream>

#include "SpriteSheet.h"
#include "Level.h"

#define TILE_WIDTH 16
#define FONT_SPRITESHEET L"javidx9_nesfont8x8.spr"
#define TILE_SPRITESHEET L"loztheme.png.spr"
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

// embeded fill icon meta, should have a better way to store this...
string fillSpriteData = "8 8 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 15 9608 0 0 0 0 0 0 0 0 12 9608 12 9608 12 9608 12 9608 15 9608 0 0 0 0 0 0 12 9608 15 9608 12 9608 12 9608 12 9608 15 9608 0 0 0 0 12 9608 0 9608 15 9608 12 9608 12 9608 12 9608 15 9608 0 0 12 9608 0 9608 0 9608 15 9608 12 9608 15 9608 0 0 0 0 12 9608 0 0 0 9608 0 9608 15 9608 0 9608 0 0 0 0 0 0 0 0 0 0 0 9608 0 9608 0 0 0 0";

enum class Popup {
	NONE,
	NEW_MAP_SIZE,
};

struct popup_t {
	Popup popup = Popup::NONE;
	bool menuActive = false;
	struct {
		wstring width;
		wstring height;
		int field; /// 0 - width, 1 - height
	} newMapSize;

	popup_t() {

	}

	~popup_t() {

	}
};

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

	wstring file = L"";
	wstring spriteSheetFile = TILE_SPRITESHEET;

	popup_t popup;

	void DrawStringFont(int x, int y, const wstring& characters) {
		// will use ascii
		for (wchar_t c : characters) {
			// int index = this->characters.find(c);
			if (c - ' ' >= font.GetTileCount()) {
				continue;
			}
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

		// search for the default sprite sheet
		fstream spriteFile(TILE_SPRITESHEET);
		if (!spriteFile.good()) {
			ImportSpriteSheet();
		}
		else {
			level.LoadSpriteSheet(TILE_SPRITESHEET, TILE_WIDTH);
			tiles = level.GetSpriteSheet();
		}
		if (spriteFile.is_open()) {
			spriteFile.close();
		}

		uiWidth = 400 - uiBase;
		tilesPerRow = uiWidth / tiles->GetTileWidth();
		tilesPerColumn = (200 - 23) / tiles->GetTileHeight();
		tilesPerPage = tilesPerColumn * tilesPerRow;
		pageCount = (tiles->GetTileCount() / tilesPerPage) + 1;

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

		if (popup.menuActive) {
			switch (popup.popup) {
			case Popup::NEW_MAP_SIZE: {
					static bool blink = false;
					Fill(110, 70, 290, 150, ' ', FG_DARK_GREY | BG_DARK_GREY);
					DrawStringFont(150, 75, L"New Map Size");
					DrawStringFont(125, 90, L"Width");
					DrawStringFont(210, 90, L"Height");
					Fill(170, 130, 240, 140, ' ', FG_GREY | BG_GREY);
					DrawStringFont(180, 131, L"Create");
					switch (popup.newMapSize.field) {
					case 0:
						Fill(125, 100, 200, 108, ' ', FG_GREY | BG_GREY);
						Fill(210, 100, 280, 108, ' ', FG_BLACK | BG_BLACK);
						if (IsFocused()) {
							if (GetKey(L'0').bPressed || GetKey(VK_NUMPAD0).bPressed) popup.newMapSize.width.append(L"0");
							if (GetKey(L'1').bPressed || GetKey(VK_NUMPAD1).bPressed) popup.newMapSize.width.append(L"1");
							if (GetKey(L'2').bPressed || GetKey(VK_NUMPAD2).bPressed) popup.newMapSize.width.append(L"2");
							if (GetKey(L'3').bPressed || GetKey(VK_NUMPAD3).bPressed) popup.newMapSize.width.append(L"3");
							if (GetKey(L'4').bPressed || GetKey(VK_NUMPAD4).bPressed) popup.newMapSize.width.append(L"4");
							if (GetKey(L'5').bPressed || GetKey(VK_NUMPAD5).bPressed) popup.newMapSize.width.append(L"5");
							if (GetKey(L'6').bPressed || GetKey(VK_NUMPAD6).bPressed) popup.newMapSize.width.append(L"6");
							if (GetKey(L'7').bPressed || GetKey(VK_NUMPAD7).bPressed) popup.newMapSize.width.append(L"7");
							if (GetKey(L'8').bPressed || GetKey(VK_NUMPAD8).bPressed) popup.newMapSize.width.append(L"8");
							if (GetKey(L'9').bPressed || GetKey(VK_NUMPAD9).bPressed) popup.newMapSize.width.append(L"9");
							if (GetKey(VK_BACK).bPressed) if(popup.newMapSize.width.length() != 0) popup.newMapSize.width = popup.newMapSize.width.substr(0, popup.newMapSize.width.size() - 1);
						}
						break;
					case 1:
						Fill(125, 100, 200, 108, ' ', FG_BLACK | BG_BLACK);
						Fill(210, 100, 280, 108, ' ', FG_GREY | BG_GREY);
						if (IsFocused()) {
							if (GetKey(L'0').bPressed || GetKey(VK_NUMPAD0).bPressed) popup.newMapSize.height.append(L"0");
							if (GetKey(L'1').bPressed || GetKey(VK_NUMPAD1).bPressed) popup.newMapSize.height.append(L"1");
							if (GetKey(L'2').bPressed || GetKey(VK_NUMPAD2).bPressed) popup.newMapSize.height.append(L"2");
							if (GetKey(L'3').bPressed || GetKey(VK_NUMPAD3).bPressed) popup.newMapSize.height.append(L"3");
							if (GetKey(L'4').bPressed || GetKey(VK_NUMPAD4).bPressed) popup.newMapSize.height.append(L"4");
							if (GetKey(L'5').bPressed || GetKey(VK_NUMPAD5).bPressed) popup.newMapSize.height.append(L"5");
							if (GetKey(L'6').bPressed || GetKey(VK_NUMPAD6).bPressed) popup.newMapSize.height.append(L"6");
							if (GetKey(L'7').bPressed || GetKey(VK_NUMPAD7).bPressed) popup.newMapSize.height.append(L"7");
							if (GetKey(L'8').bPressed || GetKey(VK_NUMPAD8).bPressed) popup.newMapSize.height.append(L"8");
							if (GetKey(L'9').bPressed || GetKey(VK_NUMPAD9).bPressed) popup.newMapSize.height.append(L"9");
							if (GetKey(VK_BACK).bPressed) if (popup.newMapSize.height.length() != 0) popup.newMapSize.height = popup.newMapSize.height.substr(0, popup.newMapSize.height.size() - 1);
						}
						break;
					default:
						popup.newMapSize.field = 0;
					}
					DrawStringFont(125, 100, popup.newMapSize.width);
					DrawStringFont(210, 100, popup.newMapSize.height);
					if (m_mouse[0].bPressed) {
						if (m_mousePosX > 125 && m_mousePosX < 200 && m_mousePosY > 100 && m_mousePosY < 108) {
							popup.newMapSize.field = 0;
						}
						if (m_mousePosX > 210 && m_mousePosX < 280 && m_mousePosY > 100 && m_mousePosY < 108) {
							popup.newMapSize.field = 1;
						}
						if (m_mousePosX > 170 && m_mousePosX < 240 && m_mousePosY > 130 && m_mousePosY < 140) {
							// Create the map
							popup.popup = Popup::NONE;
							popup.menuActive = false;
							int width = stoi(popup.newMapSize.width);
							int height = stoi(popup.newMapSize.height);
							if (width != 0 && height != 0) {
								popup.newMapSize.width = L"";
								popup.newMapSize.height = L"";
								level.Create(width, height);
								for (int i = 0; i < level.GetWidth() * level.GetHeight(); i++) {
									level[i].SetLevel(&level);
									level[i].SetSpriteId(DEFAULT_TILE);
								}
								level.LoadSpriteSheet(spriteSheetFile, TILE_WIDTH);
								tiles = level.GetSpriteSheet();
								file = L"";
							}
						}
					}
				}
				break;
			}
		}
		else if(IsFocused()) {
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
			if (GetKey(L'W').bPressed || (GetKey(VK_SHIFT).bHeld && GetKey(L'W').bHeld)) {
				moved = true;
				worldOffsetY += 16;
			}
			if ((!m_keys[VK_CONTROL].bHeld && GetKey(L'S').bPressed) || (GetKey(VK_SHIFT).bHeld && GetKey(L'S').bHeld)) {
				moved = true;
				worldOffsetY -= 16;
			}
			if (GetKey(L'A').bPressed || (GetKey(VK_SHIFT).bHeld && GetKey(L'A').bHeld)) {
				moved = true;
				worldOffsetX += 16;
			}
			if (GetKey(L'D').bPressed || (GetKey(VK_SHIFT).bHeld && GetKey(L'D').bHeld)) {
				moved = true;
				worldOffsetX -= 16;
			}
			if (m_keys[VK_CONTROL].bHeld && m_keys[L'S'].bPressed) {
				if (file.length() == 0) {
					SaveLevel();
				}
				else {
					level.Save(file);
				}
			}
			if (m_keys[VK_CONTROL].bHeld && m_keys[L'L'].bPressed) {
				LoadLevel();
			}
			if (m_keys[L'T'].bPressed) {
				tool = (Tool)((int)tool + 1);
				if (tool == Tool::LAST) {
					tool = Tool::TILES;
				}
			}
			if (m_keys[L'G'].bPressed) {
				grid = !grid;
			}
			if (m_keys[L'F'].bPressed) {
				floodMode = !floodMode;
			}
			if (m_keys[VK_LEFT].bPressed) {
				if (pageCount != 0) {
					page--;
					if (page < 0) {
						page = 0;
					}
				}
			}
			if (m_keys[VK_RIGHT].bPressed) {
				if (pageCount != 0) {
					page++;
					if (page >= pageCount) {
						page = pageCount - 1;
					}
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
		if (tileX >= 0 && tileY >= 0 && tileX < level.GetWidth() && tileY < level.GetHeight() && !popup.menuActive) {
			switch (selectedMetaTool) {
			case MetaTools::SOLID_BRUSH:
				{
					// change the tile
					if (m_mouse[0].bHeld) {
						if (floodMode || m_keys[VK_CONTROL].bHeld) {
							FloorFillSolid(tileX, tileY, true);
						}
						else {
							level[tileX + tileY * level.GetWidth()].SetSolid(true);
						}
					}
					else if (m_mouse[1].bHeld) {
						if (floodMode || m_keys[VK_CONTROL].bHeld) {
							FloorFillSolid(tileX, tileY, false);
						}
						else {
							level[tileX + tileY * level.GetWidth()].SetSolid(false);
						}
					}
				}
				break;
			}
		}
	}

	void exportAndImportTool() {
		DrawStringFont(uiBase + 6, 5, L"NEW");
		DrawStringFont(uiBase + 1, 25, L"IMPORT:");
		DrawStringFont(uiBase + 6, 35, L"LEVEL");
		DrawStringFont(uiBase + 6, 45, L"SPRITESHEET");
		DrawStringFont(uiBase + 1, 75, L"EXPORT:");
		DrawStringFont(uiBase + 6, 85, L"LEVEL");
		DrawStringFont(uiBase + 6, 95, L"SPRITE");
		if (popup.menuActive) return;
		if (m_mouse[0].bPressed) {
			// new
			if (m_mousePosX > uiBase + 6 && m_mousePosX < 400 && m_mousePosY > 5 && m_mousePosY < 5 + 8) {
				popup.menuActive = true;
				popup.popup = Popup::NEW_MAP_SIZE;
			}
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
			spriteSheetFile = filename;
			level.LoadSpriteSheet(filename, 16);
			tiles = level.GetSpriteSheet();
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
			pageText.append(std::to_wstring(page + 1));
			pageText.append(L"/");
			pageText.append(std::to_wstring(pageCount));
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
			row = row - page * tilesPerColumn;
			int y = 23 + row * tiles->GetTileHeight();
			int x = uiBase + col * tiles->GetTileWidth();
			DrawLine(x, y, x + 16, y, PIXEL_SOLID, BG_RED | FG_RED);
			DrawLine(x, y, x, y + 16, PIXEL_SOLID, BG_RED | FG_RED);
			DrawLine(x + 16, y, x + 16, y + 16, PIXEL_SOLID, BG_RED | FG_RED);
			DrawLine(x, y + 16, x + 16, y + 16, PIXEL_SOLID, BG_RED | FG_RED);
		}

		// are we in the selection menu
		if (GetMouseX() >= uiBase && GetMouseX() < uiBase + tilesPerRow * tiles->GetTileWidth() && GetMouseY() > 28 && !popup.menuActive) {
			int menuX = GetMouseX() - uiBase;
			int menuY = GetMouseY() - 28;
			int col = menuX / tiles->GetTileWidth();
			int row = menuY / tiles->GetTileHeight();
			int index = (col + row * tilesPerRow) + page * tilesPerPage;
			int y = 23 + row * tiles->GetTileHeight();
			int x = uiBase + col * tiles->GetTileWidth();
			DrawLine(x, y, x + 16, y, PIXEL_SOLID, BG_RED | FG_DARK_RED);
			DrawLine(x, y, x, y + 16, PIXEL_SOLID, BG_RED | FG_DARK_RED);
			DrawLine(x + 16, y, x + 16, y + 16, PIXEL_SOLID, BG_RED | FG_DARK_RED);
			DrawLine(x, y + 16, x + 16, y + 16, PIXEL_SOLID, BG_RED | FG_DARK_RED);
			if (m_mouse[0].bPressed) {
				selectedSprite = index;
			}
		}

		// are we in the world editor
		if (tileX >= 0 && tileY >= 0 && tileX < level.GetWidth() && tileY < level.GetHeight() && !popup.menuActive && GetMouseX() <= 300) {
			// change the tile
			if (m_mouse[0].bHeld) {
				if (floodMode || m_keys[VK_CONTROL].bHeld) {
					FloodFillTile(tileX, tileY);
				}
				else {
					level[tileX + tileY * level.GetWidth()].SetSpriteId(selectedSprite);
				}
			}
			else if (m_mouse[1].bPressed) {
				selectedSprite = level[tileX + tileY * level.GetWidth()].GetSpriteId();
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

	void FloorFillSolid(int x, int y, bool fill) {
		fillTileOfType = level[x + y * level.GetWidth()].GetSpriteId();
		solidStart = fill;
		queue<pair<int, int>> q;
		q.push(pair<int, int>(x, y));
		while (q.size() != 0) {
			pair<int, int> xy = q.front();
			q.pop();
			level[xy.first + xy.second * level.GetWidth()].SetSolid(fill);
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