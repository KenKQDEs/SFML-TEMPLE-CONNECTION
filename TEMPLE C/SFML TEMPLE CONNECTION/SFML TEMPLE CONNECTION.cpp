#include <iostream>
#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include <SFML/Window.hpp>
#include "STRUCTURA_DATE_HARTA.hpp"
#include "MyVector.hpp"
#include <assert.h>
#include <math.h>
#include <fstream>
#include <iomanip>
#include <cstring>
std::fstream f;
sf::RenderWindow window(sf::VideoMode(1600, 900), "TEMPLE CONNECTION", sf::Style::Close);
char filename[30];
char buffer[1000];
char istr[10];
int b_iter = 0;
vector<pereche<int,bool>> undoStack; // bool: 1 = INSERT 0=REMOVE
int undoBufferSize = 0;
int maxUndoBuffer = 0;
int textX, textY;
vector<int> prioritati;
struct _temples
{
    int i = -1, j = -1;
    sf::Sprite temple;
}temples[3];
struct bridge_
{
    sf::Sprite pod;
    pereche<int, int> dir[4];
    int i = -1, j = -1;
    sf::RectangleShape Collision[4];
    float sgn = 1; //(+ sau nu)
    int maxCol;
    sf::Vector2f lastpos[1000];
    int last_it = 0;
}bridge[7];
struct GameState
{
    float BoardState[16][16];
    int pozitie = -1;
    sf::Vector2f poz[7];
    sf::Vector2f dir[7][4];
    sf::Vector2f tile[7];
    float rotatie[7];
}situatie[256];
struct meniu
{
    int selectedOptionIndex = 0;
    sf::Font font;
    sf::Text option[3];
    sf::RectangleShape button[3];
}m;
struct levelpanel {
    sf::RectangleShape rectangle;
    sf::Texture texture;
}lvl[20];
sf::Sprite tiles[5][5];
sf::Text ClockTimer;
sf::Vector2f mousePos;
sf::RectangleShape Deck;
sf::CircleShape triangle(10.f, 3);
sf::Sprite border;
sf::Sprite donebutton;
sf::Sprite undobutton;
sf::Sprite redobutton;
sf::Sprite homebutton;
sf::Sprite winmark;
sf::Sprite winbutton;
sf::Sprite ceas_bkg;
sf::Sprite menu_bkg;
sf::Text text;
sf::Text Deck_Text;
sf::SoundBuffer meep_merp;
sf::SoundBuffer bpress;
sf::Sound wrong, right;
sf::SoundBuffer charge;
sf::Sound win;
sf::SoundBuffer check;
sf::Sound checkbutton;
sf::Clock Counter;
sf::Time TimeElapsed;
int pozitie=-1;
int ok = 0;
float BoardState[16][16];
bool isDebugging = 0;
bool needsUpdate = 1;
bool needsUndo = 0;
bool isTemple1present = 0,
isTemple2present = 0,
isTemple3present = 0;
bool isIngame = 0;
bool viz[16][16];
bool isDragging = 0;
bool alreadyWon = 0;
bool still_undoing = 0;
bool keepTimer = 0;
bool x = true;
bool playx = false;
bool instructionsx = false;
int last_action = 3;
//scaling
const float div1 = 3.f;
const float div2 = 3.f;
const float tileW = floor(385 / div1); //385 div 3 ~128px tile w and h
const float tileH = floor(385 / div2);
bool punePiesa(sf::Sprite& tile, int i, int j, int flag);
void destroy(int i);
void load1()
{
    situatie[0].poz[0] = bridge[0].pod.getPosition();
    situatie[0].poz[1] = bridge[1].pod.getPosition();
    situatie[0].poz[2] = bridge[2].pod.getPosition();
    situatie[0].poz[3] = bridge[3].pod.getPosition();
    situatie[0].poz[4] = bridge[4].pod.getPosition();
    situatie[0].poz[5] = bridge[5].pod.getPosition();
    situatie[0].poz[6] = bridge[6].pod.getPosition();
    for (int i = 0; i < 16; i++)
        for (int j = 0; j < 16; j++)
            situatie[0].BoardState[i][j] = BoardState[i][j];
    for (int i = 0; i < 7; i++)
    {
        for (int j = 0; j < 4; j++)
        {
            situatie[0].dir[i][j].x = bridge[i].dir[j].first;
            situatie[0].dir[i][j].y = bridge[i].dir[j].second;
        }
        situatie[0].rotatie[i] = bridge[i].pod.getRotation();
    }

}
void buffer_clear()
{
    for (int i = 0; i < b_iter; i++)
        buffer[i] = '\0';
    b_iter = 0;
}
bool contains(int i, float x, float y)
{
    sf::FloatRect bounds= bridge[i].pod.getGlobalBounds();
    if (bounds.contains(x, y))
        return 1;

    return 0;
}
void doRedo()
{
    if (undoBufferSize +1 >maxUndoBuffer||still_undoing==0) return;
    undoBufferSize++;
    bridge[0].pod.setPosition(situatie[undoBufferSize].poz[0]);
    bridge[1].pod.setPosition(situatie[undoBufferSize].poz[1]);
    bridge[2].pod.setPosition(situatie[undoBufferSize].poz[2]);
    bridge[3].pod.setPosition(situatie[undoBufferSize].poz[3]);
    bridge[4].pod.setPosition(situatie[undoBufferSize].poz[4]);
    bridge[5].pod.setPosition(situatie[undoBufferSize].poz[5]);
    bridge[6].pod.setPosition(situatie[undoBufferSize].poz[6]);
    for (int i = 0; i < 16; i++)
        for (int j = 0; j < 16; j++)
            BoardState[i][j] = situatie[undoBufferSize].BoardState[i][j];
    for (int i = 0; i < 7; i++)
    {
        for (int j = 0; j < 4; j++)
        {
            bridge[i].dir[j].first = situatie[undoBufferSize].dir[i][j].x;
            bridge[i].dir[j].second = situatie[undoBufferSize].dir[i][j].y;
        }
        bridge[i].pod.setRotation(situatie[undoBufferSize].rotatie[i]);
    }
    for (int i = 0; i < 7; i++)
    {
        bridge[i].i = situatie[undoBufferSize].tile[i].x;
        bridge[i].j = situatie[undoBufferSize].tile[i].y;
    }
    if (isDebugging == 1)  std::cout << std::endl;
    for (int k = 0; k < 15; k++)
    {
        for (int q = 0; q < 15; q++)
            if (isDebugging == 1)  std::cout << std::setw(4) << BoardState[k][q] << " ";
        if (isDebugging == 1)  std::cout << std::endl;
    }
}
void doUndo()
{
    if (undoBufferSize - 1 < 0)
    {
        return;
    }
        undoBufferSize--;
        still_undoing = 1;
        bridge[0].pod.setPosition(situatie[undoBufferSize].poz[0]);
        bridge[1].pod.setPosition(situatie[undoBufferSize].poz[1]);
        bridge[2].pod.setPosition(situatie[undoBufferSize].poz[2]);
        bridge[3].pod.setPosition(situatie[undoBufferSize].poz[3]);
        bridge[4].pod.setPosition(situatie[undoBufferSize].poz[4]);
        bridge[5].pod.setPosition(situatie[undoBufferSize].poz[5]);
        bridge[6].pod.setPosition(situatie[undoBufferSize].poz[6]);
        for (int i = 0; i < 16; i++)
            for (int j = 0; j < 16; j++)
                 BoardState[i][j] = situatie[undoBufferSize].BoardState[i][j];
        for (int i = 0; i < 7; i++)
        {
            for (int j = 0; j < 4; j++)
            {
                bridge[i].dir[j].first = situatie[undoBufferSize].dir[i][j].x;
                bridge[i].dir[j].second = situatie[undoBufferSize].dir[i][j].y;
            }
            bridge[i].pod.setRotation(situatie[undoBufferSize].rotatie[i]);
        }
        for (int i = 0; i < 7; i++)
        {
            bridge[i].i = situatie[undoBufferSize].tile[i].x;
                bridge[i].j = situatie[undoBufferSize].tile[i].y;
        }
        if (isDebugging == 1)  std::cout <<std:: endl;
        for (int k = 0; k < 15; k++)
        {
            for (int q = 0; q < 15; q++)
                if (isDebugging == 1)  std::cout << std::setw(4) << BoardState[k][q] << " ";
            if (isDebugging == 1)  std::cout << std::endl;
        }
}
void setPriorities()
{
    prioritati.clear();
   if(isDebugging==1) std::cout << std::endl;
    needsUpdate = 0;
    pereche<int, int> p[4];
    int it = 0;
    p[0].first = -1;
    p[0].second = -1;
    p[1].first = -1;
    p[1].second = -1;
    p[2].first = -1;
    p[2].second = -1;
    int crossroad[4];
    for (int i = 0; i < 4; i++)
        crossroad[i] = -1;
    int cit = 0;


    for (int i = 0; i < 5; i++)
        for (int j = 0; j < 5; j++)
        {
            int X = 3 * i + 1;
            int Y = 3 * j + 1;
            if (BoardState[X - 1][Y] != BoardState[X + 1][Y] && BoardState[X][Y] && BoardState[X - 1][Y] && BoardState[X + 1][Y])
            {
                if (BoardState[X - 1][Y] > 0 && BoardState[X - 1][Y] < 10)
                {
                    p[it].first = BoardState[X - 1][Y] - 1;

                }
                else if (BoardState[X - 1][Y] >=10)
                {
                    p[it].first = BoardState[X - 1][Y];
                }
                if (BoardState[X + 1][Y] > 0 && BoardState[X + 1][Y] < 10)
                {
                    p[it].second = BoardState[X + 1][Y] - 1;

                }
                else if (BoardState[X - 1][Y] >= 10)
                {
                    p[it].second = BoardState[X + 1][Y];
                }
                it++;
                crossroad[cit] = BoardState[X][Y] - 1;
                cit++;
            }
            else
                if (BoardState[X][Y - 1] != BoardState[X][Y + 1] && BoardState[X][Y] && BoardState[X][Y - 1] && BoardState[X][Y + 1])
                {

                    if (BoardState[X][Y - 1] > 0 && BoardState[X][Y - 1] < 10)
                    {
                        p[it].first = BoardState[X][Y - 1] - 1;

                    }
                    else if (BoardState[X][Y-1] >= 10)
                    {
                        p[it].first = BoardState[X][Y - 1];
                    }
                    if (BoardState[X][Y + 1] > 0 && BoardState[X][Y + 1] < 10)
                    {
                        p[it].second = BoardState[X][Y + 1] - 1;

                    }
                    else if (BoardState[X][Y+1] >= 10)
                    {
                        p[it].second = BoardState[X][Y + 1];
                    }
                    it++;
                    crossroad[cit] = BoardState[X][Y] - 1;
                    cit++;
                }

        }
    for (int i = 0; i < 3; i++)
    {
        int OK = 0;
        for (int j = 0; j < cit; j++)
        {
            if (crossroad[j] == i)
            {
                OK = 1; 
                break;
            }
        }
      if(OK==0)  prioritati.push_back(i);
    }
    for (int i = 0; i < cit; i++)
     prioritati.push_back(crossroad[i]);
    for (int i = 3; i < 7; i++)
    {
        int OK = 0;
        for (int j = 0; j < it; j++)
            if (i == p[j].first || i == p[j].second)
            {
                OK = 1;
                break;
            }
        if (OK == 0) prioritati.push_back(i);
    }
    for (int i = 0; i < it; i++)
    {
        if (p[i].first >= 10)
        {
            prioritati.push_back(p[i].first);
            prioritati.push_back(p[i].second);
        }
        else if (p[i].second >= 10)
        {
            prioritati.push_back(p[i].second);
            prioritati.push_back(p[i].first);
        }
    }
    for (int i = 0; i < it; i++)
        if (p[i].first < 10 && p[i].second < 10)
        {
            prioritati.push_back(p[i].first);
            prioritati.push_back(p[i].second);
        }
   
 
    if (isTemple1present&&prioritati.find(10)==-1) prioritati.push_back(10);
    if (isTemple2present&&prioritati.find(11) == -1) prioritati.push_back(11);
    if (isTemple3present&& prioritati.find(12) == -1)  prioritati.push_back(12);
    if (isDragging == 1) prioritati.push_back(pozitie);
    for (int i = 0; i < 15; i++)
        if (isDebugging == 1) std::cout << prioritati[i] << " ";
    if (isDebugging == 1) std::cout << std::endl;
    if (isDebugging == 1)std::cout << "FINAL:crossroads,perechi" << std::endl;
    for (int i = 0; i < cit; i++)
        if (isDebugging == 1) std::cout << crossroad[i] << " ";
    if (isDebugging == 1) std::cout << std::endl;
    for (int i = 0; i < it; i++)
        if (isDebugging == 1)   std::cout << p[i].first << " " << p[i].second << std::endl;

}
void DoRotation(int poz, bool dreapta);
void read();
void analyze();
void reset_level()
{
    read();
    if (keepTimer == 0) Counter.restart();
    else keepTimer = 0;
    undoBufferSize = maxUndoBuffer = 0;

    isTemple1present = 0;
    isTemple2present = 0;
    isTemple3present = 0;
    temples[0].i = -1;
    temples[0].j = -1;
    temples[1].i = -1;
    temples[1].j = -1;
    temples[2].i = -1;
    temples[2].j = -1;
    analyze();
    alreadyWon = 0;
    pozitie = -1;
    ok = 0;
    needsUpdate=1;
    isDragging = 0;
    for (int i = 0; i < 16; i++)
        for (int j = 0; j < 16; j++)
            viz[i][j] = 0;
    bridge[0].dir[0].first = 0;
    bridge[0].dir[0].second = 1;
    bridge[0].dir[1].first = 1;
    bridge[0].dir[1].second = 0;
    bridge[0].dir[2].first = bridge[0].dir[3].first = 322;
    bridge[0].dir[2].second = bridge[0].dir[3].second = 322;
    ////--
    bridge[1].dir[0].first = 0;
    bridge[1].dir[0].second = 1;
    bridge[1].dir[1].first = 1;
    bridge[1].dir[1].second = 0;
    bridge[1].dir[2].first = bridge[0].dir[3].first = 322;
    bridge[1].dir[2].second = bridge[0].dir[3].second = 322;
    ////--
    bridge[2].dir[0].first = 0;
    bridge[2].dir[0].second = 1;
    bridge[2].dir[1].first = 322;
    bridge[2].dir[1].second = 322;
    bridge[2].dir[2].first = bridge[0].dir[3].first = 322;
    bridge[2].dir[2].second = bridge[0].dir[3].second = 322;
    ////--
    bridge[3].dir[0].first = -1;
    bridge[3].dir[0].second = 0;
    bridge[3].dir[1].first = 0;
    bridge[3].dir[1].second = -1;
    bridge[3].dir[2].first = bridge[0].dir[3].first = 322;
    bridge[3].dir[2].second = bridge[0].dir[3].second = 322;
    ////--

    bridge[4].dir[0].first = -1;
    bridge[4].dir[0].second = 0;
    bridge[4].dir[1].first = 0;
    bridge[4].dir[1].second = 1;
    bridge[4].dir[2].first = bridge[0].dir[3].first = 322;
    bridge[4].dir[2].second = bridge[0].dir[3].second = 322;
    ////--
    bridge[5].dir[0].first = -1;
    bridge[5].dir[0].second = 0;
    bridge[5].dir[1].first = 0;
    bridge[5].dir[1].second = 1;
    bridge[5].dir[2].first = bridge[0].dir[3].first = 322;
    bridge[5].dir[2].second = bridge[0].dir[3].second = 322;
    ////--
    bridge[6].dir[0].first = 0;
    bridge[6].dir[0].second = -1;
    bridge[6].dir[1].first = -1;
    bridge[6].dir[1].second = 0;
    bridge[6].dir[2].first = bridge[0].dir[3].first = 322;
    bridge[6].dir[2].second = bridge[0].dir[3].second = 322;

    for (int i = 0; i < 7; i++)
    {
        bridge[i].i = bridge[i].j = -1;
        bridge[i].pod.setRotation(0);
    }

    /* const float transformspeed = 0.5;
     sf::Clock time;
     sf::Time timpcurent;
     for (int i = 0; i < 7; i++)
         for (int t = 255; t > 0; t = t - transformspeed*timpcurent.asSeconds())
         {
             timpcurent = time.restart();
             bridge[i].pod.setColor(sf::Color(255, 255, 255, t));
         }
         */
    bridge[0].pod.setPosition(935, 219);

    bridge[1].pod.setPosition(1191, 219);

    bridge[2].pod.setPosition(1319, 366);

    bridge[3].pod.setPosition(1036, 631);

    bridge[4].pod.setPosition(1049, 463);

    bridge[5].pod.setPosition(1321, 553);

    bridge[6].pod.setPosition(1230, 700);
    situatie[0].poz[0] = bridge[0].pod.getPosition();
    situatie[0].poz[1] = bridge[1].pod.getPosition();
    situatie[0].poz[2] = bridge[2].pod.getPosition();
    situatie[0].poz[3] = bridge[3].pod.getPosition();
    situatie[0].poz[4] = bridge[4].pod.getPosition();
    situatie[0].poz[5] = bridge[5].pod.getPosition();
    situatie[0].poz[6] = bridge[6].pod.getPosition();
    for (int i = 0; i < 16; i++)
        for (int j = 0; j < 16; j++)
            situatie[0].BoardState[i][j] = BoardState[i][j];
    for (int i = 0; i < 7; i++)
    {
        for (int j = 0; j < 4; j++)
        {
            situatie[0].dir[i][j].x = bridge[i].dir[j].first;
            situatie[0].dir[i][j].y = bridge[i].dir[j].second;
        }
        situatie[0].rotatie[i] = bridge[i].pod.getRotation();
    }

  
  

}

