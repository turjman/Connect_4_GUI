#include <SFML/Graphics.hpp>
#include <iostream>
#include <string>
#include <map>
#include <filesystem>
#include <fstream>
#include <windows.h>
#include <chrono>
#include <ctime> 
#include <vector>
#include <algorithm>
#include <iterator>


#define SPEED	5

#define MAX +10000
#define MIN -10000
#define INF +1000000000000000000


#define WINDOW_LENGTH  4
#define EMPTY_PIECE  0

#pragma warning(disable : 4996) //_CRT_SECURE_NO_WARNINGS

typedef std::vector<std::string> stringvec;

using std::cout;
using std::cin;


struct BestMove {
	int col = -1;
	long long value = 0;
};

struct Piece {
	int row = -1;
	int col = -1;
};

struct Window {
	const int windowLENGTH = WINDOW_LENGTH;
	int emptyPieceCount = 0;
	int pieceCount = 0;
	Piece piece;
	// Mode 1 -> Horizontal
	// Mode 2 -> Vertical
	// Mode 3 -> Diagonal (\)
	// Mode 4 -> Diagonal (/)
	void count(int** gameBoard, int piece, short mode) {
		Window::emptyPieceCount = 0;
		Window::pieceCount = 0;
		if (mode == 1) {
			for (int i = 0; i < windowLENGTH; i++) {
				if (gameBoard[Window::piece.row][Window::piece.col + i] == EMPTY_PIECE)
					Window::emptyPieceCount++;
				else if (gameBoard[Window::piece.row][Window::piece.col + i] == piece)
					Window::pieceCount++;
			}
		}
		else if (mode == 2) {
			for (int i = 0; i < windowLENGTH; i++) {
				if (gameBoard[Window::piece.row + i][Window::piece.col] == EMPTY_PIECE)
					Window::emptyPieceCount++;
				else if (gameBoard[Window::piece.row + i][Window::piece.col] == piece)
					Window::pieceCount++;
			}
		}
		else if (mode == 3) {
			for (int i = 0; i < windowLENGTH; i++) {
				if (gameBoard[Window::piece.row - i][Window::piece.col - i] == EMPTY_PIECE)
					Window::emptyPieceCount++;
				else if (gameBoard[Window::piece.row - i][Window::piece.col - i] == piece)
					Window::pieceCount++;
			}
		}
		else if (mode == 4) {
			for (int i = 0; i < windowLENGTH; i++) {
				if (gameBoard[Window::piece.row + i][Window::piece.col - i] == EMPTY_PIECE)
					Window::emptyPieceCount++;
				else if (gameBoard[Window::piece.row + i][Window::piece.col - i] == piece)
					Window::pieceCount++;
			}
		}
	}
};

struct Player {
	int gameBoardID = -1;
	std::string Name[11];
	long long Score = 0;
	sf::Sprite* BallSprite = NULL;
	std::string Ball[11];
	bool isWinner = 0;
	bool Active = 0;
	std::map<std::pair<int, int>, int> winPos;
};

struct Move {
	int row = -1;
	int col = -1;
	bool gameEnd = 0;
	bool played = 0;
	int playedBy = -1; // index of the player 
	float posX = -1;
	float posY = -1;
};

struct List {
	int firstInd = -1;
	int selIndex = -1;
	int index = 0;
	int size = 0;
};

std::string subSTR(std::string str, int start, int end);
void read_directory(const std::string& path, stringvec& v);
bool drawAlert(std::string s, sf::Vector2i alertWindowPos);
bool playAI(sf::RenderWindow& welcomeWindow, int** gameBoard, int ROWS, int COLS, Player player[], int* posX, std::string& posY);
bool playOffline(sf::RenderWindow& welcomeWindow, int** gameBoard, int ROWS, int COLS, Player player[], int* posX, std::string& posY, int mode, int lvl);
bool playWindow(int** gameBoard, int ROWS, int COLS, Player player[], int* posX, std::string& posY, int mode, int lvl);
Move drawGameBoard(sf::RenderWindow& window, sf::Sprite& boardSprite, const float Xstart, const float Ystart, int ROWS, int COLS, int** gameBoard, Player player[], bool isWin, int* posX, std::string& posY, int mode, int lvl);
Move animateMove(sf::RenderWindow& window, sf::Sprite& boardSprite, const float Xstart, const float Ystart, int ROWS, int COLS, int** gameBoard, Player player[], Move& move);
int evaluateWinner(int** gameBoard, int ROWS, int COLS, Player player[], Move move, bool AI);
int scoreBoard(int** gameBoard, int ROWS, int COLS, Player player[], int* posX, std::string& posY, Move move);
BestMove alphaBeta(int** gameBoard, int ROWS, int COLS, Player player[], int* posX, Move move, int depth, long long alpha, long long  beta, bool isMximizingPlayer);
int evaluateWindow(Window window, bool AI);
bool gameRep(sf::RenderWindow& welcomeWindow, int** gameBoard, int ROWS, int COLS, Player player[], int* posX, std::string& posY);

int main()
{
	int ROWS = 0, COLS = 0;

	std::string posY;

	// Making Dynamic 1D Array
	int* posX = NULL;
	Player* player = NULL;

	// Making Dynamic 2D Array
	int** gameBoard = NULL;

	// redBall Configuration
	sf::Texture redBallTexture;
	if (!redBallTexture.loadFromFile("images//redBallSmall.png"))
	{
		// error...
	}
	redBallTexture.setSmooth(true);
	sf::Sprite redBallSprite;
	redBallSprite.setTexture(redBallTexture);
	// yellowBall Configuration
	sf::Texture yellowBallTexture;
	if (!yellowBallTexture.loadFromFile("images//yellowBallSmall.png"))
	{
		// error...
	}
	yellowBallTexture.setSmooth(true);
	sf::Sprite yellowBallSprite;
	yellowBallSprite.setTexture(yellowBallTexture);

	bool isClosed = 0;
	bool isClicked = 0;

	sf::Vector2i welcomeWindowDim;
	welcomeWindowDim.x = 900;
	welcomeWindowDim.y = 600;

	sf::VideoMode desktopMode = sf::VideoMode::getDesktopMode();

	// Welcome Window
	sf::RenderWindow welcomeWindow(sf::VideoMode(welcomeWindowDim.x, welcomeWindowDim.y, 32), "CONNECT FOUR GAME !", sf::Style::None);
	welcomeWindow.setMouseCursorVisible(false);

	sf::Vector2i welcomeWindowPos((desktopMode.width - welcomeWindowDim.x) / 2, (desktopMode.height - welcomeWindowDim.y) / 2);
	welcomeWindow.setPosition(welcomeWindowPos);


	// background Configuration
	sf::Texture backgroundWelcomeTexture;
	if (!backgroundWelcomeTexture.loadFromFile("images//back10.jpg"))
	{
		// error...
	}
	backgroundWelcomeTexture.setSmooth(true);
	sf::Sprite backgroundWelcomeSprite;
	backgroundWelcomeSprite.setTexture(backgroundWelcomeTexture);
	backgroundWelcomeSprite.setOrigin(sf::Vector2f(0.f, 0.f));

	backgroundWelcomeSprite.setScale(
		welcomeWindowDim.x / backgroundWelcomeSprite.getLocalBounds().width,
		welcomeWindowDim.y / backgroundWelcomeSprite.getLocalBounds().height);


	sf::Clock clock;
	sf::Time elapsedTime = clock.getElapsedTime();

	while (elapsedTime.asSeconds() < 3) {
		elapsedTime = clock.getElapsedTime();
		welcomeWindow.clear();
		welcomeWindow.draw(backgroundWelcomeSprite);
		welcomeWindow.display();
	}

	welcomeWindow.setMouseCursorVisible(true);

	// background Configuration
	sf::Texture backgroundTexture;
	if (!backgroundTexture.loadFromFile("images//back9.jpg"))
	{
		// error...
	}
	backgroundTexture.setSmooth(true);
	sf::Sprite backgroundSprite;
	backgroundSprite.setTexture(backgroundTexture);
	backgroundSprite.setOrigin(sf::Vector2f(0.f, 0.f));

	backgroundSprite.setScale(
		welcomeWindowDim.x / backgroundSprite.getLocalBounds().width,
		welcomeWindowDim.y / backgroundSprite.getLocalBounds().height);



	float Xstart = 600, Ystart = 150;
	sf::Vector2i mousePos;
	// Button Configuration
	sf::Texture buttonTexture;
	if (!buttonTexture.loadFromFile("images//buttonReleasedSmall.png"))
	{
		// error...
	}
	buttonTexture.setSmooth(true);
	sf::Sprite buttonSprite;
	buttonSprite.setTexture(buttonTexture);
	buttonSprite.setOrigin(sf::Vector2f(0.f, 0.f));
	buttonSprite.setPosition(Xstart, Ystart);

	// Button Hover Configuration
	sf::Texture buttonHoverTexture;
	if (!buttonHoverTexture.loadFromFile("images//buttonHoverSmall.png"))
	{
		// error...
	}
	buttonHoverTexture.setSmooth(true);
	sf::Sprite buttonHoverSprite;
	buttonHoverSprite.setTexture(buttonHoverTexture);
	buttonHoverSprite.setOrigin(sf::Vector2f(0.f, 0.f));
	buttonHoverSprite.setPosition(Xstart, Ystart);


	sf::Font textFont;
	if (!textFont.loadFromFile("fonts/mvboli.ttf")) {
		//error
	}

	sf::Text buttonTitle;
	sf::String titleText;

	buttonTitle.setFont(textFont);
	buttonTitle.setCharacterSize(25);
	buttonTitle.setStyle(sf::Text::Bold);
	buttonTitle.setFillColor(sf::Color::Black);

	sf::Text windowTitle;

	windowTitle.setFont(textFont);
	windowTitle.setCharacterSize(35);
	windowTitle.setStyle(sf::Text::Bold);
	windowTitle.setFillColor(sf::Color::Black);
	windowTitle.setString("Welcome :)");
	windowTitle.setPosition(620, 25);


	while (welcomeWindow.isOpen() && !isClosed)
	{
		sf::Event event;
		buttonHoverSprite.setColor(sf::Color(255, 255, 255));

		welcomeWindow.clear();
		welcomeWindow.draw(backgroundSprite);
		welcomeWindow.draw(windowTitle);
		buttonSprite.setPosition(Xstart, Ystart);
		welcomeWindow.draw(buttonSprite);

		buttonSprite.setPosition(Xstart, (Ystart + 100));
		welcomeWindow.draw(buttonSprite);

		buttonSprite.setPosition(Xstart, (Ystart + 200));
		welcomeWindow.draw(buttonSprite);

		buttonSprite.setPosition(Xstart, (Ystart + 300));
		welcomeWindow.draw(buttonSprite);


		isClicked = 0;
		while (welcomeWindow.pollEvent(event)) {
			if (event.type == sf::Event::Closed) {
				welcomeWindow.close();
				isClosed = 1;
			}
			if (event.type == sf::Event::MouseButtonPressed) {
				if (event.mouseButton.button == sf::Mouse::Left) {
					isClicked = 1;
				}
			}
			if (event.type == sf::Event::TextEntered)
			{
				if (event.text.unicode == 27)
					welcomeWindow.close();
			}
		}
		mousePos = sf::Mouse::getPosition(welcomeWindow);
		if (mousePos.y >= Ystart &&
			mousePos.y < (buttonSprite.getLocalBounds().height + Ystart) &&
			mousePos.x >= Xstart &&
			mousePos.x < (buttonSprite.getLocalBounds().width + Xstart)) {
			if (isClicked) {
				buttonHoverSprite.setColor(sf::Color(255, 255, 255, 128));
				isClicked = 0;
				isClosed = playOffline(welcomeWindow, gameBoard, ROWS, COLS, player, posX, posY, 0, 0);
			}
			else {
				buttonHoverSprite.setPosition(Xstart, Ystart);
				welcomeWindow.draw(buttonHoverSprite);
			}
		}
		else if (mousePos.y >= Ystart + 100 &&
			mousePos.y < (buttonSprite.getLocalBounds().height + Ystart + 100) &&
			mousePos.x >= Xstart &&
			mousePos.x < (buttonSprite.getLocalBounds().width + Xstart)) {
			if (isClicked) {
				buttonHoverSprite.setColor(sf::Color(255, 255, 255, 128));
				isClicked = 0;
				isClosed = playAI(welcomeWindow, gameBoard, ROWS, COLS, player, posX, posY);
			}
			else {
				buttonHoverSprite.setPosition(Xstart, Ystart + 100);
				welcomeWindow.draw(buttonHoverSprite);
			}
		}
		else if (mousePos.y >= Ystart + 200 &&
			mousePos.y < (buttonSprite.getLocalBounds().height + Ystart + 200) &&
			mousePos.x >= Xstart &&
			mousePos.x < (buttonSprite.getLocalBounds().width + Xstart)) {
			if (isClicked) {
				buttonHoverSprite.setColor(sf::Color(255, 255, 255, 128));
				isClicked = 0;
			}
			else {
				buttonHoverSprite.setPosition(Xstart, Ystart + 200);
				welcomeWindow.draw(buttonHoverSprite);
			}
		}
		else if (mousePos.y >= Ystart + 300 &&
			mousePos.y < (buttonSprite.getLocalBounds().height + Ystart + 300) &&
			mousePos.x >= Xstart &&
			mousePos.x < (buttonSprite.getLocalBounds().width + Xstart)) {
			if (isClicked) {
				buttonHoverSprite.setColor(sf::Color(255, 255, 255, 128));
				isClicked = 0;
				isClosed = gameRep(welcomeWindow, gameBoard, ROWS, COLS, player, posX, posY);
			}
			else {
				buttonHoverSprite.setPosition(Xstart, Ystart + 300);
				welcomeWindow.draw(buttonHoverSprite);
			}
		}

		titleText = "Player vs Player";
		buttonTitle.setString(titleText);
		buttonTitle.setPosition(Xstart + 20, Ystart + 10);
		welcomeWindow.draw(buttonTitle);

		titleText = "Player vs AI";
		buttonTitle.setString(titleText);
		buttonTitle.setPosition(Xstart + 45, (Ystart + 100 + 10));
		welcomeWindow.draw(buttonTitle);

		titleText = "Multi-Player";
		buttonTitle.setString(titleText);
		buttonTitle.setPosition(Xstart + 50, (Ystart + 200 + 10));
		welcomeWindow.draw(buttonTitle);

		titleText = "Game Replay";
		buttonTitle.setString(titleText);
		buttonTitle.setPosition(Xstart + 50, (Ystart + 300 + 10));
		welcomeWindow.draw(buttonTitle);

		welcomeWindow.display();
	}




	return 0;
}

