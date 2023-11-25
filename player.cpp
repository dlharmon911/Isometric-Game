#include "main.h"

PLAYER_CLASS player;

void PLAYER_CLASS::setup()
{    
    x_pos=32;
    y_pos=16;
    frame=0;
    dir=RIGHT;
    pos=8259;
    cx=0;
    cy=0;
    walk=false;
    gx=352;
    gy=304;

    for (int i=0; i<8; i++)
    {
        sprite[i]=create_bitmap(32,32);
        blit((BITMAP*)data[CHARLIE].dat,sprite[i],32*(i%2),32*(i/2),0,0,32,32);   
    }
}

void PLAYER_CLASS::uninit()
{
    for (int i=0; i<8; i++)
    {
        destroy_bitmap(sprite[i]);
    }
}

void PLAYER_CLASS::draw(BITMAP *bmp,int x, int y)
{    
    gx=x+x_pos;
    gy=y+y_pos;
    draw_sprite(bmp,(BITMAP*)data[HALO].dat,x+x_pos-12,y+y_pos-9);
    putpixel(bmp,x+x_pos,y+y_pos,255);
    draw_sprite(bmp,sprite[frame],x+x_pos-16,y+y_pos-28);
}

void PLAYER_CLASS::draw2(BITMAP *bmp,int x, int y)
{    
    color_map = &trans_table;
    draw_trans_sprite(bmp,(BITMAP*)data[HALO].dat,x+x_pos-12,y+y_pos-9);
    draw_trans_sprite(bmp,sprite[frame],x+x_pos-16,y+y_pos-28);
}

bool PLAYER_CLASS::check_hit()
{
    if (map.tiles[pos].level[3]==1) return true;
    return false;
}

int PLAYER_CLASS::do_move(int dir)
{
    int p,ox,oy,op,of;

    ox=x_pos;
    oy=y_pos;
    op=pos;
    of=frame;

    frame++;

    if (dir==UP)
    {
        y_pos-=2;        
        if (frame<0 || frame>1) frame=0;
    }
    if (dir==DOWN)
    {
        y_pos+=2;
        if (frame<4 || frame>5) frame=4;
    }
    if (dir==LEFT)
    {
        x_pos-=2;
        if (frame<6 || frame>7) frame=6;
    }
    if (dir==RIGHT)
    {
        x_pos+=2;
        if (frame<2 || frame>3) frame=2;
    }

    if (x_pos<0)
    {
        x_pos+=64;
        pos=pos+WORLD_X-1;
    }
    if (x_pos>63)
    {
        x_pos-=64;
        pos=pos-WORLD_X+1;
    }
    if (y_pos>31)
    {
        y_pos-=32;
        pos=pos+WORLD_X+1;
    }

    p=getpixel((BITMAP*)data[COLOR_HELPER].dat,x_pos,y_pos);    
    if (p!=1)
    {
        if (p==2)
        {
            x_pos+=32;
            y_pos+=16;
            pos=pos-1;
        }
        if (p==3)
        { 
            x_pos-=32;
            y_pos+=16;
            pos=pos-WORLD_X;
        }
        if (p==4)
        { 
            x_pos+=32;
            y_pos-=16;
            pos=pos+WORLD_X;
        }
        if (p==5) 
        {
            x_pos-=32;
            y_pos-=16;
            pos=pos+1;
        }
    }

    if (check_hit())
    {
        x_pos=ox;
        y_pos=oy;
        pos=op;
        frame=of;
        return 0;
    }

    return 2;
}

void PLAYER_CLASS::move()
{    
    if (walk)
    {
        if (cy<0) cy+=do_move(UP);
        if (cy>0) cy-=do_move(DOWN);
        if (ABS(cy)<2) cy=0;


        if (cx<0) cx+=do_move(LEFT);
        if (cx>0) cx-=do_move(RIGHT);
        if (ABS(cx)<2) cx=0;

        if (cx==0 && cy==0) walk=false;

        doLineSight();
    }
}

void PLAYER_CLASS::set_movement(int x, int y)
{
    int xx,yy,p,l,ps;
    ps=map.getPos(x,y);
    l=map.scroll+(ps/GRID_X)*WORLD_X+(ps%GRID_X); 
    p=l-player.pos;
    xx=p%200;                
    if (p<0) xx+=200;
    if (xx>100) xx-=200;                
    if (p<0)
    {
        yy=(p-xx)/200;
    }
    else
    {
        yy=p/200;
    }
    if (p>0 && xx<0) yy+=1;
    cx=cy=0;

    walk=true;
    cx=(xx-yy)*32-x_pos+x-(map.overlay.tiles[ps].x+X_OFFSET-map.shift_x);
    cy=(xx+yy)*16-y_pos+y-(map.overlay.tiles[ps].y+Y_OFFSET-map.shift_y);
}

void PLAYER_CLASS::load()
{

}

void PLAYER_CLASS::save()
{

}

void PLAYER_CLASS::doLineSight()
{

}