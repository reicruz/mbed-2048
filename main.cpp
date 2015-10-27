// 2048 Game by Reinaldo Cruz and Douglas Sigelbaum
// ECE 4180 Gatech

#include "mbed.h"
#include "TFT_4DGL.h"
#include <math.h>
#include <sstream>
Serial printer(USBTX, USBRX);

// overwrite 4DGL library screen size settings in TFT_4DGL.h
#define SIZE_X  479
#define SIZE_Y  639
//
#define BOARD_SIZE 4
#define MOVE_UP 1
#define MOVE_DOWN 2
#define MOVE_LEFT 3
#define MOVE_RIGHT 4


class Piece {
    public:
        Piece();
        Piece(int);
        int level;
        bool matches(const Piece&);
        void makeEmpty();
        void setLevel(int);
        void upgrade();
        bool isEmpty();

};
class GameBoard {
    public:
        GameBoard(); 
        GameBoard(const GameBoard&);
        ~GameBoard();
        void addRandomPiece();
        bool moveBoard(int);
        bool move(int, int, int);
        Piece* pieces[BOARD_SIZE];
        unsigned colors[];
        TFT_4DGL* panel;
        void repaint(int, int, int);
        unsigned score;
        void updateScore();
        bool checkLose();
        void lose();
        int empty;
};

Piece::Piece() {
    this->level = 0;
}


void Piece::setLevel(int level) {
    this->level = level;
}

void Piece::upgrade() {
    this->level = (this->level)*2;
}

bool Piece::isEmpty() {
    return this->level == 0;
}
Piece::Piece(int lev) {
    this->level = lev;
}

bool Piece::matches(const Piece& piece) {
    return piece.level == this->level && piece.level != 0;
}

void Piece::makeEmpty() {
    this->level = 0;
}

GameBoard::GameBoard() {
    score = 0;
    empty = 16;
    panel = new TFT_4DGL(p9, p10, p11);
    panel->baudrate(115200);
    panel->text_mode(TRANSPARENT);
//// added - Set Display to 640 by 480 mode
    panel->display_control(0x0c, 0x01);
    srand(NULL);
    for(int i = 0; i < BOARD_SIZE; i++) {
        pieces[i] = new Piece[BOARD_SIZE];
    }
    addRandomPiece();
    addRandomPiece();
    for(int i = 0; i < 4; i++) {
        for(int j = 0; j < 4; j++) {
            repaint(pieces[i][j].level, j, i);
        }
    }
    panel->graphic_string("2048", 280, 10, FONT_8X8, WHITE, 2, 2);
    panel->graphic_string("Score: ", 125, 440, FONT_8X8, WHITE, 2, 2);
}

void GameBoard::repaint(int level, int x, int y) {
    switch(level) {
    case 0:
        panel->text_button("    ", UP, x*100+125, y*100+50, BG_COLOR, FONT_8X8, DTEXT, 2, 8);
        break;
    case 2:
        panel->text_button(" 2  ", UP, x*100+125, y*100+50, LGREY, FONT_8X8, DTEXT, 2, 8);        
        break;
    case 4:
        panel->text_button(" 4  ", UP, x*100+125, y*100+50, DGREY, FONT_8X8, DTEXT, 2, 8);
        break;
    case 8:
        panel->text_button(" 8  ", UP, x*100+125, y*100+50, LORANGE, FONT_8X8, LTEXT, 2, 8);        
        break;
    case 16:
        panel->text_button(" 16 ", UP, x*100+125, y*100+50, DORANGE, FONT_8X8, LTEXT, 2, 8);     
        break;
    case 32:
        panel->text_button(" 32 ", UP, x*100+125, y*100+50, LRED, FONT_8X8, LTEXT, 2, 8);     
        break;
    case 64:
        panel->text_button(" 64 ", UP, x*100+125, y*100+50, DRED, FONT_8X8, LTEXT, 2, 8);     
        break;
    case 128:
        panel->text_button("128 ", UP, x*100+125, y*100+50, GOLD128, FONT_8X8, LTEXT, 2, 8);     
        break;
    case 256:
        panel->text_button("256 ", UP, x*100+125, y*100+50, GOLD256, FONT_8X8, LTEXT, 2, 8);     
        break;
    case 512:
        panel->text_button("512 ", UP, x*100+125, y*100+50, GOLD512, FONT_8X8, LTEXT, 2, 8);     
        break;
    case 1024:
        panel->text_button("1024", UP, x*100+125, y*100+50, GOLD1024, FONT_8X8, LTEXT, 2, 8);     
        break;
    case 2048:
        panel->text_button("2048", UP, x*100+125, y*100+50, BLUE2048, FONT_8X8, LTEXT, 2, 8);
        panel->graphic_string("YOU WIN", 80, 200, FONT_8X8, BLUE2048, 8, 8);
        break;
    }
}


