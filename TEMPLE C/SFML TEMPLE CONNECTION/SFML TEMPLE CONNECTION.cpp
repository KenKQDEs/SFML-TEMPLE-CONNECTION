#include <iostream>
#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include <SFML/Window.hpp>
#include "STRUCTURA_DATE_HARTA.hpp"
#include <assert.h>
#include <math.h>
#define nmax 5
#define mmax 5
#define pi 3.141592653589793
#define rad 180/pi
bool isDragging = 0;
sf::Sprite tiles[nmax][mmax];
sf::Vector2f mousePos;
sf::RectangleShape Deck;
sf::Text Deck_Text;
sf::SoundBuffer meep_merp;
sf::SoundBuffer bpress;
sf::Sound wrong,right;
int pozitie;
int ok = 0;
int BoardState[16][16];
//scaling
const float div1 = 3.f;
const float div2 = 3.f;
const float tileW = floor(385/div1); //385 div 3 ~128px tile w and h
const float tileH = floor(385/div2);
const sf::Vector2f roadScale(0.5f, 0.5f);

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
        //etc
    };
}
struct bridge_
{
    sf::Sprite pod;
    Texture::ID tex;
    pereche<int,int> dir[4];
    int i = -1, j = -1;
}bridge[7];
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

template<typename Identificatori,typename Resurse>
struct ResourceHolder
{
    harta_stl<Identificatori, Resurse> mResourceMap;
    Resurse& get(Identificatori id)
    {
        //struct pereche<Identificatori,Resurse>* 
       typename harta_stl<Identificatori,Resurse>::iterator 
         it = mResourceMap.find(id);
        if (it == mResourceMap.end) exit(0);
        return it->second;
    }
    void load(Identificatori id, const char filename[100])
    {
       Resurse rez;
       if(!rez.loadFromFile(filename)) 
           std::cout<<"ResourceHolder::load Incarcare esuata ";
       mResourceMap.insert(id, std::move (rez));
    }
    ResourceHolder() {}
  
};
ResourceHolder<Texture::ID, sf::Texture> a;
ResourceHolder<Fonts::ID, sf::Font> fontc;

