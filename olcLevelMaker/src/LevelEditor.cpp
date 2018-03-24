#include "LevelEditor.hpp"

#include "ui\Panel.hpp"

#include "popups\NewMapPopup.hpp"
#include "popups\ErrorPopup.hpp"

namespace olcLevelEditor {
	
	using namespace ui;

	bool LevelEditor::OnUserCreate() {
		mapPanel = new Panel(0, 0, WIDTH - MENU_WIDTH, HEIGHT);
		menuPanel = new Panel(WIDTH - MENU_WIDTH, 0, MENU_WIDTH, HEIGHT);
		
		defaultFont = new Font(L"javidx9_nesfont8x8.spr", 8);

		return true;
	}
	
	bool LevelEditor::OnUserUpdate(float fElapsedTime) {

		if (map != nullptr) {
			// editor
			DrawMap();

			// draw menu
			menuPanel->Fill(0, 0, WIDTH, HEIGHT, ' ', BG_GREY);
			// currentTab->OnDrawMenu(menuPanel);
		}
		else {
			// show main menu
			DrawMainMenu();
		}


		if (popup != nullptr && popup->IsFinished() && destroyPopup) {
			destroyPopup = false;
			delete popup;
			popup = nullptr;
		}

		if (popup != nullptr) {

			popup->OnDraw();

			if (popup->IsFinished()) {
				destroyPopup = true;
			}

		}


		return true;
	}

	void LevelEditor::DrawMap() {
		mapPanel->Fill(0, 0, mapPanel->GetWidth(), mapPanel->GetHeight(), ' ', BG_BLACK);

		int topX = worldOffsetX > 0 ? (worldOffsetX) / TILE_WIDTH : 0;
		int topY = worldOffsetY > 0 ? (worldOffsetY) / TILE_WIDTH : 0;
		if (map->TileExists(topX, topY)) {
			Tile* topTile = map->GetTile(topX, topY);

			int minWidth = min((WIDTH - MENU_WIDTH) / TILE_WIDTH, map->GetWidth());
			int minHeight = min(HEIGHT / TILE_WIDTH, map->GetHeight());

			int spritesheetWidth = spritesheet.nWidth / TILE_WIDTH;

			for (int my = 0; my < minHeight; my++) {
				for (int mx = 0; mx < minWidth; mx++) {
					if (!map->TileExists(topTile->GetX() + mx, topTile->GetY() + my)) continue;
					Tile* currentTile = map->GetTile(topTile->GetX() + mx, topTile->GetY() + my);
					int screenX = currentTile->GetX() * TILE_WIDTH - worldOffsetX;
					int screenY = currentTile->GetY() * TILE_WIDTH - worldOffsetY;
					int ox = currentTile->GetID() % spritesheetWidth;
					int oy = currentTile->GetID() / spritesheetWidth;
					DrawPartialSprite(screenX, screenY, &spritesheet, ox, oy, TILE_WIDTH, TILE_WIDTH);
				}
			}
		}
		if (DoInput()) {
			// draw hovered tile/selection
			Tile* hoveredTile = GetHoveredTile();
			if (hoveredTile != nullptr) {
				int tileX = hoveredTile->GetX();
				int tileY = hoveredTile->GetY();

				int screenX = tileX * TILE_WIDTH - worldOffsetX;
				int screenY = tileY * TILE_WIDTH - worldOffsetY;

				DrawRect(screenX, screenY, TILE_WIDTH, TILE_WIDTH);
			}

			if (GetKey('W').bHeld) worldOffsetY += 1 * (GetKey(VK_SHIFT).bHeld ? 2 : 1);
			if (GetKey('S').bHeld) worldOffsetY -= 1 * (GetKey(VK_SHIFT).bHeld ? 2 : 1);
			if (GetKey('A').bHeld) worldOffsetX += 1 * (GetKey(VK_SHIFT).bHeld ? 2 : 1);
			if (GetKey('D').bHeld) worldOffsetX -= 1 * (GetKey(VK_SHIFT).bHeld ? 2 : 1);
		}
	}

