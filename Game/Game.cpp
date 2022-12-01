#include "Game.h"
#include "../Field/Field.h"
#include "../Manipulation/Manipulation.h"
#include "../Stats/Stats.h"
#include <random>
#include <ctime>
#include <windows.h>

void Game::CreateCells() {
    //init
    isMineExploded = false;
    isGamePaused = false;

    IdMineCells.clear();
    cell.clear();
    cellDraw.clear();
    cell.resize(numOfCells);
    cellDraw.resize(numOfCells);
    //for random
    vector<int> Ids;
    for(int id = 0; id < numOfCells; ++id) Ids.push_back(id);
    mt19937 rng(chrono::steady_clock::now().time_since_epoch().count());
    shuffle(Ids.begin(), Ids.end(), rng);
    for(int i = 0; i < numOfMines; ++i) IdMineCells.push_back(Ids[i]);
    cout << "Mines: ";
    for(int i = 0; i < numOfMines; ++i) {
        int id = IdMineCells[i];
        cell[id].isMine = true;
        cout << id << ' ';
    }
    cout << '\n';
    //
    for(int id = 0, i = 0, j = 0; id < numOfCells; ++id) {
        SetImage(id);

        cellDraw[id].SetPosition(i * cellDraw[id].sz + sqrtNumOfCells * 0.5f, j * cellDraw[id].sz + sqrtNumOfCells * 5.7f);
        cell[id].SetPosition(i * cell[id].sz + sqrtNumOfCells * 0.5f, j * cell[id].sz + sqrtNumOfCells * 5.7f);

        if (++i == sqrtNumOfCells) {
            i = 0;
            ++j;
        }
    }
    for(int id = 0, i = 0, j = 0; id < numOfCells; ++id) {
        cout << CountSurroundedMines(id) << ' ';

        if (++i == sqrtNumOfCells) {
            i = 0;
            ++j;cout << '\n';
        }
    }
}
pair<int, int> Game::toCoord(int id) {
    return {id / Nrow, id % Ncol};
}
int Game::toId(int x, int y) {
    return x * Nrow + y;
}
int Game::CountSurroundedMines(int id) {
    int cnt = 0;
    int x, y; tie(x, y) = toCoord(id);

    for(int i = 0; i < 8; ++i) {
        int u = x + dx[i], v = y + dy[i];

        if (u < 0 || v < 0 || u >= Nrow || v >= Ncol) continue;

        int near_id = toId(u, v);
        cnt += cell[near_id].isMine;
    }
    return cnt;
}
void Game::SetImage(int id) {
    cellDraw[id].SetTexture("Images\\UnCheckedCell.png");
    if (cell[id].IsMine()) {
        cell[id].SetTexture("Images\\Mine.png");
        return;
    }
    switch(CountSurroundedMines(id)) {
        case 0:
            cell[id].SetTexture("Images\\EmptyCell.png");
            cell[id].isEmpty = true;
            break;
        case 1:
            cell[id].SetTexture("Images\\One.png");
            break;
        case 2:
            cell[id].SetTexture("Images\\Two.png");
            break;
        case 3:
            cell[id].SetTexture("Images\\Three.png");
            break;
        case 4:
            cell[id].SetTexture("Images\\Four.png");
            break;
        case 5:
            cell[id].SetTexture("Images\\Five.png");
            break;
        case 6:
            cell[id].SetTexture("Images\\Six.png");
            break;
        case 7:
            cell[id].SetTexture("Images\\Seven.png");
            break;
        case 8:
            cell[id].SetTexture("Images\\Eighth.png");
            break;
    }
}
//////////////////////////////////////////////////////////
void Game::SetGameWindowParameters(int n) {
	if (n < 100) {
		sqrtNumOfCells = 10;
		numOfCells = 100;
		Nrow = sqrtNumOfCells, Ncol = sqrtNumOfCells;
	}
	else if (n > 400) {
		sqrtNumOfCells = 20;
		numOfCells = 400;
		Nrow = sqrtNumOfCells, Ncol = sqrtNumOfCells;
	}
	else {
		sqrtNumOfCells = round(sqrt(n));
		numOfCells = pow(sqrtNumOfCells, 2);
		Nrow = sqrtNumOfCells, Ncol = sqrtNumOfCells;
	}

	max_x = sqrtNumOfCells * 31.f;
	max_y = sqrtNumOfCells * 36.7f;

	if (numOfMines >= numOfCells) {
		numOfMines  = numOfCells - 1;
	}
	else if (numOfMines <= 0) {
		numOfMines = 1;
	}
	cout << max_x << ' ' << max_y << ' ' << sqrtNumOfCells << ' ' << numOfCells << ' ' << numOfMines << '\n';
}
void Game::StartGameWindow(RenderWindow& window, Text start, InputBar cellGrid, InputBar NumberOfMines) {
    Vector2i mousePosition = Mouse::getPosition(window);

    if (mousePosition.x >= start.getPosition().x && mousePosition.x <= 210) {
        if (mousePosition.y >= start.getPosition().y && mousePosition.y <= 166) {
            start.setFillColor(Color::Yellow);
            if (Mouse::isButtonPressed(Mouse::Left)) {
                numOfMines = NumberOfMines.GetInput();
                SetGameWindowParameters(cellGrid.GetInput());
                window.close();
                CreateGameWindow();
            }
        } else {
            start.setFillColor(Color::White);
        }
    }
//    cout << cellGrid.GetInput() << ' ' << numOfMines << '\n';
}
void Game::CreateGameWindow() {
    Sleep(80);
    RenderWindow window(VideoMode(max_x, max_y), "Minesweeper", Style::Titlebar | Style::Close);
    Event event;
    Field field(max_x, max_y);
    Manipulation Player;
    Player.init(numOfCells, Nrow, Ncol);
    Stats gameStats;
    gameStats.init(numOfMines);

    Font font;
	Text win;
	font.loadFromFile("Fonts\\arial.ttf");
	win.setFont(font);
	win.setFillColor(Color::Green);
	win.setString("You Win!");
	win.setCharacterSize(40);
	win.setPosition(max_x / 2 - 82, max_y / 2 - 30);

	Text lose;
	font.loadFromFile("Fonts\\arial.ttf");
	lose.setFont(font);
	lose.setFillColor(Color::Red);
	lose.setString("You Lose!");
	lose.setCharacterSize(40);
	lose.setPosition(max_x / 2 - 82, max_y / 2 - 30);

    gameStats.SetPosition(max_x, max_y);

    CreateCells();

    int numCheckedCell = 0;
    int numOfFlags = 0;

    while(window.isOpen()) {
        while(window.pollEvent(event)) {
            if (event.type == Event::Closed) {
                window.close();
                CreateSettingsWindow();
            }
        }

        if (isGamePaused == false) {
            Player.LeftClickOnCell(window, cell, cellDraw, isMineExploded, numCheckedCell);
            Player.RightClickOnCell(window, cell, cellDraw, numOfFlags);
            Sleep(44);
            gameStats.UpdateFlags(numOfFlags);
            gameStats.UpdateTimer();
        }

        if (gameStats.isClickedOnStart(window) == true) {
            isGamePaused = false;
            CreateCells();
            numCheckedCell = 0;
            numOfFlags = 0;
            gameStats.init(numOfMines);
            Player.init(numOfCells, Nrow, Ncol);
        }

        window.clear();
        window.draw(field.GetRectangleShape());

        for(int id = 0; id < numOfCells; ++id) {
            window.draw(cellDraw[id].GetRectangleShape());
        }

        if (isMineExploded == true) {
            for(int id : IdMineCells) {
                cellDraw[id].SetTexture("Images\\ExplodedMine.png");
                window.draw(cellDraw[id].GetRectangleShape());
            }
            window.draw(lose);
            isGamePaused = true;
        }

        if (numCheckedCell + numOfMines == numOfCells) {
            window.draw(win);
            isGamePaused = true;
        }

        window.draw(gameStats.GetStartButtonShape());
		window.draw(gameStats.GetMinesCounterShape());
		window.draw(gameStats.GetTimerShape());
		window.draw(gameStats.GetMinesCounterText());
		window.draw(gameStats.GetTimerText());

        window.display();
    }
}
void Game::CreateSettingsWindow() {
    RenderWindow window(VideoMode(310.f, 367.f), "Settings", Style::Titlebar | Style::Close);
    Event event;
    InputBar cellGrid(170, 30, 7.f, 317.f, "cell grid X*X = ");
    InputBar minesNumber(120, 30, 183.f, 317.f, "mines = ");

    Font font;
    Text start;
    font.loadFromFile("Fonts\\arial.ttf");
    start.setFont(font);
    start.setFillColor(Color::White);
    start.setString("Start");
    start.setCharacterSize(60);
    start.setPosition(85.f, 103.f);

    while(window.isOpen()) {
        while(window.pollEvent(event)) {
            if (event.type == Event::Closed) {
                window.close();
            }
            if (event.type == Event::TextEntered) {
                cellGrid.SetInputText(window, event);
                minesNumber.SetInputText(window, event);
            }
        }
        cellGrid.MouseOverInputBox(window);
        minesNumber.MouseOverInputBox(window);
        StartGameWindow(window, start, cellGrid, minesNumber);

        window.clear();
        window.draw(cellGrid.GetInputShape());
        window.draw(cellGrid.inputText);
        window.draw(minesNumber.GetInputShape());
        window.draw(minesNumber.GetInputText());
        window.draw(start);
        window.display();
    }
}
