
#include <iostream>
#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include <SFML/Window.hpp>
#include "MyVector.hpp"
#include "STRUCTURA_DATE_HARTA.hpp"
#include <fstream>
std::ofstream g;
sf::RenderWindow window(sf::VideoMode(1600, 900), "text", sf::Style::Close);
char filename[30];
sf::Sprite tiles[5][5];
int pozitie=-1;
int ok = -1;
sf::Vector2f mousePos;
sf::RectangleShape done,reset;
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
struct _temples
{
    int i = -1, j = -1;
    sf::Sprite temple;
    sf::Vector2i dir[2];

}temples[3];
float BoardState[16][16];
const float div1 = 3.f;
const float div2 = 3.f;
const float tileW = floor(385 / div1); //385 div 3 ~128px tile w and h
const float tileH = floor(385 / div2);
bool isDragging = 0;
bool contains(int i, float x, float y)
{
    sf::FloatRect bounds = temples[i].temple.getGlobalBounds();
    if (bounds.contains(x, y))
        return 1;

    return 0;
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
ResourceHolder<Texture::ID, sf::Texture> a;
ResourceHolder<Fonts::ID, sf::Font> fontc;
void DoRotation(int poz, bool dreapta)
{


    for (int i = 0; i < 2; i++)
        if (temples[poz].dir[i].x != 322 && temples[poz].dir[i].y != 322)
            if (!dreapta)
            {
                if (temples[poz].dir[i].x == 0 && temples[poz].dir[i].y == -1)
                {
                    temples[poz].dir[i].x = 1; temples[poz].dir[i].y = 0;
                }
                else
                    if (temples[poz].dir[i].x == 1 && temples[poz].dir[i].y == 0)
                    {
                        temples[poz].dir[i].x = 0; temples[poz].dir[i].y = 1;
                    }
                    else
                        if (temples[poz].dir[i].x == 0 && temples[poz].dir[i].y == 1)
                        {
                            temples[poz].dir[i].x = -1; temples[poz].dir[i].y = 0;
                        }
                        else
                            if (temples[poz].dir[i].x == -1 && temples[poz].dir[i].y == 0)
                            {
                                temples[poz].dir[i].x = 0; temples[poz].dir[i].y = -1;
                            }
            }
            else
            {
                if (temples[poz].dir[i].x == 0 && temples[poz].dir[i].y == -1)
                {
                    temples[poz].dir[i].x = -1; temples[poz].dir[i].y = 0;
                }
                else
                    if (temples[poz].dir[i].x == -1 && temples[poz].dir[i].y == 0)
                    {
                        temples[poz].dir[i].x = 0; temples[poz].dir[i].y = 1;
                    }
                    else
                        if (temples[poz].dir[i].x == 0 && temples[poz].dir[i].y == 1)
                        {
                            temples[poz].dir[i].x = 1; temples[poz].dir[i].y = 0;
                        }
                        else
                            if (temples[poz].dir[i].x == 1 && temples[poz].dir[i].y == 0)
                            {
                                temples[poz].dir[i].x = 0; temples[poz].dir[i].y = -1;
                            }
            }

}
void writetofile()
{
    g.open("level.txt");
    std::cout << "Se scrie in fisier level.txt"<<std::endl;
    for (int i = 0; i < 15; i++)
    {
        for (int j = 0; j < 15; j++)
            g << BoardState[i][j] << " ";
        g << std::endl;
    }
    g.close();

}
bool punePiesa(sf::Sprite &tile,int i, int j)
{
    int X = 3 * i + 1;
    int Y = 3 * j + 1;
    sf::FloatRect tile_bounds = tile.getGlobalBounds();
    temples[pozitie].temple.setPosition(tile_bounds.left + tile_bounds.width / 2, tile_bounds.top + tile_bounds.height / 2);
    switch (pozitie)
    {
    case 0:
        BoardState[X][Y] = 10;
        for (int d = 0; d < 2; d++)
            BoardState[X + temples[0].dir[d].x][Y + temples[0].dir[d].y] = 10;
        break;
    case 1:
        BoardState[X][Y] = 11;
        BoardState[X + temples[1].dir[0].x][Y + temples[1].dir[0].y] = 11;
        BoardState[X + 2*temples[1].dir[0].x][Y + 2*temples[1].dir[0].y] = 11;
        BoardState[X + temples[1].dir[1].x][Y + temples[1].dir[1].y] = 11;
        break;
    case 2:
        BoardState[X][Y] = 12;
        BoardState[X + temples[2].dir[0].x][Y + temples[2].dir[0].y] = 12;
        BoardState[X + 2 * temples[2].dir[0].x][Y + 2 * temples[2].dir[0].y] = 12;
        BoardState[X + temples[2].dir[1].x][Y + temples[2].dir[1].y] = 12;
        break;
    }
    temples[pozitie].i = i;
    temples[pozitie].j = j;
    return 1;
}
void destroy(int pos)
{
    int X = 3 * temples[pos].i + 1;
    int Y = 3 * temples[pos].j + 1;
    switch (pos)
    {
    case 0:
        BoardState[X][Y] = 0;
        for (int d = 0; d < 2; d++)
            BoardState[X + temples[0].dir[d].x][Y + temples[0].dir[d].y] = 0;
        break;
    case 1:
        BoardState[X][Y] = 0;
        BoardState[X + temples[1].dir[0].x][Y + temples[1].dir[0].y] = 0;
        BoardState[X + 2 * temples[1].dir[0].x][Y + 2 * temples[1].dir[0].y] = 0;
        BoardState[X + temples[1].dir[1].x][Y + temples[1].dir[1].y] = 0;
        break;
    case 2:
        BoardState[X][Y] = 0;
        BoardState[X + temples[2].dir[0].x][Y + temples[2].dir[0].y] = 0;
        BoardState[X + 2 * temples[2].dir[0].x][Y + 2 * temples[2].dir[0].y] = 0;
        BoardState[X + temples[2].dir[1].x][Y + temples[2].dir[1].y] = 0;
        break;
    }

}
int handleMouseClick(sf::Event e)
{
    int pos = -1;
    if (e.mouseButton.button == sf::Mouse::Left)
    {
        if (isDragging == 0)
        {
                    for (int i = 2; i >= 0; i--)
                        if (contains(i, e.mouseButton.x, e.mouseButton.y))
                        {

                            destroy(i);
                            isDragging = 1;
                            return i;
                        }
                        
        }

        
        else
        {

            {
                for (int i = 0; i < 5; i++)
                    for (int j = 0; j < 5; j++)
                        if (tiles[i][j].getGlobalBounds().contains(e.mouseButton.x, e.mouseButton.y))
                        {
                            bool success = 0;
                            
                            success = punePiesa(tiles[i][j], i, j);
                            if (success)
                                isDragging = 0;
                            return -1;
                        }
            }

        }
        if (done.getGlobalBounds().contains(e.mouseButton.x, e.mouseButton.y))
        writetofile();
        if (reset.getGlobalBounds().contains(e.mouseButton.x, e.mouseButton.y))
        {
            for (int i = 0; i < 15; i++)
                for (int j = 0; j < 15; j++)
                    BoardState[i][j] = 0;
            isDragging = 0;
            temples[0].temple.setPosition(1000, 300);
            temples[1].temple.setPosition(1200, 400);
            temples[2].temple.setPosition(1500, 600);
            std::cout << "MATRICE RESETATA"<<std::endl;

        }


    }
    return pos;
}
void all_load()
{
  

    a.load(Texture::tile, "tile2.png");
    a.load(Texture::temple1, "temple1.png");
    a.load(Texture::temple2, "temple2.png");
    a.load(Texture::temple3, "temple3.png");
    done.setOutlineThickness(6);
    done.setOutlineColor(sf::Color::Red);
    done.setPosition(1000, 500);
    done.setSize(sf::Vector2f(200, 200));

    reset.setOutlineThickness(6);
    reset.setOutlineColor(sf::Color::Blue);
    reset.setPosition(1200, 500);
    reset.setSize(sf::Vector2f(200, 200));

   
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
    temples[0].temple.setOrigin(65, 65);
    temples[1].temple.setOrigin(65, 65);
    temples[2].temple.setOrigin(65, 125);

    temples[0].dir[0] = sf::Vector2i(-1, 0);
    temples[0].dir[1] = sf::Vector2i(0, 1);

    temples[1].dir[0] = sf::Vector2i(0, 1);
    temples[1].dir[1] = sf::Vector2i(+1, 0);

    temples[2].dir[0] = sf::Vector2i(-1, 0);
    temples[2].dir[1] = sf::Vector2i(0, -1);
  temples[0].temple.setPosition(1000, 300);
            temples[1].temple.setPosition(1200, 400);
            temples[2].temple.setPosition(1500, 600);
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
                    
                    DoRotation(pozitie, 0);
                    temples[pozitie].temple.rotate(-90);
                }
                if (numTicks < 0)
                {
                    
                    DoRotation(pozitie, 1);
                    temples[pozitie].temple.rotate(+90);
                }
            }
            break;
        case sf::Event::Closed: window.close(); break;
        case sf::Event::MouseButtonPressed:
            ok = handleMouseClick(event);
            if (ok >=0&&ok<=2)
            {
                pozitie = ok;
            }
         
            break;
        case sf::Event::MouseMoved:
            mousePos.x = event.mouseMove.x;
            mousePos.y = event.mouseMove.y;
            break;
        }
    }
}
void drawThings(sf::RenderWindow& window)
{
    for (int i = 0; i < 5; i++)
        for (int j = 0; j < 5; j++)
            window.draw(tiles[i][j]);
    if (isDragging) temples[pozitie].temple.setPosition(mousePos);
    for (int i = 0; i < 3; i++)
        window.draw(temples[i].temple);
    window.draw(done);
    window.draw(reset);

}
int main()
{
    all_load();
    window.setFramerateLimit(120);
    while (window.isOpen())
    {
        handle_Events(window);
        window.clear(sf::Color(0, 127, 196));
        drawThings(window);
        window.display();
    }
}


