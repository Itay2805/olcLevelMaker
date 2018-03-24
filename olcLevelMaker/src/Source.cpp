
#include "LevelEditor.hpp"

int main() {
	using namespace olcLevelEditor;

	LevelEditor::Get()->ConstructConsole(WIDTH, HEIGHT, 4, 4);
	LevelEditor::Get()->Start();
}