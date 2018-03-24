#pragma once

#include "../ui/Popup.hpp"

#include "../LevelEditor.hpp"

namespace olcLevelEditor {

	class ErrorPopup : public ui::Popup {
	private:
		ui::Panel panel = ui::Panel(0, 0, 400, 200);
		std::string text;
	public:
		ErrorPopup(const std::string& text)
			: ui::Popup(&panel)
			, text(text)
		{

		}

		virtual void OnDraw() {
			panel.Fill(0, 0, panel.GetWidth(), panel.GetHeight(), ' ', BG_DARK_GREY);
			panel.DrawStringFontCenter(panel.GetWidth() / 2, 5, "Error");
			panel.DrawStringFontCenter(0, 0, text);
			if (panel.Button(64, 50, "Okey")) {
				finished = true;
			}
		}
	};

}