void read_directory(const std::string& path, stringvec& fileName){
	std::string pattern(path);
	pattern.append("\\*");
	WIN32_FIND_DATA data;
	HANDLE hFind;
	if ((hFind = FindFirstFile(pattern.c_str(), &data)) != INVALID_HANDLE_VALUE) {
		do {
			fileName.push_back(data.cFileName);
		} while (FindNextFile(hFind, &data) != 0);
		FindClose(hFind);
	}
}

bool playOffline(sf::RenderWindow & welcomeWindow, int** gameBoard, int ROWS, int COLS, Player player[], int* posX, std::string & posY, int mode, int lvl) {
	std::string* str;
	int playersCount = 2;
	bool isNext = 0;
	bool isClicked = 0;
	bool isPlayFirstChecked = 0;
	bool isRedChecked = 1;
	bool inputIsOk = 1;
	int inputActive = 1;
	bool isBack = false;


	// redBall Configuration
	sf::Texture redBallTexture;
	if (!redBallTexture.loadFromFile("images//redBallSmall.png"))
	{
		// error...
	}
	redBallTexture.setSmooth(true);
	sf::Sprite redBallSprite;
	redBallSprite.setTexture(redBallTexture);
	// yellowBall Configuration
	sf::Texture yellowBallTexture;
	if (!yellowBallTexture.loadFromFile("images//yellowBallSmall.png"))
	{
		// error...
	}
	yellowBallTexture.setSmooth(true);
	sf::Sprite yellowBallSprite;
	yellowBallSprite.setTexture(yellowBallTexture);

	// redBallSmall Configuration
	sf::Texture redBallSmallTexture;
	if (!redBallSmallTexture.loadFromFile("images//redBallVerySmall.png"))
	{
		// error...
	}
	redBallSmallTexture.setSmooth(true);
	sf::Sprite redBallSmallSprite;
	redBallSmallSprite.setTexture(redBallSmallTexture);
	redBallSmallSprite.setPosition(760, 200);

	// yellowBallSmall Configuration
	sf::Texture yellowBallSmallTexture;
	if (!yellowBallSmallTexture.loadFromFile("images//yellowBallVerySmall.png"))
	{
		// error...
	}
	yellowBallSmallTexture.setSmooth(true);
	sf::Sprite yellowBallSmallSprite;
	yellowBallSmallSprite.setTexture(yellowBallSmallTexture);
	yellowBallSmallSprite.setPosition(840, 200);


	// multiCheckedBox Configuration
	sf::Texture multiCheckedBoxTexture;
	if (!multiCheckedBoxTexture.loadFromFile("images//checkedBoxSmall.png"))
	{
		// error...
	}
	multiCheckedBoxTexture.setSmooth(true);
	sf::Sprite multiCheckedBoxSprite;
	multiCheckedBoxSprite.setTexture(multiCheckedBoxTexture);


	// multiUncheckedBox Configuration
	sf::Texture multiUncheckedBoxTexture;
	if (!multiUncheckedBoxTexture.loadFromFile("images//uncheckedBoxSmall.png"))
	{
		// error...
	}
	multiUncheckedBoxTexture.setSmooth(true);
	sf::Sprite multiUncheckedBoxSprite;
	multiUncheckedBoxSprite.setTexture(multiUncheckedBoxTexture);


	// checkedBox Configuration
	sf::Texture checkedBoxTexture;
	if (!checkedBoxTexture.loadFromFile("images//checkedBoxSmall.png"))
	{
		// error...
	}
	checkedBoxTexture.setSmooth(true);
	sf::Sprite checkedBoxSprite;
	checkedBoxSprite.setTexture(checkedBoxTexture);
	checkedBoxSprite.setPosition(725, 270);

	// uncheckedBox Configuration
	sf::Texture uncheckedBoxTexture;
	if (!uncheckedBoxTexture.loadFromFile("images//uncheckedBoxSmall.png"))
	{
		// error...
	}
	uncheckedBoxTexture.setSmooth(true);
	sf::Sprite uncheckedBoxSprite;
	uncheckedBoxSprite.setTexture(uncheckedBoxTexture);
	uncheckedBoxSprite.setPosition(725, 270);

	// background Configuration
	sf::Texture backgroundWelcomeTexture;
	if (!backgroundWelcomeTexture.loadFromFile("images//back9.jpg"))
	{
		// error...
	}
	backgroundWelcomeTexture.setSmooth(true);
	sf::Sprite backgroundWelcomeSprite;
	backgroundWelcomeSprite.setTexture(backgroundWelcomeTexture);
	backgroundWelcomeSprite.setOrigin(sf::Vector2f(0.f, 0.f));

	backgroundWelcomeSprite.setScale(
		welcomeWindow.getSize().x / backgroundWelcomeSprite.getLocalBounds().width,
		welcomeWindow.getSize().y / backgroundWelcomeSprite.getLocalBounds().height);

	float Xstart = 775, Ystart = 540;
	sf::Vector2i mousePos;
	// Button Configuration
	sf::Texture buttonTexture;
	if (!buttonTexture.loadFromFile("images//buttonReleasedVerySmall.png"))
	{
		// error...
	}
	buttonTexture.setSmooth(true);
	sf::Sprite buttonSprite;
	buttonSprite.setTexture(buttonTexture);
	buttonSprite.setOrigin(sf::Vector2f(0.f, 0.f));
	buttonSprite.setPosition(Xstart, Ystart);

	// Button Hover Configuration
	sf::Texture buttonHoverTexture;
	if (!buttonHoverTexture.loadFromFile("images//buttonHoverVerySmall.png"))
	{
		// error...
	}
	buttonHoverTexture.setSmooth(true);
	sf::Sprite buttonHoverSprite;
	buttonHoverSprite.setTexture(buttonHoverTexture);
	buttonHoverSprite.setOrigin(sf::Vector2f(0.f, 0.f));
	buttonHoverSprite.setPosition(Xstart, Ystart);

	// Back Button Configuration
	sf::Texture backButtonTexture;
	if (!backButtonTexture.loadFromFile("images//buttonReleasedVerySmall.png"))
	{
		// error...
	}
	backButtonTexture.setSmooth(true);
	sf::Sprite backButtonSprite;
	backButtonSprite.setTexture(backButtonTexture);
	backButtonSprite.setOrigin(sf::Vector2f(0.f, 0.f));
	backButtonSprite.setPosition(Xstart - 225, Ystart);

	// Back Button Hover Configuration
	sf::Texture backButtonHoverTexture;
	if (!backButtonHoverTexture.loadFromFile("images//buttonHoverVerySmall.png"))
	{
		// error...
	}
	backButtonHoverTexture.setSmooth(true);
	sf::Sprite backButtonHoverSprite;
	backButtonHoverSprite.setTexture(backButtonHoverTexture);
	backButtonHoverSprite.setOrigin(sf::Vector2f(0.f, 0.f));
	backButtonHoverSprite.setPosition(Xstart - 225, Ystart);


	sf::Font textFont;
	if (!textFont.loadFromFile("fonts/mvboli.ttf")) {
		//error
	}

	sf::Text buttonTitle;
	sf::Text backButtonTitle;
	sf::Text windowTitle;
	sf::Text nameText;
	sf::Text nameBallColorText;
	sf::Text namePlayFirstText;
	sf::Text rowsText;
	sf::Text colsText;
	sf::Text name2Text;
	sf::Text input1;
	sf::Text input2;
	sf::Text input3;
	sf::Text input4;
	sf::Text arrow;
	sf::Text alert;

	sf::String windowTitleText;
	sf::String input1Text;
	sf::String input2Text = '6';
	sf::String input3Text = '7';
	sf::String input4Text;

	buttonTitle.setFont(textFont);
	buttonTitle.setCharacterSize(25);
	buttonTitle.setStyle(sf::Text::Bold);
	buttonTitle.setFillColor(sf::Color::Black);
	buttonTitle.setString("Next");

	backButtonTitle.setFont(textFont);
	backButtonTitle.setCharacterSize(25);
	backButtonTitle.setStyle(sf::Text::Bold);
	backButtonTitle.setFillColor(sf::Color::Black);
	backButtonTitle.setString("Back");

	windowTitle.setFont(textFont);
	windowTitle.setCharacterSize(35);
	windowTitle.setStyle(sf::Text::Bold);
	windowTitle.setFillColor(sf::Color::White);
	windowTitleText = "Players' Info.";
	windowTitle.setString(windowTitleText);
	windowTitle.setPosition(580, 25);

	nameText.setFont(textFont);
	nameText.setCharacterSize(25);
	nameText.setStyle(sf::Text::Bold);
	nameText.setFillColor(sf::Color::Black);
	if (mode == 0) nameText.setString("Player 1's Name (Max 11)");
	else if (mode == 1) nameText.setString("Computer's Name (Max 11)");
	nameText.setPosition(540, 100);

	nameBallColorText.setFont(textFont);
	nameBallColorText.setCharacterSize(25);
	nameBallColorText.setStyle(sf::Text::Bold);
	nameBallColorText.setFillColor(sf::Color::Black);
	nameBallColorText.setString("Ball Color ?");
	nameBallColorText.setPosition(550, 200);

	namePlayFirstText.setFont(textFont);
	namePlayFirstText.setCharacterSize(25);
	namePlayFirstText.setStyle(sf::Text::Bold);
	namePlayFirstText.setFillColor(sf::Color::Black);
	namePlayFirstText.setString("Play First ?");
	namePlayFirstText.setPosition(550, 270);


	rowsText.setFont(textFont);
	rowsText.setCharacterSize(25);
	rowsText.setStyle(sf::Text::Bold);
	rowsText.setFillColor(sf::Color::Black);
	rowsText.setString("Row Size (Max 8): ");
	rowsText.setPosition(550, 320);

	colsText.setFont(textFont);
	colsText.setCharacterSize(25);
	colsText.setStyle(sf::Text::Bold);
	colsText.setFillColor(sf::Color::Black);
	colsText.setString("Col Size (Max 8): ");
	colsText.setPosition(550, 370);

	name2Text.setFont(textFont);
	name2Text.setCharacterSize(25);
	name2Text.setStyle(sf::Text::Bold);
	name2Text.setFillColor(sf::Color::Black);
	name2Text.setString("Player 2's Name (Max 11)");
	name2Text.setPosition(550, 420);


	input1.setFont(textFont);
	input1.setCharacterSize(25);
	input1.setStyle(sf::Text::Bold);
	input1.setFillColor(sf::Color::Black);
	input1.setPosition(630, 150);


	input2.setFont(textFont);
	input2.setCharacterSize(25);
	input2.setStyle(sf::Text::Bold);
	input2.setFillColor(sf::Color::Black);
	input2.setPosition(820, 322);
	input2.setString(input2Text);

	input3.setFont(textFont);
	input3.setCharacterSize(25);
	input3.setStyle(sf::Text::Bold);
	input3.setFillColor(sf::Color::Black);
	input3.setPosition(820, 374);
	input3.setString(input3Text);

	input4.setFont(textFont);
	input4.setCharacterSize(25);
	input4.setStyle(sf::Text::Bold);
	input4.setFillColor(sf::Color::Black);
	input4.setPosition(630, 470);

	arrow.setFont(textFont);
	arrow.setCharacterSize(25);
	arrow.setStyle(sf::Text::Bold);
	arrow.setFillColor(sf::Color::Black);
	arrow.setString("->");
	arrow.setPosition(600, 150);

	alert.setFont(textFont);
	alert.setCharacterSize(20);
	alert.setStyle(sf::Text::Bold);
	alert.setFillColor(sf::Color::Red);
	alert.setString("Names should NOT be empty !");
	alert.setPosition(540, 70);

	while (!isNext && welcomeWindow.isOpen()) {

		sf::Event event;
		buttonHoverSprite.setColor(sf::Color(255, 255, 255));
		backButtonHoverSprite.setColor(sf::Color(255, 255, 255));
		buttonSprite.setPosition(Xstart, Ystart);
		backButtonSprite.setPosition(Xstart - 225, Ystart);
		welcomeWindow.clear();
		welcomeWindow.draw(backgroundWelcomeSprite);
		welcomeWindow.draw(windowTitle);
		welcomeWindow.draw(nameText);
		welcomeWindow.draw(nameBallColorText);
		welcomeWindow.draw(namePlayFirstText);
		welcomeWindow.draw(rowsText);
		welcomeWindow.draw(colsText);
		welcomeWindow.draw(name2Text);
		welcomeWindow.draw(input1);
		welcomeWindow.draw(input2);
		welcomeWindow.draw(input3);
		welcomeWindow.draw(input4);
		welcomeWindow.draw(redBallSmallSprite);
		welcomeWindow.draw(yellowBallSmallSprite);
		welcomeWindow.draw(arrow);
		if (input1Text.getSize() == 0 || input4Text.getSize() == 0) {
			inputIsOk = 0;
		}
		else {
			inputIsOk = 1;
		}

		if (!inputIsOk) {
			welcomeWindow.draw(alert);
		}
		if (isPlayFirstChecked)
			welcomeWindow.draw(checkedBoxSprite);
		else
			welcomeWindow.draw(uncheckedBoxSprite);
		if (isRedChecked) {
			multiCheckedBoxSprite.setPosition(725, 200);
			multiUncheckedBoxSprite.setPosition(805, 200);
			welcomeWindow.draw(multiCheckedBoxSprite);
			welcomeWindow.draw(multiUncheckedBoxSprite);
		}
		else {
			multiCheckedBoxSprite.setPosition(805, 200);
			multiUncheckedBoxSprite.setPosition(725, 200);
			welcomeWindow.draw(multiCheckedBoxSprite);
			welcomeWindow.draw(multiUncheckedBoxSprite);
		}

		isClicked = 0;
		while (welcomeWindow.pollEvent(event)) {
			if (event.type == sf::Event::Closed) {
				welcomeWindow.close();
			}
			if (event.type == sf::Event::MouseButtonPressed) {
				if (event.mouseButton.button == sf::Mouse::Left) {
					isClicked = 1;
				}
			}
			if (event.type == sf::Event::TextEntered)
			{
				if (event.text.unicode == 9) {
					if (inputActive == 4)
						inputActive = 1;
					else
						inputActive++;
				}
				if (inputActive == 1) {
					arrow.setPosition(600, 150);
					if ((event.text.unicode >= 'a' && event.text.unicode <= 'z') || (event.text.unicode >= 'A' && event.text.unicode <= 'Z')) {
						if (input1Text.getSize() < 11) {
							input1Text += event.text.unicode;
							input1.setString(input1Text);
						}
					}
					if (event.text.unicode == ' ') {
						if (input1Text.getSize() < 11 && input1Text.getSize() > 0) {
							input1Text += " ";
							input1.setString(input1Text);
						}
					}
					if (event.text.unicode == 8 && input1Text.getSize() > 0) {
						input1Text.erase(input1Text.getSize() - 1);
						input1.setString(input1Text);
					}
				}
				else if (inputActive == 2) {
					arrow.setPosition(520, 320);
					if ((event.text.unicode >= '6' && event.text.unicode <= '8')) {
						if (input2Text.getSize() <= 1) {
							input2Text.erase(input2Text.getSize() - 1);
							input2Text += event.text.unicode;
							input2.setString(input2Text);
						}
					}
				}

				else if (inputActive == 3) {
					arrow.setPosition(520, 370);
					if ((event.text.unicode >= '6' && event.text.unicode <= '8')) {
						if (input3Text.getSize() <= 1) {
							input3Text.erase(input2Text.getSize() - 1);
							input3Text += event.text.unicode;
							input3.setString(input3Text);
						}
					}
				}
				else if (inputActive == 4) {
					arrow.setPosition(600, 470);
					if ((event.text.unicode >= 'a' && event.text.unicode <= 'z') || (event.text.unicode >= 'A' && event.text.unicode <= 'Z')) {
						if (input4Text.getSize() < 11) {
							input4Text += event.text.unicode;
							input4.setString(input4Text);
						}
					}
					if (event.text.unicode == ' ') {
						if (input4Text.getSize() < 11 && input4Text.getSize() > 0) {
							input4Text += " ";
							input4.setString(input4Text);
						}
					}
					if (event.text.unicode == 8 && input4Text.getSize() > 0) {
						input4Text.erase(input4Text.getSize() - 1);
						input4.setString(input4Text);
					}
				}
				if (event.text.unicode == 27)
					welcomeWindow.close();
				//	cout << event.text.unicode << "\n";
			}

		}
		welcomeWindow.draw(buttonSprite);
		welcomeWindow.draw(backButtonSprite);
		mousePos = sf::Mouse::getPosition(welcomeWindow);
		sf::Vector2f mousePosF(static_cast<float>(mousePos.x), static_cast<float>(mousePos.y));
		//	cout << mousePos.x << ' ' << mousePos.y << "\n";
		if (multiUncheckedBoxSprite.getGlobalBounds().contains(mousePosF)) {
			if (isClicked) {
				isClicked = 0;
				if (isRedChecked)
					isRedChecked = 0;
				else
					isRedChecked = 1;
			}
		}
		if (multiCheckedBoxSprite.getGlobalBounds().contains(mousePosF)) {
			if (isClicked) {
				isClicked = 0;
				if (isRedChecked)
					isRedChecked = 0;
				else
					isRedChecked = 1;
			}
		}
		if (uncheckedBoxSprite.getGlobalBounds().contains(mousePosF)) {
			if (isClicked) {
				isClicked = 0;
				if (!isPlayFirstChecked) {
					isPlayFirstChecked = 1;
					welcomeWindow.draw(checkedBoxSprite);
				}
				else {
					isPlayFirstChecked = 0;
					welcomeWindow.draw(uncheckedBoxSprite);
				}
			}
		}
		if (mousePos.y >= Ystart &&
			mousePos.y < (buttonSprite.getLocalBounds().height + Ystart) &&
			mousePos.x >= Xstart &&
			mousePos.x < (buttonSprite.getLocalBounds().width + Xstart)) {
			if (isClicked) {
				if (inputIsOk) {
					buttonHoverSprite.setColor(sf::Color(255, 255, 255, 128));
					isClicked = 0;
					isNext = 1;
				}

			}
			else {
				buttonHoverSprite.setPosition(Xstart, Ystart);
				welcomeWindow.draw(buttonHoverSprite);
			}
		}
		if (mousePos.y >= Ystart &&
			mousePos.y < (buttonSprite.getLocalBounds().height + Ystart) &&
			mousePos.x >= Xstart - 225 &&
			mousePos.x < (buttonSprite.getLocalBounds().width + Xstart - 225)) {
			if (isClicked) {
				backButtonHoverSprite.setColor(sf::Color(255, 255, 255, 128));
				isClicked = 0;
				isBack = 1;
			}
			else {
				backButtonHoverSprite.setPosition(Xstart - 225, Ystart);
				welcomeWindow.draw(backButtonHoverSprite);
			}
		}

		buttonTitle.setPosition(Xstart + 20, Ystart + 4);
		backButtonTitle.setPosition(Xstart - 225 + 20, Ystart + 4);
		welcomeWindow.draw(buttonTitle);
		welcomeWindow.draw(backButtonTitle);

		if (isBack) {
			posY.clear();

			delete[] posX;
			posX = NULL;

			delete[] player;
			player = NULL;

			return 0;
		}
		welcomeWindow.display();
	}





	str = new std::string;
	*str = input2.getString();
	ROWS = std::stoi(*str);
	*str = input3.getString();
	COLS = std::stoi(*str);

	delete str;
	str = NULL;


	// Making Dynamic 2D Array
	gameBoard = new int* [ROWS];
	for (int i = 0; i < ROWS; ++i)
		gameBoard[i] = new int[COLS];

	bool firstGame = 1;
	bool saveGame = 1;
	std::ofstream saveRep;

	auto start = std::chrono::system_clock::now();

	char buf[1024];
	GetCurrentDirectoryA(1024, buf);
	cout << std::string(buf) + '\\' << "\n";

	if (CreateDirectory("GameLogs", NULL))
	{
		// Directory created
		cout << "Directory had been created!!\n";
	}
	else if (ERROR_ALREADY_EXISTS == GetLastError())
	{
		// Directory already exists
		cout << "The directory is already exists !!\n";
	}
	else
	{
		cout << "Unable to create directory\n";
		// Failed for some other reason
	}

	if (isNext) {
		do {
			if(mode !=  2)
			if (!firstGame && saveGame) {
				auto end = std::chrono::system_clock::now();

				std::chrono::duration<double> elapsed_seconds = end - start;
				std::time_t end_time = std::chrono::system_clock::to_time_t(end);

				cout << "Game finished @ " << std::ctime(&end_time)
					<< "elapsed time: ~" << elapsed_seconds.count() << "s\n";


				time_t rawtime;
				struct tm* timeinfo;
				char buffer[80];

				time(&rawtime);
				timeinfo = localtime(&rawtime);

				strftime(buffer, 80, "[%m-%d-%Y][%H-%M-%S]", timeinfo);

				saveRep.open((".//GameLogs//" + std::string(buffer) + ".TUR").c_str());
				saveRep << posY << "\n\n\n@"<<ROWS<<"*"<<COLS<<"#\n\n\n";
				for (int i = ROWS - 1; i >= 0; i--) {
					for (int j = 0; j < COLS; j++) {
						saveRep << gameBoard[i][j] << ' ';
					}
					saveRep << "\n";
				}
				saveRep << "\n\n";
				saveRep << "1&" << *player[0].Name << "_1&" << *player[0].Ball << "->" << player[0].Score << "\n\n";
				saveRep << "2&" << *player[1].Name << "_2&" << *player[1].Ball << "->" << player[1].Score << "\n\n";
				saveRep << "Game finished @ " << std::ctime(&end_time)
					<< "elapsed time: ~" << elapsed_seconds.count() << "s\n";
				saveRep.close();

				start = std::chrono::system_clock::now();
			}

			cout << posY << "\n";

			posY.clear();
			delete[] posX;
			delete[] player;

			player = new Player[playersCount]();
			player[0].gameBoardID = (int)1;
			player[1].gameBoardID = (int)2;

			*player[0].Name = input1Text;
			*player[1].Name = input4Text;

			player[0].Active = isPlayFirstChecked;
			player[1].Active = !isPlayFirstChecked;


			if (isRedChecked) {
				*player[0].Ball = "Red";
				*player[1].Ball = "Yellow";
			}
			else {
				*player[1].Ball = "Red";
				*player[0].Ball = "Yellow";
			}


			if (*player[0].Ball == "Red")
				(player[0].BallSprite) = &redBallSprite;
			else if (*player[0].Ball == "Yellow")
				(player[0].BallSprite) = &yellowBallSprite;

			if (*player[1].Ball == "Red")
				(player[1].BallSprite) = &redBallSprite;
			else if (*player[1].Ball == "Yellow")
				(player[1].BallSprite) = &yellowBallSprite;


			posX = new int[COLS]();

			if (isPlayFirstChecked)
				posY = "0#";
			else
				posY = "1#";

			// Init The Board With Zeros
			for (int i = 0; i < ROWS * COLS; ++i)
				gameBoard[i / COLS][i % COLS] = 0;

			firstGame = 0;
		} while (playWindow(gameBoard, ROWS, COLS, player, posX, posY, mode, lvl));
	}
	if (mode != 2)
	if (saveGame && isNext) {
		auto end = std::chrono::system_clock::now();

		std::chrono::duration<double> elapsed_seconds = end - start;
		std::time_t end_time = std::chrono::system_clock::to_time_t(end);

		cout << "Game finished @ " << std::ctime(&end_time)
			<< "elapsed time: ~" << elapsed_seconds.count() << "s\n";


		time_t rawtime;
		struct tm* timeinfo;
		char buffer[80];

		time(&rawtime);
		timeinfo = localtime(&rawtime);

		strftime(buffer, 80, "[%m-%d-%Y][%H-%M-%S]", timeinfo);

		saveRep.open((".//GameLogs//" + std::string(buffer) + ".TUR").c_str());
		saveRep << posY << "\n\n\n@" << ROWS << "*" << COLS << "#\n\n\n";
		for (int i = ROWS - 1; i >= 0; i--) {
			for (int j = 0; j < COLS; j++) {
				saveRep << gameBoard[i][j] << ' ';
			}
			saveRep << "\n";
		}
		saveRep << "\n\n";
		saveRep << "1&" << *player[0].Name << "_1&" << *player[0].Ball << "->" << player[0].Score << "\n\n";
		saveRep << "2&" << *player[1].Name << "_2&" << *player[1].Ball << "->" << player[1].Score << "\n\n";
		saveRep << "Game finished @ " << std::ctime(&end_time)
			<< "elapsed time: ~" << elapsed_seconds.count() << "s\n";
		saveRep.close();

	}
	cout << posY << "\n";
	// For Deletion
	posY.clear();

	delete[] posX;
	posX = NULL;

	delete[] player;
	player = NULL;

	for (int i = 0; i < ROWS; ++i)
		delete[] gameBoard[i];
	delete[] gameBoard;
	gameBoard = NULL;

	return 0;
}

