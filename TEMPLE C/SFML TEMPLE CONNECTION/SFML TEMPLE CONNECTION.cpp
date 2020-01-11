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
char filename[30];
char buffer[1000];
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
}bridge[7];
sf::Sprite tiles[5][5];
sf::Vector2f mousePos;
sf::RectangleShape Deck;
sf::Sprite donebutton;
sf::Text Deck_Text;
sf::SoundBuffer meep_merp;
sf::SoundBuffer bpress;
sf::Sound wrong, right;
sf::SoundBuffer charge;
sf::Sound win;
sf::SoundBuffer check;
sf::Sound checkbutton;
bool isTemple1present = 0,
isTemple2present = 0,
isTemple3present = 0;
bool needsUpdate = 1;
int pozitie=-1;
int ok = 0;
float BoardState[16][16];
bool viz[16][16];
bool isDragging = 0;
bool alreadyWon = 0;
//scaling
const float div1 = 3.f;
const float div2 = 3.f;
const float tileW = floor(385 / div1); //385 div 3 ~128px tile w and h
const float tileH = floor(385 / div2);
bool contains(int i, float x, float y)
{
    sf::FloatRect bounds= bridge[i].pod.getGlobalBounds();
    if (bounds.contains(x, y))
        return 1;

    return 0;
}
void setPriorities()
{
    prioritati.clear();
    std::cout << std::endl;
    needsUpdate = 0;
    pereche<int, int> p[4];
    int it = 0;
    p[0].first = -1;
    p[0].second = -1;
    p[1].first = -1;
    p[1].second = -1;
    p[2].first = -1;
    p[2].second = -1;
    int crossroad[3];
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
       if(i!=pozitie||isDragging==0) prioritati.push_back(crossroad[i]);
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
        std::cout << prioritati[i] << " ";
    std::cout << std::endl;
    std::cout << "FINAL:crossroads,perechi" << std::endl;
    for (int i = 0; i < cit; i++)
        std::cout << crossroad[i] << " ";
    std::cout << std::endl;
    for (int i = 0; i < it; i++)
        std::cout << p[i].first << " " << p[i].second << std::endl;

}
void DoRotation(int poz, bool dreapta);
void read();
void analyze();
void reset_level()
{
    read();
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
    bridge[0].pod.setPosition(935, 216);
    bridge[1].pod.setPosition(1171, 216);
    bridge[2].pod.setPosition(1278, 253);
    bridge[3].pod.setPosition(934, 553);
    bridge[4].pod.setPosition(1043, 520);
    bridge[5].pod.setPosition(1321, 553);
    bridge[6].pod.setPosition(1147, 722);


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
    };
}

