#include "main.h"

void BUTTON_CLASS::create(int x, int y, int w, int h, const char *tx)
{
    x_pos=x;
    y_pos=y;
    width=w;
    height=h;
    strcpy(text,tx);
    strcpy(alt,"");
    sprite=NULL;
}

void BUTTON_CLASS::create(int x, int y, int w, int h, BITMAP *bmp,const char *a)
{
    x_pos=x;
    y_pos=y;
    width=w;
    height=h;
    strcpy(text,"");
    strcpy(alt,a);
    sprite=bmp;
}

void BUTTON_CLASS::draw(BITMAP *bmp)
{
    bool f=false;
    int flag=UP,c1,c2;
    c1=COLOR_UP;
    c2=COLOR_DOWN;

    if (isMouse(mouse_x,mouse_y))
    {
        if (mouse_b & 1)
        {
            flag=DOWN;
            c2=COLOR_UP;
            c1=COLOR_DOWN;
        }
        f=true;
        doStatus(alt);
    }

    drawWindow(bmp,x_pos,y_pos,x_pos+width-1,y_pos+height-1,flag);

    if (strlen(text)>0)
    {
       dtextout_centre(bmp,my_font,text,x_pos+width/2,y_pos+(height-text_height(my_font))/2,c1,c2);
    }
    if (sprite!=NULL)
    {
        draw_sprite(bmp,sprite,x_pos+(width-sprite->w)/2,y_pos+(height-sprite->h)/2);
    }
}

bool BUTTON_CLASS::isMouse(int x, int y)
{
    if (x>=x_pos && x<=(x_pos+width-1) &&
        y>=y_pos && y<=(y_pos+height-1))
    {
        return true;
    }
    return false;
}
