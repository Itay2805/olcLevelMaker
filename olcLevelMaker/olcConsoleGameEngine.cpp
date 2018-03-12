#include "olcConsoleGameEngine.h"

atomic<bool> olcConsoleGameEngine::m_bAtomActive = false;
condition_variable olcConsoleGameEngine::m_cvGameFinished;
mutex olcConsoleGameEngine::m_muxGame;