namespace Texture
{
    enum ID
    {
        roundSquare,
        tile,
        road1,
        road2,
        road3,
        bridge1,
        bridge2,
        bridge3,
        bridge4,
        temple1,
        temple2,
        temple3,
        donebutton,
        undobutton,
        redobutton,
        ceasbkg,
        border1,
        border2,
        home,
        winicon,
        winbutton,
        menu_bkg
    };
}

namespace Fonts
{
    enum ID
    {
        arial,
        courier,
        lunabar,
        digitalclock,
        bookantq,//etc.
    };
}

template<typename Identificatori, typename Resurse>
struct ResourceHolder
{
    harta_stl<Identificatori, Resurse> mResourceMap;
    Resurse& get(Identificatori id)
    {
        //struct pereche<Identificatori,Resurse>* 
        typename harta_stl<Identificatori, Resurse>::iterator
            it = mResourceMap.find(id);
        if (it == mResourceMap.end) exit(0);
        return it->second;
    }
    void load(Identificatori id, const char filename[100])
    {
        Resurse rez;
        if (!rez.loadFromFile(filename))
            std::cout << "ResourceHolder::load Incarcare esuata ";
        mResourceMap.insert(id, std::move(rez));
    }
    ResourceHolder() {}

};
ResourceHolder<Texture::ID, sf::Texture> a;
ResourceHolder<Fonts::ID, sf::Font> fontc;

