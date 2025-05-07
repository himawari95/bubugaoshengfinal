#include<graphics.h>
#include<conio.h>
#include <windows.h>
#include<iostream>
#include <mmsystem.h>
#pragma comment(lib, "winmm.lib")
using namespace std;

const int WIDTH = 600;
const int HEIGHT = 800;
const int PLAYER_SIZE = 30;
const int PLATFORM_WIDTH = 100;

IMAGE imgbackground;
IMAGE imgplayer1_left;
IMAGE imgplayer1_right;
IMAGE imgplayer2_left;
IMAGE imgplayer2_right;
IMAGE imgplayer3_left;
IMAGE imgplayer3_right;
IMAGE imgplatform;
IMAGE imgLoading;
IMAGE imgCharacterSelect;

bool isleft = false;
int score = 0;
int selectedCharacter = 1; // 初始选择第一个角色

struct platform {
    int x, y;
    bool counted;
};
struct player {
    int x, y;
    float velocity;
};

player person;
platform platforms[10];

void putimagePNG(int x, int y, IMAGE* picture) {
    DWORD* dst = GetImageBuffer();
    DWORD* draw = GetImageBuffer(picture);

    int picture_width = picture->getwidth();
    int picture_height = picture->getheight();

    for (int iy = 0; iy < picture_height; iy++) {
        for (int ix = 0; ix < picture_width; ix++) {
            int srcX = x + ix;
            int srcY = y + iy;

            if (srcX >= 0 && srcX < WIDTH && srcY >= 0 && srcY < HEIGHT) {
                int alpha = (draw[iy * picture_width + ix] & 0xff000000) >> 24;
                if (alpha == 255) {
                    dst[srcY * WIDTH + srcX] = draw[iy * picture_width + ix];
                }
                else if (alpha != 0) {
                    unsigned char r = GetRValue(draw[iy * picture_width + ix]);
                    unsigned char g = GetGValue(draw[iy * picture_width + ix]);
                    unsigned char b = GetBValue(draw[iy * picture_width + ix]);
                    unsigned char r1 = GetRValue(dst[srcY * WIDTH + srcX]);
                    unsigned char g1 = GetGValue(dst[srcY * WIDTH + srcX]);
                    unsigned char b1 = GetBValue(dst[srcY * WIDTH + srcX]);
                    dst[srcY * WIDTH + srcX] = RGB(
                        (r * alpha + r1 * (255 - alpha)) / 255,
                        (g * alpha + g1 * (255 - alpha)) / 255,
                        (b * alpha + b1 * (255 - alpha)) / 255
                    );
                }
            }
        }
    }
}

void showLoadingScreen() {
    
    loadimage(&imgLoading, L"tietu/loading.jpg");
    putimage(0, 0, &imgLoading);
    
    while (true) {
        if (GetAsyncKeyState(VK_SPACE)) {
            break;
        }
        Sleep(10);
    }
   
}

void showCharacterSelectScreen() {
   
    loadimage(&imgCharacterSelect, L"tietu/character_select.jpg");
    putimage(0, 0, &imgCharacterSelect);

    // 显示角色图片
    putimagePNG(WIDTH / 6 - PLAYER_SIZE / 2, HEIGHT / 2 - PLAYER_SIZE / 2, &imgplayer1_right);
    putimagePNG(3 * WIDTH / 6 - PLAYER_SIZE / 2, HEIGHT / 2 - PLAYER_SIZE / 2, &imgplayer2_right);
    putimagePNG(5 * WIDTH / 6 - PLAYER_SIZE / 2, HEIGHT / 2 - PLAYER_SIZE / 2, &imgplayer3_right);

    // 等待玩家选择角色
    while (true) {
        if (GetAsyncKeyState('1')) {
            selectedCharacter = 1;
            break;
        }
        if (GetAsyncKeyState('2')) {
            selectedCharacter = 2;
            break;
        }
        if (GetAsyncKeyState('3')) {
            selectedCharacter = 3;
            break;
        }
        Sleep(10);
    }

    
}

