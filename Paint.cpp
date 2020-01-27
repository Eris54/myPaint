#include "Paint.h"

//should've done it in a stack to avoid function over-looping
void Paint::menuState() {
	system("cls");

	if (wrongFile) {
		wrongFile = false;
		cout << "File header not recognized! Try a different one." << endl << endl;
	} else if (saved) {
		saved = false;
		cout << "Saved!" << endl << endl;
	} else if (againMenu) {
		againMenu = false;
		cout << "Try again!" << endl << endl;
	} else if (welcome) {
		welcome = false;
		cout << "Hi! Welcome to my homemade Paint!" << endl;
	}

	cout
		<< "0. Reset" << endl
		<< "1. Draw" << endl
		<< "2. Load" << endl
		<< "3. Save" << endl
		<< "4. Help" << endl
		<< "5. Quit" << endl << endl << ">> ";

	cin >> choice; cout << endl;

	if (cin.fail()) {	//clear if wrong input
		cin.clear();
		cin.ignore(1000000000, '\n');
		againMenu = true;
		choice = -1;
		menuState();
	} else if (choice > 5 || choice < 0) {
		choice = -1;
		againMenu = true;
		menuState();
	} else if (choice == 0) {

		if (consoleCursor.bVisible) { //the console has been resized! -.-
			whyUGottaResize();
		}

		system("cls");

		if (!SetConsoleMode(hIn, ENABLE_EXTENDED_FLAGS)) {
			cout << "Paint::menuState(): SetConsoleMode() did not work!\nError: " << GetLastError() << endl;
			system("pause");
		}

		resetDrawing();
		paintState();

	} else if (choice == 1) {

		if (consoleCursor.bVisible) { //the console has been resized! -.-
			whyUGottaResize();
		}

		system("cls");

		if (!SetConsoleMode(hIn, ENABLE_EXTENDED_FLAGS)) {
			cout << "Paint::menuState(): SetConsoleMode() did not work!\nError: " << GetLastError() << endl;
			system("pause");
		}

		save2file("paint.bmp.tmp");
		loadFromFile("paint.bmp.tmp");
		paintState();

	} else if (choice == 2) {
		cout << "Write the name of the file:\n>> ";
		cin >> path;
		system("cls");

		loadFromFile(path);

		if (consoleCursor.bVisible) { //the console has been resized! -.-
			whyUGottaResize();
		}

		if (!SetConsoleMode(hIn, ENABLE_EXTENDED_FLAGS)) {
			cout << "Paint::menuState(): SetConsoleMode() did not work!\nError: " << GetLastError() << endl;
			system("pause");
		}

		paintState();

	} else if (choice == 3) {
		cout << "Write the name of the file:\n>> ";
		cin >> path;
		save2file(path);
		saved = true;
		menuState();

	} else if (choice == 4) {
		system("cls");
		cout
			<< "Help:" << endl
			<< "The following keys work only in drawing mode," << endl
			<< "stick to the instruction in the main menu mode." << endl
			<< endl
			<< "Esc - menu" << endl
			<< "Q   - red" << endl
			<< "W   - green" << endl
			<< "E   - blue" << endl
			<< "Color key - turn on the given color" << endl
			<< "Shift + color key - turn off the color" << endl
			<< endl
			<< "    ################################" << endl
			<< "    ### DO NOT RESIZE THE WINDOW ###" << endl
			<< "    ################################" << endl << endl;

		system("pause");
		menuState();

	} else if (choice == 5) {
		quit();
	}
}

void Paint::whyUGottaResize() {
	if (!SetConsoleMode(hIn, normalMode)) {
		cout << "Paint::whyUGottaResize(): SetConsoleMode() did not work!\nError: " << GetLastError() << endl;
		system("pause");
	}

	save2file("paint.bmp.tmp");
	system("cls");
	while (Iunderstand != "understood" && Iunderstand != "\"understood\"") {
		cout << "DO NOT RESIZE THE CONSOLE!" << endl << "Write: \"understood\"" << endl;
		cin >> Iunderstand;
		system("cls");
		paintState();
	}

	Iunderstand = "I don't understand";
	system("cls");
	consoleSetup();

	if (!SetConsoleCursorInfo(hOut, &consoleCursor)) {
		cout << "Paint::menuState(): SetConsoleCursorInfo() did not work!\nError: " << GetLastError() << endl;
		system("pause");
	}

	menuState();
}