bool playWindow(int** gameBoard, int ROWS, int COLS, Player player[], int* posX, std::string & posY, int mode, int lvl) {

	Move move;
	sf::VideoMode desktopMode = sf::VideoMode::getDesktopMode();


	// Play Window
	sf::RenderWindow window(sf::VideoMode(desktopMode.width,
		desktopMode.height,
		desktopMode.bitsPerPixel),
		"CONNECT FOUR GAME !",
		sf::Style::Fullscreen);

	// When a key is pressed, sf::Event::KeyPressed will be true only once
	window.setKeyRepeatEnabled(false);


	// Board Configuration
	sf::Texture boardTexture;
	if (!boardTexture.loadFromFile("images//boardSmall.png"))
	{
		// error...
	}
	boardTexture.setSmooth(true);
	sf::Sprite boardSprite;
	boardSprite.setTexture(boardTexture);


	// background Configuration
	sf::Texture backgroundTexture;
	if (!backgroundTexture.loadFromFile("images//back3.jpg"))
	{
		// error...
	}
	backgroundTexture.setSmooth(true);
	sf::Sprite backgroundSprite;
	backgroundSprite.setTexture(backgroundTexture);
	backgroundSprite.setOrigin(sf::Vector2f(0.f, 0.f));

	backgroundSprite.setScale(
		desktopMode.width / backgroundSprite.getLocalBounds().width,
		desktopMode.height / backgroundSprite.getLocalBounds().height);


	sf::Font textFont;
	if (!textFont.loadFromFile("fonts/mvboli.ttf")) {
		//error
	}

	sf::Text buttonTitle;
	sf::String buttonTitleText;

	buttonTitle.setFont(textFont);
	buttonTitle.setCharacterSize(25);
	buttonTitle.setStyle(sf::Text::Bold);
	buttonTitle.setFillColor(sf::Color::Black);
	buttonTitleText = "Again ? -> Enter";
	buttonTitle.setString(buttonTitleText);
	buttonTitle.setPosition(45, 612);

	// Button Configuration
	sf::Texture buttonTexture;
	if (!buttonTexture.loadFromFile("images//buttonReleasedSmall.png"))
	{
		// error...
	}
	buttonTexture.setSmooth(true);
	sf::Sprite buttonSprite;
	buttonSprite.setTexture(buttonTexture);
	buttonSprite.setOrigin(sf::Vector2f(0.f, 0.f));
	buttonSprite.setPosition(30, 600);





	sf::Text gameTitle;
	sf::Text playerName;
	sf::Text playerScore;
	sf::Text playerActive;
	sf::Text playerBall;
	sf::String titleText;
	sf::String nameText;
	sf::String scoreText;
	sf::String activeText;
	sf::String ballText;
	gameTitle.setFont(textFont);
	gameTitle.setCharacterSize(50);
	gameTitle.setStyle(sf::Text::Bold);
	gameTitle.setFillColor(sf::Color::White);
	titleText = "CONNECT FOUR GUI";
	gameTitle.setString(titleText);
	gameTitle.setPosition(((desktopMode.width / 2) - titleText.getSize() * (gameTitle.getCharacterSize() / (float)3.05)), 10);
	playerName.setFont(textFont);
	playerName.setCharacterSize(25);
	playerName.setStyle(sf::Text::Bold);
	playerName.setFillColor(sf::Color::White);
	playerScore.setFont(textFont);
	playerScore.setCharacterSize(25);
	playerScore.setStyle(sf::Text::Bold);
	playerScore.setFillColor(sf::Color::White);
	playerActive.setFont(textFont);
	playerActive.setCharacterSize(25);
	playerActive.setStyle(sf::Text::Bold);
	playerActive.setFillColor(sf::Color::White);
	playerBall.setFont(textFont);
	playerBall.setCharacterSize(25);
	playerBall.setStyle(sf::Text::Bold);
	playerBall.setFillColor(sf::Color::White);


	sf::Event event;

	while (window.isOpen())
	{
		//	cout << sf::Mouse::getPosition(window).x << ' ' << sf::Mouse::getPosition(window).y << "\n";
		window.clear();
		window.draw(backgroundSprite);
		window.draw(gameTitle);
		nameText = "Player " + std::to_string(player[0].gameBoardID) + "'s Name : " + *player[0].Name;
		playerName.setString(nameText);
		playerName.setPosition(30, 100);
		scoreText = "Player " + std::to_string(player[0].gameBoardID) + "'s Score : " + std::to_string(player[0].Score);
		playerScore.setString(scoreText);
		playerScore.setPosition(30, 150);
		if (player[0].Active) {
			activeText = "TURN ? -> YES";
		}
		else {
			activeText = "TURN ? -> NO";
		}
		playerActive.setString(activeText);
		playerActive.setPosition(30, 200);
		ballText = "Ball Color : " + *player[0].Ball;
		playerBall.setString(ballText);
		playerBall.setPosition(30, 250);
		window.draw(playerName);
		window.draw(playerScore);
		window.draw(playerActive);
		window.draw(playerBall);

		nameText = "Player " + std::to_string(player[1].gameBoardID) + "'s Name : " + *player[1].Name;
		playerName.setString(nameText);
		playerName.setPosition(30, 350);
		scoreText = "Player " + std::to_string(player[1].gameBoardID) + "'s Score : " + std::to_string(player[1].Score);
		playerScore.setString(scoreText);
		playerScore.setPosition(30, 400);
		if (player[1].Active) {
			activeText = "TURN ? -> YES";
		}
		else {
			activeText = "TURN ? -> NO";
		}
		playerActive.setString(activeText);
		playerActive.setPosition(30, 450);
		ballText = "Ball Color : " + *player[1].Ball;
		playerBall.setString(ballText);
		playerBall.setPosition(30, 500);
		window.draw(playerName);
		window.draw(playerScore);
		window.draw(playerActive);
		window.draw(playerBall);

		if (move.played) {
			move = animateMove(window, boardSprite, 500, 75, ROWS, COLS, gameBoard, player, move);

		}
		else {
			move = drawGameBoard(window, boardSprite, 500, 75, ROWS, COLS, gameBoard, player,
				evaluateWinner(gameBoard, ROWS, COLS, player, move, false), posX, posY, mode, lvl);
		}
		if (move.gameEnd) {
			if (mode != 2)
			window.draw(buttonSprite);
			while (window.pollEvent(event)) {
				if (event.type == sf::Event::TextEntered) {
					if (event.text.unicode == 27)
						window.close();
					if (mode != 2)
					if (event.text.unicode == 13) {
						move.row = -1;
						move.col = -1;
						move.gameEnd = 0;
						move.played = 0;
						move.playedBy = -1; // index of the player 
						move.posX = -1;
						move.posY = -1;
						return 1;
					}
				}
			}
			if (mode != 2)
			window.draw(buttonTitle);
		}
		window.display();
	}


	return 0;
}