void parcurgere(int i, int j)
{
    if (isDebugging == 1) std::cout << "_______________" << std::endl;
    if (i < 0 || i>14 || j < 0 || j>14) return;
    if (BoardState[i][j] && viz[i][j] != 1) viz[i][j] = 1;
    else return;
    for (int k = 0; k < 15; k++)
    {
        for (int q = 0; q < 15; q++)
            if (isDebugging == 1) std::cout << viz[k][q] << " ";
        if (isDebugging == 1) std::cout << std::endl;
    }
    if ((i - 1) % 3 == 0 && (j - 1) % 3 == 0)
    {

        if (BoardState[i - 1][j] > 3 && BoardState[i + 1][j] > 3 && BoardState[i - 1][j] != BoardState[i + 1][j])
        {
            if (BoardState[i][j] != 15) viz[i][j] = 0;
            parcurgere(i - 1, j);
            parcurgere(i + 1, j);
            BoardState[i - 1][j] = 0;
            BoardState[i + 1][j] = 0;
        }
        else
            if (BoardState[i][j - 1] > 3 && BoardState[i][j + 1] > 3 && BoardState[i][j - 1] != BoardState[i][j + 1])
            {
                if (BoardState[i][j] != 15) viz[i][j] = 0;
                parcurgere(i, j - 1);
                parcurgere(i, j + 1);
                BoardState[i][j - 1] = 0;
                BoardState[i][j + 1] = 0;
            }
            else
            {
                parcurgere(i - 1, j);
                parcurgere(i + 1, j);
                parcurgere(i, j - 1);
                parcurgere(i, j + 1);
            }
    }
    else
    {
        parcurgere(i - 1, j);
        parcurgere(i + 1, j);
        parcurgere(i, j - 1);
        parcurgere(i, j + 1);
    }
}
void prepWork()
{
    for (int i = 0; i < 5; i++)
        for (int j = 0; j < 5; j++)
        {
            int X = 3 * i + 1;
            int Y = 3 * j + 1;
            if (BoardState[X][Y] == 0)
                if (BoardState[X - 1][Y] > 3 && BoardState[X + 1][Y] > 3 && BoardState[X - 1][Y] != BoardState[X + 1][Y])
                    BoardState[X][Y] = 15;
                else
                    if (BoardState[X][Y - 1] > 3 && BoardState[X][Y + 1] > 3 && BoardState[X][Y - 1] != BoardState[X][Y + 1])
                        BoardState[X][Y] = 15;

        }
    for (int i = 3; i < 7; i++)
    {
        int X = 3 * bridge[i].i + 1 + 3 * bridge[i].dir[0].first;
        int Y = 3 * bridge[i].j + 1 + 3 * bridge[i].dir[0].second;
        if (BoardState[X][Y])
            if (BoardState[X + bridge[i].dir[0].first][Y + bridge[i].dir[0].second] == 0)
            {
                BoardState[X - bridge[i].dir[0].first][Y - bridge[i].dir[0].second] = 0;
            }
    }
    if (isDebugging == 1)std::cout << "----------" << std::endl;
    for (int p = 0; p < 15; p++)
    {
        for (int q = 0; q < 15; q++)
            if (isDebugging == 1)  std::cout << std::setw(4) << BoardState[p][q] << " ";
        if (isDebugging == 1)  std::cout << std::endl;
    }

}
void checkWin()
{
    prepWork();
    for (int i = 0; i < 3; i++)
        if (temples[i].i != -1 && temples[i].j != -1)
        {
       
            parcurgere(3 * temples[i].i + 1, 3 * temples[i].j + 1);
            break;
        }

    for (int i = 0; i < 15; i++)
    {
        for (int j = 0; j < 15; j++)
            if (isDebugging == 1) std::cout << viz[i][j] << " ";
        if (isDebugging == 1) std::cout << std::endl;
    }
    if (isDebugging == 1)std::cout << isTemple1present << " " << isTemple2present << " " << isTemple3present << std::endl;
    for (int i = 0; i <= 2; i++)
        if (isDebugging == 1) std::cout << temples[i].i << " " << temples[i].j << std::endl;
    if (
        (!isTemple1present || viz[3 * temples[0].i + 1][3 * temples[0].j + 1]) &&
        (!isTemple2present || viz[3 * temples[1].i + 1][3 * temples[1].j + 1]) &&
        (!isTemple3present || viz[3 * temples[2].i + 1][3 * temples[2].j + 1])
       )
    {
        if (isDebugging == 1)  std::cout << "WIN" << std::endl;
        //wait 5 seconds
        //back to menu
        win.play();
        alreadyWon = 1;
        sf::sleep(sf::seconds(2));
    }

    else {
        if (isDebugging == 1)  std::cout << "LOSE" << std::endl;
        keepTimer = 1;
        reset_level();
    }
}
void analyze()
{

    for (int i = 0; i < 5; i++)
        for (int j = 0; j < 5; j++)
        {
            int X = 3 * i + 1;
            int Y = 3 * j + 1;
            if (BoardState[X][Y] == 10)
            {
                sf::FloatRect tile_bounds = tiles[i][j].getGlobalBounds();
                temples[0].temple.setOrigin(57, 72);

                temples[0].temple.setPosition(tile_bounds.left + tile_bounds.width / 2.f, tile_bounds.top + tile_bounds.height / 2);
                if(BoardState[X-1][Y]&&BoardState[X][Y+1]) temples[0].temple.setRotation(0);
                if (BoardState[X][Y + 1] && BoardState[X + 1][Y]) temples[0].temple.setRotation(90);
                if (BoardState[X][Y - 1] && BoardState[X + 1][Y]) temples[0].temple.setRotation(180);
                if (BoardState[X - 1][Y] && BoardState[X][Y - 1]) temples[0].temple.setRotation(270);
                isTemple1present = 1;
                temples[0].i = i;
                temples[0].j = j;
            }
            if (BoardState[X][Y] == 11)
            {
                sf::FloatRect tile_bounds = tiles[i][j].getGlobalBounds();
                temples[1].temple.setOrigin(56, 56);
                temples[1].temple.setPosition(tile_bounds.left + tile_bounds.width / 2.f, tile_bounds.top + tile_bounds.height / 2);
                if (BoardState[X][Y + 1] && BoardState[X + 1][Y] && BoardState[X][Y + 2]) temples[1].temple.setRotation(0);
                if (BoardState[X][Y - 1] && BoardState[X + 1][Y] && BoardState[X + 2][Y]) temples[1].temple.setRotation(90);
                if (BoardState[X][Y - 2] && BoardState[X][Y - 1] && BoardState[X + 1][Y]) temples[1].temple.setRotation(180);
                if (BoardState[X - 1][Y] && BoardState[X - 2][Y] && BoardState[X][Y + 1]) temples[1].temple.setRotation(270);
                isTemple2present = 1;
                temples[1].i = i;
                temples[1].j = j;
                
            }
            if (BoardState[X][Y] == 12)
            {
                sf::FloatRect tile_bounds = tiles[i][j].getGlobalBounds();
                temples[2].temple.setOrigin(74, 129);
                temples[2].temple.setPosition(tile_bounds.left + tile_bounds.width / 2.f, tile_bounds.top + tile_bounds.height / 2);
                if (BoardState[X][Y - 1] && BoardState[X - 1][Y] && BoardState[X - 2][Y]) temples[2].temple.setRotation(0);
                if (BoardState[X - 1][Y] && BoardState[X][Y + 1] && BoardState[X][Y + 2]) temples[2].temple.setRotation(90);
                if (BoardState[X + 1][Y] && BoardState[X + 2][Y] && BoardState[X][Y + 1]) temples[2].temple.setRotation(180);
                if (BoardState[X][Y - 1] && BoardState[X][Y - 2] && BoardState[X + 1][Y]) temples[2].temple.setRotation(270);
                isTemple3present = 1;
                temples[2].i = i;
                temples[2].j = j;
            }
        }
    if (isTemple1present == 0) temples[0].temple.setColor(sf::Color(255, 255, 255, 0));
        else temples[0].temple.setColor(sf::Color(255, 255, 255, 255));
    if (isTemple2present == 0) temples[1].temple.setColor(sf::Color(255, 255, 255, 0));
        else temples[1].temple.setColor(sf::Color(255, 255, 255, 255));
    if (isTemple3present == 0) temples[2].temple.setColor(sf::Color(255, 255, 255, 0));
        else temples[2].temple.setColor(sf::Color(255, 255, 255, 255));
}
void all_load()
{
    meep_merp.loadFromFile("immune.wav");
    bpress.loadFromFile("Button_Push.wav");
    charge.loadFromFile("charge.wav");
    check.loadFromFile("checkbutton.wav");
    right.setBuffer(bpress);
    wrong.setBuffer(meep_merp);
    wrong.setVolume(50);
    win.setBuffer(charge);
    checkbutton.setBuffer(check);
    for (int i = 0; i < 16; i++)
        for (int j = 0; j < 16; j++)
            viz[i][j] = 0;
    ////
    ////initializare directii
    ////
    ////----
    bridge[0].dir[0].first = 0;
    bridge[0].dir[0].second = +1;
    bridge[0].dir[1].first = 1;
    bridge[0].dir[1].second = 0;
    bridge[0].dir[2].first = bridge[0].dir[3].first = 322;
    bridge[0].dir[2].second = bridge[0].dir[3].second = 322;
    ////--
    bridge[1].dir[0].first = 0;
    bridge[1].dir[0].second = 1;
    bridge[1].dir[1].first = 1;
    bridge[1].dir[1].second = 0;
    bridge[1].dir[2].first = bridge[0].dir[3].first = 322;
    bridge[1].dir[2].second = bridge[0].dir[3].second = 322;
    ////--
    bridge[2].dir[0].first = 0;
    bridge[2].dir[0].second = 1;
    bridge[2].dir[1].first = 322;
    bridge[2].dir[1].second = 322;
    bridge[2].dir[2].first = bridge[0].dir[3].first = 322;
    bridge[2].dir[2].second = bridge[0].dir[3].second = 322;
    ////--
    bridge[3].dir[0].first = -1;
    bridge[3].dir[0].second = 0;
    bridge[3].dir[1].first = 0;
    bridge[3].dir[1].second = -1;
    bridge[3].dir[2].first = bridge[0].dir[3].first = 322;
    bridge[3].dir[2].second = bridge[0].dir[3].second = 322;
    ////--

    bridge[4].dir[0].first = -1;
    bridge[4].dir[0].second = 0;
    bridge[4].dir[1].first = 0;
    bridge[4].dir[1].second = 1;
    bridge[4].dir[2].first = bridge[0].dir[3].first = 322;
    bridge[4].dir[2].second = bridge[0].dir[3].second = 322;
    ////--
    bridge[5].dir[0].first = -1;
    bridge[5].dir[0].second = 0;
    bridge[5].dir[1].first = 0;
    bridge[5].dir[1].second = 1;
    bridge[5].dir[2].first = bridge[0].dir[3].first = 322;
    bridge[5].dir[2].second = bridge[0].dir[3].second = 322;
    ////--
    bridge[6].dir[0].first = 0;
    bridge[6].dir[0].second = -1;
    bridge[6].dir[1].first = -1;
    bridge[6].dir[1].second = 0;
    bridge[6].dir[2].first = bridge[0].dir[3].first = 322;
    bridge[6].dir[2].second = bridge[0].dir[3].second = 322;
    fontc.load(Fonts::bookantq, "BKANT.TTF");
    a.load(Texture::tile, "tile2.png");
    a.load(Texture::road1, "road1_mask.png");
    a.load(Texture::road2, "road2_mask.png");
    a.load(Texture::road3, "road3_mask.png");
    a.load(Texture::bridge1, "bridge1_mask.png");
    a.load(Texture::bridge2, "bridge2_mask.png");
    a.load(Texture::bridge3, "bridge3_mask.png");
    a.load(Texture::bridge4, "bridge4_mask.png");
    a.load(Texture::temple1, "temple1.png");
    a.load(Texture::temple2, "temple2.png");
    a.load(Texture::temple3, "temple3.png");
    a.load(Texture::donebutton, "donebutton.png");
    a.load(Texture::undobutton, "undo.png");
    a.load(Texture::redobutton, "redo.png");
    a.load(Texture::ceasbkg, "backgroundceas.png");
    a.load(Texture::border1, "border2.png");
    a.load(Texture::home, "home.png");
    a.load(Texture::winicon, "checkmark.png");
    a.load(Texture::winbutton, "backtomenuwithlevelsbutton.png");
    fontc.load(Fonts::digitalclock, "digital-7.ttf");
    ClockTimer.setFont(fontc.get(Fonts::digitalclock));
    ClockTimer.setPosition(778, 59);
    textX = 778;
    textY = 59;
    ceas_bkg.setTexture(a.get(Texture::ceasbkg));
    ceas_bkg.setPosition(780, 60);
    ceas_bkg.setOrigin(285, 75);
    ceas_bkg.setScale(0.5, 0.5);
    temples[0].i = -1;
    temples[0].j = -1;
    temples[1].i = -1;
    temples[1].j = -1;
    temples[2].i = -1;
    temples[2].j = -1;
    for (int i = 0; i < 5; i++) //Initialize tiles
        for (int j = 0; j < 5; j++)
        {
            tiles[i][j].setTexture(a.get(Texture::tile));
            tiles[i][j].setOrigin(tileW / 2, tileH / 2);
            tiles[i][j].setPosition(sf::Vector2f(100 + j * tileH, 165 + i * tileW));
            tiles[i][j].setScale(sf::Vector2f(1 / div1, 1 / div2));
        }
    temples[0].temple.setTexture(a.get(Texture::temple1));
    temples[1].temple.setTexture(a.get(Texture::temple2));
    temples[2].temple.setTexture(a.get(Texture::temple3));
    Deck.setOrigin(0, 0);
    Deck.setPosition(840, 143);
    Deck.setSize(sf::Vector2f(720, 645));
    Deck.setFillColor(sf::Color(0, 119, 255));
    Deck.setOutlineColor(sf::Color::White);
    Deck.setOutlineThickness(3);
    Deck_Text.setPosition(1381, 105);
    Deck_Text.setFont(fontc.get(Fonts::bookantq));
    Deck_Text.setString("DECK AREA");
    Deck_Text.setFillColor(sf::Color::White);
    border.setTexture(a.get(Texture::border1));
    border.setPosition(100-52,165-56);


    donebutton.setTexture(a.get(Texture::donebutton));
    donebutton.setPosition(178, 852);
    donebutton.setOrigin(345 / 2.f, 141 / 2.f);
    donebutton.setScale(0.5, 0.5);

    undobutton.setTexture(a.get(Texture::undobutton));
    undobutton.setPosition(130, 60);
    undobutton.setOrigin(512 / 2.f, 512 / 2.f);
    undobutton.setScale(0.2, 0.2);

    redobutton.setTexture(a.get(Texture::redobutton));
    redobutton.setPosition(235,60);
    redobutton.setOrigin(512 / 2.f, 512 / 2.f);
    redobutton.setScale(0.2, 0.2);
    
    homebutton.setTexture(a.get(Texture::home));
    homebutton.setScale(1 / 7.f, 1 / 7.f);
    homebutton.setPosition(1490, 20);
    
    winmark.setTexture(a.get(Texture::winicon));
    winmark.setOrigin(256, 256);
    winmark.scale(2 / 3.f, 2 / 3.f);
    winmark.setPosition(800, 350);
    winbutton.setTexture(a.get(Texture::winbutton));
    winbutton.setPosition(620, 550);
    winbutton.setScale(0.5, 0.5);
    

    
    bridge[0].pod.setOrigin(50, 55);
    bridge[0].pod.setTexture(a.get(Texture::road1));
    bridge[0].maxCol = 1;
    





   
    bridge[1].pod.setOrigin(50, 55);
    bridge[1].pod.setTexture(a.get(Texture::road2));
    bridge[1].maxCol = 1;



   
    bridge[2].pod.setOrigin(45, 70);
    bridge[2].pod.setTexture(a.get(Texture::road3));



    bridge[3].pod.setOrigin(196, 130);
    // bridge[3].pod.setScale(roadScale);
    bridge[3].pod.setTexture(a.get(Texture::bridge1));
   

    bridge[4].pod.setOrigin(60, 215 - 85);

    // bridge[4].pod.setScale(roadScale);
    bridge[4].pod.setTexture(a.get(Texture::bridge2));



    bridge[5].pod.setOrigin(37, 145);
    // bridge[5].pod.setScale(roadScale);
    bridge[5].pod.setTexture(a.get(Texture::bridge3));


  
    bridge[6].pod.setOrigin(137, 167);
    // bridge[6].pod.setScale(roadScale);
    bridge[6].pod.setTexture(a.get(Texture::bridge4));
    /////
    bridge[0].pod.setPosition(935, 219);

    bridge[1].pod.setPosition(1191, 219);

    bridge[2].pod.setPosition(1319, 366);

    bridge[3].pod.setPosition(1036, 631);

    bridge[4].pod.setPosition(1049, 463);

    bridge[5].pod.setPosition(1321, 553);

    bridge[6].pod.setPosition(1230, 700);
 
    

}
void drawThings(sf::RenderWindow& window)
{
    window.draw(Deck);
    window.draw(Deck_Text);
    if(alreadyWon==0) TimeElapsed = Counter.getElapsedTime();
    char s[30];
    _itoa_s(TimeElapsed.asSeconds(), s, 10);
    int aux = TimeElapsed.asSeconds();
    int spc = 0;
    while (aux)
    {
        aux /= 10;
        spc++;
    }
    ClockTimer.setPosition(sf::Vector2f(textX-(int)ClockTimer.getCharacterSize()/10 * spc, textY));
    ClockTimer.setString(s);
    window.draw(ceas_bkg);
    window.draw(ClockTimer);
    window.draw(donebutton);
    window.draw(undobutton);
    window.draw(redobutton);
    window.draw(homebutton);
   
    for (int i = 0; i < 5; i++)
        for (int j = 0; j < 5; j++)
        {
            window.draw(tiles[i][j]);
        }
    window.draw(border);
   if(needsUpdate) setPriorities();
   if (isDragging == 1)
   {
       bridge[pozitie].pod.setPosition(mousePos);
       for (int h = 0; h < 4; h++)
           bridge[pozitie].Collision[h].setPosition(bridge[pozitie].pod.getPosition() +bridge[pozitie].sgn* bridge[pozitie].pod.getOrigin());
   }
    for (int i = 0; i < prioritati.size; i++)
        if (prioritati[i] >8) 
            switch (prioritati[i])
            {
            case 10:window.draw(temples[0].temple);
                break;
            case 11: window.draw(temples[1].temple);
                break;
            case 12: window.draw(temples[2].temple);
                break;
            }   
        else
            window.draw(bridge[prioritati[i]].pod);
    if (alreadyWon == 1)
    {
        window.draw(winmark);
        window.draw(winbutton);
    }
   
}
bool checkAvailability(int i, int j)
{
    for (int k = 0; k < 15; k++)
    {
        for (int q = 0; q < 15; q++)
            if (isDebugging == 1)   std::cout << std::setw(4) << BoardState[k][q] << " ";
        if (isDebugging == 1)  std::cout << std::endl;
    }
    if (isDebugging == 1) std::cout << "-----------" << std::endl;
    if (BoardState[1 + i * 3][3 * j + 1])
    {
        wrong.play();
        return 0;
    }
    {
        int X, Y;
        switch (pozitie) //in detaliu
        {
        case 0:
        case 1:
            X = 3 * i + 1;
            Y = 3 * j + 1;
            for (int c = 0; c < 4; c++)
                if (bridge[pozitie].dir[c].first != 322 && bridge[pozitie].dir[c].second != 322)
                {
                    
                        if (BoardState[X + bridge[pozitie].dir[c].first][Y + bridge[pozitie].dir[c].second])
                        {
                            wrong.play();
                            return 0;
                        }
                        if (BoardState[X - bridge[pozitie].dir[c].first][Y - bridge[pozitie].dir[c].second])
                        {
                            wrong.play();
                            return 0;
                        }
                }

            break;
        case 3:
        case 4:
            X = 3 * i + 1;
            Y = 3 * j + 1;
            for (int c = 0; c < 4; c++)
                if (bridge[pozitie].dir[c].first != 322 && bridge[pozitie].dir[c].second != 322)
                {
                    if (BoardState[X + bridge[pozitie].dir[c].first][Y + bridge[pozitie].dir[c].second])
                    {
                        wrong.play();
                        return 0;
                    }
                    if (BoardState[X - bridge[pozitie].dir[c].first][Y - bridge[pozitie].dir[c].second])
                    {
                        wrong.play();
                        return 0;

                    }
                    if (c == 0);
                }

            break;
        case 5:
        case 6:
            X = 3 * i + 1;
            Y = 3 * j + 1;
            for (int c = 0; c < 4; c++)
                if (bridge[pozitie].dir[c].first != 322 && bridge[pozitie].dir[c].second != 322)
                {
                    if (BoardState[X + bridge[pozitie].dir[c].first][Y + bridge[pozitie].dir[c].second])
                    {
                        wrong.play();
                        return 0;
                    }
                    if (BoardState[X - bridge[pozitie].dir[c].first][Y - bridge[pozitie].dir[c].second])
                    {
                        wrong.play();
                        return 0;
                    }
                }

            break;
        case 2:
            X = 3 * i + 1;
            Y = 3 * j + 1;
            for (int c = 0; c < 4; c++)
                if (bridge[pozitie].dir[c].first != 322 && bridge[pozitie].dir[c].second != 322)
                {
                    if (BoardState[X + bridge[pozitie].dir[c].first][Y + bridge[pozitie].dir[c].second])
                    {
                        wrong.play();
                        return 0;
                    }
                    DoRotation(pozitie, 0);

                    if (BoardState[X + bridge[pozitie].dir[c].first][Y + bridge[pozitie].dir[c].second] ||
                        BoardState[X - bridge[pozitie].dir[c].first][Y - bridge[pozitie].dir[c].second])
                    {
                        DoRotation(pozitie, 1);
                        wrong.play();
                        return 0;
                    }

                    DoRotation(pozitie, 1);
                    if (BoardState[X - bridge[pozitie].dir[c].first][Y - bridge[pozitie].dir[c].second])
                    {
                        wrong.play();
                        return 0;
                    }
                }

            break;
        }
    }

    for (int c = 0; c < 4; c++)
        if (bridge[pozitie].dir[c].first != 322 && bridge[pozitie].dir[c].second != 322)
        {
            int X = 3 * i + 1 + 3 * bridge[pozitie].dir[c].first;
            int Y = 3 * j + 1 + 3 * bridge[pozitie].dir[c].second;
            if (X > 15 || X < 0 || Y>15 || Y < 0)
            {
                wrong.play();
                return 0;
            }
            int stg, dr;
            switch (pozitie)
            {
            case 0:
            case 1:
                if (BoardState[X][Y])

                {
                    wrong.play();
                    return 0;
                }
                if (c == 0)
                {
                    if(BoardState[X-bridge[pozitie].dir[1].first][Y-bridge[pozitie].dir[1].second])
                    {
                        wrong.play();
                        return 0;
                    }
                    if (BoardState[X + bridge[pozitie].dir[1].first][Y + bridge[pozitie].dir[1].second])
                    {
                        wrong.play();
                        return 0;
                    }
                    if (BoardState[X - bridge[pozitie].dir[0].first][Y - bridge[pozitie].dir[0].second])
                    {
                        wrong.play();
                        return 0;
                    }
                }
                else
                {
                    if (BoardState[X - bridge[pozitie].dir[c].first][Y - bridge[pozitie].dir[c].second])
                    {
                        wrong.play();
                        return 0;
                    }
                    if (BoardState[X + bridge[pozitie].dir[c].first][Y + bridge[pozitie].dir[c].second])
                    {
                        wrong.play();
                        return 0;
                    }
                }
                break;
            case 2:
                if (BoardState[X][Y])

                {
                    wrong.play();
                    return 0;
                }

                if (BoardState[X + bridge[pozitie].dir[c].first][Y + bridge[pozitie].dir[c].second]
                    || BoardState[X - bridge[pozitie].dir[c].first][Y - bridge[pozitie].dir[c].second])
                {
                    wrong.play();
                    return 0;
                }

                break;
            case 3:
            case 4:
                if (c != 0)
                    if (BoardState[X][Y])

                    {
                        wrong.play();
                        return 0;
                    }
                if (c != 0)
                {
                    if (BoardState[X + bridge[pozitie].dir[c].first][Y + bridge[pozitie].dir[c].second]
                        || BoardState[X - bridge[pozitie].dir[c].first][Y - bridge[pozitie].dir[c].second])
                    {
                        wrong.play();
                        return 0;
                    }

                }
                else
                {
                    if (BoardState[X - bridge[pozitie].dir[c].first][Y - bridge[pozitie].dir[c].second])
                    {
                        wrong.play();
                        return 0;
                    }
                    DoRotation(pozitie, 0);
                    stg = -100, dr = -1000;
                    stg = BoardState[X - bridge[pozitie].dir[c].first][Y - bridge[pozitie].dir[c].second];
                    DoRotation(pozitie, 1);
                    DoRotation(pozitie, 1);
                    dr = BoardState[X - bridge[pozitie].dir[c].first][Y - bridge[pozitie].dir[c].second];
                    DoRotation(pozitie, 0);
                    if (stg != dr)
                    {
                        wrong.play();
                        return 0;
                    }

                }

                break;
            case 5:
                if (c != 0)
                    if (BoardState[X][Y])
                    {
                        wrong.play();
                        return 0;
                    }
                    else
                        if (BoardState[X + bridge[5].dir[1].first][Y + bridge[5].dir[1].second])
                        {
                            wrong.play();
                            return 0;
                        }
                if (BoardState[X - bridge[pozitie].dir[c].first][Y - bridge[pozitie].dir[c].second])
                {
                    wrong.play();
                    return 0;
                }
                DoRotation(pozitie, 0);
                stg = -100, dr = -1000;
                stg = BoardState[X - bridge[pozitie].dir[c].first][Y - bridge[pozitie].dir[c].second];
                DoRotation(pozitie, 1);
                DoRotation(pozitie, 1);
                dr = BoardState[X - bridge[pozitie].dir[c].first][Y - bridge[pozitie].dir[c].second];
                DoRotation(pozitie, 0);
                if (stg != dr)
                {
                    wrong.play();
                    return 0;
                }
                if (c == 1)
                    if (BoardState[X + bridge[pozitie].dir[0].first][Y + bridge[pozitie].dir[0].second])
                    {
                        wrong.play();
                        return 0;
                    }
                if (c == 1)
                    if (BoardState[X + bridge[pozitie].dir[1].first][Y + bridge[pozitie].dir[1].second])
                    {
                        wrong.play();
                        return 0;
                    }
                break;
            case 6:
                if (c != 0)
                    if (BoardState[X][Y])
                    {
                        wrong.play();
                        return 0;
                    }
                    else
                        if (BoardState[X + bridge[6].dir[1].first][Y + bridge[6].dir[1].second])
                        {
                            wrong.play();
                            return 0;
                        }
                if (BoardState[X - bridge[pozitie].dir[c].first][Y - bridge[pozitie].dir[c].second])
                {
                    wrong.play();
                    return 0;
                }
                DoRotation(pozitie, 0);
                stg = -100, dr = -1000;
                stg = BoardState[X - bridge[pozitie].dir[c].first][Y - bridge[pozitie].dir[c].second];
                DoRotation(pozitie, 1);
                DoRotation(pozitie, 1);
                dr = BoardState[X - bridge[pozitie].dir[c].first][Y - bridge[pozitie].dir[c].second];
                DoRotation(pozitie, 0);
                if (stg != dr)
                {
                    wrong.play();
                    return 0;
                }
                if (c == 1)
                {
                    if (BoardState[X - bridge[pozitie].dir[c].first][Y - bridge[pozitie].dir[c].second])
                    {
                        wrong.play();
                        return 0;
                    }
                    DoRotation(pozitie, 1);
                    if (BoardState[X - bridge[pozitie].dir[c].first][Y - bridge[pozitie].dir[c].second]
                        || BoardState[X + bridge[pozitie].dir[c].first][Y + bridge[pozitie].dir[c].second])
                    {
                        wrong.play();
                        return 0;
                    }
                    DoRotation(pozitie, 0);
                    if (BoardState[X + bridge[pozitie].dir[c].first][Y + bridge[pozitie].dir[c].second])
                    {
                        wrong.play();
                        return 0;
                    }
                }
                break;
            }

        }
    right.play();
    return 1;
}
bool punePiesa(sf::Sprite& tile, int i, int j,int flag=0)
{
    if (flag==1||checkAvailability(i,j))
    {
        sf::FloatRect tile_bounds = tile.getGlobalBounds();
        bridge[pozitie].pod.setPosition(tile_bounds.left + tile_bounds.width / 2, tile_bounds.top + tile_bounds.height / 2);
        for (int k = 0; k < 4; k++)
            bridge[pozitie].Collision[k].setPosition(bridge[pozitie].pod.getPosition() +bridge[pozitie].sgn* bridge[pozitie].pod.getOrigin());
        bridge[pozitie].i = i;
        bridge[pozitie].j = j;
        //
        undoBufferSize++;
        situatie[undoBufferSize].poz[0] = bridge[0].pod.getPosition();
        situatie[undoBufferSize].poz[1] = bridge[1].pod.getPosition();
        situatie[undoBufferSize].poz[2] = bridge[2].pod.getPosition();
        situatie[undoBufferSize].poz[3] = bridge[3].pod.getPosition();
        situatie[undoBufferSize].poz[4] = bridge[4].pod.getPosition();
        situatie[undoBufferSize].poz[5] = bridge[5].pod.getPosition();
        situatie[undoBufferSize].poz[6] = bridge[6].pod.getPosition();
       
        for (int i = 0; i < 7; i++)
        {
            for (int j = 0; j < 4; j++)
            {
                situatie[undoBufferSize].dir[i][j].x = bridge[i].dir[j].first;
                situatie[undoBufferSize].dir[i][j].y = bridge[i].dir[j].second;
            }
            situatie[undoBufferSize].rotatie[i] = bridge[i].pod.getRotation();
        }
        if (still_undoing == 1)
        {
            maxUndoBuffer == undoBufferSize ;
            still_undoing = 0;
        }
        else
        if (undoBufferSize > maxUndoBuffer) maxUndoBuffer = undoBufferSize;
       
       
        //
        BoardState[1 + i * 3][3 * j + 1] = 1 + pozitie;
        {
            int X, Y;
            switch (pozitie) //in detaliu
            {
            case 0:
            case 1:
            case 3:
            case 4:
            case 5:
            case 6:
                X = 3 * i + 1;
                Y = 3 * j + 1;
                for (int c = 0; c < 4; c++)
                    if (bridge[pozitie].dir[c].first != 322 && bridge[pozitie].dir[c].second != 322)
                        BoardState[X + bridge[pozitie].dir[c].first][Y + bridge[pozitie].dir[c].second] = 1 + pozitie;
                break;
            case 2:
                X = 3 * i + 1;
                Y = 3 * j + 1;
                for (int c = 0; c < 4; c++)
                    if (bridge[pozitie].dir[c].first != 322 && bridge[pozitie].dir[c].second != 322)
                    {
                        BoardState[X + bridge[pozitie].dir[c].first][Y + bridge[pozitie].dir[c].second] = 1 + pozitie;
                        DoRotation(pozitie, 0);
                        BoardState[X + bridge[pozitie].dir[c].first][Y + bridge[pozitie].dir[c].second] = 1 + pozitie;
                        DoRotation(pozitie, 1);
                    }
                break;

            }
        }

        for (int c = 0; c < 4; c++)
            if (bridge[pozitie].dir[c].first != 322 && bridge[pozitie].dir[c].second != 322)
            {
                int X = 3 * i + 1 + 3 * bridge[pozitie].dir[c].first;
                int Y = 3 * j + 1 + 3 * bridge[pozitie].dir[c].second;
                switch (pozitie)
                {
                case 0:
                case 1:
                    BoardState[X][Y] = 1 + pozitie;
                    if (c == 1)
                    {
                        BoardState[X + bridge[pozitie].dir[c].first][Y + bridge[pozitie].dir[c].second] = 1 + pozitie;
                        BoardState[X - bridge[pozitie].dir[c].first][Y - bridge[pozitie].dir[c].second] = 1 + pozitie;
                    }   
                    else
                    {
                        BoardState[X - bridge[pozitie].dir[c].first][Y - bridge[pozitie].dir[c].second] = 1 + pozitie;
                        BoardState[X + bridge[pozitie].dir[1].first][Y + bridge[pozitie].dir[1].second] = 1 + pozitie;
                    }
                    break;

                case 2:
                    BoardState[X][Y] = 1 + pozitie;
                    BoardState[X + bridge[pozitie].dir[c].first][Y + bridge[pozitie].dir[c].second] = 1 + pozitie;
                    BoardState[X - bridge[pozitie].dir[c].first][Y - bridge[pozitie].dir[c].second] = 1 + pozitie;
                    break;
                case 3:
                case 4:
                    if (c != 0)
                        BoardState[X][Y] = 1 + pozitie;
                    if (c != 0)
                    {
                        BoardState[X + bridge[pozitie].dir[c].first][Y + bridge[pozitie].dir[c].second] = 1 + pozitie;
                        BoardState[X - bridge[pozitie].dir[c].first][Y - bridge[pozitie].dir[c].second] = 1 + pozitie;

                    }
                    else
                    {
                        BoardState[X - bridge[pozitie].dir[c].first][Y - bridge[pozitie].dir[c].second] = 1 + pozitie;
                    }

                    break;
                case 5:
                    if (c == 1)
                    {
                        BoardState[X][Y] = 1 + pozitie;
                        BoardState[X - bridge[pozitie].dir[c].first][Y - bridge[pozitie].dir[c].second] = 1 + pozitie;
                        BoardState[X + bridge[pozitie].dir[0].first][Y + bridge[pozitie].dir[0].second] = 1 + pozitie;
                    }
                    else
                        BoardState[X - bridge[pozitie].dir[c].first][Y - bridge[pozitie].dir[c].second] = 1 + pozitie;

                    break;
                case 6:
                    if (c == 1)
                        BoardState[X][Y] = 1 + pozitie;
                    BoardState[X - bridge[pozitie].dir[c].first][Y - bridge[pozitie].dir[c].second] = 1 + pozitie;
                    if (c == 1)
                    {

                        DoRotation(pozitie, 0);
                        BoardState[X - bridge[pozitie].dir[c].first][Y - bridge[pozitie].dir[c].second] = 1 + pozitie;
                        DoRotation(pozitie, 1);
                    }
                    break;
                }

            }
      
        for (int k = 0; k < 15; k++)
        {
            for (int q = 0; q < 15; q++)
                if (isDebugging == 1)  std::cout << std::setw(4) << BoardState[k][q] << " ";
            if (isDebugging == 1)  std::cout << std::endl;
        }
        needsUpdate = 1;
        //
        for (int i = 0; i < 16; i++)
            for (int j = 0; j < 16; j++)
                situatie[undoBufferSize].BoardState[i][j] = BoardState[i][j];
        for (int i = 0; i < 7; i++)
        {
            situatie[undoBufferSize].tile[i].x = bridge[i].i;
            situatie[undoBufferSize].tile[i].y = bridge[i].j;
        }

        //
        return 1;
    }
    else return 0;

}
void destroy(int pos)
{
    if (isDebugging == 1) std::cout << "se distruge podul" << pos << std::endl;

    int i_board = 3 * bridge[pos].i + 1;
    int j_board = 3 * bridge[pos].j + 1;
    BoardState[i_board][j_board] = 0;
    {
        int X = i_board, Y = j_board;
        switch (pos) //in detaliu
        {
        case 0:
        case 1:
        case 3:
        case 4:
        case 5:
        case 6:
            for (int c = 0; c < 4; c++)
                if (bridge[pos].dir[c].first != 322 && bridge[pos].dir[c].second != 322)
                    BoardState[X + bridge[pos].dir[c].first][Y + bridge[pos].dir[c].second] = 0;
            break;
        case 2:
            for (int c = 0; c < 4; c++)
                if (bridge[pos].dir[c].first != 322 && bridge[pos].dir[c].second != 322)
                {
                    BoardState[X + bridge[pos].dir[c].first][Y + bridge[pos].dir[c].second] = 0;
                    DoRotation(pos, 0);
                    BoardState[X + bridge[pos].dir[c].first][Y + bridge[pos].dir[c].second] = 0;
                    DoRotation(pos, 1);
                }
            break;
        }
    }

    for (int c = 0; c < 4; c++)
        if (bridge[pos].dir[c].first != 322 && bridge[pos].dir[c].second != 322)
        {
            int  X = i_board + 3 * bridge[pos].dir[c].first;
            int  Y = j_board + 3 * bridge[pos].dir[c].second;
            switch (pos)
            {
            case 0:
            case 1:
                BoardState[X][Y] = 0;
                if (c == 1)
                {
                    BoardState[X + bridge[pos].dir[c].first][Y + bridge[pos].dir[c].second] = 0;
                    BoardState[X - bridge[pos].dir[c].first][Y - bridge[pos].dir[c].second] = 0;
                }
                else
                {
                    BoardState[X - bridge[pos].dir[c].first][Y - bridge[pos].dir[c].second] = 0;
                    BoardState[X + bridge[pos].dir[1].first][Y + bridge[pos].dir[1].second] =0;
                }
                break;
            case 2:
                BoardState[X][Y] = 0;
                BoardState[X + bridge[pos].dir[c].first][Y + bridge[pos].dir[c].second] = 0;
                BoardState[X - bridge[pos].dir[c].first][Y - bridge[pos].dir[c].second] = 0;

                break;
            case 3:
            case 4:
                if (c != 0)
                    BoardState[X][Y] = 0;
                if (c != 0)
                {
                    BoardState[X + bridge[pos].dir[c].first][Y + bridge[pos].dir[c].second] = 0;
                    BoardState[X - bridge[pos].dir[c].first][Y - bridge[pos].dir[c].second] = 0;

                }
                else
                {
                    BoardState[X - bridge[pos].dir[c].first][Y - bridge[pos].dir[c].second] = 0;
                }

                break;
            case 5:
                if (c == 1)
                {
                    BoardState[X][Y] = 0;
                    BoardState[X - bridge[pos].dir[c].first][Y - bridge[pos].dir[c].second] = 0;
                    BoardState[X + bridge[pos].dir[0].first][Y + bridge[pos].dir[0].second] = 0;
                }
                else
                    BoardState[X - bridge[pos].dir[c].first][Y - bridge[pos].dir[c].second] = 0;
                break;
            case 6:
                if (c == 1)
                {
                    BoardState[X][Y] = 0;
                    BoardState[X - bridge[pos].dir[0].first][Y - bridge[pos].dir[0].second] = 0;
                    BoardState[X - bridge[pos].dir[1].first][Y - bridge[pos].dir[1].second] = 0;
                }
                else
                {
                    BoardState[X - bridge[pos].dir[0].first][Y - bridge[pos].dir[0].second] = 0;
                }

            }
        }
    bridge[pos].i = -1;
    bridge[pos].j = -1;
    //undo
    /*
    if (needsUndo == 1) return;
    undoBufferSize++;
    situatie[undoBufferSize].poz[0] = bridge[0].pod.getPosition();
    situatie[undoBufferSize].poz[1] = bridge[1].pod.getPosition();
    situatie[undoBufferSize].poz[2] = bridge[2].pod.getPosition();
    situatie[undoBufferSize].poz[3] = bridge[3].pod.getPosition();
    situatie[undoBufferSize].poz[4] = bridge[4].pod.getPosition();
    situatie[undoBufferSize].poz[5] = bridge[5].pod.getPosition();
    situatie[undoBufferSize].poz[6] = bridge[6].pod.getPosition();
    for (int i = 0; i < 16; i++)
        for (int j = 0; j < 16; j++)
            situatie[undoBufferSize].BoardState[i][j] = BoardState[i][j];
       */
 
}
int handleMouseClick(sf::Event e)
{
    int pos = -1;
    if (e.mouseButton.button == sf::Mouse::Left)
    {
        if (isDragging == 0)
        {
            if (alreadyWon == 0)
            {
                if (Deck.getGlobalBounds().contains(e.mouseButton.x, e.mouseButton.y))
                    for (int i = 6; i >= 0; i--)
                        if (contains(i, e.mouseButton.x, e.mouseButton.y))
                        {
                            if (!Deck.getGlobalBounds().contains(e.mouseButton.x, e.mouseButton.y))
                                destroy(i);
                            needsUpdate = 1;
                            isDragging = 1;
                            return i;
                        }
                        else continue;
                else
                    for (int i = 0; i < 7; i++)
                        if (contains(i, e.mouseButton.x, e.mouseButton.y))
                        {
                            if (!Deck.getGlobalBounds().contains(e.mouseButton.x, e.mouseButton.y))
                                destroy(i);
                            needsUpdate = 1;
                            isDragging = 1;
                            return i;
                        }


            }
               

        }
        else
        {
            if (Deck.getGlobalBounds().contains(e.mouseButton.x, e.mouseButton.y))
            {
                bridge[pozitie].pod.setPosition(e.mouseButton.x, e.mouseButton.y);
                isDragging = 0;
            }
            else
            {
                for (int i = 0; i < 5; i++)
                    for (int j = 0; j < 5; j++)
                        if (tiles[i][j].getGlobalBounds().contains(e.mouseButton.x, e.mouseButton.y))
                        {
                            bool success = 0;
                            if (isDebugging == 1)std::cout << "TILE-UL: " << i * 5 + j << std::endl;
                            success = punePiesa(tiles[i][j], i, j);
                            if (isDebugging == 1)  std::cout << "Success:" << success << std::endl;
                            if (success)
                                isDragging = 0;
                            return -1;
                        }
            }
            
        }
        
        if (donebutton.getGlobalBounds().contains(e.mouseButton.x, e.mouseButton.y))
        {
            if (alreadyWon == 0)
                checkbutton.play();
            checkWin();
        }
        
        if (undobutton.getGlobalBounds().contains(e.mouseButton.x, e.mouseButton.y))
        {
            if(!isDragging&&!alreadyWon)
            doUndo();
        }
        if (redobutton.getGlobalBounds().contains(e.mouseButton.x, e.mouseButton.y))
        {
            if (!isDragging && !alreadyWon)
            doRedo();
        }
        if (homebutton.getGlobalBounds().contains(e.mouseButton.x, e.mouseButton.y))
        {
     
            if (isDebugging == 1) std::cout << "AI APASAT HOMEBUTTON" << std::endl;
            isIngame = 0;
            x = 1;
        }
        if (winbutton.getGlobalBounds().contains(e.mouseButton.x, e.mouseButton.y))
        {
            
            if (isDebugging == 1) std::cout << "AI APASAT WINBUTTON" << std::endl;
            alreadyWon = 0;
            playx = 1;
            isIngame = 0;
        }
    
    }
    if (e.mouseButton.button == sf::Mouse::Right && isDragging)
    {
        isDragging = 0;
        if (Deck.getGlobalBounds().contains(sf::Vector2f(e.mouseButton.x, e.mouseButton.y)))
            bridge[pozitie].pod.setPosition(sf::Vector2f(e.mouseButton.x, e.mouseButton.y));
        else
        {
            bridge[pozitie].pod.setPosition(1271, 520);
            for(int q=0;q<4;q++)
            bridge[pozitie].Collision[q].setPosition(bridge[pozitie].pod.getPosition() +bridge[pozitie].sgn* bridge[pozitie].pod.getOrigin());
        }

    }
    return pos;
}
void DoRotation(int poz, bool dreapta)
{


    for (int i = 0; i < 4; i++)
        if (bridge[poz].dir[i].first != 322 && bridge[poz].dir[i].second != 322)
            if (!dreapta)
            {
                if (bridge[poz].dir[i].first == 0 && bridge[poz].dir[i].second == -1)
                {
                    bridge[poz].dir[i].first = 1; bridge[poz].dir[i].second = 0;
                }
                else
                    if (bridge[poz].dir[i].first == 1 && bridge[poz].dir[i].second == 0)
                    {
                        bridge[poz].dir[i].first = 0; bridge[poz].dir[i].second = 1;
                    }
                    else
                        if (bridge[poz].dir[i].first == 0 && bridge[poz].dir[i].second == 1)
                        {
                            bridge[poz].dir[i].first = -1; bridge[poz].dir[i].second = 0;
                        }
                        else
                            if (bridge[poz].dir[i].first == -1 && bridge[poz].dir[i].second == 0)
                            {
                                bridge[poz].dir[i].first = 0; bridge[poz].dir[i].second = -1;
                            }
                if (isDebugging == 1) std::cout << "LA STANGA:" << bridge[poz].dir[i].first << " " << bridge[poz].dir[i].second << std::endl;
                if (isDebugging == 1)  std::cout << "POZ:" << poz << std::endl;
            
            }
            else
            {
                if (bridge[poz].dir[i].first == 0 && bridge[poz].dir[i].second == -1)
                {
                    bridge[poz].dir[i].first = -1; bridge[poz].dir[i].second = 0;
                }
                else
                    if (bridge[poz].dir[i].first == -1 && bridge[poz].dir[i].second == 0)
                    {
                        bridge[poz].dir[i].first = 0; bridge[poz].dir[i].second = 1;
                    }
                    else
                        if (bridge[poz].dir[i].first == 0 && bridge[poz].dir[i].second == 1)
                        {
                            bridge[poz].dir[i].first = 1; bridge[poz].dir[i].second = 0;
                        }
                        else
                            if (bridge[poz].dir[i].first == 1 && bridge[poz].dir[i].second == 0)
                            {
                                bridge[poz].dir[i].first = 0; bridge[poz].dir[i].second = -1;
                            }
                if (isDebugging == 1)  std::cout << "LA DREAPTA:" << bridge[poz].dir[i].first << " " << bridge[poz].dir[i].second << std::endl;
            }
    if (isDebugging == 1)std::cout << "_______________" << std::endl;
}
void handle_Events(sf::RenderWindow& window)
{
    sf::Event event;
    while (window.pollEvent(event))
    {
        switch (event.type)
        {

        case sf::Event::MouseWheelScrolled:
            if (isDragging)
            {
                int numTicks = 0;
                numTicks = event.mouseWheelScroll.delta;

                if (numTicks > 0)
                {
                    bridge[pozitie].pod.setRotation(bridge[pozitie].pod.getRotation() - 90);
                    DoRotation(pozitie, 0);
                }
                if (numTicks < 0)
                {
                    bridge[pozitie].pod.setRotation(bridge[pozitie].pod.getRotation() + 90);
                    DoRotation(pozitie, 1);
                }
            }
            break;
        case sf::Event::TextEntered:
            buffer[b_iter] =toupper( event.text.unicode);
            b_iter++;
            if (strstr(buffer, "DEBUG")) 
            {
                isDebugging = !isDebugging;
                buffer_clear();
                if (isDebugging == 0)
                {
                    system("cls");
                    std::cout << "EXITING DEBUG MODE" << std::endl;
                }
                else std::cout << "ENTERING DEBUG MODE" << std::endl;
            }
            break;

        case sf::Event::Closed: window.close(); break;
        case sf::Event::MouseButtonPressed:
            ok = handleMouseClick(event);
            if (ok != -1)
            {

                pozitie = ok;
            }
           if(isDebugging==1) std::cout << event.mouseButton.x << " " << event.mouseButton.y << std::endl;
            break;
        case sf::Event::MouseMoved:
            mousePos.x = event.mouseMove.x;
            mousePos.y = event.mouseMove.y;
            break;
        case sf::Event::KeyPressed:
            if(isDebugging)
            switch (event.key.code)
            {
            case sf::Keyboard::Num1:
                strcpy_s(filename, "level1.txt");

                reset_level();
                break;
            case sf::Keyboard::Num2:
                strcpy_s(filename, "level2.txt");
            
                reset_level();
                break;
            case sf::Keyboard::Num3:
                strcpy_s(filename, "level3.txt");
             
                reset_level();
                break;
            }
        
            break;
        }
    }
}
void read()
{
    f.open(filename);
    for (int i = 0; i < 15; i++)
        for (int j = 0; j < 15; j++)
            f >> BoardState[i][j];
    analyze();
    f.close();
}
void all_load_menu()
{
    a.load(Texture::menu_bkg, "homescreen.jpg");
    menu_bkg.setTexture(a.get(Texture::menu_bkg));
    fontc.load(Fonts::arial, "arial.ttf");


    ///
    triangle.setFillColor(sf::Color::Red);
    triangle.setOutlineColor(sf::Color::Black);
    triangle.setOutlineThickness(2);
    triangle.setPosition(sf::Vector2f(288, 225));
    if (!m.font.loadFromFile("arial.ttf"))
    {

    }
    m.option[0].setFont(m.font);
    m.option[0].setCharacterSize(90);
    m.option[0].setFillColor(sf::Color::Black);
    m.option[0].setString("Play");
    m.option[0].setPosition(sf::Vector2f(1600 / 2.3, 900 / 3 * 1.2));

    m.option[1].setFont(m.font);
    m.option[1].setCharacterSize(90);
    m.option[1].setFillColor(sf::Color::White);
    m.option[1].setString("Instructions");
    m.option[1].setPosition(sf::Vector2f(1600 / 2.9, 900 / 3 * 1.6));

    m.option[2].setFont(m.font);
    m.option[2].setCharacterSize(90);
    m.option[2].setFillColor(sf::Color::White);
    m.option[2].setString("Exit");
    m.option[2].setPosition(sf::Vector2f(1600 / 2.3, 900 / 3 * 2));

    m.button[0].setSize(sf::Vector2f(90, 120));
    m.button[0].setPosition(sf::Vector2f(1600 / 2.3, 900 / 3 * 1.2));
    m.button[1].setSize(sf::Vector2f(90, 120));
    m.button[1].setPosition(sf::Vector2f(1600 / 2.9, 900 / 3 * 1.6));
    m.button[2].setSize(sf::Vector2f(90, 120));
    m.button[2].setPosition(sf::Vector2f(1600 / 2.3, 900 / 3 * 2));

    m.selectedOptionIndex = 0;
    ////
    if (!lvl[0].texture.loadFromFile("ch1.png"))
    {

    }
    if (!lvl[1].texture.loadFromFile("ch2.png"))
    {

    }
    if (!lvl[2].texture.loadFromFile("ch3.png"))
    {

    }
    if (!lvl[3].texture.loadFromFile("ch4.png"))
    {

    }
    if (!lvl[4].texture.loadFromFile("ch5.png"))
    {

    }
    if (!lvl[5].texture.loadFromFile("ch6.png"))
    {

    }
    if (!lvl[6].texture.loadFromFile("ch7.png"))
    {

    }
    if (!lvl[7].texture.loadFromFile("ch8.png"))
    {

    }
    if (!lvl[8].texture.loadFromFile("ch9.png"))
    {

    }
    if (!lvl[9].texture.loadFromFile("ch10.png"))
    {

    }
    if (!lvl[10].texture.loadFromFile("ch11.png"))
    {

    }
    if (!lvl[11].texture.loadFromFile("ch12.png"))
    {

    }
    if (!lvl[12].texture.loadFromFile("ch13.png"))
    {

    }
    if (!lvl[13].texture.loadFromFile("ch14.png"))
    {

    }
    if (!lvl[14].texture.loadFromFile("ch15.png"))
    {

    }
    if (!lvl[15].texture.loadFromFile("ch16.png"))
    {

    }
    if (!lvl[16].texture.loadFromFile("ch17.png"))
    {

    }
    if (!lvl[17].texture.loadFromFile("ch18.png"))
    {

    }
    if (!lvl[18].texture.loadFromFile("ch19.png"))
    {

    }
    if (!lvl[19].texture.loadFromFile("ch20.png"))
    {

    }

    for (int i = 0; i < 20; i++)
    {
        lvl[i].rectangle.setSize(sf::Vector2f(150.f, 150.f));
        lvl[i].rectangle.setTexture(&lvl[i].texture);
    }

    lvl[0].rectangle.setPosition(sf::Vector2f(225, 70));
    lvl[1].rectangle.setPosition(sf::Vector2f(475, 70));
    lvl[2].rectangle.setPosition(sf::Vector2f(725, 70));
    lvl[3].rectangle.setPosition(sf::Vector2f(975, 70));
    lvl[4].rectangle.setPosition(sf::Vector2f(1225, 70));
    lvl[5].rectangle.setPosition(sf::Vector2f(225, 250));
    lvl[6].rectangle.setPosition(sf::Vector2f(475, 250));
    lvl[7].rectangle.setPosition(sf::Vector2f(725, 250));
    lvl[8].rectangle.setPosition(sf::Vector2f(975, 250));
    lvl[9].rectangle.setPosition(sf::Vector2f(1225, 250));
    lvl[10].rectangle.setPosition(sf::Vector2f(225, 430));
    lvl[11].rectangle.setPosition(sf::Vector2f(475, 430));
    lvl[12].rectangle.setPosition(sf::Vector2f(725, 430));
    lvl[13].rectangle.setPosition(sf::Vector2f(975, 430));
    lvl[14].rectangle.setPosition(sf::Vector2f(1225, 430));
    lvl[15].rectangle.setPosition(sf::Vector2f(225, 610));
    lvl[16].rectangle.setPosition(sf::Vector2f(475, 610));
    lvl[17].rectangle.setPosition(sf::Vector2f(725, 610));
    lvl[18].rectangle.setPosition(sf::Vector2f(975, 610));
    lvl[19].rectangle.setPosition(sf::Vector2f(1225, 610));

    text.setCharacterSize(35);
    text.setFillColor(sf::Color::White);
    text.setPosition(sf::Vector2f(1600 / 5.4, 900 / 3 * 1.2));
    text.setFont(fontc.get(Fonts::arial));
    text.setString("  Temple Connection is a game in which you have to connect the  \n temples between them so you can walk from each temple to every \nother one. You have 3 pieces with roads and 4 with bridges which\n you can use. The first 5 challenges are at a Starter Difficulty\n    Level, the next 5 are at the Junior Difficulty Level, the   \n challenges from 11 to 15 are at an Expert Difficulty Level and \nthe last 5 are at the Master Difficulty Level. Be careful! There\n	    is only one way you can connect those temples.");




}
void instructions(sf::RenderWindow& window,sf::Event event)
{
    
    switch (event.type)
    {
    case sf::Event::KeyReleased:
        if (event.key.code == sf::Keyboard::Escape)
        {
            instructionsx = false;
            x = true;
        }

    }
}
void moveLeft(int& i)
{
    if (i > 0)
    {
        if ((i > 1 && i < 5) || (i > 5 && i < 10) || (i > 10 && i < 15) || (i > 15 && i < 20))
        {
            triangle.setPosition(sf::Vector2f(triangle.getPosition().x - 250, triangle.getPosition().y));
        }

        else
        {
            triangle.setPosition(sf::Vector2f(1295, triangle.getPosition().y - 180));
        }

        i--;

    }
}

