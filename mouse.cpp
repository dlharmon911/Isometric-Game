#include "main.h"

MOUSE_CLASS mouse;

void MOUSE_CLASS::init()
{
    under=create_bitmap(32,32);
    mouse_down=false;
    type=0;
}

void MOUSE_CLASS::uninit()
{
    destroy_bitmap(under);
}

bool MOUSE_CLASS::click()
{
    if (mouse_b & 1)
    {
        mouse_down=true;
    }
    else
    {
        if (mouse_down)
        {
            mouse_down=false;
            return true;
        }
        mouse_down=false;
    }

    return false;
}

void MOUSE_CLASS::draw(BITMAP *bmp)
{
    int p=type;

    if ((p==2 || p==1) && mouse_y>=WINDOW_TOP)
    {
        p=0;
    }

    draw_sprite(buffer,(BITMAP*)data[MOUSE01+p].dat,mouse_x,mouse_y);
}