void all_load()
{
    meep_merp.loadFromFile("immune.wav");
    bpress.loadFromFile("Button_Push.wav");
    right.setBuffer(bpress);
    wrong.setBuffer(meep_merp);
    wrong.setVolume(50);
    ////
    ////initializare directii
    ////
    ////----
    bridge[0].dir[0].first = 0;
    bridge[0].dir[0].second =-1;
    bridge[0].dir[1].first =1;
    bridge[0].dir[1].second = 0;
    bridge[0].dir[2].first = bridge[0].dir[3].first = 322;
    bridge[0].dir[2].second = bridge[0].dir[3].second = 322;
    ////--
    bridge[1].dir[0].first = 0;
    bridge[1].dir[0].second = -1;
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
    bridge[3].dir[0].first = 0;
    bridge[3].dir[0].second = -1;
    bridge[3].dir[1].first =-1 ;
    bridge[3].dir[1].second = 0;
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
    bridge[6].dir[0].first = -1;
    bridge[6].dir[0].second = 0;
    bridge[6].dir[1].first = 0;
    bridge[6].dir[1].second = -1;
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

    for (int i = 0; i < 5; i++) //Initialize tiles
        for (int j = 0; j < 5; j++)
        {
            tiles[i][j].setTexture(a.get(Texture::tile));
            tiles[i][j].setOrigin(tileW / 2, tileH / 2);
            tiles[i][j].setPosition(sf::Vector2f(100 + j * tileH, 130 + i * tileW));
            tiles[i][j].setScale(sf::Vector2f(1 / div1, 1 / div2));
        }
    Deck.setOrigin(0, 0);
    Deck.setPosition(740, 108);
    Deck.setSize(sf::Vector2f(760, 645));
    Deck.setFillColor(sf::Color(0, 119, 255));
    Deck_Text.setPosition(757,129);
    Deck_Text.setFont(fontc.get(Fonts::bookantq));
    Deck_Text.setString("DECK AREA");
    Deck_Text.setFillColor(sf::Color::White);
    
    bridge[0].pod.setPosition(935, 216);
    bridge[0].pod.setOrigin(190, 60);
    bridge[0].pod.setTexture(a.get(Texture::road1));
    bridge[0].tex = Texture::road1;
    bridge[1].pod.setPosition(1171, 216);
    bridge[1].pod.setOrigin(190, 60);
    bridge[1].pod.setTexture(a.get(Texture::road2));
    bridge[1].tex = Texture::road2;

    bridge[2].pod.setPosition(1278, 253);
    bridge[2].pod.setOrigin(45, 65);
    bridge[2].pod.setTexture(a.get(Texture::road3));
    bridge[2].tex = Texture::road3;

    bridge[3].pod.setPosition(934, 553);
    bridge[3].pod.setOrigin(201, 215);
    // bridge[3].pod.setScale(roadScale);
    bridge[3].pod.setTexture(a.get(Texture::bridge1));
    bridge[3].tex = Texture::bridge1;

    bridge[4].pod.setPosition(1043, 520);
    bridge[4].pod.setOrigin(45, 215);
    // bridge[4].pod.setScale(roadScale);
    bridge[4].pod.setTexture(a.get(Texture::bridge2));
    bridge[4].tex = Texture::bridge2;

    bridge[5].pod.setPosition(1321, 553);
    bridge[5].pod.setOrigin(45, 145);
    // bridge[5].pod.setScale(roadScale);
    bridge[5].pod.setTexture(a.get(Texture::bridge3));
    bridge[5].tex = Texture::bridge3;

    bridge[6].pod.setPosition(1147, 722);
    bridge[6].pod.setOrigin(127, 170);
    // bridge[6].pod.setScale(roadScale);
    bridge[6].pod.setTexture(a.get(Texture::bridge4));
    bridge[6].tex = Texture::bridge4;
}
void drawThings(sf::RenderWindow& window)
{
    window.draw(Deck);
    window.draw(Deck_Text);
    for(int i=0;i<5;i++)
        for (int j = 0; j < 5; j++)
        {
           window.draw(tiles[i][j]);
        }
    for (int i = 0; i < 7; i++)
        if (i == pozitie)
        {
            if (isDragging == 1)
                bridge[i].pod.setPosition(mousePos);
        }
        else window.draw(bridge[i].pod);
    window.draw(bridge[pozitie].pod);
}
bool checkAvailability(int i, int j)
{
    for (int k = 0; k < 15; k++)
    {
        for (int q = 0; q < 15; q++)
            std::cout << BoardState[k][q] << " ";
        std::cout << std::endl;
    }
    std::cout << "-----------" << std::endl;
    if (BoardState[1 + i * 3][3 * j + 1])
    {
        wrong.play();
        return 0;
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
            if (BoardState[X][Y] == 1)
            {
                wrong.play();
                return 0;
            }
        }
    right.play();
    return 1;
}
bool punePiesa(sf::Sprite& tile,int i, int j)
{
    if (checkAvailability(i,j))
    {
        sf::FloatRect tile_bounds = tile.getGlobalBounds();
        bridge[pozitie].pod.setPosition(tile_bounds.left + tile_bounds.width / 2, tile_bounds.top + tile_bounds.height / 2);
        BoardState[1 + i * 3][3 * j+1] = 1;
        for (int c = 0; c < 4; c++)
            if (bridge[pozitie].dir[c].first != 322 && bridge[pozitie].dir[c].second != 322)
            {
                int X = 3 * i + 1 + 3 * bridge[pozitie].dir[c].first;
                int Y = 3 * j + 1 + 3 * bridge[pozitie].dir[c].second;
                BoardState[X][Y] = 1;
            }
        bridge[pozitie].i = i;
        bridge[pozitie].j = j;
        for (int k = 0; k < 15; k++)
        {
            for (int q = 0; q < 15; q++)
                std::cout << BoardState[k][q] << " ";
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
    
    for (int c = 0; c < 4; c++)
        if (bridge[pos].dir[c].first != 322 && bridge[pos].dir[c].second != 322)
        {
            BoardState[i_board + 3*bridge[pos].dir[c].first][j_board + 3 * bridge[pos].dir[c].second] = 0;
        }
}
int handleMouseClick(sf::Event e)
{
    int pos = -1;
    if (e.mouseButton.button == sf::Mouse::Left)
        if (isDragging == 0)
        {
            for (int i = 0; i < 7; i++)
                if (bridge[i].pod.getGlobalBounds().contains(e.mouseButton.x, e.mouseButton.y))
                {
                    if (Deck.getGlobalBounds().contains(e.mouseButton.x, e.mouseButton.y) == 0)
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
void DoRotation(int poz,bool dreapta)
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
                std::cout << "POZ:" << poz;
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
            if (ok != -1) pozitie = ok;
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
                    DoRotation(pozitie,0);
                }
                if (numTicks < 0)
                {
                    bridge[pozitie].pod.setRotation(bridge[pozitie].pod.getRotation() + 90);
                    DoRotation(pozitie,1);
                }
            }
            break;
        }
    }
}

int main()
{
    all_load();
    sf::RenderWindow window(sf::VideoMode(1600, 900), "text", sf::Style::Close);
    window.setFramerateLimit(240);
    while (window.isOpen())
    {
        handle_Events(window);
        window.clear(sf::Color(0,127,196));
        drawThings(window);
        window.display();
    }
}