Move drawGameBoard(sf::RenderWindow & window, sf::Sprite & boardSprite, const float Xstart, const float Ystart, int ROWS, int COLS, int** gameBoard, Player player[], bool isWin, int* posX, std::string & posY, int mode , int lvl) {
	bool isDraw = 1;
	bool isAlertClosed = 0;
	for (int T = 0; T < COLS; T++) {
		if (posX[T] < ROWS) {
			isDraw = 0;
			break;
		}
	}
	if (isAlertClosed) {
		isDraw = 0;
	}
	// winBall Configuration
	sf::Texture winBallTexture;
	if (!winBallTexture.loadFromFile("images//winSmall.png"))
	{
		// error...
	}
	winBallTexture.setSmooth(true);
	sf::Sprite winBallSprite;
	winBallSprite.setTexture(winBallTexture);
	sf::Vector2f winBallSpritePos;
	Move move;
	sf::Event* mouseClicked = new sf::Event;
	sf::Vector2i mousePos;
	bool isClicked = 0;
	int i, j;
	float X = Xstart, Y = Ystart;
	for (i = ROWS - 1; i >= 0; i--) {
		X = Xstart;
		for (j = 0; j < COLS; j++) {
			boardSprite.setPosition(X, Y);
			if (mode != 2)
			if (!isWin && posY[posY.size() - 1] != '$' && !isDraw) {
				mousePos = sf::Mouse::getPosition(window);
				if (mousePos.y >= Ystart &&
					mousePos.y < ((boardSprite.getLocalBounds().height * ROWS) + Ystart) &&
					mousePos.x >= (Xstart + (boardSprite.getLocalBounds().width * j)) &&
					mousePos.x < ((boardSprite.getLocalBounds().width * (j + 1)) + Xstart)) {
					while (window.pollEvent((*mouseClicked))) {
						if ((*mouseClicked).type == sf::Event::TextEntered) {
							if ((*mouseClicked).text.unicode == 27)
								window.close();
						}
						if ((*mouseClicked).type == sf::Event::MouseButtonPressed) {
							if ((*mouseClicked).mouseButton.button == sf::Mouse::Left) {
								if(mode != 2)
								isClicked = 1;
							}
						}
					}
					if (posX[j] < ROWS) {
						if (mode == 1) {
							if (!player[0].Active) {
								boardSprite.setColor(sf::Color(255, 255, 255, 128));
							}
						}
						else {
							if (mode != 2)
							boardSprite.setColor(sf::Color(255, 255, 255, 128));
						}
						if (isClicked) {
							if (player[0].Active && !(mode == 1)) {
								move.row = posX[j];
								move.col = j;
								move.played = true;
								move.playedBy = 0;
								gameBoard[posX[j]][j] = player[0].gameBoardID;
								player[0].Score += scoreBoard(gameBoard, ROWS, COLS, player, posX, posY, move);
								gameBoard[posX[j]][j] = -1;
								posX[j]++;
								player[0].Active = false;
								player[1].Active = true;
								posY += std::to_string(j + 1);
							}
							else if (player[1].Active) {
								move.row = posX[j];
								move.col = j;
								move.played = true;
								move.playedBy = 1;
								gameBoard[posX[j]][j] = player[1].gameBoardID;
								player[1].Score += scoreBoard(gameBoard, ROWS, COLS, player, posX, posY, move);
								gameBoard[posX[j]][j] = -1;
								posX[j]++;
								player[1].Active = false;
								player[0].Active = true;
								posY += std::to_string(j + 1);
							}
							isClicked = 0;
						}
					}
				}
				else {
					boardSprite.setColor(sf::Color(255, 255, 255));
				}
			}
			else {
				if(mode != 2)
				if (posY[posY.size() - 1] != '$') {
					if (isDraw) {
						posY += "!";
						isAlertClosed = drawAlert("Draw", sf::Vector2i((window.getSize().x - 400) / 2, (window.getSize().y - 200) / 2));
					}
					posY += "$";
				}
				move.gameEnd = 1;
			}
			if (gameBoard[i][j] == player[0].gameBoardID && move.played == 0) {
				(*player[0].BallSprite).setPosition(X, Y);
				window.draw(*player[0].BallSprite);
				if ((player[0].winPos).find(std::make_pair(i, j)) != (player[0].winPos).end() && player[0].isWinner) {
					winBallSpritePos.x = (((*player[0].BallSprite).getGlobalBounds().width - winBallSprite.getGlobalBounds().width) / 2);
					winBallSpritePos.y = ((*player[0].BallSprite).getGlobalBounds().height - winBallSprite.getGlobalBounds().height) / 2;
					winBallSprite.setPosition(X + winBallSpritePos.x, Y + winBallSpritePos.y);
					window.draw(winBallSprite);
				}
			}
			else if (gameBoard[i][j] == player[1].gameBoardID && move.played == 0) {
				(*player[1].BallSprite).setPosition(X, Y);
				window.draw(*player[1].BallSprite);
				if ((player[1].winPos).find(std::make_pair(i, j)) != (player[1].winPos).end() && player[1].isWinner) {
					winBallSpritePos.x = ((*player[1].BallSprite).getGlobalBounds().width - winBallSprite.getGlobalBounds().width) / 2;
					winBallSpritePos.y = ((*player[1].BallSprite).getGlobalBounds().height - winBallSprite.getGlobalBounds().height) / 2;
					winBallSprite.setPosition(X + winBallSpritePos.x, Y + winBallSpritePos.y);
					window.draw(winBallSprite);
				}
			}
			window.draw(boardSprite);
			X += boardSprite.getLocalBounds().width;
		}
		Y += boardSprite.getLocalBounds().height;
	}
	if (player[0].Active && mode == 1 && !move.played && !player[0].isWinner && !player[1].isWinner) {
		Sleep(250);
		move.col = alphaBeta(gameBoard, ROWS, COLS, player, posX, move, lvl, -INF, +INF, true).col;
		move.row = posX[move.col];
		move.played = true;
		move.playedBy = 0;
		gameBoard[move.row][move.col] = player[0].gameBoardID;
		player[0].Score += scoreBoard(gameBoard, ROWS, COLS, player, posX, posY, move);
		gameBoard[move.row][move.col] = -1;
		posX[move.col]++;
		player[0].Active = false;
		player[1].Active = true;
		posY += std::to_string(move.col + 1);
	}
	if (mode == 2) {
		if (!posY.size())
			move.gameEnd = 1;
		if (player[0].Active && !move.played && !move.gameEnd) {
			move.row = posX[std::stoi(subSTR(posY,0,0)) - 1];
			move.col = std::stoi(subSTR(posY, 0, 0)) - 1;
			move.played = true;
			move.playedBy = 0;
			gameBoard[move.row][move.col] = player[0].gameBoardID;
			player[0].Score += scoreBoard(gameBoard, ROWS, COLS, player, posX, posY, move);
			gameBoard[move.row][move.col] = -1;
			posX[move.col]++;
			player[0].Active = false;
			player[1].Active = true;
			if (posY.size())
			posY.erase(posY.begin());
			Sleep(500);
		}
		else if (player[1].Active && !move.played && !move.gameEnd) {
			move.row = posX[std::stoi(subSTR(posY, 0, 0)) - 1];
			move.col = std::stoi(subSTR(posY, 0, 0)) - 1;
			move.played = true;
			move.playedBy = 1;
			gameBoard[move.row][move.col] = player[1].gameBoardID;
			player[1].Score += scoreBoard(gameBoard, ROWS, COLS, player, posX, posY, move);
			gameBoard[move.row][move.col] = -1;
			posX[move.col]++;
			player[1].Active = false;
			player[0].Active = true;
			if (posY.size())
			posY.erase(posY.begin());
			Sleep(500);
		}
	}

	move.posX = (Xstart + (boardSprite.getLocalBounds().width * move.col));
	move.posY = (Ystart - ((boardSprite.getLocalBounds().height) / 2));
	boardSprite.setColor(sf::Color(255, 255, 255));
	delete mouseClicked;
	mouseClicked = NULL;
	return move;
}

