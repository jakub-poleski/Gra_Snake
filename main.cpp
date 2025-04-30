#include <SFML/Graphics.hpp>
#include <fstream>
#include <iostream>
#include <vector>
#include <time.h>

using namespace sf;
//ten kod dodaje dla testu
enum GameState { MENU, GRA, USTAWIENIA, WYJSCIE, GAMEOVER };
Font globalFont;
void initializeText(Text& text, const Font& font, const std::string& content, int size, float x, float y, Color color = Color::Black)   //funkcja do tekstu
{
    text.setFont(font);
    text.setString(content);
    text.setCharacterSize(size);
    text.setPosition(x, y);
    text.setFillColor(color);
}
Texture backgroundTexture;
Sprite backgroundSprite;

class Fruit
{
public:
    int x, y;
    Fruit(int boardwidth, int boardheight)
    {
        respawn(boardwidth, boardheight, {});
    }
    void respawn(int boardwidth, int boardheight, const std::vector<Vector2i>& snakeBody) //pojawianie sie owocka
    {
        do
        {
            x = rand() % boardwidth;
            y = rand() % boardheight;
        }
        while (std::find(snakeBody.begin(), snakeBody.end(), Vector2i(x, y)) != snakeBody.end()); // zabezpieczenie
    }
};


class Snake
{
private:
    std::vector<Vector2i> body;
    int direction; // 0: Dół, 1: Lewo, 2: Prawo, 3: Góra

public:
    Snake(int startX, int startY)
    {
        direction = 2;
        body.push_back({startX, startY});
        body.push_back({startX - 1, startY});
        body.push_back({startX - 2, startY});
        body.push_back({startX - 3, startY});
    }

    void grow()
    {
        body.push_back(body.back());
    }

    void move(int boardwidth, int boardheight)
    {
        for (int i = body.size() - 1; i > 0; --i)
        {
            body[i] = body[i - 1];
        }
        if (direction == 0) body[0].y += 1; // 0: Dół, 1: Lewo, 2: Prawo, 3: Góra
        if (direction == 1) body[0].x -= 1;
        if (direction == 2) body[0].x += 1;
        if (direction == 3) body[0].y -= 1;

        if (body[0].x >= boardwidth) body[0].x = 0;
        if (body[0].x < 0) body[0].x = boardwidth - 1;
        if (body[0].y >= boardheight) body[0].y = 0;
        if (body[0].y < 0) body[0].y = boardheight - 1;
    }
    bool checkCollision()
    {
        for (size_t i = 1; i < body.size(); ++i)
        {
            if (body[0] == body[i])
                return true;
        }
        return false;
    }

    void setDirection(int newDirection)
    {
        if ((direction == 0 && newDirection != 3) ||
            (direction == 1 && newDirection != 2) ||
            (direction == 2 && newDirection != 1) ||
            (direction == 3 && newDirection != 0))
        {
            direction = newDirection;
        }
    }

    const std::vector<Vector2i>& getBody() const
    {
        return body;
    }

    Vector2i getHead() const
    {
        return body[0];
    }
};

class Menu
{
public:
    Text title, options[3];
    int selectedOption;
    Menu() : selectedOption(0)
    {
        initializeText(title, globalFont, "GRA SNAKE", 70, 200, 100);
        std::string menuTexts[] = {"GRAJ", "USTAWIENIA", "WYJSCIE"};
        for (int i = 0; i < 3; ++i)
        {
            initializeText(options[i], globalFont, menuTexts[i], 50, 200, 200 + i * 50);
        }
    }

    void render(RenderWindow& window)
    {
        window.clear(Color::Black);
        window.draw(backgroundSprite);
        window.draw(title);
        for (int i = 0; i < 3; ++i)
        {
            if (i == selectedOption)
            {
                options[i].setFillColor(Color::Red);
            }
            else
            {
                options[i].setFillColor(Color::Black);
            }
            window.draw(options[i]);
        }
        window.display();
    }

    GameState handleInput(RenderWindow& window)
    {
        Event event;
        while (window.pollEvent(event))
        {
            if (event.type == Event::Closed) window.close();
            if (Keyboard::isKeyPressed(Keyboard::Up))
            {
                selectedOption = (selectedOption - 1 + 3) % 3;
                while (Keyboard::isKeyPressed(Keyboard::Up)) {}
            }
            if (Keyboard::isKeyPressed(Keyboard::Down))
            {
                selectedOption = (selectedOption + 1) % 3;
                while (Keyboard::isKeyPressed(Keyboard::Down)) {}
            }
            if (Keyboard::isKeyPressed(Keyboard::Enter))
            {
                if (selectedOption == 0) return GRA;
                if (selectedOption == 1) return USTAWIENIA;
                if (selectedOption == 2) return WYJSCIE;
            }

        }
        return MENU;
    }
};

