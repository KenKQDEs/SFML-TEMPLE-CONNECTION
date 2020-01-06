#include <iostream>
#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include <SFML/Window.hpp>
#include "STRUCTURA_DATE_HARTA.hpp"
#include <assert.h>
#include <math.h>
#include <fstream>
#include <iomanip>
std::fstream f;

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
int pozitie;
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
void DoRotation(int poz, bool dreapta);
void read();
void reset_level()
{
    read();
    alreadyWon = 0;
    pozitie = 0;
    ok = 0;
    isDragging = 0;
    for (int i = 0; i < 16; i++)
        for (int j = 0; j < 16; j++)
            viz[i][j] = 0;
    bridge[0].dir[0].first = 0;
    bridge[0].dir[0].second = -1;
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
    sf::sleep(sf::milliseconds(100));
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
        int X = 3 * bridge[i].i+1+3*bridge[i].dir[0].first;
        int Y = 3 * bridge[i].j+1 + 3 * bridge[i].dir[0].second;
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
            std::cout <<std::setw(4)<< BoardState[p][q] << " ";
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
    if (
        (!isTemple1present || viz[3 * temples[0].i + 1][3 * temples[0].j + 1]) &&
        (!isTemple2present || viz[3 * temples[1].i + 1][3 * temples[1].j + 1]) &&
        (!isTemple2present || viz[3 * temples[2].i + 1][3 * temples[2].j + 1])
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
                temples[0].temple.setOrigin(56, 72);

                temples[0].temple.setPosition(tile_bounds.left + tile_bounds.width / 2.f, tile_bounds.top + tile_bounds.height / 2);

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
                temples[1].temple.setOrigin(56, 55);
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

    ////
    ////initializare directii
    ////
    ////----
    bridge[0].dir[0].first = 0;
    bridge[0].dir[0].second = -1;
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
    Deck.setPosition(740, 108);
    Deck.setSize(sf::Vector2f(760, 645));
    Deck.setFillColor(sf::Color(0, 119, 255));
    Deck_Text.setPosition(757, 129);
    Deck_Text.setFont(fontc.get(Fonts::bookantq));
    Deck_Text.setString("DECK AREA");
    Deck_Text.setFillColor(sf::Color::White);

    donebutton.setTexture(a.get(Texture::donebutton));
    donebutton.setPosition(178, 802);
    donebutton.setOrigin(345 / 2.f, 141 / 2.f);
    donebutton.setScale(0.5, 0.5);



    bridge[0].pod.setPosition(935, 216);
    bridge[0].pod.setOrigin(190, 60);
    bridge[0].pod.setTexture(a.get(Texture::road1));
    bridge[0].maxCol = 2;
    /*  bridge[0].Collision[0].setFillColor(sf::Color(0, 0, 0, 0));
    bridge[0].Collision[1].setFillColor(sf::Color(0, 0, 0, 0));
    bridge[0].Collision[0].setOutlineColor(sf::Color(0, 255, 255, 0));
    bridge[0].Collision[1].setOutlineColor(sf::Color(0, 255, 255, 0));
    bridge[0].Collision[0].setOutlineThickness(5);*/







    bridge[1].pod.setPosition(1171, 216);
    bridge[1].pod.setOrigin(235-190, 60);
    bridge[1].pod.setTexture(a.get(Texture::road2));


    bridge[2].pod.setPosition(1278, 253);
    bridge[2].pod.setOrigin(45, 65);
    bridge[2].pod.setTexture(a.get(Texture::road3));


    bridge[3].pod.setPosition(934, 553);
    bridge[3].pod.setOrigin(201, 215 - 85);
    // bridge[3].pod.setScale(roadScale);
    bridge[3].pod.setTexture(a.get(Texture::bridge1));


    bridge[4].pod.setPosition(1043, 520);
    bridge[4].pod.setOrigin(45, 215 - 85);
    // bridge[4].pod.setScale(roadScale);
    bridge[4].pod.setTexture(a.get(Texture::bridge2));


    bridge[5].pod.setPosition(1321, 553);
    bridge[5].pod.setOrigin(45, 145);
    // bridge[5].pod.setScale(roadScale);
    bridge[5].pod.setTexture(a.get(Texture::bridge3));


    bridge[6].pod.setPosition(1147, 722);
    bridge[6].pod.setOrigin(127, 170);
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
    if (isDragging) bridge[pozitie].pod.setPosition(mousePos);
    for (int i = 0; i < 3; i++)
        if (!(bridge[i].i >= 0 && bridge[i].j >= 0))
        {
            if (i != pozitie)
                window.draw(bridge[i].pod);
        }


    for (int i = 0; i < 3; i++)
        if ((bridge[i].i >= 0 && bridge[i].j >= 0))
        {
            if (i != pozitie)
                window.draw(bridge[i].pod);
        }

    if (!isDragging) window.draw(bridge[pozitie].pod);

    for (int i = 3; i < 7; i++)
        if ((bridge[i].i >= 0 && bridge[i].j >= 0))
        {
            if (i != pozitie)
                window.draw(bridge[i].pod);
        }

    for (int i = 3; i < 7; i++)
        if (!(bridge[i].i >= 0 && bridge[i].j >= 0))
        {
            if (i != pozitie)
                window.draw(bridge[i].pod);
        }
    for (int i = 0; i < 3; i++)
        window.draw(temples[i].temple);
    if (isDragging) window.draw(bridge[pozitie].pod);


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
                    if (c == 1)
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
                    else
                    {
                        if (BoardState[X - bridge[pozitie].dir[c].first][Y - bridge[pozitie].dir[c].second])
                        {
                            wrong.play();
                            return 0;
                        }
                        if(BoardState[X+bridge[1].dir[1].first][Y+bridge[1].dir[1].second])
                        {
                            wrong.play();
                            return 0;
                        }
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
                            BoardState[X + bridge[1].dir[1].first][Y + bridge[1].dir[1].second] = 1 + pozitie;
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
                    BoardState[X + bridge[pozitie].dir[c].first][Y + bridge[pozitie].dir[c].second] = 0;
                    BoardState[X - bridge[pozitie].dir[c].first][Y - bridge[pozitie].dir[c].second] = 0;
                }
                else
                {
                    BoardState[X - bridge[pozitie].dir[c].first][Y - bridge[pozitie].dir[c].second] = 0;
                    BoardState[X + bridge[1].dir[1].first][Y + bridge[1].dir[1].second] = 0;
                }
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
                    BoardState[X][Y] = 0;
                BoardState[X - bridge[pos].dir[c].first][Y - bridge[pos].dir[c].second] = 0;
                if (c == 1)
                {

                    DoRotation(pos, 0);
                    BoardState[X - bridge[pos].dir[c].first][Y - bridge[pos].dir[c].second] = 0;
                    DoRotation(pos, 1);
                }
                else
                    BoardState[X + bridge[pos].dir[c].first][Y + bridge[pos].dir[c].second] = 0;
                break;

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
                for (int i = 6; i >= 0; i--)
                    if (bridge[i].pod.getGlobalBounds().contains(e.mouseButton.x, e.mouseButton.y))
                    {
                        if (!Deck.getGlobalBounds().contains(e.mouseButton.x, e.mouseButton.y))
                            destroy(i);

                        isDragging = 1;
                        return i;
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
            bridge[pozitie].pod.setPosition(900, 240);

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
        }
    }
}
void read()
{
    f.open("level1.txt");
    for (int i = 0; i < 15; i++)
        for (int j = 0; j < 15; j++)
            f >> BoardState[i][j];
    f.close();
}
int main()
{
    all_load();
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