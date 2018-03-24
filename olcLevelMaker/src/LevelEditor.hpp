#pragma once

#define WIDTH 400
#define HEIGHT 200
#define MENU_WIDTH 100
#define TILE_WIDTH 16

#include "olcConsoleGameEngine.hpp"

#include "olcMapFormat.hpp"
#include "ui\Popup.hpp"
#include "ui\Font.hpp"
#include "ui\Panel.hpp"
#include "ui\TextInput.hpp"

#include <unordered_map>
#include <type_traits>

namespace olcLevelEditor {
	
	using namespace olcMapFormat;
	using namespace ui;

	template<typename Base, typename T>
	inline bool instanceof(const T*) {
		return std::is_base_of<Base, T>::value;
	}

	class MenuTab {
	public:
		virtual void OnDrawTile(Panel* mapPanel) = 0;
		virtual void OnDrawMenu(Panel* menuPanel) = 0;
		virtual void AfterDraw() = 0;
	};

	class LevelEditor : public olcConsoleGameEngine {
	public:
		static LevelEditor * Get() {
			static LevelEditor instance;
			return &instance;
		}

	private:
		std::vector<MenuTab*> tabs;
		MenuTab* currentTab;
		Panel* menuPanel;
		Panel* mapPanel;
		Map* map = nullptr;
		Popup* popup = nullptr;
		bool destroyPopup = false;

		olcSprite spritesheet;
		float worldOffsetX;
		float worldOffsetY;

		Font* defaultFont;

	private:
		virtual bool OnUserCreate();
		virtual bool OnUserUpdate(float fElapsedTime);

		virtual bool OnUserDestroy() {
			// call it just incase
			this->~LevelEditor();
			return true;
		}

	public:
		inline MenuTab* GetCurrentTab() const { return currentTab; }
		
		void SetCurrentTab(int i) {
			currentTab = tabs[i];
		}

		template<class T>
		void RegisterTab() {
			if (std::is_default_constructible<T>()) {
				T t = new T();
				tabs.push_back(t);
			}
			else {
				static_assert(false, "MenuTab must have a default constructor");
			}
		}

		~LevelEditor() {
			for (MenuTab* tab : tabs) {
				delete tab;
			}
			tabs.clear();
		}

	public:
		void DrawStringFont(int x, int y, const std::string& text, Font* font = nullptr);
		void DrawStringFontCenter(int x, int y, const std::string& text, Font* font = nullptr);
		void DrawRect(int x, int y, int width, int height, wchar_t c = 0x2588, short col = 0x000F);

		bool Button(int x, int y, const std::string& text, bool centered = false, Font* font = nullptr, bool fromPopup = false);
		inline bool DoInput() { return popup == nullptr; }
		inline bool HasPopup() { return popup != nullptr && popup->IsFinished(); }

		inline Font* GetDefaultFont() { return this->defaultFont; }

		std::wstring OpenFileDialog(const wchar_t* title, const wchar_t* filter);
		std::wstring SaveFileDialog(const wchar_t* title, const wchar_t* filter, int* selectedIndex = nullptr);
	
		Tile* GetHoveredTile();
	private:
		void DrawMap();
		void DrawMainMenu();
	};

}