class Settings
{
public:
    Text speedText, sizeText, infoText;
    int speed, boardSize;
    Settings() : speed(150), boardSize(15)
    {
        initializeText(speedText,globalFont,"",50,200,100);
        initializeText(sizeText, globalFont, "", 50, 200, 150);
        initializeText(infoText, globalFont, "Nacisnij ESC aby wyjsc", 40, 200, 380);
        updateTexts();
    }

    void updateTexts()
    {
        speedText.setString("Predkosc: " + std::to_string(speed) + " ms");
        sizeText.setString("Rozmiar planszy: " + std::to_string(boardSize) + "x" + std::to_string(boardSize));
    }

    GameState handleInput(RenderWindow& window)
    {
        Event event;
        while (window.pollEvent(event))
        {
            if (event.type == Event::Closed) return WYJSCIE;
            if (event.type == Event::KeyPressed)
            {
                if (event.key.code == Keyboard::Escape) return MENU;
                if (event.key.code == Keyboard::Up)
                {
                    speed = std::min(speed + 25, 250);
                    updateTexts();
                }
                if (event.key.code == Keyboard::Down)
                {
                    speed = std::max(speed - 25, 50);
                    updateTexts();
                }
                if (event.key.code == Keyboard::Right)
                {
                    boardSize = std::min(boardSize + 1, 20);
                    updateTexts();
                }
                if (event.key.code == Keyboard::Left)
                {
                    boardSize = std::max(boardSize - 1, 10);
                    updateTexts();
                }
            }
        }
        return USTAWIENIA;
    }
    void render(RenderWindow& window)
    {
        window.clear(Color::Black);
        window.draw(backgroundSprite);
        window.draw(speedText);
        window.draw(sizeText);
        window.draw(infoText);
        window.display();
    }

    int getSpeed() const
    {
        return speed;
    }
    int getBoardSize() const
    {
        return boardSize;
    }
};

class Game
{
public:
    RenderWindow window;
    Texture snakeTexture, fruitTexture, tileTexture;
    Sprite snakeSprite, fruitSprite, tileSprite;
    GameState state;
    Menu menu;
    Settings settings;
    Snake snake;
    Fruit fruit;
    int tileSize;
    int boardwidth, boardheight;
    int speed;
    int points;
    Clock clock;
    int highscore = OdczytNajlepszegoWyniku();

    Game()
        : window(VideoMode(800, 640), "Snake Game"),
        state(MENU),
        snake(10, 10),
        fruit(15, 15),
        tileSize(32),
        boardwidth(15),
        boardheight(15),
        speed(150),
        points(0)
    {
        snakeTexture.loadFromFile("C:/Users/poles/OneDrive/Dokumenty/snake_gra/assets/images/red.png");
        fruitTexture.loadFromFile("C:/Users/poles/OneDrive/Dokumenty/snake_gra/assets/images/blue.png");
        tileTexture.loadFromFile("C:/Users/poles/OneDrive/Dokumenty/snake_gra/assets/images/white.png");
        snakeSprite.setTexture(snakeTexture);
        fruitSprite.setTexture(fruitTexture);
        tileSprite.setTexture(tileTexture);
        backgroundSprite.setTexture(backgroundTexture);
    }

    int OdczytNajlepszegoWyniku()
    {
        std::ifstream plik("PUNKTY.txt");
        int highScore = 0;
        if (plik.is_open())
        {
            plik >> highScore;
            plik.close();
        }
        return highScore;
    }