	void LevelEditor::DrawMainMenu() {
		Fill(0, 0, WIDTH, HEIGHT, ' ', BG_BLACK);

		// render
		DrawStringFontCenter(WIDTH / 2, 20, "olcLevelEditor");
		if (Button(WIDTH / 2 - 20, 40, "New Map")) {
			popup = new NewMapPopup();
		}
		if (Button(WIDTH / 2 - 20, 60, "Load Map")) {
			std::wstring mapFile = OpenFileDialog(L"Load Map", L"sprmap (*.map)\0*.map\0level (*.lvl)\0*.lvl\0");
			if (mapFile.length() != 0) {
				this->map = new Map();
				const char* error = this->map->LoadFromFile(mapFile);
				if (error != nullptr) {
					std::wstring spritesheet = OpenFileDialog(L"Load Sprite Sheet", L"olcSprite (*.spr)\0*.spr\0");
					if (spritesheet.length() != 0) {
						this->spritesheet.Load(spritesheet);
					}
					else {
						popup = new ErrorPopup("Please choose spritesheet to load");
					}
				}
				else {
					popup = new ErrorPopup(error);
				}
			}
			else {
				popup = new ErrorPopup("Please choose map to load");
			}
		}

		// check for popup
		if (HasPopup()) {
			// only if NewMapPopup
			NewMapPopup* newMapPopup = dynamic_cast<NewMapPopup*>(popup);
			if (newMapPopup != nullptr) {
				std::wstring spritesheet = OpenFileDialog(L"Load Sprite Sheet", L"olcSprite (*.spr)\0*.spr\0");
				if (spritesheet.length() != 0) {
					this->spritesheet.Load(spritesheet);
					map = new Map(newMapPopup->GetWidth(), newMapPopup->GetHeight(), newMapPopup->GetName());
					map->EmptyMap();
				}
				else {
					popup = new ErrorPopup("Please choose spritesheet to load");
				}
			}
		}
	}

	std::wstring LevelEditor::OpenFileDialog(const wchar_t* title, const wchar_t* filter) {
		wchar_t filename[MAX_PATH];
		OPENFILENAME ofn;
		ZeroMemory(&filename, sizeof(filename));
		ZeroMemory(&ofn, sizeof(ofn));
		ofn.lStructSize = sizeof(ofn);
		ofn.hwndOwner = NULL;
		ofn.lpstrFile = filename;
		ofn.nMaxFile = MAX_PATH;
		ofn.Flags = OFN_FILEMUSTEXIST;
		ofn.lpstrFilter = filter;
		ofn.lpstrTitle = title;
		if (GetOpenFileName(&ofn)) {
			return std::wstring(filename);
		}
		else {
			return L"";
		}
	}

	std::wstring LevelEditor::SaveFileDialog(const wchar_t* title, const wchar_t* filter, int* selectedIndex) {
		wchar_t filename[MAX_PATH];
		OPENFILENAME ofn;
		ZeroMemory(&filename, sizeof(filename));
		ZeroMemory(&ofn, sizeof(ofn));
		ofn.lStructSize = sizeof(ofn);
		ofn.hwndOwner = NULL;
		ofn.lpstrFile = filename;
		ofn.nMaxFile = MAX_PATH;
		ofn.Flags = OFN_FILEMUSTEXIST;
		ofn.lpstrFilter = filter;
		ofn.lpstrTitle = title;
		if (GetSaveFileName(&ofn)) {
			if(selectedIndex != nullptr) *selectedIndex = ofn.nFilterIndex;
			return std::wstring(filename);
		}
		else {
			return L"";
		}
	}

#pragma region Drawing utils

	void LevelEditor::DrawStringFont(int x, int y, const std::string& text, Font* font) {
		if (font == nullptr) {
			font = defaultFont;
		}

		font->Draw(this, x, y, text);
	}

	void LevelEditor::DrawStringFontCenter(int x, int y, const std::string& text, Font* font) {
		if (font == nullptr) {
			font = defaultFont;
		}

		font->Draw(this, x - (text.length() / 2) * font->GetWidth(), y, text);
	}

	void LevelEditor::DrawRect(int x, int y, int width, int height, wchar_t c, short col) {
		DrawLine(x, y, x + width, y, c, col);
		DrawLine(x, y, x, y + height, c, col);
		DrawLine(x + width, y, x + width, y + height, c, col);
		DrawLine(x, y + height, x + width, y + width, c, col);
	}

#pragma endregion

#pragma region Widgets

	bool LevelEditor::Button(int x, int y, const std::string& text, bool centered, Font* font, bool fromPopup) {
		if (font == nullptr) {
			font = defaultFont;
		}
		bool pressed = GetMouseX() >= x && GetMouseY() >= y && GetMouseX() <= x + text.length() * font->GetWidth() + 3 && GetMouseY() <= y + font->GetWidth() + 3 && GetMouse(0).bPressed;
		Fill(x, y, x + text.length() * font->GetWidth() + 3, y + font->GetWidth() + 3, PIXEL_SOLID, (pressed ? FG_WHITE : FG_BLACK) | BG_GREY);
		DrawStringFont(x + 2, y + 2, text, font);
		if (popup != nullptr && !fromPopup) {
			return false;
		}
		return pressed;
	}

#pragma endregion

#pragma region Utils

	Tile* LevelEditor::GetHoveredTile() {
		if (GetMouseX() >= WIDTH - MENU_WIDTH) {
			return nullptr;
		}
		int x = (GetMouseX() + worldOffsetX) / TILE_WIDTH;
		if (x >= map->GetWidth()) return nullptr;
		int y = (GetMouseY() + worldOffsetY) / TILE_WIDTH;
		if (y >= map->GetHeight()) return nullptr;
		return map->GetTile(x, y);
	}

#pragma endregion


}

