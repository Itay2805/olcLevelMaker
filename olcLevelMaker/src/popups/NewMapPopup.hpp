#pragma once

#include "..\ui\Popup.hpp"
#include "..\ui\TextInput.hpp"

#include "..\LevelEditor.hpp"

namespace olcLevelEditor {

	class NewMapPopup : public ui::Popup {
	private:
		ui::Panel panel = ui::Panel(110, 70, 180, 80);
		ui::TextInput name = ui::TextInput(10, 15, 20, &panel, LevelEditor::Get()->GetDefaultFont());
		ui::TextInput width = ui::TextInput(15, 30, 5, &panel, LevelEditor::Get()->GetDefaultFont());
		ui::TextInput height = ui::TextInput(180 - 15 - 5 * 8, 30, 5, &panel, LevelEditor::Get()->GetDefaultFont());

	public:
		NewMapPopup()
			: ui::Popup(&panel)
		{
			width.AllowCharacters(false);
			width.AllowSpecial(false);
			height.AllowCharacters(false);
			height.AllowSpecial(false);
		}

		~NewMapPopup() {
		}

		virtual void OnDraw() {
			panel.Fill(0, 0, panel.GetWidth(), panel.GetHeight(), ' ', BG_DARK_GREY);
			panel.DrawStringFontCenter(90, 5, "New Map");

			name.Draw();
			width.Draw();
			height.Draw();
			if (panel.Button(65, 50, "Create")) {
				finished = true;
			}
		}

		int GetWidth() {
			return std::atoi(width.GetText().c_str());
		}

		int GetHeight() {
			return std::atoi(height.GetText().c_str());
		}

		const std::string& GetName() {
			return name.GetText();
		}
	};

}