bool GameBoard::moveBoard(int dir) {
    bool moved = false;
    int i,j;
    switch(dir) {
    case MOVE_UP:
        for(j = 0; j < BOARD_SIZE; j++) {
            for(i = 1; i < BOARD_SIZE; i++) {
                if(move(j,i,dir)) moved = true;
            }
        }
        break;
    case MOVE_DOWN:
        for(j = 0; j < BOARD_SIZE; j++) {
            for(i = BOARD_SIZE - 2; i >= 0; i--) {
                if(move(j,i,dir)) moved = true;
            }
        }
        break;
    case MOVE_LEFT:
        for(i = 0; i < BOARD_SIZE; i++) {
            for(j = 1; j < BOARD_SIZE; j++) {
                if(move(j,i,dir)) moved = true;             
            }
        }
        break;        
    case MOVE_RIGHT:
        for(i = 0; i < BOARD_SIZE; i++) {
            for(j = BOARD_SIZE - 2; j >= 0; j--) {
                if(move(j,i,dir)) moved = true;
            }
        }
        break;    
    }
    return moved;
}
void GameBoard::updateScore() {
    std::string s;
    std::stringstream out;
    out << score;
    s = out.str();
    panel->rectangle(230, 440, 400, 460, BLACK);
    panel->graphic_string(s.c_str(), 230, 440, FONT_8X8, WHITE, 2, 2);    
}


void GameBoard::lose() {
    panel->graphic_string("YOU LOSE", 80, 200, FONT_8X8, DRED, 8, 8);
}


bool GameBoard::checkLose() {
    for (int i = 0; i < 4; i++) {
        for (int j = 1; j < 4; j++) {
            if (pieces[i][j].level == pieces[i][j-1].level ||
                pieces[j][i].level == pieces[j-1][i].level) 
                return true;
        }
    }
    return false;    
}

