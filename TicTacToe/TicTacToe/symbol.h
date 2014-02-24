class symbol : public sf::Text
{
public:
	symbol(sf::Font &font);
	int symbolState;
};

symbol::symbol(sf::Font &font)
{
	symbolState = 0;
	setColor(sf::Color::White);
	setFont(font);
	setCharacterSize(60);
	setString("-");
	setOrigin(getGlobalBounds().width / 2, getGlobalBounds().height /2);
}



class logicalGridRect : public sf::RectangleShape //THIS CLASS IS LITERALLY JUST SF::RECTANGLESHAPE WITH THE ABILITY TO CHECK IF THE MOUSE INTERSECTS IT
{
public:
	bool checkMouse(sf::RenderWindow & window);
};

bool logicalGridRect::checkMouse(sf::RenderWindow & window)
{
	if (sf::Mouse::getPosition(window).x > getPosition().x &&
		sf::Mouse::getPosition(window).x < getPosition().x + getGlobalBounds().width &&
		sf::Mouse::getPosition(window).y > getPosition().y &&
		sf::Mouse::getPosition(window).y < getPosition().y + getGlobalBounds().height)
	{
		return true;
	}
	else return false;
}
