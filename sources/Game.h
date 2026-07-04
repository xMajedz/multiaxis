#include "raylib.h"

class Game {
public:	
    static Game& GetInstance()
    {
        static Game instance;
        return instance;
    }

    void Draw();
	
	Game(const Game&) = delete;
    Game& operator=(const Game&) = delete;
private:
	Game();
	~Game();
};
