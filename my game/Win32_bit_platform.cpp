#include <windows.h>
#include <stdio.h>      /* printf, scanf, puts, NULL */
#include <stdlib.h>     /* srand, rand */
#include <time.h>  
#include <chrono>
#include "BMP.h"
#include <thread>
#include <conio.h>
#include <math.h>

using namespace std;

int ischanged[200];
int ispressed[200];
int pucaj = 0;

bool running = true;
void* buffer_memory;
int buffer_width;
int buffer_heigth;
BITMAPINFO buffer_bitmap_info;

int f(int x, int y, BMP* a, int color)
{
	int c = 0;
	if (x >= 1600 || y >= 900)
		c++;

	return a->data[3 * (y * a->bmp_info_header.width + x) + color];
}

int windowheigth = 3000;
int windowwidth = 3000;

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

float abss(float x) {
	if (x < 0) x *= -1;
	return x;
}

float distanca(float x2, float y2, float x1, float y1) {

	return sqrt((x2 - x1) * (x2 - x1) + (y1 - y2) * (y1 - y2));
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void putpixel(unsigned int* pixel, int x, int y, int red, int green, int blue)
{
	unsigned int* target = pixel + x + (buffer_heigth - 1 - y) * buffer_width;
	//unsigned int* target = pixel + x + y * buffer_width;
	if (x >= 0 && x < buffer_width && y >= 0 && y < buffer_heigth)
		*target = (red << 16) + (green << 8) + (blue);
}

void drawcircle(unsigned int* pixel, int x, int y, int r, int red, int green, int blue)
{
	int j = r;
	for (int i = 0; i < r;)
	{
		if ((i + 1) * (i + 1) + j * j <= r * r)
			i++;
		else
			j--;


		if (1)
			putpixel(pixel, x + i, y + j, red, green, blue);
		if (1)
			putpixel(pixel, x + i, y - j, red, green, blue);
		if (1)
			putpixel(pixel, x - i, y + j, red, green, blue);
		if (1)
			putpixel(pixel, x - i, y - j, red, green, blue);
	}
	putpixel(pixel, x, y + r, red, green, blue);
	putpixel(pixel, x, y - r, red, green, blue);
}

int f1(float a, float b, float c, float x) {
	return (int)(a * x * x * x * x + b * x + c);
}

void drawnesto(unsigned int* pixel, int x1, int y1, int r, int red, int green, int blue) {

	putpixel(pixel, x1 + r / 2, y1 + r / 2, red, green, blue);
	for (int i = 0; i < r; i++) {
		putpixel(pixel, x1 - r / 2 + i, y1 - r / 2, red, green, blue);
		putpixel(pixel, x1 - r / 2 + i, y1 + r / 2, red, green, blue);
		putpixel(pixel, x1 - r / 2, y1 - r / 2 + i, red, green, blue);
		putpixel(pixel, x1 + r / 2, y1 - r / 2 + i, red, green, blue);
	}

}

int abss(int x) {
	if (x < 0) return x * -1;
	return x;
}

bool secanje(float x1, float y1, int a, float x2, float y2, int r) {
	if (x2 <= x1 + r + a / 2 && x2 >= x1 - r - a / 2 && y2 <= y1 + r + a / 2 && y2 >= y1 - r - a / 2) return 1;
	return 0;
}

//////////////////////////////////////////////

BMP meni1("meni1.BMP");

int xmisa=0;
int ymisa=0;

int pucaj1[5];
int doradakretanja[5][4];

int tajmerreseta;

float xigraca[6];
float yigraca[6];

int brigraca = 6;
int ply[6] = { 1,1,1,1,1,1 };

int scores[6];

int playersleft;

float velicina = 1;

int brzina = 2 * velicina;
int orjentacija[6];

float xmetka[5][6];
float ymetka[5][6];
int brzinametka = 3 * velicina;
int orjentacijametka[5][6] = { 0 };
int used[5][6] = { 0 };
int tajmermetkova[5][6] = { 0 };
bool bounced[5][6] = { 0 };
int boja[3][6];

int ispaljeno = 0;

int mapa[50][50];
int height1 = 14;
int width1 = 30;

/////////////////////////////////////////////

bool bfscheckraod(int x1, int y1, int x2, int y2, int n, int m) {
	int used123[100][100];
	for (int i = 0; i < n; i++) {
		for (int j = 0; j < m; j++) {
			used123[i][j] = 0;
		}
	}

	vector <pair <int, int> > boom;
	boom.push_back(make_pair(x1, y1));
	for (int k = 0; k < boom.size(); k++) {
		if (boom[k].first == x2 && boom[k].second == y2) return 1;

		used123[boom[k].first][boom[k].second] = 1;
		if (boom[k].first - 1 >= 0 && boom[k].second >= 0 && boom[k].first - 1 < n && boom[k].second < m && mapa[boom[k].first - 1][boom[k].second] == 1 && used123[boom[k].first - 1][boom[k].second] == 0) boom.push_back(make_pair(boom[k].first - 1, boom[k].second));
		if (boom[k].first + 1 >= 0 && boom[k].second >= 0 && boom[k].first + 1 < n && boom[k].second < m && mapa[boom[k].first + 1][boom[k].second] == 1 && used123[boom[k].first + 1][boom[k].second] == 0) boom.push_back(make_pair(boom[k].first + 1, boom[k].second));
		if (boom[k].first >= 0 && boom[k].second - 1 >= 0 && boom[k].first < n && boom[k].second - 1 < m && mapa[boom[k].first][boom[k].second - 1] == 1 && used123[boom[k].first][boom[k].second - 1] == 0) boom.push_back(make_pair(boom[k].first, boom[k].second - 1));
		if (boom[k].first >= 0 && boom[k].second + 1 >= 0 && boom[k].first < n && boom[k].second + 1 < m && mapa[boom[k].first][boom[k].second + 1] == 1 && used123[boom[k].first][boom[k].second + 1] == 0) boom.push_back(make_pair(boom[k].first, boom[k].second + 1));
	}
	return 0;
}

void generation(int x1, int y1, int x2, int y2, int n, int m) {

	mapa[x1][y1] = 1;
	mapa[x2][y2] = 1;

	int broj = 0;
	do {
		int randx = 1 + rand() % (n - 1);
		int randy = 1 + rand() % (m - 1);
		mapa[randx][randy] = 1;
		broj++;
		if (broj % 1 == 0 && bfscheckraod(x1, y1, x2, y2, n, m)) break;
	} while (1);
}

void punjenjebfs(int x1, int y1, int n, int m) {
	int used123[50][50];

	for (int i = 0; i < n; i++) {
		for (int j = 0; j < m; j++) {
			used123[i][j] = 0;
		}
	}

	vector <pair <int, int> > boom;
	boom.push_back(make_pair(x1, y1));
	for (int k = 0; k < boom.size(); k++) {

		used123[boom[k].first][boom[k].second] = 1;
		if (boom[k].first - 1 >= 0 && boom[k].second >= 0 && boom[k].first - 1 < n && boom[k].second < m && mapa[boom[k].first - 1][boom[k].second] == 1 && used123[boom[k].first - 1][boom[k].second] == 0) boom.push_back(make_pair(boom[k].first - 1, boom[k].second));
		if (boom[k].first + 1 >= 0 && boom[k].second >= 0 && boom[k].first + 1 < n && boom[k].second < m && mapa[boom[k].first + 1][boom[k].second] == 1 && used123[boom[k].first + 1][boom[k].second] == 0) boom.push_back(make_pair(boom[k].first + 1, boom[k].second));
		if (boom[k].first >= 0 && boom[k].second - 1 >= 0 && boom[k].first < n && boom[k].second - 1 < m && mapa[boom[k].first][boom[k].second - 1] == 1 && used123[boom[k].first][boom[k].second - 1] == 0) boom.push_back(make_pair(boom[k].first, boom[k].second - 1));
		if (boom[k].first >= 0 && boom[k].second + 1 >= 0 && boom[k].first < n && boom[k].second + 1 < m && mapa[boom[k].first][boom[k].second + 1] == 1 && used123[boom[k].first][boom[k].second + 1] == 0) boom.push_back(make_pair(boom[k].first, boom[k].second + 1));
	}

	for (int i = 0; i < n; i++) {
		for (int j = 0; j < m; j++) {
			mapa[i][j] = used123[i][j] == 1;
		}
	}
}

void pravljenjemape(unsigned int* pixel) {

	for (int i = 0; i < (int)(width1 / velicina); i++) {
		for (int j = 0; j < (int)(height1 / velicina); j += 1) {
			mapa[i][j] = 0;
		}
	}
	
	int x1[6];
	int y1[6];

	for (int i = 0; i < brigraca; i++) {
		x1[i] = (xigraca[i] - 25) / (50 * velicina);
		y1[i] = (yigraca[i] - 25) / (50 * velicina);
		mapa[x1[i]][y1[i]];
	}
	for (int i = 0; i < brigraca; i++) {
		for (int j = i + 1; j < brigraca; j++) {
			generation(x1[i], y1[i], x1[j], y1[j], (width1 / velicina), (height1 / velicina));
		}
	}
	punjenjebfs(x1[0], y1[0], (width1 / velicina), (height1 / velicina));
}

///mapa/////////

void refreshgame() {

	playersleft = brigraca;
	velicina = 1 + (rand() % 200 + 1) / 100;
	int x1[6];
	int y1[6];

	tajmerreseta = 200;

	for (int i = 0; i < brigraca; i++) ply[i] = 1;

	for (int i = 0; i < brigraca; i++) {
		x1[i] = 1 + rand() % (int)(width1 / velicina - 1);
		y1[i] = 1 + rand() % (int)(height1 / velicina - 1);

		xigraca[i] = x1[i] * (int)(50 * velicina) + 25 * velicina;
		yigraca[i] = y1[i] * (int)(50 * velicina) + 25 * velicina;

		orjentacija[i] = rand() % 360;
	}
	brzina = 2 * velicina;
	brzinametka = 3 * velicina;

	for (int i = 0; i < 5; i++) {
		for (int j = 0; j < brigraca; j++) {
			orjentacijametka[i][j] = 0;
			used[i][j] = 0;
			tajmermetkova[i][j] = 0;
			xmetka[i][j] = -10;
			ymetka[i][j] = -10;
			bounced[i][j] = 0;
		}
	}
	ispaljeno = 0;

	for (int i = 0; i < 50; i++) {
		for (int j = 0; j < 50; j++) {
			mapa[i][j] = 0;
		}
	}
}

///restart/////////

bool zid(float x, float y, int ii) {
	for (int j = -10 * velicina; j <= 10 * velicina; j += 1)
		for (int i = -15 * velicina; i <= 15 * velicina; i += 1) {
			//putpixel(pixel, (int)(xigraca[0] + j * cos(((orjentacija[0] + 90) * 3.14159265359) / 180.0) + i * cos((orjentacija[0] * 3.14159265359) / 180.0) + 0.5), (int)(yigraca[0] + j * sin(((orjentacija[0] + 90) * 3.14159265359) / 180.0) + i * sin((orjentacija[0] * 3.14159265359) / 180.0) + 0.5), 255, 0, 0);
			int x1 = (int)(x + j * cos(((orjentacija[ii] + 90) * 3.14159265359) / 180.0) + i * cos((orjentacija[ii] * 3.14159265359) / 180.0) + 0.5);
			int y1 = (int)(y + j * sin(((orjentacija[ii] + 90) * 3.14159265359) / 180.0) + i * sin((orjentacija[ii] * 3.14159265359) / 180.0) + 0.5);
			if (mapa[(int)(x1 / (50 * velicina))][(int)(y1 / (50 * velicina))] == 0) return 1;
		}
	return 0;

	/*for (int j = -3 * velicina; j <= 3 * velicina; j++)
		for (int i = 0; i <= 19 * velicina; i++) {

		}
	*/
}

void funkcijamis() {
	float temp = 0;
	if (xmisa == xigraca[5]) {
		temp = 3.14159265359/2;
		if (yigraca[5] > ymisa) temp *= -1;
	}
	else temp = atan(((float)ymisa - yigraca[5]) / ((float)xmisa - xigraca[5]));
	if (xmisa < xigraca[5]) temp = (3.14159265359 - temp)*-1;

	if (sin(temp) * cos((orjentacija[5] * 3.14159265359) / 180.0) - cos(temp) * sin((orjentacija[5] * 3.14159265359) / 180.0) < 0) orjentacija[5] -= 3;
	else orjentacija[5] += 3;

}

void input() {

	float xigraca1[6] = { xigraca[0],xigraca[1],xigraca[2],xigraca[3],xigraca[4],xigraca[5] };
	float yigraca1[6] = { yigraca[0],yigraca[1],yigraca[2],yigraca[3],yigraca[4],yigraca[5] };

	if (ply[0] == 1) {
		if (ispressed['W']) { xigraca[0] += brzina * cos((orjentacija[0] * 3.14159265359) / 180.0); yigraca[0] += brzina * sin((orjentacija[0] * 3.14159265359) / 180.0); }
		if (ispressed['S']) { xigraca[0] -= brzina * cos(((orjentacija[0]) * 3.14159265359) / 180.0); yigraca[0] -= brzina * sin(((orjentacija[0]) * 3.14159265359) / 180.0); }
		if (ispressed['D']){
			if(ispressed['S']) orjentacija[0] -= 2;
			else orjentacija[0] += 2;

		}
		if (ispressed['A']) {
			if (ispressed['S']) orjentacija[0] += 2;
			else orjentacija[0] -= 2;

		}

	}
	
	if (ply[1] == 1) {
		if (ispressed['I']) { xigraca[1] += brzina * cos((orjentacija[1] * 3.14159265359) / 180.0); yigraca[1] += brzina * sin((orjentacija[1] * 3.14159265359) / 180.0); }
		if (ispressed['K']) { xigraca[1] -= brzina * cos((orjentacija[1] * 3.14159265359) / 180.0); yigraca[1] -= brzina * sin((orjentacija[1] * 3.14159265359) / 180.0); }
		if (ispressed['L']) {
			if (ispressed['K']) orjentacija[1] -= 2;
			else orjentacija[1] += 2;

		}
		if (ispressed['J']) {
			if (ispressed['K']) orjentacija[1] += 2;
			else orjentacija[1] -= 2;

		}
	}
	
	if (ply[2] == 1) {
		if (ispressed['T']) { xigraca[2] += brzina * cos((orjentacija[2] * 3.14159265359) / 180.0); yigraca[2] += brzina * sin((orjentacija[2] * 3.14159265359) / 180.0); }
		if (ispressed['G']) { xigraca[2] -= brzina * cos((orjentacija[2] * 3.14159265359) / 180.0); yigraca[2] -= brzina * sin((orjentacija[2] * 3.14159265359) / 180.0); }
		if (ispressed['H']) {
			if (ispressed['G']) orjentacija[2] -= 2;
			else orjentacija[2] += 2;

		}
		if (ispressed['F']) {
			if (ispressed['G']) orjentacija[2] += 2;
			else orjentacija[2] -= 2;

		}
	}
	
	if (ply[3] == 1) {
		if (ispressed[VK_NUMPAD5]) { xigraca[3] += brzina * cos((orjentacija[3] * 3.14159265359) / 180.0); yigraca[3] += brzina * sin((orjentacija[3] * 3.14159265359) / 180.0); }
		if (ispressed[VK_NUMPAD2]) { xigraca[3] -= brzina * cos((orjentacija[3] * 3.14159265359) / 180.0); yigraca[3] -= brzina * sin((orjentacija[3] * 3.14159265359) / 180.0); }
		if (ispressed[VK_NUMPAD3]) {
			if (ispressed[VK_NUMPAD2]) orjentacija[3] -= 2;
			else orjentacija[3] += 2;

		}
		if (ispressed[VK_NUMPAD1]) {
			if (ispressed[VK_NUMPAD2]) orjentacija[3] += 2;
			else orjentacija[3] -= 2;

		}
	}
	
	if (ply[4] == 1) {
		if (ispressed[VK_UP]) { xigraca[4] += brzina * cos((orjentacija[4] * 3.14159265359) / 180.0); yigraca[4] += brzina * sin((orjentacija[4] * 3.14159265359) / 180.0); }
		if (ispressed[VK_DOWN]) { xigraca[4] -= brzina * cos((orjentacija[4] * 3.14159265359) / 180.0); yigraca[4] -= brzina * sin((orjentacija[4] * 3.14159265359) / 180.0); }
		if (ispressed[VK_RIGHT]) {
			if (ispressed[VK_DOWN]) orjentacija[4] -= 2;
			else orjentacija[4] += 2;

		}
		if (ispressed[VK_LEFT]) {
			if (ispressed[VK_DOWN]) orjentacija[4] += 2;
			else orjentacija[4] -= 2;

		}
	}
	
	if (ply[5] == 1) {

		funkcijamis();
		if (ispressed[VK_RBUTTON]) { xigraca[5] += brzina * cos((orjentacija[5] * 3.14159265359) / 180.0); yigraca[5] += brzina * sin((orjentacija[5] * 3.14159265359) / 180.0); }
	}

	for (int i = 0; i < brigraca; i++) {
		if (ply[i] == 0) continue;
		xigraca[i] = max(0, xigraca[i]);
		yigraca[i] = max(0, yigraca[i]);
		xigraca[i] = min(buffer_width, xigraca[i]);
		yigraca[i] = min(buffer_heigth, yigraca[i]);

		if (zid(xigraca[i], yigraca[i], i)) {
			//boja[1][i] = 255;
			if (zid(xigraca1[i], yigraca[i], i)) {
				if (zid(xigraca[i], yigraca1[i], i)) {
					yigraca[i] = yigraca1[i];
					xigraca[i] = xigraca1[i];
				}
				else {
					yigraca[i] = yigraca1[i];
				}
			}
			else {
				xigraca[i] = xigraca1[i];
			}
		}
		else {
			//boja[1][i] = 0;
		}

		if (xigraca[i] == xigraca1[i] && yigraca[i] == yigraca1[i] && zid(xigraca[i], yigraca[i], i)) {
			for (float strana = 0; strana <= 5; strana += 0.1) {
				if (zid(xigraca[i] + strana, yigraca[i], i) == 0) { xigraca[i] += strana; break; }
				if (zid(xigraca[i] - strana, yigraca[i], i) == 0) { xigraca[i] -= strana; break; }
				if (zid(xigraca[i], yigraca[i] + strana, i) == 0) { yigraca[i] += strana; break; }
				if (zid(xigraca[i], yigraca[i] - strana, i) == 0) { yigraca[i] -= strana; break; }

				if (zid(xigraca[i] + strana, yigraca[i] + strana, i) == 0) { xigraca[i] += strana; yigraca[i] += strana; break; }
				if (zid(xigraca[i] - strana, yigraca[i] - strana, i) == 0) { xigraca[i] -= strana; yigraca[i] -= strana; break; }
				if (zid(xigraca[i] - strana, yigraca[i] + strana, i) == 0) { xigraca[i] -= strana; yigraca[i] += strana; break; }
				if (zid(xigraca[i] + strana, yigraca[i] - strana, i) == 0) { xigraca[i] += strana; yigraca[i] -= strana; break; }
			}
		}
		if (orjentacija[i] >= 360) orjentacija[i] -= 360;
		if (orjentacija[i] < 0) orjentacija[i] += 360;
	}
 
	if (ply[0] == 1)	if (pucaj1[0]) {
			for (int i = 0; i < 5; i++) {
				if (used[i][0] == 1) continue;
				used[i][0] = 1;

				xmetka[i][0] = xigraca[0] + 19 * velicina * cos((orjentacija[0] * 3.14159265359) / 180.0);
				ymetka[i][0] = yigraca[0] + 19 * velicina * sin((orjentacija[0] * 3.14159265359) / 180.0);

				bounced[i][0] = 0;

				int xmape = (int)(xmetka[i][0] / 50);
				int ymape = (int)(ymetka[i][0] / 50);

				tajmermetkova[i][0] = 500;

				orjentacijametka[i][0] = orjentacija[0];

				ispaljeno++;
				break;
			}
			pucaj1[0] = 0;
		}

	if (ply[1] == 1)	if (pucaj1[1]) {
			for (int i = 0; i < 5; i++) {
				if (used[i][1] == 1) continue;
				used[i][1] = 1;

				xmetka[i][1] = xigraca[1] + 19 * velicina * cos((orjentacija[1] * 3.14159265359) / 180.0);
				ymetka[i][1] = yigraca[1] + 19 * velicina * sin((orjentacija[1] * 3.14159265359) / 180.0);

				bounced[i][1] = 0;

				int xmape = (int)(xmetka[i][1] / 50);
				int ymape = (int)(ymetka[i][1] / 50);

				tajmermetkova[i][1] = 500;

				orjentacijametka[i][1] = orjentacija[1];

				ispaljeno++;
				break;
			}
			pucaj1[1] = 0;
		}

	if (ply[2] == 1)	if (pucaj1[2]) {
			for (int i = 0; i < 5; i++) {
				if (used[i][2] == 1) continue;
				used[i][2] = 1;
				xmetka[i][2] = xigraca[2] + 19 * velicina * cos((orjentacija[2] * 3.14159265359) / 180.0);
				ymetka[i][2] = yigraca[2] + 19 * velicina * sin((orjentacija[2] * 3.14159265359) / 180.0);

				bounced[i][2] = 0;

				int xmape = (int)(xmetka[i][2] / 50);
				int ymape = (int)(ymetka[i][2] / 50);

				tajmermetkova[i][2] = 500;

				orjentacijametka[i][2] = orjentacija[2];

				ispaljeno++;
				break;
			}
			pucaj1[2] = 0;
		}

	if (ply[3] == 1)	if (pucaj1[3]) {
			for (int i = 0; i < 5; i++) {
				if (used[i][3] == 1) continue;
				used[i][3] = 1;

				xmetka[i][3] = xigraca[3] + 19 * velicina * cos((orjentacija[3] * 3.14159265359) / 180.0);
				ymetka[i][3] = yigraca[3] + 19 * velicina * sin((orjentacija[3] * 3.14159265359) / 180.0);

				bounced[i][3] = 0;

				int xmape = (int)(xmetka[i][3] / 50);
				int ymape = (int)(ymetka[i][3] / 50);

				tajmermetkova[i][3] = 500;

				orjentacijametka[i][3] = orjentacija[3];

				ispaljeno++;
				break;
			}
			pucaj1[3] = 0;
		}

	if (ply[4] == 1)	if (pucaj1[4]) {
			for (int i = 0; i < 5; i++) {
				if (used[i][4] == 1) continue;
				used[i][4] = 1;

				xmetka[i][4] = xigraca[4] + 19 * velicina * cos((orjentacija[4] * 3.14159265359) / 180.0);
				ymetka[i][4] = yigraca[4] + 19 * velicina * sin((orjentacija[4] * 3.14159265359) / 180.0);

				bounced[i][4] = 0;

				int xmape = (int)(xmetka[i][4] / 50);
				int ymape = (int)(ymetka[i][4] / 50);

				tajmermetkova[i][4] = 500;

				orjentacijametka[i][4] = orjentacija[4];

				ispaljeno++;
				break;
			}
			pucaj1[4] = 0;
		}

	if (ply[5] == 1)	if (ispressed[VK_LBUTTON] && ischanged[VK_LBUTTON]) {
			for (int i = 0; i < 5; i++) {
				if (used[i][5] == 1) continue;
				used[i][5] = 1;

				xmetka[i][5] = xigraca[5] + 19 * velicina * cos((orjentacija[5] * 3.14159265359) / 180.0);
				ymetka[i][5] = yigraca[5] + 19 * velicina * sin((orjentacija[5] * 3.14159265359) / 180.0);

				bounced[i][5] = 0;

				int xmape = (int)(xmetka[i][5] / 50);
				int ymape = (int)(ymetka[i][5] / 50);

				tajmermetkova[i][5] = 500;

				orjentacijametka[i][5] = orjentacija[5];

				ispaljeno++;
				pucaj = 0;
				break;
			}
		}					
}

void letmetkova() {
	for (int i = 0; i < 5; i++) {
		for (int i5 = 0; i5 < brigraca; i5++) {
			//if (ply[i5] == 0) continue;
			if (used[i][i5] == 0)continue;

			xmetka[i][i5] += brzinametka * cos((orjentacijametka[i][i5] * 3.14159265359) / 180.0); ymetka[i][i5] += brzinametka * sin((orjentacijametka[i][i5] * 3.14159265359) / 180.0);

			tajmermetkova[i][i5]--;

			int xmape = (int)(xmetka[i][i5] / (50 * velicina));
			int ymape = (int)(ymetka[i][i5] / (50 * velicina));

			if (mapa[xmape][ymape] == 0 && secanje(xmape * 50 * velicina + 25 * velicina, ymape * 50 * velicina + 25 * velicina, 50 * velicina, (int)xmetka[i][i5], (int)ymetka[i][i5], 6 * velicina)) { orjentacijametka[i][i5] *= -1; orjentacijametka[i][i5] = 180 - orjentacijametka[i][i5]; bounced[i][i5] = 1; }

			if (mapa[xmape + 1][ymape] == 0 && (orjentacijametka[i][i5] < 90 || orjentacijametka[i][i5] > 270) && secanje(xmape * 50 * velicina + 75 * velicina, ymape * 50 * velicina + 25 * velicina, 50 * velicina, (int)xmetka[i][i5], (int)ymetka[i][i5], 6 * velicina)) { orjentacijametka[i][i5] = 180 - orjentacijametka[i][i5]; bounced[i][i5] = 1; }
			if (mapa[xmape - 1][ymape] == 0 && (orjentacijametka[i][i5] > 90 && orjentacijametka[i][i5] < 270) && secanje(xmape * 50 * velicina - 25 * velicina, ymape * 50 * velicina + 25 * velicina, 50 * velicina, (int)xmetka[i][i5], (int)ymetka[i][i5], 6 * velicina)) { orjentacijametka[i][i5] = 180 - orjentacijametka[i][i5]; bounced[i][i5] = 1; }
			if (mapa[xmape][ymape + 1] == 0 && (orjentacijametka[i][i5] < 180) && secanje(xmape * 50 * velicina + 25 * velicina, ymape * 50 * velicina + 75 * velicina, 50 * velicina, (int)xmetka[i][i5], (int)ymetka[i][i5], 6 * velicina)) { orjentacijametka[i][i5] *= -1; bounced[i][i5] = 1; }
			if (mapa[xmape][ymape - 1] == 0 && (orjentacijametka[i][i5] > 180) && secanje(xmape * 50 * velicina + 25 * velicina, ymape * 50 * velicina - 25 * velicina, 50 * velicina, (int)xmetka[i][i5], (int)ymetka[i][i5], 6 * velicina)) { orjentacijametka[i][i5] *= -1; bounced[i][i5] = 1; }

			if (xmetka[i][i5] <= 6 * velicina && (orjentacijametka[i][i5] < 90 || orjentacijametka[i][i5] > 270)) { orjentacijametka[i][i5] = 180 - orjentacijametka[i][i5]; bounced[i][i5] = 1; }
			if (ymetka[i][i5] <= 6 * velicina && orjentacijametka[i][i5] > 180) { orjentacijametka[i][i5] *= -1; bounced[i][i5] = 1; }

			if (orjentacijametka[i][i5] >= 360) orjentacijametka[i][i5] -= 360;
			if (orjentacijametka[i][i5] < 0) orjentacijametka[i][i5] += 360;

			if (tajmermetkova[i][i5] == 0 || xmetka[i][i5] < 0 || xmetka[i][i5] >= buffer_width || ymetka[i][i5] < 0 || ymetka[i][i5] >= buffer_heigth) {
				used[i][i5] = 0;
				ispaljeno--;
			}
		}
	}
}

int sudar() {
	for (int i = 0; i < brigraca; i++) {
		if (ply[i] == 0) continue;
		for (int k = 0; k < brigraca; k++) {
			//if (k == i) continue;
			for (int j = 0; j < 5; j++) {
				if (used[j][k] == 1) {
					if (i == k && bounced[j][k] == 0) continue;
					//if (secanje(xigraca[i], yigraca[i], 20, xmetka[j][k], ymetka[j][k], 6)) return i+1;
					for (int jj = -10 * velicina; jj <= 10 * velicina; jj++)
						for (int ii = -15 * velicina; ii <= 15 * velicina; ii++) {
							float x11 = xigraca[i] + jj * cos(((orjentacija[i] + 90) * 3.14159265359) / 180.0) + ii * cos((orjentacija[i] * 3.14159265359) / 180.0);
							float y11 = yigraca[i] + jj * sin(((orjentacija[i] + 90) * 3.14159265359) / 180.0) + ii * sin((orjentacija[i] * 3.14159265359) / 180.0);
							if ((x11 - xmetka[j][k]) * (x11 - xmetka[j][k]) + (y11 - ymetka[j][k]) * (y11 - ymetka[j][k]) <= 36 * velicina * velicina) {
								ply[i] = 0;
								used[j][k] = 0;
								return i + 1;
							}
						}
				}
			}
		}
	}
	return 0;
}

///game/////

void resolution() {
	
	height1 = buffer_heigth / 50 -3 ;
	width1 = buffer_width / 50 - 1;
}

void drawbackground(unsigned int* pixel, unsigned int* pixel1, float velicina1) {
	for (int i = 0; i <= (int)(width1 / velicina); i++) {
		for (int j = 0; j <= (int)(height1 / velicina); j += 1) {
			for (int kk = 1; kk <= 50 * velicina1; kk++)
				drawnesto(pixel, i * 50 * velicina1 + 25 * velicina1, j * 50 * velicina1 + 25 * velicina1, kk, 200, 200, 200);
		}
	}

	for (int i = 0; i <= (int)(width1 / velicina); i++) {
		for (int j = 0; j <= (int)(height1 / velicina); j += 1) {
			//levo
			if (i && (mapa[i - 1][j] == 0 && mapa[i][j] == 1)) {
				for (int kk = 0; kk <= 50 * velicina1; kk++) {
					for (int jj = 0; jj <= 5 * velicina1; jj++)putpixel(pixel, (i - 1) * 50 * velicina1 + 50 * velicina1 - jj, j * velicina1 * 50 + kk, 0, 0, 0);
				}
			}
			//desno
			if (i < (int)(width1 / velicina) && (mapa[i + 1][j] == 0 && mapa[i][j] == 1)) {
				for (int kk = 0; kk <= 50 * velicina1; kk++) {
					for (int jj = 0; jj <= 5 * velicina1; jj++)putpixel(pixel, (i + 1) * 50 * velicina1 + jj, j * velicina1 * 50 + kk, 0, 0, 0);
				}
			}
			//gore
			if (j && (mapa[i][j - 1] == 0 && mapa[i][j] == 1)) {
				for (int kk = 0; kk <= 50 * velicina1; kk++) {
					for (int jj = 0; jj <= 5 * velicina1; jj++)putpixel(pixel, (i-1) * 50 * velicina1 + 50 * velicina1 + kk, (j) * velicina1 * 50 - jj, 0, 0, 0);
				}
			}
			//dole
			if (j < (int)(height1 / velicina) && (mapa[i][j + 1] == 0 && mapa[i][j] == 1)) {
				for (int kk = 0; kk <= 50 * velicina1; kk++) {
					for (int jj = 0; jj <= 5 * velicina1; jj++)putpixel(pixel, (i-1) * 50 * velicina1 + 50 * velicina1 + kk, (j + 1) * velicina1 * 50 + jj, 0, 0, 0);
				}
			}
		}
	}
	


	// background
}

void drawing(unsigned int* pixel, unsigned int* pixel1, float velicina1) {

	for (int igrac = 0; igrac < brigraca; igrac++) {

		for (int i = 0; i < 5; i++) if (used[i][igrac] == 1) for (int kk = 1; kk <= 6 * velicina1; kk++)drawcircle(pixel1, (int)xmetka[i][igrac], (int)ymetka[i][igrac], kk, 0, 0, 0);
		//bullets

		if (ply[igrac] == 0) continue;

		for (float j = -10 * velicina1; j <= 10 * velicina1; j += 0.5)
			for (float i = -15 * velicina1; i <= 15 * velicina1; i += 0.5) {
				if (j <= -10 * velicina1 + velicina1 || j >= 10 * velicina1 - velicina1 || i <= -15 * velicina1 + velicina1 || i >= 15 * velicina1 - velicina1) {
					putpixel(pixel, (int)(xigraca[igrac] + j * cos(((orjentacija[igrac] + 90) * 3.14159265359) / 180.0) + i * cos((orjentacija[igrac] * 3.14159265359) / 180.0) + 0.5), (int)(yigraca[igrac] + j * sin(((orjentacija[igrac] + 90) * 3.14159265359) / 180.0) + i * sin((orjentacija[igrac] * 3.14159265359) / 180.0) + 0.5), 0, 0, 0);
				}
				else {
					putpixel(pixel, (int)(xigraca[igrac] + j * cos(((orjentacija[igrac] + 90) * 3.14159265359) / 180.0) + i * cos((orjentacija[igrac] * 3.14159265359) / 180.0) + 0.5), (int)(yigraca[igrac] + j * sin(((orjentacija[igrac] + 90) * 3.14159265359) / 180.0) + i * sin((orjentacija[igrac] * 3.14159265359) / 180.0) + 0.5), boja[0][igrac] * (100 - 3/velicina1 * max(abs(j), abs(i))) / 100, boja[1][igrac] * (100 - 3/velicina1 * max(abs(j), abs(i))) / 100, boja[2][igrac] * (100 - 3/velicina1 * max(abs(j), abs(i))) / 100);
				}
			}

		for (float i = -1; i < velicina1; i += 0.5)
			drawcircle(pixel, xigraca[igrac], yigraca[igrac], 8 * velicina1 + i, 0, 0, 0);

		for (float j = -4 * velicina1; j <= 4 * velicina1; j += 0.5)
			for (float i = 0; i <= 19 * velicina1; i += 0.5) {
				if (j <= (int)(-4 * velicina1) + velicina1 || j >= (int)(4 * velicina1) - velicina1 || i >= (int)(19 * velicina1) - velicina1) {
					if (i * i + j * j >= 16 * velicina1 * 3.14)
						putpixel(pixel, xigraca[igrac] + j * cos(((orjentacija[igrac] + 90) * 3.14159265359) / 180.0) + i * cos((orjentacija[igrac] * 3.14159265359) / 180.0), yigraca[igrac] + j * sin(((orjentacija[igrac] + 90) * 3.14159265359) / 180.0) + i * sin((orjentacija[igrac] * 3.14159265359) / 180.0), 0, 0, 0);
				}
				else {
					putpixel(pixel, xigraca[igrac] + j * cos(((orjentacija[igrac] + 90) * 3.14159265359) / 180.0) + i * cos((orjentacija[igrac] * 3.14159265359) / 180.0), yigraca[igrac] + j * sin(((orjentacija[igrac] + 90) * 3.14159265359) / 180.0) + i * sin((orjentacija[igrac] * 3.14159265359) / 180.0), boja[0][igrac], boja[1][igrac], boja[2][igrac]);
				}
			}
		for (int i = 0; i < 5; i++) if (used[i][igrac] == 1) for (int kk = 1; kk <= 6 * velicina1; kk++)drawcircle(pixel1, (int)xmetka[i][igrac], (int)ymetka[i][igrac], kk, 0, 0, 0);
		//tank

	}
	//tanks
}

void refreshscreen(unsigned int* pixel, unsigned int* pixel1, float velicina1) {

	int boj[3] = { 200,200,200 };

	for (int igrac = 0; igrac < brigraca; igrac++) {


		for (int i = 0; i < 5; i++) 
			if (used[i][igrac] == 1) 
				for (int kk = 1; kk <= 6 * velicina1; kk++)drawcircle(pixel1, (int)xmetka[i][igrac], (int)ymetka[i][igrac], kk, boj[0], boj[1], boj[2]);
		// bullets

		if (ply[igrac] == 0) continue;

		for (float j = -10 * velicina1; j <= 10 * velicina1; j += 0.5)
			for (float i = -15 * velicina1; i <= 15 * velicina1; i += 0.5) {
				if (mapa[(int)((xigraca[igrac] + j * cos(((orjentacija[igrac] + 90) * 3.14159265359) / 180.0) + i * cos((orjentacija[igrac] * 3.14159265359) / 180.0)) / (50 * velicina))][(int)((yigraca[igrac] + j * sin(((orjentacija[igrac] + 90) * 3.14159265359) / 180.0) + i * sin((orjentacija[igrac] * 3.14159265359) / 180.0)) / (50 * velicina))] == 0) { boj[0] = 0; boj[1] = 0; boj[2] = 0; }
				else { boj[0] = 200; boj[1] = 200; boj[2] = 200; }
				putpixel(pixel, (int)(xigraca[igrac] + j * cos(((orjentacija[igrac] + 90) * 3.14159265359) / 180.0) + i * cos((orjentacija[igrac] * 3.14159265359) / 180.0) + 0.5), (int)(yigraca[igrac] + j * sin(((orjentacija[igrac] + 90) * 3.14159265359) / 180.0) + i * sin((orjentacija[igrac] * 3.14159265359) / 180.0) + 0.5), boj[0], boj[1], boj[2]);
			}

		for (float j = -4 * velicina1; j <= 4 * velicina1; j += 0.5)
			for (float i = 0; i <= 19 * velicina1; i += 0.5) {
				if (mapa[(int)((xigraca[igrac] + j * cos(((orjentacija[igrac] + 90) * 3.14159265359) / 180.0) + i * cos((orjentacija[igrac] * 3.14159265359) / 180.0)) / (50 * velicina))][(int)((yigraca[igrac] + j * sin(((orjentacija[igrac] + 90) * 3.14159265359) / 180.0) + i * sin((orjentacija[igrac] * 3.14159265359) / 180.0)) / (50 * velicina))] == 0) { boj[0] = 0; boj[1] = 0; boj[2] = 0; }
				else { boj[0] = 200; boj[1] = 200; boj[2] = 200; }
				putpixel(pixel, xigraca[igrac] + j * cos(((orjentacija[igrac] + 90) * 3.14159265359) / 180.0) + i * cos((orjentacija[igrac] * 3.14159265359) / 180.0), yigraca[igrac] + j * sin(((orjentacija[igrac] + 90) * 3.14159265359) / 180.0) + i * sin((orjentacija[igrac] * 3.14159265359) / 180.0), boj[0], boj[1], boj[2]);
			}
		//tanks
	}
}

void colorchange() {
	boja[0][0] = 255;
	boja[1][0] = 0;
	boja[2][0] = 0;

	boja[0][1] = 0;
	boja[1][1] = 0;
	boja[2][1] = 255;
	
	boja[0][2] = 0;
	boja[1][2] = 255;
	boja[2][2] = 0;
	
	boja[0][3] = 255;
	boja[1][3] = 0;
	boja[2][3] = 255;
	
	boja[0][4] = 0;
	boja[1][4] = 255;
	boja[2][4] = 255;

	boja[0][5] = 255;
	boja[1][5] = 255;
	boja[2][5] = 0;
}

void drawscores() {

}

void deathanimation(int n) {
	int tajmerneki = 100;

}

///drawing///

void drawmeni1(unsigned int* pixel) {
	for (int x = 0; x < 856; x++)
		for (int y = 0; y < 501; y++)
			*(pixel + (x+(buffer_width-856)/2) + (y + (buffer_heigth - 501) / 2) * buffer_width) = f(x, y, &meni1, 0) + (f(x, y, &meni1, 1) << 8) + (f(x, y, &meni1, 2) << 16);
}

void meni() {
	unsigned int* pixell = (unsigned int*)buffer_memory;
	drawmeni1(pixell);
}

///meni///

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

LRESULT CALLBACK window_callback(_In_ HWND hwnd, _In_ UINT uMsg, _In_ WPARAM wParam, _In_ LPARAM lParam)
{
	LRESULT result = 0;

	switch (uMsg)
	{
	case WM_CLOSE:
	case WM_DESTROY:
	{
		running = false;
	} break;
	case WM_SIZE:
	{
		RECT rect;
		GetClientRect(hwnd, &rect);
		buffer_width = rect.right - rect.left;
		buffer_heigth = rect.bottom - rect.top;

		//buffer_heigth = height * 20;
		//buffer_width = 20 * width;

		int buffer_size = buffer_width * buffer_heigth * sizeof(unsigned int);

		if (buffer_memory)VirtualFree(buffer_memory, 0, MEM_RELEASE);
		buffer_memory = VirtualAlloc(0, buffer_size, MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);

		buffer_bitmap_info.bmiHeader.biSize = sizeof(buffer_bitmap_info.bmiHeader);
		buffer_bitmap_info.bmiHeader.biWidth = buffer_width;
		buffer_bitmap_info.bmiHeader.biHeight = buffer_heigth;
		buffer_bitmap_info.bmiHeader.biPlanes = 1;
		buffer_bitmap_info.bmiHeader.biBitCount = 32;
		buffer_bitmap_info.bmiHeader.biCompression = BI_RGB;

	} break;
	default:
	{
		result = DefWindowProc(hwnd, uMsg, wParam, lParam);
	}
	}
	return result;
};

int WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nShowCmd)
{
	srand(time(0));

	WNDCLASS window_class = {};
	window_class.style = CS_HREDRAW | CS_VREDRAW;
	window_class.lpszClassName = "Game Window Class";
	window_class.lpfnWndProc = window_callback;

	RegisterClass(&window_class);

	HWND window = CreateWindow(window_class.lpszClassName, "THE GAME", WS_OVERLAPPEDWINDOW | WS_VISIBLE,
		CW_USEDEFAULT, CW_USEDEFAULT, 2000, 1000, 0, 0, hInstance, 0);
	{
		//Fullscreen
		SetWindowLong(window, GWL_STYLE, GetWindowLong(window, GWL_STYLE) & ~WS_OVERLAPPEDWINDOW);
		MONITORINFO mi = { sizeof(mi) };
		GetMonitorInfo(MonitorFromWindow(window, MONITOR_DEFAULTTOPRIMARY), &mi);
		SetWindowPos(window, HWND_TOP, mi.rcMonitor.left, mi.rcMonitor.top, mi.rcMonitor.right - mi.rcMonitor.left, mi.rcMonitor.bottom - mi.rcMonitor.top, SWP_NOOWNERZORDER | SWP_FRAMECHANGED);
	}
	HDC hdc = GetDC(window);

	unsigned int* pixel = (unsigned int*)buffer_memory;
	unsigned int* pixel1;
	pixel1 = pixel;

	bool hasgamestarted = 0;

	colorchange();
	drawmeni1(pixel1);
	resolution();
	RECT ppp;
	ppp.top = 800;
	ppp.bottom = 900;
	ppp.left = 100;
	ppp.right = 200;
	while (running)
	{
		//DrawText(hdc, "12345", 5, &ppp, DT_BOTTOM);

		auto start = std::chrono::high_resolution_clock::now();

		MSG message;
		for (int i = 0; i < 200; i++)
		{
			ischanged[i] = 0;
		}
		while (PeekMessage(&message, window, 0, 0, PM_REMOVE))
		{

			switch (message.message)
			{
			case WM_MOUSEMOVE:
			{
				xmisa = message.lParam & 0xFFFF;
				ymisa = message.lParam >> 16;
			}break;
			case WM_KEYUP:
			case WM_KEYDOWN:
			{
				int key = message.wParam;
				bool is_down = ((message.lParam & (1 << 31)) == 0);
				ispressed[key] = is_down;
				ischanged[key] = true;

			} break;
			case WM_RBUTTONDOWN:
			{
				ischanged[VK_RBUTTON] = 1;
				ispressed[VK_RBUTTON] = 1;
			}break;
			case WM_RBUTTONUP:
			{
				ischanged[VK_RBUTTON] = 1;
				ispressed[VK_RBUTTON] = 0;
			}break;

			case WM_LBUTTONDOWN:
			{
				ischanged[VK_LBUTTON] = 1;
				ispressed[VK_LBUTTON] = 1;
			}break;
			case WM_LBUTTONUP:
			{
				ischanged[VK_LBUTTON] = 1;
				ispressed[VK_LBUTTON] = 0;
			}break;

			default:
			{
				TranslateMessage(&message);
				DispatchMessage(&message);
			}
			}

			if (ispressed['E'] && ischanged['E'])pucaj1[0] = 1;
			if (ispressed['P'] && ischanged['P'])pucaj1[1] = 1;
			if (ispressed['Y'] && ischanged['Y'])pucaj1[2] = 1;
			if (ispressed[VK_NUMPAD6] && ischanged[VK_NUMPAD6])pucaj1[3] = 1;
			if (ispressed[VK_SPACE] && ischanged[VK_SPACE])pucaj1[4] = 1;
		};

		StretchDIBits(hdc, 0, 0, buffer_width, buffer_heigth, 0, 0, buffer_width, buffer_heigth, buffer_memory,
			&buffer_bitmap_info, DIB_RGB_COLORS, SRCCOPY);

		if (ispressed[VK_ESCAPE] && ischanged[VK_ESCAPE]) {
			if (hasgamestarted == 0) break;
			hasgamestarted = 0;
			meni();
		}

		if (ispressed[VK_RETURN] && hasgamestarted == 0) {
			hasgamestarted = 1;
			for (int i = 0; i < 6; i++) scores[i] = 0;
			refreshgame();
			pravljenjemape(pixel);
			drawbackground(pixel, pixel1, velicina);
		}

		if (hasgamestarted == 0) continue;

		refreshscreen(pixel, pixel1, velicina);

		if (sudar() == 0) input();

		letmetkova();

		if (sudar()) {
			drawing(pixel, pixel1, velicina);
			playersleft--;
		};

		if (playersleft <= 1) {
			tajmerreseta--;
		}

		if (tajmerreseta == 0) {
			for (int i = 0; i < 6; i++) scores[i] += ply[i];
			refreshgame();
			pravljenjemape(pixel);
			drawbackground(pixel, pixel1, velicina);
			continue;
		}

		drawing(pixel, pixel1, velicina);

		auto elapsed = std::chrono::high_resolution_clock::now() - start;

		long long microseconds = std::chrono::duration_cast<std::chrono::microseconds>(elapsed).count();
		if (microseconds < 10000)
			std::this_thread::sleep_for(std::chrono::milliseconds(10 - microseconds / 1000));


	};
};