    void Zapis(int arg_points)
    {
        int highScore = OdczytNajlepszegoWyniku();
        if (arg_points > highScore)
        {
            std::ofstream strumien("PUNKTY.txt", std::ios::trunc);
            if (strumien.is_open())
            {
                strumien << arg_points << std::endl;
                strumien.close();
            }
        }
    }
    void resetGame()
    {
        snake = Snake(boardwidth / 2, boardheight / 2);
        fruit.respawn(boardwidth, boardheight, snake.getBody());
        points = 0;
    }
    void run()
    {
        while (window.isOpen())
        {
            switch (state)
            {
            case MENU:
                menuLoop();
                break;
            case GRA:
                gameLoop();
                break;
            case USTAWIENIA:
                settingsLoop();
                break;
            case WYJSCIE:
                window.close();
                break;
            case GAMEOVER:
                gameOverLoop();
                break;
            }
        }
    }
    void menuLoop()
    {
        while (state == MENU && window.isOpen())
        {
            state = menu.handleInput(window);
            menu.render(window);
        }
    }
    void settingsLoop()
    {
        while (state == USTAWIENIA && window.isOpen())
        {
            state = settings.handleInput(window);
            if (state == MENU)
            {
                speed = settings.getSpeed();
                boardwidth = boardheight = settings.getBoardSize();
                resetGame();
            }
            settings.render(window);
        }
    }
    void gameLoop()
    {
        while (state == GRA && window.isOpen())
        {
            handleEvents();
            if (clock.getElapsedTime().asMilliseconds() > speed)
            {
                snake.move(boardwidth, boardheight);
                if (snake.getHead() == Vector2i(fruit.x, fruit.y))
                {
                    points++;
                    snake.grow();
                    fruit.respawn(boardwidth, boardheight, snake.getBody());
                }
                if (snake.checkCollision()) state = GAMEOVER;
                clock.restart();
            }
            renderGame();
        }
    }


    void gameOverLoop()
    {
        Text gameOverText, pointsText,highScore;
        initializeText(gameOverText,globalFont,"GRA SKONCZONA! \nNacisnij ESC by wrocic do MENU \nlub enter zeby rozpoczac nowa gre",30,200,300);
        initializeText(pointsText,globalFont,"Zdobyte punkty: "+std::to_string(points),50,200,100);
        initializeText(highScore, globalFont, "Rekord: " + std::to_string(highscore), 30, 200, 160);
        Zapis(points);
        highscore = std::max(highscore, points);
        points = 0;
        while (state == GAMEOVER && window.isOpen())
        {

            window.clear(Color::Black);
            window.draw(backgroundSprite);
            window.draw(gameOverText);
            window.draw(pointsText);
            window.draw(highScore);
            window.display();
            Event event;
            while (window.pollEvent(event))
            {
                if (event.type == Event::Closed) window.close();
                if (Keyboard::isKeyPressed(Keyboard::Escape))
                {
                    resetGame();
                    state = MENU;
                }
                if (Keyboard::isKeyPressed(Keyboard::Enter))
                {
                    resetGame();
                    state = GRA;
                }
            }
        }
    }

    void handleEvents()
    {
        Event event;
        while (window.pollEvent(event))
        {
            if (event.type == Event::Closed) window.close();
            if (event.type == Event::KeyPressed)
            {
                if (event.key.code == Keyboard::Up) snake.setDirection(3);
                if (event.key.code == Keyboard::Down) snake.setDirection(0);
                if (event.key.code == Keyboard::Left) snake.setDirection(1);
                if (event.key.code == Keyboard::Right) snake.setDirection(2);
                if (event.key.code ==Keyboard::Escape)
                {
                    resetGame();
                    state = MENU;
                }
            }
        }
    }

    void renderGame()
    {
        window.clear();
        window.draw(backgroundSprite);
        for (int i = 0; i < boardheight; i++)
        {
            for (int j = 0; j < boardwidth; j++)
            {
                tileSprite.setPosition(j * tileSize, i * tileSize);
                window.draw(tileSprite);
            }
        }
        for (const auto& segment : snake.getBody())
        {
            snakeSprite.setPosition(segment.x * tileSize, segment.y * tileSize);
            window.draw(snakeSprite);
        }
        Text scoreText,highScore;
        initializeText(scoreText,globalFont,"Punkty: "+ std::to_string(points),30,650,100);
        initializeText(highScore, globalFont, "Rekord: " + std::to_string(highscore), 30, 650, 60);
        window.draw(highScore);
        window.draw(scoreText);
        fruitSprite.setPosition(fruit.x * tileSize, fruit.y * tileSize);
        window.draw(fruitSprite);
        window.display();
    }

};
int main()
{
    globalFont.loadFromFile("C:/Users/poles/OneDrive/Dokumenty/snake_gra/assets/fonts/arial.ttf");
    backgroundTexture.loadFromFile("C:/Users/poles/OneDrive/Dokumenty/snake_gra/assets/images/tlo.png");
    backgroundSprite.setTexture(backgroundTexture);
    srand(time(NULL));
    Game game;
    game.run();
    return 0;
}
