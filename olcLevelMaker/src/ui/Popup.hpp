#pragma once

#include "Panel.hpp"

namespace olcLevelEditor {
	namespace ui {

		class Popup {
		protected:
			Panel * panel;
			bool finished = false;

		public:
			Popup(Panel* panel) 
				: panel(panel)
			{

			}

			virtual void OnDraw() = 0;

			inline Panel* GetPanel() { return panel; }
			inline bool IsFinished() { return finished; }

		};

	}
}