bool GameBoard::move(int x, int y, int dir) {
    bool moved = false;
    switch(dir) {
    case MOVE_UP:
        while(y-1 >= 0 && !(pieces[y][x].isEmpty())){
            if(pieces[y-1][x].isEmpty()) {
                pieces[y-1][x].setLevel(pieces[y][x].level);
                repaint(pieces[y-1][x].level, x, y-1);
                pieces[y][x].makeEmpty();
                repaint(pieces[y][x].level, x, y);
                y--;
                moved = true;
            } else if(pieces[y-1][x].matches(pieces[y][x])) {
                score += pieces[y][x].level * 2;
                pieces[y-1][x].upgrade();
                repaint(pieces[y-1][x].level, x, y-1);
                pieces[y][x].makeEmpty();
                repaint(pieces[y][x].level, x, y);
                moved = true;
                empty++;
                updateScore();
                break;
            } else {
                return moved;
            }
        }
        break;
    case MOVE_DOWN:
        while(y+1 < BOARD_SIZE && !(pieces[y][x].isEmpty())){
            if(pieces[y+1][x].isEmpty()) {
                pieces[y+1][x].setLevel(pieces[y][x].level);
                repaint(pieces[y+1][x].level, x, y+1);
                pieces[y][x].makeEmpty();
                repaint(pieces[y][x].level, x, y);
                y++;
                moved = true;
            } else if(pieces[y+1][x].matches(pieces[y][x])) {
                score += pieces[y][x].level * 2;
                pieces[y+1][x].upgrade();
                repaint(pieces[y+1][x].level, x, y+1);
                pieces[y][x].makeEmpty();
                repaint(pieces[y][x].level, x, y);
                moved = true;
                empty++;
                updateScore();
                break;
            } else {
                return moved;
            }
        }
        break;
    case MOVE_LEFT:
        while(x-1 >= 0 && !(pieces[y][x].isEmpty())){
            if(pieces[y][x-1].isEmpty()) {
                pieces[y][x-1].setLevel(pieces[y][x].level);
                repaint(pieces[y][x-1].level, x-1, y);
                pieces[y][x].makeEmpty();
                repaint(pieces[y][x].level, x, y);
                x--;
                moved = true;
            } else if(pieces[y][x-1].matches(pieces[y][x])) {
                score += pieces[y][x].level * 2;
                pieces[y][x-1].upgrade();
                repaint(pieces[y][x-1].level, x-1, y);
                pieces[y][x].makeEmpty();
                repaint(pieces[y][x].level, x, y);
                moved = true;
                empty++;
                updateScore();
                break;
            } else {
                return moved;
            }
        }
        break;
    case MOVE_RIGHT:
        while(x+1 < BOARD_SIZE && !(pieces[y][x].isEmpty())){
            if(pieces[y][x+1].isEmpty()) {
                pieces[y][x+1].setLevel(pieces[y][x].level);
                repaint(pieces[y][x+1].level, x+1, y);
                pieces[y][x].makeEmpty();
                repaint(pieces[y][x].level, x, y);
                x++;
                moved = true;
            } else if(pieces[y][x+1].matches(pieces[y][x])) {
                score += pieces[y][x].level * 2;
                pieces[y][x+1].upgrade();
                repaint(pieces[y][x+1].level, x+1, y);
                pieces[y][x].makeEmpty();
                repaint(pieces[y][x].level, x, y);
                moved = true;
                empty++;
                updateScore();
                break;
            } else {
                return moved;
            }
        }
        break;
    }
    return moved;      
}

void GameBoard::addRandomPiece() {
    int spot1x, spot1y;
    do {
        spot1x = rand() % BOARD_SIZE;
        spot1y = rand() % BOARD_SIZE;
    } while(!(pieces[spot1y][spot1x].isEmpty()));
    float randFloat = static_cast <float> (rand()) / static_cast <float> (RAND_MAX);
    int val = 0;
    if(randFloat < 0.875) {
        val = 2;
    } else {
        val = 4;
    }
    pieces[spot1y][spot1x].level = val;
    repaint(pieces[spot1y][spot1x].level, spot1x, spot1y);
    empty--;
}


int main() {
    GameBoard board;
    DigitalIn left(p21);
    DigitalIn down(p22);
    DigitalIn up(p23);
    DigitalIn right(p24);
    left.mode(PullUp);
    down.mode(PullUp);
    up.mode(PullUp);
    right.mode(PullUp);
    wait(0.5);
    bool moved = false;
    while(1) {
        moved = false;
        if(left.read() == 0) {
            moved = board.moveBoard(MOVE_LEFT);
            printer.printf("MOVE LEFT\n");
        } else if(right.read() == 0) {
            moved = board.moveBoard(MOVE_RIGHT);
            printer.printf("MOVE RIGHT\n");
        } else if(up.read() == 0) {
            moved = board.moveBoard(MOVE_UP);
            printer.printf("MOVE UP\n");
        } else if(down.read() == 0) {
            moved = board.moveBoard(MOVE_DOWN);
            printer.printf("MOVE DOWN\n");
        }
        if(moved) {
            board.addRandomPiece();
            if(board.empty == 0 && !(board.checkLose())) board.lose();
        }
        else 
            wait(0.2);
    }    
}