void Paint::consoleSetup() {
	SetConsoleTitle("Homemade paint!");
	//getting screen and display resolution;
	RECT resolution;	//for the scaled resolution of the display
	DEVMODE x;			//for resolution of the display
	float scaledR, absR;

	GetWindowRect(GetDesktopWindow(), &resolution);
	scaledR = (float)resolution.right;

	EnumDisplaySettings(NULL, ENUM_CURRENT_SETTINGS, &x);
	absR = (float)x.dmPelsWidth;
	screenScale = absR / scaledR;

	//setting window height and stuff
	wndRect.left = 100;
	wndRect.top = 100;
	wndRect.right = 512 + 8;
	wndRect.bottom = 512 + 32;
	MoveWindow(hwnd, wndRect.left, wndRect.top, wndRect.right, wndRect.bottom, true);

	if (!GetWindowRect(hwnd, &wndRect)) {
		cout << "Paint::consoleSetup(): GetWindowRect() did not work!\nError: " << GetLastError() << endl;
		system("pause");
	}

	//disabling blinking cursor
	consoleCursor.dwSize = 20;
	consoleCursor.bVisible = 0;
	if (!SetConsoleCursorInfo(hOut, &consoleCursor)) {
		cout << "Paint::consoleSetup(): SetConsoleCursorInfo() did not work!\nError: " << GetLastError() << endl;
		system("pause");
	}


	//setting buffer size (disable vertical bar)
	COORD newBufferSize;

	GetConsoleScreenBufferInfo(hOut, &bufferInfo);
	newBufferSize.X = bufferInfo.dwSize.X;
	newBufferSize.Y = 0;

	while (!SetConsoleScreenBufferSize(hOut, newBufferSize)) {
		newBufferSize.Y += 1;
		if (newBufferSize.Y > 500) {
			cout
				<< "Paint::consoleSetup(): SetConsoleScreenBufferSize() did not work!\nError: " << GetLastError() << endl
				<< "The scroll bar will be visible!" << endl;
			system("pause");
		}
	}
	
}

void Paint::paintState() {
	while (!(GetAsyncKeyState(VK_ESCAPE) & 0x8000)) { //while esc isn't pressed
		update();
		//might've done a loop that would've timed the input, but whatever
		updateColor();
	}

	if (!SetConsoleMode(hIn, normalMode)) {
		cout << "Paint::paintState(): SetConsoleMode() did not work!\nError: " << GetLastError() << endl;
		system("pause");
	}

	menuState();
}

void Paint::update() {
	GetCursorPos(&mouse);
	GetWindowRect(hwnd, &wndRect);
	GetConsoleCursorInfo(hOut, &consoleCursor);
	if (consoleCursor.bVisible) {
		whyUGottaResize();
	}

	mouse.x = (mouse.x - wndRect.left - 8);
	mouse.y = (mouse.y - wndRect.top - 32);
	//mouse cursor size is 8x32
	

	if (keyPressed(VK_LBUTTON)) {	//if left button pressed
		if (mouse.x < 512 &&		//if mouse in the window
			mouse.x > 0 &&
			mouse.y > 0 &&
			mouse.y < 512) {
			drawPixel(mouse.x, mouse.y);
		}
	}
}

void Paint::updateColor() {
	//Red
	if (GetAsyncKeyState(0x51) & 0x8000) {			//Q
		if (GetAsyncKeyState(VK_SHIFT) & 0x8000) {	//shift
			r = 0;
		} else {
			r = 255;
		}
	}

	//Green
	if (GetAsyncKeyState(0x57) & 0x8000) {				//W
		if (GetAsyncKeyState(VK_SHIFT) & 0x8000) {		//shift
			g = 0;

		} else {
			g = 255;
		}
	}

	//Blue
	if (GetAsyncKeyState(0x45) & 0x8000) {				//E
		if (GetAsyncKeyState(VK_SHIFT) & 0x8000) {		//shift
			b = 0;
		} else {
			b = 255;
		}
	}

	//updating color value
	color = RGB(r, g, b);
}