void moveRight( int& i)
{
    if (i < 19)
    {
        if ((i >= 0 && i < 4) || (i > 4 && i < 9) || (i > 9 && i < 14) || (i > 14 && i < 19))
        {
            triangle.setPosition(sf::Vector2f(triangle.getPosition().x + 250, triangle.getPosition().y));
        }

        else
        {
            triangle.setPosition(sf::Vector2f(295, triangle.getPosition().y + 180));
        }

        i++;
    }
}
void MoveUp()
{
    if (m.selectedOptionIndex - 1 >= 0)
    {
        m.option[m.selectedOptionIndex].setFillColor(sf::Color::White);
        m.selectedOptionIndex--;
        m.option[m.selectedOptionIndex].setFillColor(sf::Color::Black);
    }
}

void MoveDown()
{
    if (m.selectedOptionIndex + 1 < 3)
    {
        m.option[m.selectedOptionIndex].setFillColor(sf::Color::White);
        m.selectedOptionIndex++;
        m.option[m.selectedOptionIndex].setFillColor(sf::Color::Black);
    }
}

int GetPressedOption()
{
    if (x == true)
    {
        return m.selectedOptionIndex;
    }
}
void lvlDraw(sf::RenderWindow& window)
{
    for (int i = 0; i < 20; i++)
    {
        window.draw(lvl[i].rectangle);
    }
}