void initialize() {
    
    settextstyle(24, 0, _T("微软雅黑"));
    settextcolor(BLACK);
    setbkmode(TRANSPARENT);
    loadimage(&imgbackground, L"tietu/beijing.jpg");
    loadimage(&imgplatform, L"tietu/jieti.jpg");
    loadimage(&imgplayer1_left, L"tietu/wusaqi1.png");
    loadimage(&imgplayer1_right, L"tietu/wusaqi2.png");
    loadimage(&imgplayer2_left, L"tietu/xiaoba1.png");
    loadimage(&imgplayer2_right, L"tietu/xiaoba2.png");
    loadimage(&imgplayer3_left, L"tietu/jiyi1.png");
    loadimage(&imgplayer3_right, L"tietu/jiyi2.png");

    person = { WIDTH / 2, HEIGHT / 2, 0 };

    for (int i = 0; i < 10; i++) {
        platforms[i] = {
            rand() % (WIDTH - 100),
            HEIGHT - i * 80,
            false
        };
    }
    
    mciSendString(L"open \"music.wav\" type waveaudio alias bgm", NULL, 0, NULL);
    mciSendString(L"play bgm repeat", NULL, 0, NULL);
}

void draw() {
    putimage(0, 0, &imgbackground);

    for (const platform& p : platforms) {
        putimage(p.x, p.y, PLATFORM_WIDTH, 20, &imgplatform, 0, 0);
    }

    IMAGE* currentImage;
    if (selectedCharacter == 1) {
        currentImage = isleft ? &imgplayer1_left : &imgplayer1_right;
    }
    else if (selectedCharacter == 2) {
        currentImage = isleft ? &imgplayer2_left : &imgplayer2_right;
    }
    else {
        currentImage = isleft ? &imgplayer3_left : &imgplayer3_right;
    }
    putimagePNG(person.x - PLAYER_SIZE / 2, person.y - PLAYER_SIZE / 2, currentImage);

    TCHAR scoreText[20];
    _stprintf_s(scoreText, _T("分数:%d"), score);
    outtextxy(10, 10, scoreText);
    Sleep(10);
}

void update() {
    if (GetAsyncKeyState(VK_LEFT)) {
        person.x -= 5;
        isleft = true;
    }
    if (GetAsyncKeyState(VK_RIGHT)) {
        person.x += 5;
        isleft = false;
    }
    person.velocity += 0.5;
    person.y += person.velocity;

    for (platform& p : platforms) {
        if (person.y + PLAYER_SIZE / 2 > p.y &&
            person.y + PLAYER_SIZE / 2 < p.y + 20 &&
            person.x > p.x - PLAYER_SIZE / 2 &&
            person.x < p.x + PLATFORM_WIDTH + PLAYER_SIZE / 2)
        {
            person.velocity = -15;
            if (!p.counted) {
                score += 10;
                p.counted = true;
            }
        }
    }
    if (person.y < HEIGHT / 2)
    {
        person.y = HEIGHT / 2;
        for (platform& p : platforms) {
            p.y += 5;
            if (p.y > HEIGHT)
                p = { rand() % (WIDTH - 100), 0, false };
        }
    }
    if (person.y > HEIGHT) {
        score = 0;
        person = { WIDTH / 2, HEIGHT / 2, 0 };
        for (int i = 0; i < 10; i++) {
            platforms[i] = { rand() % (WIDTH - 100), HEIGHT - i * 80, false };
        }
    }
}

int main()
{
    initgraph(WIDTH, HEIGHT);
    showLoadingScreen();
    showCharacterSelectScreen();
    initialize();

    while (!_kbhit()) {
        update();
        draw();
    }
    mciSendString(L"stop bgm", NULL, 0, NULL);
    mciSendString(L"close bgm", NULL, 0, NULL);

    closegraph();
    return 0;
}
    while (!_kbhit()) {
        update();
        draw();
    }
    closegraph();
    return 0;
}
