#include <iostream>
#include <SFML/Graphics.hpp>

struct meniu
{
	int selectedOptionIndex=0;
	sf::Font font;
	sf::Text option[3];
}m;

void menuConst(meniu& m)
{
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

	m.selectedOptionIndex = 0;
}

void menuDraw(sf::RenderWindow& window,meniu& m,bool& x)
{
	if(x==true)
		for (int i = 0; i < 3; i++)
		{
			window.draw(m.option[i]);
		}

}

void MoveUp(meniu& m)
{
	if (m.selectedOptionIndex - 1 >= 0)
	{
		m.option[m.selectedOptionIndex].setFillColor(sf::Color::White);
		m.selectedOptionIndex--;
		m.option[m.selectedOptionIndex].setFillColor(sf::Color::Black);
	}
}

void MoveDown(meniu& m)
{
	if (m.selectedOptionIndex + 1 < 3)
	{
		m.option[m.selectedOptionIndex].setFillColor(sf::Color::White);
		m.selectedOptionIndex++;
		m.option[m.selectedOptionIndex].setFillColor(sf::Color::Black);
	}
}

int GetPressedOption(meniu& m, bool x) 
{ 
	if (x == true)
	{
		return m.selectedOptionIndex;
	}
}

struct levelpanel {
	sf::RectangleShape rectangle;
	sf::Texture texture;
}lvl[20];


void playLoad(levelpanel lvl[20])
{
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


}



void moveLeft(levelpanel lvl[20], int& i, sf::CircleShape& triangle)
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

void moveRight(levelpanel lvl[20], int& i,sf::CircleShape& triangle)
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

void lvlDraw(sf::RenderWindow& window, levelpanel lvl[20])
{
	for (int i = 0; i < 20; i++)
	{
		window.draw(lvl[i].rectangle);
	}
}
void play(sf::RenderWindow& window, bool& playx, bool& x, int& i, sf::CircleShape& triangle)
{
	if (playx == true)
	{
		playLoad(lvl);
		lvlDraw(window, lvl);
		sf::Event event;
		while (window.pollEvent(event))
		{
			switch (event.type)
			{
			case sf::Event::KeyReleased:
				switch (event.key.code)
				{
				case sf::Keyboard::Escape:
					playx = false;
					x = true;
				case sf::Keyboard::Left:
					moveLeft(lvl, i,triangle);
					break;
				case sf::Keyboard::Right:
					moveRight(lvl, i,triangle);
					break;
				}

			}
		}
		
	}
}


void instructions(sf::RenderWindow& window, bool& instructionsx, bool& x)
{
	if (instructionsx == true)
	{
		sf::Event event;
		sf::Text text;
		sf::Font font;
		if (!font.loadFromFile("arial.ttf"))
		{

		}
		text.setCharacterSize(35);
		text.setFillColor(sf::Color::White);
		text.setPosition(sf::Vector2f(1600 / 5.4, 900 / 3 * 1.2));
		text.setFont(font);
		text.setString("  Temple Connection is a game in which you have to connect the  \n temples between them so you can walk from each temple to every \nother one. You have 3 pieces with roads and 4 with bridges which\n you can use. The first 5 challenges are at a Starter Difficulty\n    Level, the next 5 are at the Junior Difficulty Level, the   \n challenges from 11 to 15 are at an Expert Difficulty Level and \nthe last 5 are at the Master Difficulty Level. Be careful! There\n	    is only one way you can connect those temples.");

		window.draw(text);
		while (window.pollEvent(event))
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
	}
}

void events(sf::RenderWindow& window, bool& x, bool& playx, bool& instructionsx)
{
	sf::Event event;
	while (window.pollEvent(event))
	{
		switch (event.type)
		{
		case sf::Event::KeyReleased:
			switch (event.key.code)
			{
			case sf::Keyboard::Up:
				MoveUp(m);
				break;

			case sf::Keyboard::Down:
				MoveDown(m);
				break;

			case sf::Keyboard::Return:
				switch (GetPressedOption(m, x))
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
				}

			}



		}
	}
}

void designLoad(sf::RenderWindow& window, sf::Sprite& sprite, sf::Texture& texture, sf::CircleShape& triangle)
{
	if (!texture.loadFromFile("homescreen.jpg"))
	{

	}

	sprite.setTexture(texture);
	triangle.setFillColor(sf::Color::Red);
	triangle.setOutlineColor(sf::Color::Black);
	triangle.setOutlineThickness(2);
	triangle.setPosition(sf::Vector2f(288, 225));

	menuConst(m);
}
int main()
{
	bool x = true;
	bool playx = false;
	bool instructionsx = false;
	int i = 0;

	sf::RenderWindow window(sf::VideoMode(1600, 900), "Temple Connection");

	sf::Texture texture;
	sf::Sprite sprite;

	sf::CircleShape triangle(10.f, 3);
	designLoad(window, sprite, texture, triangle);

	while (window.isOpen())
	{
		events(window, x, playx, instructionsx);
		window.clear();

		window.draw(sprite);
		menuDraw(window, m, x);
		play(window, playx , x, i, triangle);
		instructions(window, instructionsx, x);
		if (playx == true)
		{
			window.draw(triangle);
		}
		

		window.display();

	}
	
}