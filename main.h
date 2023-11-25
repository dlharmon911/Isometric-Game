#define _CRT_SECURE_NO_WARNINGS

#include <allegro.h>
#include <string.h>
#include <stdio.h>
#include "data.h"

//#define for if(false) {} else for

#define PASSWORD        "lf0 -3nv 20f"

#define WORLD_X         200
#define WORLD_Y         200

#define GRID_X          28
#define GRID_Y          28

#define UP              0
#define DOWN            1
#define LEFT            2
#define RIGHT           3

#define X_OFFSET        -544//(SCREEN_W-((GRID_X-2)*64))/2
#define Y_OFFSET        -176//(SCREEN_H-((GRID_Y-2)*32))/2

#define NUMBER_LAYER    5
#define WINDOW_TOP      380

#define COLOR_UP        153
#define COLOR_DOWN      168

#define MAX_CHILDREN    4

class MAP_CLASS
{
public:
class OVERLAY
{
    public:
        class TILE_CLASS
        {
        public:
            int x;
            int y;
            void setup();
            bool inside(int x, int y);        
        };
        TILE_CLASS tiles[GRID_X*GRID_Y];
        int map_overlay[GRID_X*GRID_Y];
        void setup();
    };
    class TILE_CLASS
    {
    public:
        int level[NUMBER_LAYER];
    };

    TILE_CLASS tiles[WORLD_X*WORLD_Y];
    OVERLAY overlay;
    int scroll;
    int shift_x;
    int shift_y;

    void setup();
    void edraw(BITMAP *bmp,int v);
    void draw(BITMAP *bmp);
    void drawMouse(BITMAP *bmp);
    void edit();
    void save();
    void load();
    void move(int dir);
    int getPos(int x,int y);
    bool insideTriangle(int x, int y);
};

class PLAYER_CLASS
{
public:
    int pos;
    int x_pos;
    int y_pos;
    int frame;
    int dir;
    int gx,gy;
    int cx,cy;
    bool walk;
    BITMAP *sprite[8];

    void setup();
    void uninit();
    void draw(BITMAP *bmp,int x, int y);
    void draw2(BITMAP *bmp,int x, int y);
    int do_move(int dir);
    void move();
    void set_movement(int x,int y);
    bool check_hit();
    void load();
    void save();
    void doLineSight();
};

class BUTTON_CLASS
{
public:
    int x_pos;
    int y_pos;
    int width;
    int height;
    int flag;
    char text[40];
    char alt[40];
    BITMAP *sprite;

    void create(int x, int y, int w, int h, const char *tx);
    void create(int x, int y, int w, int h, BITMAP *bmp,const char *a);
    void draw(BITMAP *bmp);
    bool isMouse(int x, int y);
};

class MOUSE_CLASS
{
public:
    bool mouse_down;
    int type;
    BITMAP *under;

    void init();
    void uninit();
    bool click();
    void update();
    void draw(BITMAP *bmp);
    void change(int t);
};



/* A* class */ 
 
typedef struct NodeClass
{

    long f, h;
    int g, tmpg;
    int x, y;
    int NodeNum;
    NodeClass *Parent;
    NodeClass *Child[8];
    NodeClass *NextNode;

} NodeClass;

typedef struct StackClass
{

    NodeClass *NodePtr;
    StackClass *NextStackPtr;

} StackClass;

class AstarPathFinderClass
{

public:
 
    NodeClass   *OpenNode,
                *ClosedNode,
                *PathNode;
    StackClass  *Stack;
    
    bool isPath;
    
    int Height,
        Width,
        TotalSize,
        *TileMap;

public:
    AstarPathFinderClass(MAP_CLASS &map);
    ~AstarPathFinderClass();
    void InitAstarTileMap(MAP_CLASS &map);
    void RedoAstarTileMap(MAP_CLASS &map);
    bool NewPath(int sx, int sy, int dx, int dy);
    bool ReachedGoal(void);
    void PathNextNode(void);
    int NodeGetX(void);
    int NodeGetY(void);
    int TileNum(int x, int y);
    bool FreeTile(int x, int y);

private:
    void BoundaryTiles(void);
    void FreeNodes(void);
    void FindPath(int sx, int sy, int dx, int dy);
    NodeClass *ReturnBestNode(void);
    void GenerateSuccessors(NodeClass *BestNode, int dx, int dy);
    void GenerateSucc(NodeClass *BestNode,int x, int y, int dx, int dy);
    NodeClass *CheckOPEN(int tilenum);
    NodeClass *CheckCLOSED(int tilenum);
    void Insert(NodeClass *Successor);
    void PropagateDown(NodeClass *Old);
    void Push(NodeClass *NodeClass);
    NodeClass *Pop(void);
};

extern PLAYER_CLASS player;
extern MAP_CLASS map;
extern BITMAP *buffer;
extern DATAFILE *data;
extern FONT *my_font;
extern MOUSE_CLASS mouse;
extern bool DAN;
extern int FONT_H;
extern AstarPathFinderClass *astar;

extern RGB_MAP rgb_table;
extern COLOR_MAP light_table;
extern COLOR_MAP trans_table;

/******************** misc.cpp **************************/

void dtextout(BITMAP *bmp, const FONT *f, const char *tx, int x, int y, int fg,int bg);
void dtextout_centre(BITMAP *bmp, const FONT *f, const char *tx, int x, int y, int fg,int bg);
void dtextprintf(BITMAP *bmp, const FONT *f, int x,int y,int fg,int bg, const char *format, ...);
void dtextprintf_centre(BITMAP *bmp, const FONT *f, int x,int y,int fg,int bg, const char *format, ...);

void picFill(BITMAP *source,BITMAP *dest,int sx, int sy, int dx, int dy, int w, int h);
void drawWindow(BITMAP *bmp, int x, int y, int x2, int y2,int flag);
void drawBorder(BITMAP *bmp, int x, int y, int x2, int y2,int flag);

bool pass();

BITMAP *getBehind(BITMAP *bmp,int x1,int y1,int x2,int y2);
void putBehind(BITMAP *src,BITMAP *dest,int x,int y);

void doMessage(BITMAP *bmp,int x, int y,int amount,const char *tx,...);
bool doQuestion(BITMAP *bmp,int x, int y,int amount,const char *tx1,const char *tx2,...);

void doGameHelp();

/******************** main.cpp **************************/
int distance(int x,int y);
void doStatus(const char *tx);
int initAll();
void deleteAll();
void drawAll();
void drawScreen(BITMAP *bmp);