void Paint::drawPixel(const long& x, const long& y) {
	SetPixel(device, x * screenScale, y * screenScale, color);
	pixArray[mouse.x][mouse.y][0] = b;
	pixArray[mouse.x][mouse.y][1] = g;
	pixArray[mouse.x][mouse.y][2] = r;
}

void Paint::save2file(string path) {
	ofstream bmp(path, ios::binary | ios::out);
	bmp.write(header, 0x36);

	for (int y = 512; y > 0; y--) {
		for (int x = 0; x < 512; x++) {
			bmp.write(pixArray[x][y], 3);
		}
	}

	bmp.close();
}

void Paint::loadFromFile(string path) {
	ifstream bmp(path);
	unsigned char ch;
	int counter{};
	char check;

	//skipping header
	while (counter < 0x36) {
		bmp >> noskipws >> check;
		if (header[counter] != check) {
			wrongFile = true;
		}
		counter++; 
	}

	if (wrongFile) {
		menuState();
	}

	for (int y = 512; y > 0; y--) {
		for (int x = 0; x < 512; x++) {
			for (char bit = 0; bit < 3; bit++) {
				bmp >> noskipws >> ch;
				pixArray[x][y][bit] = ch;
			}
		}
	}


	for (int y{}; y < 512; y++) {
		for (int x{}; x < 512; x++) {
			SetPixel(device, x * screenScale, y * screenScale, RGB(pixArray[x][y][2], pixArray[x][y][1], pixArray[x][y][0]));
		}
	}
}

void Paint::resetDrawing() {
	for (short i{}; i < 512; i++) {
		for (short j{}; j < 512; j++) {
			pixArray[i][j][0] = 0;
			pixArray[i][j][1] = 0;
			pixArray[i][j][2] = 0;
		}
	}
	save2file("paint.bmp.tmp");
	loadFromFile("paint.bmp.tmp");
}

bool Paint::keyPressed(int key) {
	if ((GetAsyncKeyState(key) & 0x8000)) {
		return true;
	}
	return false;
}

void Paint::quit() {
	remove("paint.bmp.tmp");
	exit(0);
}


Paint::Paint() {
	hwnd = GetConsoleWindow();				//console window handle
	device = GetDC(hwnd);					//device
	hOut = GetStdHandle(STD_OUTPUT_HANDLE);	//output handle
	hIn = GetStdHandle(STD_INPUT_HANDLE);	//input handle

	//setting up blinking cursor
	consoleCursor.dwSize = 20;				//blinking cursor size
	consoleCursor.bVisible = false;			//blinking cursor invisible

	//setting up header
	header = new  char[0x36] {
		0x42, 0x4d,				//file magic number
		0x36, 0x00, 0x0c, 0x00,	//file size (with header)				#!#
		0x00, 0x00, 0x00, 0x00,	//firmware stuff
		0x36, 0x00, 0x00, 0x00, //pixel array position - after header
		0x28, 0x00, 0x00, 0x00,	//dpi header length
		0x00, 0x02, 0x00, 0x00,	//width px		- 512					#!#
		0x00, 0x02, 0x00, 0x00,	//height px		- 512					#!#
		0x01, 0x00,				//color plane - 1
		0x18, 0x00,				//bits per pixel - 24
		0x00, 0x00, 0x00, 0x00, //compression - none
		0x00, 0x00, 0x0c, 0x00, //img size in bytes (no header)			#!#
		0x00, 0x00, 0x00, 0x00, //vertical resolution	- default
		0x00, 0x00, 0x00, 0x00, //horizontal resolution - default
		0x00, 0x00, 0x00, 0x00, //color in plane
		0x00, 0x00, 0x00, 0x00 //impClr
		//pixel array
	};

	if (!GetConsoleMode(hIn, &normalMode)) {
		cout << "Paint::Paint(): GetConsoleMode() did not work!\nError: " << GetLastError() << endl;
		system("pause");
	}

	choice = -1;
	againMenu = 0;
	saved = 0;
	welcome = 1;
	Iunderstand = "I do not understand";

	r = 255;
	g = 255;
	b = 255;

	color = RGB(r, g, b);

	consoleSetup();
	menuState();
}

Paint::~Paint() {
}