Move animateMove(sf::RenderWindow & window, sf::Sprite & boardSprite, const float Xstart, const float Ystart, int ROWS, int COLS, int** gameBoard, Player player[], Move & move) {

	int i, j;
	float X = Xstart, Y = Ystart;
	(*player[move.playedBy].BallSprite).setPosition(move.posX, move.posY);
	move.posY += SPEED;
	window.draw(*player[move.playedBy].BallSprite);
	for (i = ROWS - 1; i >= 0; i--) {
		X = Xstart;
		for (j = 0; j < COLS; j++) {
			boardSprite.setPosition(X, Y);
			if (gameBoard[i][j] == player[0].gameBoardID) {
				(*player[0].BallSprite).setPosition(X, Y);
				window.draw(*player[0].BallSprite);
			}
			else if (gameBoard[i][j] == player[1].gameBoardID) {
				(*player[1].BallSprite).setPosition(X, Y);
				window.draw(*player[1].BallSprite);
			}
			window.draw(boardSprite);
			X += boardSprite.getLocalBounds().width;
		}
		Y += boardSprite.getLocalBounds().height;
	}

	if (move.posY >= (Ystart + ((boardSprite.getLocalBounds().height) * ((ROWS - 1) - move.row)))) {
		move.played = false;
		gameBoard[move.row][move.col] = player[move.playedBy].gameBoardID;
	}
	return move;
}

