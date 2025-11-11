#include <iostream>
#include "Game.h"
using namespace std;

int main() {
    try {
        Game game;      // init() se llama automáticamente dentro del constructor
        game.run();     // arranca el bucle principal
    }
    catch (const exception& e) {
        cerr << "[FATAL ERROR] " << e.what() << endl;
        cerr << "Presiona Enter para salir..." << endl;
        cin.get();
        return 1;
    }
    return 0;
}