void events(sf::RenderWindow& window,int &i)
{
    sf::Event event;
    while (window.pollEvent(event))
    {
        if (instructionsx == 1)
            instructions(window, event);
        switch (event.type)
        {
      
        case sf::Event::MouseButtonPressed:
            if (x == 1)
            {
                for (int j = 0; j < 3; j++)
                    if (m.option[j].getGlobalBounds().contains(event.mouseButton.x, event.mouseButton.y))
                    {
                        switch (j)
                        {
                        case 0:
                            std::cout << "Play" << std::endl;
                            x = false;
                            playx = true;
                            break;
                        case 1:
                            std::cout << "Instructions" << std::endl;
                            x = false;
                            instructionsx = true;
                            break;
                        case 2:
                            window.close();
                            break;
                        }
                    }
                   
            }
            else if (playx == true)
            {
                for (int k = 0; k < 20; k++)
                    if (lvl[k].rectangle.getGlobalBounds().contains(event.mouseButton.x, event.mouseButton.y))
                    {
                        strcpy_s(filename, "level");
                        _itoa_s(k, istr, 10);
                        strcat_s(filename, istr);
                        strcat_s(filename, ".txt");
                        isIngame = 1;
                        playx = 0;
                        reset_level();
                    }

            }
            break;
        case sf::Event::KeyReleased:
            
            switch (event.key.code)
            {
                //
           
            case sf::Keyboard::Escape:
                if (playx == true)
                {
                    playx = false;
                    x = true;
                }
                break;
            case sf::Keyboard::Left:
                if (playx == true) moveLeft(i);
                break;
            case sf::Keyboard::Right:
                if (playx == true) moveRight(i);
                break;
                //
            case sf::Keyboard::Up:
                MoveUp();
                break;

            case sf::Keyboard::Down:
                MoveDown();
                break;

            case sf::Keyboard::Return:
                if (playx == 1)
                {
                    strcpy_s(filename, "level");
                    _itoa_s(i, istr, 10);
                    strcat_s(filename, istr);
                    strcat_s(filename, ".txt");
                    isIngame = 1;
                    playx = 0;
                    reset_level();
                }
                else
                switch (GetPressedOption())
                {
                case 0:
                   std::cout << "Play" << std::endl;
                     x = false;
                    playx = true;
                    break;
                case 1:
                    std::cout << "Instructions" << std::endl;
                    x = false;
                    instructionsx = true;
                    break;
                case 2:
                    window.close();
                    break;
                }
                break;
          
            }
            break;
        case sf::Event::Closed:
            window.close();
            break;
        }
        break;
    }
}
void menuDraw(sf::RenderWindow& window)
{
    if (x == true)
        for (int i = 0; i < 3; i++)
        {
            window.draw(m.option[i]);
        }

}
int main()
{
  
    all_load_menu();
 
    load1();
    int i = 0;
    all_load();

   // window.setFramerateLimit(10);
    while (window.isOpen())
    {
        if (isIngame == 1) {
            window.clear(sf::Color(0, 127, 196));
            handle_Events(window);
            
            drawThings(window);
            window.display();
        }
        else 
        {
            window.clear();
            window.draw(menu_bkg);
            events(window,i);
            menuDraw(window);
            
            if(playx==true) lvlDraw(window);
            if (playx == true)
            {
                window.draw(triangle);
            }
           if(instructionsx) window.draw(text);
            window.display();
        }

       
    }

}