int evaluateWinner(int** gameBoard, int ROWS, int COLS, Player player[], Move move, bool AI) {

	/*
	 gameBoard ROWS = 6 COLS = 7
	(0 , 0) (0 , 1) (0 , 2) (0 , 3) (0 , 4) (0 , 5) (0 , 6)
	(1 , 0) (1 , 1) (1 , 2) (1 , 3) (1 , 4) (1 , 5) (1 , 6)
	(2 , 0) (2 , 1) (2 , 2) (2 , 3) (2 , 4) (2 , 5) (2 , 6)
	(3 , 0) (3 , 1) (3 , 2) (3 , 3) (3 , 4) (3 , 5) (3 , 6)
	(4 , 0) (4 , 1) (4 , 2) (4 , 3) (4 , 4) (4 , 5) (4 , 6)
	(5 , 0) (5 , 1) (5 , 2) (5 , 3) (5 , 4) (5 , 5) (5 , 6)
	*/

	int minRow = -1, minCol = -1;
	int maxRow = -1, maxCol = -1;
	int piece = (!player[0].Active) ? player[0].gameBoardID : player[1].gameBoardID;
	int ind = (!player[0].Active) ? 0 : 1;
	Window window;
	if (!AI) {
		// Horizontal Check
		minRow = move.row;
		maxRow = move.row + 1;

		if (move.col + 3 >= COLS)
			maxCol = COLS;
		else
			maxCol = move.col + 4;
		if (move.col - 3 < 0)
			minCol = 0;
		else
			minCol = move.col - 3;

		for (int i = minRow; i < maxRow; i++) {
			for (int j = minCol; j < maxCol - 3; j++) {
				window.piece.row = i;
				window.piece.col = j;
				window.count(gameBoard, piece, 1);
				if (window.pieceCount == 4 && window.emptyPieceCount == 0) {
					player[ind].isWinner = 1;
					(player[ind].winPos)[std::make_pair(i, j)] = player[ind].gameBoardID;
					(player[ind].winPos)[std::make_pair(i, j + 1)] = player[ind].gameBoardID;
					(player[ind].winPos)[std::make_pair(i, j + 2)] = player[ind].gameBoardID;
					(player[ind].winPos)[std::make_pair(i, j + 3)] = player[ind].gameBoardID;
					return 1;
				}
			}
		}
		// Vertical Check
		minCol = move.col;
		maxCol = move.col + 1;

		if (move.row + 3 >= ROWS)
			maxRow = ROWS;
		else
			maxRow = move.row + 4;
		if (move.row - 3 < 0)
			minRow = 0;
		else
			minRow = move.row - 3;
		for (int j = minCol; j < maxCol; j++) {
			for (int i = minRow; i < maxRow - 3; i++) {
				window.piece.row = i;
				window.piece.col = j;
				window.count(gameBoard, piece, 2);
				if (window.pieceCount == 4 && window.emptyPieceCount == 0) {
					player[ind].isWinner = 1;
					(player[ind].winPos)[std::make_pair(i, j)] = player[ind].gameBoardID;
					(player[ind].winPos)[std::make_pair(i + 1, j)] = player[ind].gameBoardID;
					(player[ind].winPos)[std::make_pair(i + 2, j)] = player[ind].gameBoardID;
					(player[ind].winPos)[std::make_pair(i + 3, j)] = player[ind].gameBoardID;
					return 1;
				}
			}
		}
		// Diagonal Check (\)
		if (move.row + 3 >= ROWS)
			maxRow = ROWS;
		else
			maxRow = move.row + 4;
		if (move.row - 3 < 0)
			minRow = 0;
		else
			minRow = move.row - 3;

		if (move.col + 3 >= COLS)
			maxCol = COLS;
		else
			maxCol = move.col + 4;
		if (move.col - 3 < 0)
			minCol = 0;
		else
			minCol = move.col - 3;
		for (int i = maxRow - 1; i >= minRow + 3; i--) {
			for (int j = maxCol - 1; j >= minCol + 3; j--) {
				window.piece.row = i;
				window.piece.col = j;
				window.count(gameBoard, piece, 3);
				if (window.pieceCount == 4 && window.emptyPieceCount == 0) {
					player[ind].isWinner = 1;
					(player[ind].winPos)[std::make_pair(i, j)] = player[ind].gameBoardID;
					(player[ind].winPos)[std::make_pair(i - 1, j - 1)] = player[ind].gameBoardID;
					(player[ind].winPos)[std::make_pair(i - 2, j - 2)] = player[ind].gameBoardID;
					(player[ind].winPos)[std::make_pair(i - 3, j - 3)] = player[ind].gameBoardID;
					return 1;
				}
			}
		}
		// Diagonal Check (/)
		if (move.row + 3 >= ROWS)
			maxRow = ROWS;
		else
			maxRow = move.row + 4;
		if (move.row - 3 < 0)
			minRow = 0;
		else
			minRow = move.row - 3;

		if (move.col + 3 >= COLS)
			maxCol = COLS;
		else
			maxCol = move.col + 4;
		if (move.col - 3 < 0)
			minCol = 0;
		else
			minCol = move.col - 3;
		for (int i = minRow; i < maxRow - 3; i++) {
			for (int j = maxCol - 1; j >= minCol + 3; j--) {
				window.piece.row = i;
				window.piece.col = j;
				window.count(gameBoard, piece, 4);
				if (window.pieceCount == 4 && window.emptyPieceCount == 0) {
					player[ind].isWinner = 1;
					(player[ind].winPos)[std::make_pair(i, j)] = player[ind].gameBoardID;
					(player[ind].winPos)[std::make_pair(i + 1, j - 1)] = player[ind].gameBoardID;
					(player[ind].winPos)[std::make_pair(i + 2, j - 2)] = player[ind].gameBoardID;
					(player[ind].winPos)[std::make_pair(i + 3, j - 3)] = player[ind].gameBoardID;
					return 1;
				}
			}
		}
	}
	else {
		piece = 2;
		int AIpiece = 1;
		// Horizontal Check
		minRow = move.row;
		maxRow = move.row + 1;
		int score = 0;

		if (move.col + 3 >= COLS)
			maxCol = COLS;
		else
			maxCol = move.col + 4;
		if (move.col - 3 < 0)
			minCol = 0;
		else
			minCol = move.col - 3;

		for (int i = minRow; i < maxRow; i++) {
			for (int j = minCol; j < maxCol - 3; j++) {
				window.piece.row = i;
				window.piece.col = j;
				window.count(gameBoard, piece, 1);
				score += evaluateWindow(window, false);
				window.count(gameBoard, AIpiece, 1);
				score += evaluateWindow(window, true);
			}
		}
		// Vertical Check
		minCol = move.col;
		maxCol = move.col + 1;

		if (move.row + 3 >= ROWS)
			maxRow = ROWS;
		else
			maxRow = move.row + 4;
		if (move.row - 3 < 0)
			minRow = 0;
		else
			minRow = move.row - 3;
		for (int j = minCol; j < maxCol; j++) {
			for (int i = minRow; i < maxRow - 3; i++) {
				window.piece.row = i;
				window.piece.col = j;
				window.count(gameBoard, piece, 2);
				score += evaluateWindow(window, false);
				window.count(gameBoard, AIpiece, 2);
				score += evaluateWindow(window, true);
			}
		}
		// Diagonal Check (\)
		if (move.row + 3 >= ROWS)
			maxRow = ROWS;
		else
			maxRow = move.row + 4;
		if (move.row - 3 < 0)
			minRow = 0;
		else
			minRow = move.row - 3;

		if (move.col + 3 >= COLS)
			maxCol = COLS;
		else
			maxCol = move.col + 4;
		if (move.col - 3 < 0)
			minCol = 0;
		else
			minCol = move.col - 3;
		for (int i = maxRow - 1; i >= minRow + 3; i--) {
			for (int j = maxCol - 1; j >= minCol + 3; j--) {
				window.piece.row = i;
				window.piece.col = j;
				window.count(gameBoard, piece, 3);
				score += evaluateWindow(window, false);
				window.count(gameBoard, AIpiece, 3);
				score += evaluateWindow(window, true);
			}
		}
		// Diagonal Check (/)
		if (move.row + 3 >= ROWS)
			maxRow = ROWS;
		else
			maxRow = move.row + 4;
		if (move.row - 3 < 0)
			minRow = 0;
		else
			minRow = move.row - 3;

		if (move.col + 3 >= COLS)
			maxCol = COLS;
		else
			maxCol = move.col + 4;
		if (move.col - 3 < 0)
			minCol = 0;
		else
			minCol = move.col - 3;
		for (int i = minRow; i < maxRow - 3; i++) {
			for (int j = maxCol - 1; j >= minCol + 3; j--) {
				window.piece.row = i;
				window.piece.col = j;
				window.count(gameBoard, piece, 4);
				score += evaluateWindow(window, false);
				window.count(gameBoard, AIpiece, 4);
				score += evaluateWindow(window, true);
			}
		}
		return score;
	}
	return 0;
}

BestMove alphaBeta(int** gameBoard, int ROWS, int COLS, Player player[], int* posX, Move move, int depth, long long alpha, long long  beta, bool isMximizingPlayer) {
	BestMove bestMove;
	bool terminal;
	long long value = 0;
	long long newScore = 0;
	int ind = (!player[0].Active) ? 0 : 1;
	terminal = evaluateWinner(gameBoard, ROWS, COLS, player, move, false);
	player[0].winPos.clear();
	player[1].winPos.clear();
	player[0].isWinner = false;
	player[1].isWinner = false;
	if (terminal) {
		if (ind == 0) {
			bestMove.value = MAX;
			return bestMove;
		}
		else {
			bestMove.value = MIN;
			return bestMove;
		}
	}
	if (depth == 0) {
		bestMove.value = evaluateWinner(gameBoard, ROWS, COLS, player, move, true);
		return bestMove;
	}

	bool isDraw = true;
	for (int i = 0; i < COLS; i++) {
		if (posX[i] < COLS) {
			isDraw = false;
			break;
		}
	}
	if (isDraw) {
		bestMove.value = 0;
		return bestMove;
	}


	if (isMximizingPlayer) {
		value = -INF;
		for (int j = 0; j < COLS; j++) {
			if (posX[j] < ROWS) {
				move.row = posX[j];
				posX[j]++;
				move.col = j;
				player[0].Active = 0;
				player[1].Active = 1;
				gameBoard[move.row][move.col] = player[0].gameBoardID;
				newScore = alphaBeta(gameBoard, ROWS, COLS, player, posX, move, depth - 1, alpha, beta, !isMximizingPlayer).value;
				if (newScore > value) {
					value = newScore;
					bestMove.col = j;
				}
				gameBoard[move.row][move.col] = 0;
				posX[move.col]--;
				alpha = std::max(value, alpha);
				if (alpha >= beta) break;
			}
		}
		bestMove.value = value;
		return bestMove;
	}
	else {
		value = +INF;
		for (int j = 0; j < COLS; j++) {
			if (posX[j] < ROWS) {
				move.row = posX[j];
				posX[j]++;
				move.col = j;
				player[0].Active = 1;
				player[1].Active = 0;
				gameBoard[move.row][move.col] = player[1].gameBoardID;
				newScore = alphaBeta(gameBoard, ROWS, COLS, player, posX, move, depth - 1, alpha, beta, !isMximizingPlayer).value;
				if (newScore < value) {
					value = newScore;
					bestMove.col = j;
				}
				gameBoard[move.row][move.col] = 0;
				posX[move.col]--;
				beta = std::min(value, beta);
				if (alpha >= beta) break;
			}
		}
		bestMove.value = value;
		return bestMove;
	}
}

int evaluateWindow(Window window, bool AI) {
	int score = 0;
	if (window.pieceCount == 4 && window.emptyPieceCount == 0)
		(AI) ? score += 100 : score -= 100;
	else if (window.pieceCount == 3 && window.emptyPieceCount == 1)
		(AI) ? score += 50 : score -= 50;
	else if (window.pieceCount == 2 && window.emptyPieceCount == 2)
		(AI) ? score += 25 : score -= 25;
	else if (window.pieceCount == 1 && window.emptyPieceCount == 3)
		(AI) ? score += 5 : score -= 5;
	return score;
}

bool drawAlert(std::string alertText, sf::Vector2i alertWindowPos) {
	//alertWindowPos.
	sf::RenderWindow windowAlert(sf::VideoMode(400, 200), "Alert", sf::Style::Close);
	windowAlert.setPosition(alertWindowPos);
	sf::Font font;
	if (!font.loadFromFile("fonts/mvboli.ttf"))
	{
		//error
	}
	sf::Texture texture;
	if (!texture.loadFromFile("images/alertBackground.jpg")) {
		//error
	}
	sf::RectangleShape rectOfPhoto;
	rectOfPhoto.setTexture(&texture);
	rectOfPhoto.setSize(sf::Vector2f(400.0, 200.0));
	sf::Text text;
	text.setFont(font);
	text.setCharacterSize(55);
	text.setFillColor(sf::Color::White);
	text.setPosition(128.0, 63.0);
	text.setString(alertText);

	while (windowAlert.isOpen()) {
		sf::Event event;
		while (windowAlert.pollEvent(event)) {
			switch (event.type)
			{
			case sf::Event::Closed:
				windowAlert.close();
				break;
			}
		}
		windowAlert.clear();
		windowAlert.draw(rectOfPhoto);
		windowAlert.draw(text);
		windowAlert.display();
	}
	return 1;
}

