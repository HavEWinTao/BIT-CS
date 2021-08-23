#include<graphics.h>


#define N 4//游戏的维数
#define BACK_SIZE 512//窗口的大小

enum KEY {
    UP,     //上
    DOWN,   //下
    LEFT,   //左
    RIGHT,  //右
    OTHER   //其他键
};

void draw(int(*gameData)[N]);//页面绘制
void play(int(*gameData)[N]);//运行逻辑
int getInput();//输入
void loadImage();//加载图片

extern "C"  void _cdecl initGame(int array[4][4], int n);
extern "C"  void _cdecl moveDown(int array[4][4], int n);
extern "C"  void _cdecl moveUp(int array[4][4], int n);
extern "C"  void _cdecl moveLeft(int array[4][4], int n);
extern "C"  void _cdecl moveRight(int array[4][4], int n);
extern "C"  int _cdecl checkGameOver(int array[4][4], int n);

//全局变量,避免多次加载图片,导致内存泄漏
PIMAGE img_background;
PIMAGE img_gameover;
PIMAGE img_block2;
PIMAGE img_block4;
PIMAGE img_block8;
PIMAGE img_block16;
PIMAGE img_block32;
PIMAGE img_block64;
PIMAGE img_block128;
PIMAGE img_block256;
PIMAGE img_block512;
PIMAGE img_block1024;
PIMAGE img_block2048;

//位置信息
int block_loc[4] = { 20,142,266,390 };
int gameover_loc[2] = { 106,236 };

int flag = 1;//标记游戏是否结束

//页面绘制
void draw(int(*gameData)[N]) {
    int i, j;
    cleardevice();
    putimage(0, 0, img_background);
    for (i = 0; i < N; i++) {
        for (j = 0; j < N; j++) {
            int num = gameData[i][j];
            switch (num) {
            case 2: {
                putimage_withalpha(NULL, img_block2, block_loc[j], block_loc[i]);
                break;
            }
            case 4: {
                putimage_withalpha(NULL, img_block4, block_loc[j], block_loc[i]);
                break;
            }
            case 8: {
                putimage_withalpha(NULL, img_block8, block_loc[j], block_loc[i]);
                break;
            }
            case 16: {
                putimage_withalpha(NULL, img_block16, block_loc[j], block_loc[i]);
                break;
            }
            case 32: {
                putimage_withalpha(NULL, img_block32, block_loc[j], block_loc[i]);
                break;
            }
            case 64: {
                putimage_withalpha(NULL, img_block64, block_loc[j], block_loc[i]);
                break;
            }
            case 128: {
                putimage_withalpha(NULL, img_block128, block_loc[j], block_loc[i]);
                break;
            }
            case 256: {
                putimage_withalpha(NULL, img_block256, block_loc[j], block_loc[i]);
                break;
            }
            case 512: {
                putimage_withalpha(NULL, img_block512, block_loc[j], block_loc[i]);
                break;
            }
            case 1024: {
                putimage_withalpha(NULL, img_block1024, block_loc[j], block_loc[i]);
                break;
            }
            case 2048: {
                putimage_withalpha(NULL, img_block2048, block_loc[j], block_loc[i]);
                break;
            }
            default:
                break;
            }
        }
    }
}

//输入
int getInput() {
    int ret = -1;
    key_msg keyMsg;
    memset(&keyMsg, 0, sizeof(keyMsg));
    keyMsg = getkey();
    if (keyMsg.msg == key_msg_down) {
        switch (keyMsg.key) {
        case VK_UP: {
            ret = UP;
            break;
        }
        case VK_DOWN: {
            ret = DOWN;
            break;
        }
        case VK_LEFT: {
            ret = LEFT;
            break;
        }
        case VK_RIGHT: {
            ret = RIGHT;
            break;
        }
        default: {
            ret = OTHER;
            break;
        }
        }
    }
    return ret;
}

//运行逻辑
void play(int(*gameData)[N]) {
    int key;
    key = getInput();
    switch (key) {
    case UP: {
        moveUp(gameData, 4);
        break;
    }
    case DOWN: {
        moveDown(gameData, 4);
        break;
    }
    case LEFT: {
        moveLeft(gameData, 4);
        break;
    }
    case RIGHT: {
        moveRight(gameData, 4);
        break;
    }
    default:
        break;
    }
    draw(gameData);
}

//加载图片
void loadImage() {
    img_background = newimage();
    getimage(img_background, "img/background.png");

    img_gameover = newimage();
    getimage(img_gameover, "img/gameover.png");
    img_block2 = newimage();
    getimage(img_block2, "img/block_2.png");
    img_block4 = newimage();
    getimage(img_block4, "img/block_4.png");
    img_block8 = newimage();
    getimage(img_block8, "img/block_8.png");
    img_block16 = newimage();
    getimage(img_block16, "img/block_16.png");
    img_block32 = newimage();
    getimage(img_block32, "img/block_32.png");
    img_block64 = newimage();
    getimage(img_block64, "img/block_64.png");
    img_block128 = newimage();
    getimage(img_block128, "img/block_128.png");
    img_block256 = newimage();
    getimage(img_block256, "img/block_256.png");
    img_block512 = newimage();
    getimage(img_block512, "img/block_512.png");
    img_block1024 = newimage();
    getimage(img_block1024, "img/block_1024.png");
    img_block2048 = newimage();
    getimage(img_block2048, "img/block_2048.png");
}

int  main() {
    initgraph(BACK_SIZE, BACK_SIZE);//初始化
    setcaption("2048");//窗口标题
    loadImage();//加载图片
    flushkey();

    int gameData[N][N];
    initGame(gameData, 4);

    draw(gameData);

    for (; is_run(); delay_fps(60)) {
        if (flag == 1) {//未结束
            play(gameData);
            if (checkGameOver(gameData, 4)) {//判断是否结束
                flag = 0;
                putimage_withalpha(NULL, img_gameover, gameover_loc[0], gameover_loc[1]);
            }
        }
    }
    return 0;
}