namespace Fonts
{
    enum ID
    {
        arial,
        courier,
        lunabar,
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
    std::cout << "_______________" << std::endl;
    if (i < 0 || i>14 || j < 0 || j>14) return;
    if (BoardState[i][j] && viz[i][j] != 1) viz[i][j] = 1;
    else return;
    for (int k = 0; k < 15; k++)
    {
        for (int q = 0; q < 15; q++)
            std::cout << viz[k][q] << " ";
        std::cout << std::endl;
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
    std::cout << "----------" << std::endl;
    for (int p = 0; p < 15; p++)
    {
        for (int q = 0; q < 15; q++)
            std::cout << std::setw(4) << BoardState[p][q] << " ";
        std::cout << std::endl;
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
            std::cout << viz[i][j] << " ";
        std::cout << std::endl;
    }
    std::cout << isTemple1present << " " << isTemple2present << " " << isTemple3present << std::endl;
    for (int i = 0; i <= 2; i++)
        std::cout << temples[i].i << " " << temples[i].j << std::endl;
    if (
        (!isTemple1present || viz[3 * temples[0].i + 1][3 * temples[0].j + 1]) &&
        (!isTemple2present || viz[3 * temples[1].i + 1][3 * temples[1].j + 1]) &&
        (!isTemple3present || viz[3 * temples[2].i + 1][3 * temples[2].j + 1])
       )
    {
        std::cout << "WIN" << std::endl;
        //wait 5 seconds
        //back to menu
        win.play();
        alreadyWon = 1;
    }

    else {
        std::cout << "LOSE" << std::endl;
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
            tiles[i][j].setPosition(sf::Vector2f(100 + j * tileH, 130 + i * tileW));
            tiles[i][j].setScale(sf::Vector2f(1 / div1, 1 / div2));
        }
    temples[0].temple.setTexture(a.get(Texture::temple1));
    temples[1].temple.setTexture(a.get(Texture::temple2));
    temples[2].temple.setTexture(a.get(Texture::temple3));
    Deck.setOrigin(0, 0);
    Deck.setPosition(840, 108);
    Deck.setSize(sf::Vector2f(720, 645));
    Deck.setFillColor(sf::Color(0, 119, 255));
    Deck_Text.setPosition(857, 129);
    Deck_Text.setFont(fontc.get(Fonts::bookantq));
    Deck_Text.setString("DECK AREA");
    Deck_Text.setFillColor(sf::Color::White);

    donebutton.setTexture(a.get(Texture::donebutton));
    donebutton.setPosition(178, 852);
    donebutton.setOrigin(345 / 2.f, 141 / 2.f);
    donebutton.setScale(0.5, 0.5);



    bridge[0].pod.setPosition(935, 216);
    bridge[0].pod.setOrigin(50, 55);
    bridge[0].pod.setTexture(a.get(Texture::road1));
    bridge[0].maxCol = 1;
    





    bridge[1].pod.setPosition(1171, 216);
    bridge[1].pod.setOrigin(50, 55);
    bridge[1].pod.setTexture(a.get(Texture::road2));
    bridge[1].maxCol = 1;



    bridge[2].pod.setPosition(1278, 253);
    bridge[2].pod.setOrigin(45, 70);
    bridge[2].pod.setTexture(a.get(Texture::road3));


    bridge[3].pod.setPosition(934, 553);
    bridge[3].pod.setOrigin(196, 130);
    // bridge[3].pod.setScale(roadScale);
    bridge[3].pod.setTexture(a.get(Texture::bridge1));
   


    bridge[4].pod.setPosition(1043, 520);
    bridge[4].pod.setOrigin(60, 215 - 85);
    // bridge[4].pod.setScale(roadScale);
    bridge[4].pod.setTexture(a.get(Texture::bridge2));


    bridge[5].pod.setPosition(1321, 553);
    bridge[5].pod.setOrigin(37, 145);
    // bridge[5].pod.setScale(roadScale);
    bridge[5].pod.setTexture(a.get(Texture::bridge3));


    bridge[6].pod.setPosition(1147, 722);
    bridge[6].pod.setOrigin(137, 167);
    // bridge[6].pod.setScale(roadScale);
    bridge[6].pod.setTexture(a.get(Texture::bridge4));

}
void drawThings(sf::RenderWindow& window)
{
    window.draw(Deck);
    window.draw(Deck_Text);
    window.draw(donebutton);
    for (int i = 0; i < 5; i++)
        for (int j = 0; j < 5; j++)
        {
            window.draw(tiles[i][j]);
        }
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
    for (int i = 0; i < 7; i++)
        for (int h = 0; h < 4; h++)
            window.draw(bridge[i].Collision[h]);
}
bool checkAvailability(int i, int j)
{
    for (int k = 0; k < 15; k++)
    {
        for (int q = 0; q < 15; q++)
            std::cout << std::setw(4) << BoardState[k][q] << " ";
        std::cout << std::endl;
    }
    std::cout << "-----------" << std::endl;
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
bool punePiesa(sf::Sprite& tile, int i, int j)
{
    if (checkAvailability(i, j))
    {
        sf::FloatRect tile_bounds = tile.getGlobalBounds();
        bridge[pozitie].pod.setPosition(tile_bounds.left + tile_bounds.width / 2, tile_bounds.top + tile_bounds.height / 2);
        for (int k = 0; k < 4; k++)
            bridge[pozitie].Collision[k].setPosition(bridge[pozitie].pod.getPosition() +bridge[pozitie].sgn* bridge[pozitie].pod.getOrigin());

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
        bridge[pozitie].i = i;
        bridge[pozitie].j = j;
        for (int k = 0; k < 15; k++)
        {
            for (int q = 0; q < 15; q++)
                std::cout << std::setw(4) << BoardState[k][q] << " ";
            std::cout << std::endl;
        }
        needsUpdate = 1;
        return 1;
    }
    else return 0;

}
void destroy(int pos)
{
    std::cout << "se distruge podul" << pos << std::endl;

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
            for (int i = 0; i < 5; i++)
                for (int j = 0; j < 5; j++)
                    if (tiles[i][j].getGlobalBounds().contains(e.mouseButton.x, e.mouseButton.y))
                    {
                        bool success = 0;
                        std::cout << "TILE-UL: " << i * 5 + j << std::endl;
                        success = punePiesa(tiles[i][j], i, j);
                        std::cout << "Success:" << success << std::endl;
                        if (success)
                            isDragging = 0;
                        return -1;
                    }
        }
        if (donebutton.getGlobalBounds().contains(e.mouseButton.x, e.mouseButton.y))
        {
            if (alreadyWon == 0)
                checkbutton.play();
            checkWin();
        }
    }
    if (e.mouseButton.button == sf::Mouse::Right && isDragging)
    {
        isDragging = 0;
        if (Deck.getGlobalBounds().contains(sf::Vector2f(e.mouseButton.x, e.mouseButton.y)))
            bridge[pozitie].pod.setPosition(sf::Vector2f(e.mouseButton.x, e.mouseButton.y));
        else
        {
            bridge[pozitie].pod.setPosition(900, 240);
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
                std::cout << "LA STANGA:" << bridge[poz].dir[i].first << " " << bridge[poz].dir[i].second << std::endl;
                std::cout << "POZ:" << poz << std::endl;
            
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
                std::cout << "LA DREAPTA:" << bridge[poz].dir[i].first << " " << bridge[poz].dir[i].second << std::endl;
            }
    std::cout << "_______________" << std::endl;
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
        case sf::Event::Closed: window.close(); break;
        case sf::Event::MouseButtonPressed:
            ok = handleMouseClick(event);
            if (ok != -1)
            {

                pozitie = ok;
            }
            std::cout << event.mouseButton.x << " " << event.mouseButton.y << std::endl;
            break;
        case sf::Event::MouseMoved:
            mousePos.x = event.mouseMove.x;
            mousePos.y = event.mouseMove.y;
            break;
        case sf::Event::KeyPressed:
            switch (event.key.code)
            {
            case sf::Keyboard::Num1:
                strcpy_s(filename, "level1.txt");
                std::cout << "SADASDASDSADASDASF";
                reset_level();
                break;
            case sf::Keyboard::Num2:
                strcpy_s(filename, "level2.txt");
                std::cout << "SADASDASDSADASDASF";
                reset_level();
                break;
            case sf::Keyboard::Num3:
                strcpy_s(filename, "level3.txt");
                std::cout << "SADASDASDSADASDASF";
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
    f.close();
}
int main()
{
    all_load();
    strcpy_s(filename, "level3.txt");
    read();
    analyze();
    sf::RenderWindow window(sf::VideoMode(1600, 900), "text", sf::Style::Close);
    window.setFramerateLimit(240);
    while (window.isOpen())
    {
        handle_Events(window);
        window.clear(sf::Color(0, 127, 196));
        drawThings(window);
        window.display();
    }
}