bool gameRep(sf::RenderWindow& welcomeWindow, int** gameBoard, int ROWS, int COLS, Player player[], int* posX, std::string& posY) {
	std::string* str;
	std::string selFile;
	std::string replaySeq;
	std::string gameDate;
	std::string line;
	std::ifstream restoreRep;
	double gameElaps;
	bool isNext = false;
	bool isBack = false;
	bool isClicked = 0;
	bool upFlag = 0;
	bool downFlag = 0;
	bool firstIndF = 0;

	List list;

	// redBall Configuration
	sf::Texture redBallTexture;
	if (!redBallTexture.loadFromFile("images//redBallSmall.png"))
	{
		// error...
	}
	redBallTexture.setSmooth(true);
	sf::Sprite redBallSprite;
	redBallSprite.setTexture(redBallTexture);
	// yellowBall Configuration
	sf::Texture yellowBallTexture;
	if (!yellowBallTexture.loadFromFile("images//yellowBallSmall.png"))
	{
		// error...
	}
	yellowBallTexture.setSmooth(true);
	sf::Sprite yellowBallSprite;
	yellowBallSprite.setTexture(yellowBallTexture);


	// background Configuration
	sf::Texture backgroundWelcomeTexture;
	if (!backgroundWelcomeTexture.loadFromFile("images//back9Rep.jpg"))
	{
		// error...
	}
	backgroundWelcomeTexture.setSmooth(true);
	sf::Sprite backgroundWelcomeSprite;
	backgroundWelcomeSprite.setTexture(backgroundWelcomeTexture);
	backgroundWelcomeSprite.setOrigin(sf::Vector2f(0.f, 0.f));

	backgroundWelcomeSprite.setScale(
		welcomeWindow.getSize().x / backgroundWelcomeSprite.getLocalBounds().width,
		welcomeWindow.getSize().y / backgroundWelcomeSprite.getLocalBounds().height);

	float Xstart = 775, Ystart = 540;
	sf::Vector2i mousePos;
	// Button Configuration
	sf::Texture buttonTexture;
	if (!buttonTexture.loadFromFile("images//buttonReleasedVerySmall.png"))
	{
		// error...
	}
	buttonTexture.setSmooth(true);
	sf::Sprite buttonSprite;
	buttonSprite.setTexture(buttonTexture);
	buttonSprite.setOrigin(sf::Vector2f(0.f, 0.f));
	buttonSprite.setPosition(Xstart, Ystart);

	// Button Hover Configuration
	sf::Texture buttonHoverTexture;
	if (!buttonHoverTexture.loadFromFile("images//buttonHoverVerySmall.png"))
	{
		// error...
	}
	buttonHoverTexture.setSmooth(true);
	sf::Sprite buttonHoverSprite;
	buttonHoverSprite.setTexture(buttonHoverTexture);
	buttonHoverSprite.setOrigin(sf::Vector2f(0.f, 0.f));
	buttonHoverSprite.setPosition(Xstart, Ystart);

	// Back Button Configuration
	sf::Texture backButtonTexture;
	if (!backButtonTexture.loadFromFile("images//buttonReleasedVerySmall.png"))
	{
		// error...
	}
	backButtonTexture.setSmooth(true);
	sf::Sprite backButtonSprite;
	backButtonSprite.setTexture(backButtonTexture);
	backButtonSprite.setOrigin(sf::Vector2f(0.f, 0.f));
	backButtonSprite.setPosition(Xstart - 225, Ystart);

	// Back Button Hover Configuration
	sf::Texture backButtonHoverTexture;
	if (!backButtonHoverTexture.loadFromFile("images//buttonHoverVerySmall.png"))
	{
		// error...
	}
	backButtonHoverTexture.setSmooth(true);
	sf::Sprite backButtonHoverSprite;
	backButtonHoverSprite.setTexture(backButtonHoverTexture);
	backButtonHoverSprite.setOrigin(sf::Vector2f(0.f, 0.f));
	backButtonHoverSprite.setPosition(Xstart - 225, Ystart);

	sf::Font textFont;
	if (!textFont.loadFromFile("fonts/mvboli.ttf")) {
		//error
	}

	sf::Text windowTitle;
	sf::Text buttonTitle;
	sf::Text backButtonTitle;
	sf::Text alert;
	sf::Text arrow;
	sf::Text fName;




	alert.setFont(textFont);
	alert.setCharacterSize(20);
	alert.setStyle(sf::Text::Bold);
	alert.setFillColor(sf::Color::Red);
	alert.setString("Please Select a Replay File !");
	alert.setPosition(540, 70);

	buttonTitle.setFont(textFont);
	buttonTitle.setCharacterSize(25);
	buttonTitle.setStyle(sf::Text::Bold);
	buttonTitle.setFillColor(sf::Color::Black);
	buttonTitle.setString("Next");

	backButtonTitle.setFont(textFont);
	backButtonTitle.setCharacterSize(25);
	backButtonTitle.setStyle(sf::Text::Bold);
	backButtonTitle.setFillColor(sf::Color::Black);
	backButtonTitle.setString("Back");

	windowTitle.setFont(textFont);
	windowTitle.setCharacterSize(35);
	windowTitle.setStyle(sf::Text::Bold);
	windowTitle.setFillColor(sf::Color::Black);
	windowTitle.setString("Game Replay");
	windowTitle.setPosition(580, 25);

	arrow.setFont(textFont);
	arrow.setCharacterSize(25);
	arrow.setStyle(sf::Text::Bold);
	arrow.setFillColor(sf::Color::Black);
	arrow.setString(">                  <");
	arrow.setPosition(600, 120);

	fName.setFont(textFont);
	fName.setCharacterSize(14);
	fName.setStyle(sf::Text::Bold);
	fName.setFillColor(sf::Color::Black);
	fName.setString("[06-17-2019][06-17-27].TUR");
	fName.setPosition(615, 126);

	char buf[1024];
	GetCurrentDirectoryA(1024, buf);
	cout << std::string(buf) + '\\' << "\n";

	if (CreateDirectory("GameLogs", NULL))
	{
		// Directory created
		cout << "Directory had been created!!\n";
	}
	else if (ERROR_ALREADY_EXISTS == GetLastError())
	{
		// Directory already exists
		cout << "The directory is already exists !!\n";
	}
	else
	{
		cout << "Unable to create directory\n";
		// Failed for some other reason
	}


	stringvec fileName;
	read_directory(".//GameLogs", fileName);

	
	for (int i = 0; i < (int)fileName.size(); i++) {
		if (fileName[i].front() != '.') {
			if (!firstIndF) {
				list.firstInd = i;
				firstIndF = true;
			}
		//	cout << fileName[i] << "\n";
			list.size++;
		}
	}
//	cout << list.size << "\n";


	while (!isNext && welcomeWindow.isOpen()) {

		sf::Event event;
		buttonHoverSprite.setColor(sf::Color(255, 255, 255));
		backButtonHoverSprite.setColor(sf::Color(255, 255, 255));
		buttonSprite.setPosition(Xstart, Ystart);
		backButtonSprite.setPosition(Xstart - 225, Ystart);
		welcomeWindow.clear();
		welcomeWindow.draw(backgroundWelcomeSprite);
		welcomeWindow.draw(windowTitle);
		welcomeWindow.draw(arrow);
		if(fileName.size())
		for (int i = list.firstInd + list.index; i < (int)((fileName.size() <= 12) ? fileName.size() : (list.firstInd + list.index + 12)); i++) {
			if (i < (int)fileName.size()) {
				fName.setString(fileName[i].c_str());
				fName.setPosition(615, fName.getPosition().y + 25);
				if (i == list.firstInd + list.selIndex)
					fName.setFillColor(sf::Color::Red);
				else
					fName.setFillColor(sf::Color::Black);
				welcomeWindow.draw(fName);
		}

		}
		fName.setPosition(615, 126);
		if (list.selIndex < 0) {
			welcomeWindow.draw(alert);
		}
		if (upFlag) {
			if (arrow.getPosition().y > 145 && arrow.getPosition().y <= 145 + 11 * 25 && list.size) {
				arrow.setPosition(600, arrow.getPosition().y - 25);
				list.selIndex--;
			}
			else {
				if (list.index > 0) {
					list.index--;
					list.selIndex--;
				}
			}
			upFlag = false;
		}
		if (downFlag) {
			if (arrow.getPosition().y >= 120 && arrow.getPosition().y < 145 + 11 * 25 && list.size && list.selIndex + list.firstInd <= list.size) {
				arrow.setPosition(600, arrow.getPosition().y + 25);
				list.selIndex++;
			}
			else {
				if ((list.index + 12) < list.size) {
					list.index++;
					list.selIndex++;
				}
			}
			downFlag = false;
		}
	//	cout << list.selIndex << "\n";
		isClicked = 0;
		while (welcomeWindow.pollEvent(event)) {
			if (event.type == sf::Event::Closed) {
				isBack = 1;
			}
			if (event.type == sf::Event::MouseButtonPressed) {
				if (event.mouseButton.button == sf::Mouse::Left) {
					isClicked = 1;
				}
			}
			if (event.type == sf::Event::TextEntered)
			{
				if (event.text.unicode == 27)
					isBack = 1;
			//		cout << event.text.unicode << "\n";
			}
			if (sf::Keyboard::isKeyPressed(sf::Keyboard::Up))
			{
				upFlag = true;
			}
			if (sf::Keyboard::isKeyPressed(sf::Keyboard::Down))
			{
				downFlag = true;
			}
		}
	//	cout << upFlag << " " << downFlag << "\n";
		welcomeWindow.draw(buttonSprite);
		welcomeWindow.draw(backButtonSprite);
		mousePos = sf::Mouse::getPosition(welcomeWindow);
		sf::Vector2f mousePosF(static_cast<float>(mousePos.x), static_cast<float>(mousePos.y));
		//	cout << mousePos.x << ' ' << mousePos.y << "\n";

		if (mousePos.y >= Ystart &&
			mousePos.y < (buttonSprite.getLocalBounds().height + Ystart) &&
			mousePos.x >= Xstart &&
			mousePos.x < (buttonSprite.getLocalBounds().width + Xstart)) {
			if (isClicked) {
				if (list.selIndex > -1) {
					buttonHoverSprite.setColor(sf::Color(255, 255, 255, 128));
					isClicked = 0;
					isNext = 1;
				}
			}
			else {
				buttonHoverSprite.setPosition(Xstart, Ystart);
				welcomeWindow.draw(buttonHoverSprite);
			}
		}
		if (mousePos.y >= Ystart &&
			mousePos.y < (buttonSprite.getLocalBounds().height + Ystart) &&
			mousePos.x >= Xstart - 225 &&
			mousePos.x < (buttonSprite.getLocalBounds().width + Xstart - 225)) {
			if (isClicked) {
					backButtonHoverSprite.setColor(sf::Color(255, 255, 255, 128));
					isClicked = 0;
					isBack = 1;
			}
			else {
				backButtonHoverSprite.setPosition(Xstart - 225, Ystart);
				welcomeWindow.draw(backButtonHoverSprite);
			}
		}

		buttonTitle.setPosition(Xstart + 20, Ystart + 4);
		backButtonTitle.setPosition(Xstart - 225 + 20, Ystart + 4);
		welcomeWindow.draw(buttonTitle);
		welcomeWindow.draw(backButtonTitle);
		welcomeWindow.display();

		if (isBack) {
			fileName.clear();
			posY.clear();

			delete[] posX;
			posX = NULL;

			delete[] player;
			player = NULL;

			for (int i = 0; i < ROWS; ++i)
				delete[] gameBoard[i];
			delete[] gameBoard;
			gameBoard = NULL;

			return 0;
		}
		if (isNext) {
			break;
		}
	}

	if(fileName.size())
	selFile = (fileName[list.selIndex + list.firstInd]).c_str();

	player = new Player[2]();

	player[0].gameBoardID = 1;
	player[1].gameBoardID = 2;

	restoreRep.open((".//GameLogs//" + selFile).c_str());
	while (std::getline(restoreRep, line))
	{
	//	cout << line << "\n";
		if (line.size())
		if (line[1] == '#') {
			if (line[0] == '0')
				player[0].Active = 1;
			else
				player[1].Active = 1;

			replaySeq = subSTR(line, 2, ((line[line.size() - 2] == '!') ? (line.size() - 3) : (line.size() - 2)));
		}
		else if (line[0] == '@') {
			str = new std::string;
			*str = subSTR(line, 1, line.find('*') - 1);
			ROWS = std::stoi(*str);
			*str = subSTR(line, line.find('*') + 1, line.find('#') - 1);
			COLS = std::stoi(*str);
			delete str;
			str = NULL;
		}
		else if (line[0] == '1' && line[1] == '&') {
			*player[0].Name = subSTR(line, 2, line.find('_') - 1);
			*player[0].Ball = subSTR(line, line.find('_') + 3, line.find('-') - 1);
		}
		else if (line[0] == '2' && line[1] == '&') {
			*player[1].Name = subSTR(line, 2, line.find('_') - 1);
			*player[1].Ball = subSTR(line, line.find('_') + 3, line.find('-') - 1);
		}
		else if (line[0] == 'G') {
			gameDate = subSTR(line, line.find('@') + 2, line.size() - 1);
		}
		else if (line[0] == 'e') {
			gameElaps = std::stof(subSTR(line, line.find('~') + 1, line.size() - 2));
		}
	}

	gameBoard = new int* [ROWS];
	for (int i = 0; i < ROWS; ++i)
		gameBoard[i] = new int[COLS];
	
	for(int i = 0; i < ROWS; ++i)
		for(int j = 0; j < COLS; ++j)
			gameBoard[i][j] = -1;

	posX = new int[COLS]();


	if (*player[0].Ball == "Red")
		(player[0].BallSprite) = &redBallSprite;
	else if (*player[0].Ball == "Yellow")
		(player[0].BallSprite) = &yellowBallSprite;

	if (*player[1].Ball == "Red")
		(player[1].BallSprite) = &redBallSprite;
	else if (*player[1].Ball == "Yellow")
		(player[1].BallSprite) = &yellowBallSprite;

	if (posY.size())
		posY.clear();

	for (int i = 0; i < (int)replaySeq.size(); ++i)
		posY += replaySeq[i];

	playWindow(gameBoard, ROWS, COLS, player, posX, posY, 2, 0);

	fileName.clear();
	posY.clear();

	delete[] posX;
	posX = NULL;

	delete[] player;
	player = NULL;

	for (int i = 0; i < ROWS; ++i)
		delete[] gameBoard[i];
	delete[] gameBoard;
	gameBoard = NULL;

	return 0;
}

