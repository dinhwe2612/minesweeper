#pragma once
#include "../Game/Game.h"
#include <bits/stdc++.h>
#include <SFML/Graphics.hpp>
using namespace std;
using namespace sf;

class Manipulation : public Game {
private:
    Texture FlagCell, UnCheckedCell, MineExplodedCell, NotAMineCell;
    int numOfCells, Unchecked = 0, Checked = 1, flagPlaced = 2;
    int Nrow, Ncol;
    int dx[8] = {-1, 0, 1, 1, 1, 0, -1, -1};
    int dy[8] = {-1, -1, -1, 0, 1, 1, 1, 0};
    //012
    //7.3
    //654
    vector<int> stateOfCells;

    pair<int, int> toCoord(int id);
    int toId(int x, int y);
    void bfs(RenderWindow& window, int id, vector<Cell>& cell, vector<Cell>& cellDraw, int& numCheckedCell);
public:
    Manipulation() {
        FlagCell.loadFromFile("Images\\Flag.png");
        UnCheckedCell.loadFromFile("Images\\UnCheckedCell.png");
        MineExplodedCell.loadFromFile("Images\\ExplodedMine.png");
        NotAMineCell.loadFromFile("Images\\NotAMine.png");
    }
    void init(int num, int Nr, int Nc) {
        numOfCells = num;
        Nrow = Nr;
        Ncol = Nc;
        cout << num << endl;
        stateOfCells.assign(num, 0);//unchecked
    }
    void LeftClickOnCell(RenderWindow& window, vector<Cell>& cell, vector<Cell>& cellDraw, bool& isMineExploded, int& numCheckedCell);
    void RightClickOnCell(RenderWindow& window, vector<Cell>& cell, vector<Cell>& cellDraw, vector<bool>& Flags, int& numOfFlags);
};
