#include <Windows.h>
#include <winbase.h>
#include <iostream>

int main() {
	std::cout << "pc won't enable screensaver while this window is open" << std::endl;
	SetThreadExecutionState(ES_DISPLAY_REQUIRED);
}