bool playAI(sf::RenderWindow& welcomeWindow, int** gameBoard, int ROWS, int COLS, Player player[], int* posX, std::string& posY) {
	bool isBack = false;
	bool isClicked = 0;
	short lvl = 0;

	// background Configuration
	sf::Texture backgroundWelcomeTexture;
	if (!backgroundWelcomeTexture.loadFromFile("images//back9.jpg"))
	{
		// error...
	}
	backgroundWelcomeTexture.setSmooth(true);
	sf::Sprite backgroundWelcomeSprite;
	backgroundWelcomeSprite.setTexture(backgroundWelcomeTexture);
	backgroundWelcomeSprite.setOrigin(sf::Vector2f(0.f, 0.f));

	backgroundWelcomeSprite.setScale(
		welcomeWindow.getSize().x / backgroundWelcomeSprite.getLocalBounds().width,
		welcomeWindow.getSize().y / backgroundWelcomeSprite.getLocalBounds().height);

	float XstartBS = 775, YstartBS = 540;
	sf::Vector2i mousePos;

	// Back Button Configuration
	sf::Texture backButtonTexture;
	if (!backButtonTexture.loadFromFile("images//buttonReleasedVerySmall.png"))
	{
		// error...
	}
	backButtonTexture.setSmooth(true);
	sf::Sprite backButtonSprite;
	backButtonSprite.setTexture(backButtonTexture);
	backButtonSprite.setOrigin(sf::Vector2f(0.f, 0.f));
	backButtonSprite.setPosition(XstartBS - 225, YstartBS);

	// Back Button Hover Configuration
	sf::Texture backButtonHoverTexture;
	if (!backButtonHoverTexture.loadFromFile("images//buttonHoverVerySmall.png"))
	{
		// error...
	}
	backButtonHoverTexture.setSmooth(true);
	sf::Sprite backButtonHoverSprite;
	backButtonHoverSprite.setTexture(backButtonHoverTexture);
	backButtonHoverSprite.setOrigin(sf::Vector2f(0.f, 0.f));
	backButtonHoverSprite.setPosition(XstartBS - 225, YstartBS);

	sf::Font textFont;
	if (!textFont.loadFromFile("fonts/mvboli.ttf")) {
		//error
	}

	sf::Text windowTitle;
	sf::Text buttonTitle;
	sf::Text backButtonTitle;
	sf::Text alert;


	alert.setFont(textFont);
	alert.setCharacterSize(20);
	alert.setStyle(sf::Text::Bold);
	alert.setFillColor(sf::Color::Red);
	alert.setString("Please Select The AI Mode !");
	alert.setPosition(540, 70);

	buttonTitle.setFont(textFont);
	buttonTitle.setCharacterSize(25);
	buttonTitle.setStyle(sf::Text::Bold);
	buttonTitle.setFillColor(sf::Color::Black);
	buttonTitle.setString("Next");

	backButtonTitle.setFont(textFont);
	backButtonTitle.setCharacterSize(25);
	backButtonTitle.setStyle(sf::Text::Bold);
	backButtonTitle.setFillColor(sf::Color::Black);
	backButtonTitle.setString("Back");

	windowTitle.setFont(textFont);
	windowTitle.setCharacterSize(25);
	windowTitle.setStyle(sf::Text::Bold);
	windowTitle.setFillColor(sf::Color::Black);
	windowTitle.setString("Beat Me if You CAN");
	windowTitle.setPosition(580, 25);

	float XstartB = 600, YstartB = 150;
	// Button Configuration
	sf::Texture buttonTexture;
	if (!buttonTexture.loadFromFile("images//buttonReleasedSmall.png"))
	{
		// error...
	}
	buttonTexture.setSmooth(true);
	sf::Sprite buttonSprite;
	buttonSprite.setTexture(buttonTexture);
	buttonSprite.setOrigin(sf::Vector2f(0.f, 0.f));
	buttonSprite.setPosition(XstartB, YstartB);

	// Button Hover Configuration
	sf::Texture buttonHoverTexture;
	if (!buttonHoverTexture.loadFromFile("images//buttonHoverSmall.png"))
	{
		// error...
	}
	buttonHoverTexture.setSmooth(true);
	sf::Sprite buttonHoverSprite;
	buttonHoverSprite.setTexture(buttonHoverTexture);
	buttonHoverSprite.setOrigin(sf::Vector2f(0.f, 0.f));
	buttonHoverSprite.setPosition(XstartB, YstartB);

	while (welcomeWindow.isOpen()) {

		sf::Event event;
		buttonHoverSprite.setColor(sf::Color(255, 255, 255));
		backButtonHoverSprite.setColor(sf::Color(255, 255, 255));
		backButtonSprite.setPosition(XstartBS, YstartBS);
		backButtonSprite.setPosition(XstartBS - 225, YstartBS);
		welcomeWindow.clear();
		welcomeWindow.draw(backgroundWelcomeSprite);
		welcomeWindow.draw(windowTitle);

		buttonSprite.setPosition(XstartB, YstartB);
		welcomeWindow.draw(buttonSprite);

		buttonSprite.setPosition(XstartB, (YstartB + 100));
		welcomeWindow.draw(buttonSprite);

		buttonSprite.setPosition(XstartB, (YstartB + 200));
		welcomeWindow.draw(buttonSprite);

		isClicked = 0;
		while (welcomeWindow.pollEvent(event)) {
			if (event.type == sf::Event::Closed) {
				welcomeWindow.close();
			}
			if (event.type == sf::Event::MouseButtonPressed) {
				if (event.mouseButton.button == sf::Mouse::Left) {
					isClicked = 1;
				}
			}
			if (event.type == sf::Event::TextEntered)
			{
				if (event.text.unicode == 27)
					welcomeWindow.close();
				//		cout << event.text.unicode << "\n";
			}

		}
		welcomeWindow.draw(backButtonSprite);
		mousePos = sf::Mouse::getPosition(welcomeWindow);
		sf::Vector2f mousePosF(static_cast<float>(mousePos.x), static_cast<float>(mousePos.y));
		//	cout << mousePos.x << ' ' << mousePos.y << "\n";
		if (mousePos.y >= YstartBS &&
			mousePos.y < (backButtonSprite.getLocalBounds().height + YstartBS) &&
			mousePos.x >= XstartBS - 225 &&
			mousePos.x < (backButtonSprite.getLocalBounds().width + XstartBS - 225)) {
			if (isClicked) {
				backButtonHoverSprite.setColor(sf::Color(255, 255, 255, 128));
				isClicked = 0;
				isBack = 1;
			}
			else {
				backButtonHoverSprite.setPosition(XstartBS - 225, YstartBS);
				welcomeWindow.draw(backButtonHoverSprite);
			}
		}
		else if (mousePos.y >= YstartB &&
			mousePos.y < (buttonSprite.getLocalBounds().height + YstartB) &&
			mousePos.x >= XstartB &&
			mousePos.x < (buttonSprite.getLocalBounds().width + XstartB)) {
			if (isClicked) {
				buttonHoverSprite.setColor(sf::Color(255, 255, 255, 128));
				isClicked = 0;
				lvl = 3;
				playOffline(welcomeWindow, gameBoard, ROWS, COLS, player, posX, posY, 1, lvl);
			}
			else {
				buttonHoverSprite.setPosition(XstartB, YstartB);
				welcomeWindow.draw(buttonHoverSprite);
			}
		}
		else if (mousePos.y >= YstartB + 100 &&
			mousePos.y < (buttonSprite.getLocalBounds().height + YstartB + 100) &&
			mousePos.x >= XstartB &&
			mousePos.x < (buttonSprite.getLocalBounds().width + XstartB)) {
			if (isClicked) {
				buttonHoverSprite.setColor(sf::Color(255, 255, 255, 128));
				isClicked = 0;
				lvl = 5;
				playOffline(welcomeWindow, gameBoard, ROWS, COLS, player, posX, posY, 1, lvl);
			}
			else {
				buttonHoverSprite.setPosition(XstartB, YstartB + 100);
				welcomeWindow.draw(buttonHoverSprite);
			}
		}
		else if (mousePos.y >= YstartB + 200 &&
			mousePos.y < (buttonSprite.getLocalBounds().height + YstartB + 200) &&
			mousePos.x >= XstartB &&
			mousePos.x < (buttonSprite.getLocalBounds().width + XstartB)) {
			if (isClicked) {
				buttonHoverSprite.setColor(sf::Color(255, 255, 255, 128));
				isClicked = 0;
				lvl = 7;
				playOffline(welcomeWindow, gameBoard, ROWS, COLS, player, posX, posY, 1, lvl);
			}
			else {
				buttonHoverSprite.setPosition(XstartB, YstartB + 200);
				welcomeWindow.draw(buttonHoverSprite);
			}
		}

		backButtonTitle.setPosition(XstartBS - 225 + 20, YstartBS + 4);

		buttonTitle.setString("   Easy Mode");
		buttonTitle.setPosition(XstartB + 20, YstartB + 10);
		welcomeWindow.draw(buttonTitle);


		buttonTitle.setString("Medium Mode");
		buttonTitle.setPosition(XstartB + 45, (YstartB + 100 + 10));
		welcomeWindow.draw(buttonTitle);

		buttonTitle.setString(" Hard Mode");
		buttonTitle.setPosition(XstartB + 44, (YstartB + 200 + 10));
		welcomeWindow.draw(buttonTitle);

		welcomeWindow.draw(buttonTitle);
		welcomeWindow.draw(backButtonTitle);
		welcomeWindow.display();

		if (isBack) {
			return 0;
		}
	}
	return 0;
}

std::string subSTR(std::string str, int start, int end) {
	std::string subSTR;
	for (int i = start; i <= end; i++) {
		subSTR += str[i];
	}
	return subSTR;
}

int scoreBoard(int** gameBoard, int ROWS, int COLS, Player player[], int* posX, std::string& posY, Move move) {
	
	int minRow = -1, minCol = -1;
	int maxRow = -1, maxCol = -1;
	int piece = player[move.playedBy].gameBoardID;
	
	Window window;
	int score = 0;
	

	// Horizontal Check
	minRow = move.row;
	maxRow = move.row + 1;

	if (move.col + 3 >= COLS)
		maxCol = COLS;
	else
		maxCol = move.col + 4;
	if (move.col - 3 < 0)
		minCol = 0;
	else
		minCol = move.col - 3;

	for (int i = minRow; i < maxRow; i++) {
		for (int j = minCol; j < maxCol - 3; j++) {
			window.piece.row = i;
			window.piece.col = j;
			window.count(gameBoard, piece, 1);
			score += evaluateWindow(window, true);
		}
	}
	// Vertical Check
	minCol = move.col;
	maxCol = move.col + 1;

	if (move.row + 3 >= ROWS)
		maxRow = ROWS;
	else
		maxRow = move.row + 4;
	if (move.row - 3 < 0)
		minRow = 0;
	else
		minRow = move.row - 3;
	for (int j = minCol; j < maxCol; j++) {
		for (int i = minRow; i < maxRow - 3; i++) {
			window.piece.row = i;
			window.piece.col = j;
			window.count(gameBoard, piece, 2);
			score += evaluateWindow(window, true);
		}
	}
	// Diagonal Check (\)
	if (move.row + 3 >= ROWS)
		maxRow = ROWS;
	else
		maxRow = move.row + 4;
	if (move.row - 3 < 0)
		minRow = 0;
	else
		minRow = move.row - 3;

	if (move.col + 3 >= COLS)
		maxCol = COLS;
	else
		maxCol = move.col + 4;
	if (move.col - 3 < 0)
		minCol = 0;
	else
		minCol = move.col - 3;
	for (int i = maxRow - 1; i >= minRow + 3; i--) {
		for (int j = maxCol - 1; j >= minCol + 3; j--) {
			window.piece.row = i;
			window.piece.col = j;
			window.count(gameBoard, piece, 3);
			score += evaluateWindow(window, true);
		}
	}
	// Diagonal Check (/)
	if (move.row + 3 >= ROWS)
		maxRow = ROWS;
	else
		maxRow = move.row + 4;
	if (move.row - 3 < 0)
		minRow = 0;
	else
		minRow = move.row - 3;

	if (move.col + 3 >= COLS)
		maxCol = COLS;
	else
		maxCol = move.col + 4;
	if (move.col - 3 < 0)
		minCol = 0;
	else
		minCol = move.col - 3;
	for (int i = minRow; i < maxRow - 3; i++) {
		for (int j = maxCol - 1; j >= minCol + 3; j--) {
			window.piece.row = i;
			window.piece.col = j;
			window.count(gameBoard, piece, 4);
			score += evaluateWindow(window, true);
		}
	}
	return score;

}

