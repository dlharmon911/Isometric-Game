#include "main.h"

MAP_CLASS map;
BITMAP *map_buffer;

void MAP_CLASS::OVERLAY::setup()
{
    int ox, oy, sx, l, a, b, k;

    
    ox=GRID_X*32-32;
    oy=0;
    sx=0;
    l=0;
    k=0;

    for (int i=0; i<GRID_X; i++)
    {
        a=b=i*GRID_X;
        map_overlay[k]=a;
        k++;
        for (int j=0; j<i; j++)
        {
            b-=(GRID_X-1);
            map_overlay[k]=b;
            k++;
        }        
    }

    for (int i=(GRID_X-1); i>=0; i--)
    {
        a++;b=a;
        map_overlay[k]=a;
        k++;
        for (int j=1; j<i; j++)
        {
            b-=(GRID_X-1);
            map_overlay[k]=b;
            k++;
        }  
    }

    for (int i=1; i<=GRID_X; i++)
    {
        for (int j=0; j<i; j++)
        {
            tiles[map_overlay[l]].x=ox+sx;
            tiles[map_overlay[l]].y=oy;
            sx+=64;
            l++;
        }
        ox-=32;
        oy+=16;
        sx=0;
    }
    ox+=64;
    for (int i=GRID_X-1; i>0; i--)
    {
        for (int j=0; j<i; j++)
        {
            tiles[map_overlay[l]].x=ox+sx;
            tiles[map_overlay[l]].y=oy;
            sx+=64;
            l++;
        }
        ox+=32;
        oy+=16;
        sx=0;
    }    
}

bool MAP_CLASS::OVERLAY::TILE_CLASS::inside(int xx, int yy)
{
    if (xx>=x && xx<(x+64) && yy>=y && yy<(y+32))
    {
        if (getpixel((BITMAP*)data[COLOR_HELPER].dat, xx-x, yy-y)==1)
            return true;
    }

    return false;
}

void MAP_CLASS::setup()
{
    /*
    for (int i=0; i<WORLD_X*WORLD_Y; i++)
    {
        for (int j=0; j<4; j++)
        {            
            tiles[i].level[j]=0;
        }
    }
    */
    load();
    overlay.setup();
    shift_x=0;
    shift_y=0;
    scroll=5245;
}

void MAP_CLASS::drawMouse(BITMAP *bmp)
{
    int i=getPos(mouse_x, mouse_y);

    draw_sprite(bmp, (BITMAP*)data[GRID_MOUSE].dat, overlay.tiles[i].x+X_OFFSET-shift_x, overlay.tiles[i].y+Y_OFFSET-shift_y);        
}

int MAP_CLASS::getPos(int x, int y)
{
    for (int i=0; i<GRID_X*GRID_Y; i++)
    {
        if (overlay.tiles[i].inside(x-X_OFFSET+shift_x, y-Y_OFFSET+shift_y))
        {
            return i;
        }
    }
    return -1;
}

void MAP_CLASS::edit()
{
    char txt[5][40]={"Floor", "Cell", "Roof", "Passable", "Special"};
    bool done=false;
    int cell=0, t=0, i, p, d[NUMBER_LAYER]={FLOOR000, TILE000, ROOF000, 0, 0};
    BITMAP *buf;
    buf=create_bitmap(BUFFER_WIDTH, BUFFER_HEIGHT);

    load();

    while (!done)
    {
        edraw(buf, t);
        //draw_sprite(buf, (BITMAP*)data[BACK].dat, 0, 0);
        if (t<3)
            draw_sprite(buf, (BITMAP*)data[cell+d[t]].dat, 9, 9);        
        
        drawMouse(buf);        
        putpixel(buf, mouse_x, mouse_y, makecol(255, 255, 255));
        textprintf(buf, font, 75, 30, 255, "Current Map Position:%d", scroll);
        textprintf(buf, font, 75, 40, makecol(255, 255, 255), "Level %d:%s", t, txt[t]); 
        textprintf(buf, font, 75, 60, makecol(255, 255, 255), "Tile #%d", cell); 

        drawScreen(buf);

        if (key[KEY_Q])
        {
            if (cell>1) cell--;
            rest(100);
        }

        if (key[KEY_W])
        {
            if ((t==0 && cell<FLOORZZZ-FLOOR000-1) ||
                (t==1 && cell<TILEZZZ-TILE000-1) ||
                (t==2 && cell<ROOFZZZ-ROOF000-1) ||
                (t==4)) cell++;
            rest(100);
        }

        if (key[KEY_1])
        {
            if (t!=0)
            {
                cell=0;
            }
            t=0;            
        }

        if (key[KEY_2])
        {
            if (t!=1)
            {
                cell=0;
            }
            t=1;
        }
        if (key[KEY_3])
        {
            if (t!=2)
            {
                cell=0;
            }
            t=2;
        }
        if (key[KEY_4])
        {
            if (t!=3)
            {
                cell=1;
            }
            t=3;
        }
        if (key[KEY_5])
        {
            cell=1;
            t=4;
        }

        if (mouse_b & 1)
        {
            i=getPos(mouse_x, mouse_y);
            p=scroll+(i/GRID_X*WORLD_X)+(i%GRID_X);
            if (p<0) p+=WORLD_X*WORLD_Y;
            if (p>=WORLD_X*WORLD_Y) p-=WORLD_X*WORLD_Y;
            tiles[p].level[t]=cell;
            if ((t==1) || (t==0 && cell==5))
                tiles[p].level[3]=1;
        }

        if (mouse_b & 2)
        {
            i=getPos(mouse_x, mouse_y);
            p=scroll+(i/GRID_X*WORLD_X)+(i%GRID_X);
            if (p<0) p+=WORLD_X*WORLD_Y;
            if (p>=WORLD_X*WORLD_Y) p-=WORLD_X*WORLD_Y;
            tiles[p].level[t]=0;
            if (t==1 || t==0)
                tiles[p].level[3]=0;
        }

        if (mouse_x<=5 || key[KEY_LEFT])
        {
            move(LEFT);
        }

        if (mouse_x>=(BUFFER_WIDTH-6) || key[KEY_RIGHT])
        {
            move(RIGHT);
        }

        if (mouse_y<=5 || key[KEY_UP])
        {
            move(UP);
        }

        if (mouse_y>=(BUFFER_HEIGHT-6) || key[KEY_DOWN])
        {
            move(DOWN);
        }

        if (key[KEY_ESC] || key[KEY_F10]) 
        {
            done=true;
        }
    }

    save();
    while (key[KEY_ESC]) {}
    destroy_bitmap(buf);
}

void MAP_CLASS::edraw(BITMAP *bmp, int v)
{
    int c, l, d[NUMBER_LAYER]={FLOOR000, TILE000, ROOF000, 0, 0};
    BITMAP *b;

    for (int i=0; i<GRID_X*GRID_Y; i++)
    {
        if (overlay.tiles[overlay.map_overlay[i]].x+X_OFFSET-shift_x>-64 &&
            overlay.tiles[overlay.map_overlay[i]].x+X_OFFSET-shift_x<BUFFER_WIDTH &&
            overlay.tiles[overlay.map_overlay[i]].y+Y_OFFSET-shift_y>-32 &&
            overlay.tiles[overlay.map_overlay[i]].y+Y_OFFSET-shift_y<BUFFER_HEIGHT)
        {
            l=scroll+(overlay.map_overlay[i]/GRID_X*WORLD_X)+(overlay.map_overlay[i]%GRID_X); 
            if (l<0) l+=WORLD_X*WORLD_Y;
            if (l>=WORLD_X*WORLD_Y) l-=WORLD_X*WORLD_Y;

            for (int j=0; j<NUMBER_LAYER; j++)
            {
                if (j!=2 || (j==2 && key[KEY_TAB]) || v==2)
                {
                    c=tiles[l].level[j];
                    if (j<3)
                    {
                        if ((j!=0 && c!=0) || (j==0))
                        {
                            b=(BITMAP*)data[c+d[j]].dat;
                            draw_sprite(bmp, b, overlay.tiles[overlay.map_overlay[i]].x+X_OFFSET-shift_x, overlay.tiles[overlay.map_overlay[i]].y+32-(b->h)+Y_OFFSET-shift_y);              
                        }
                    }
                    else
                    {
                        if (c!=0 && j==3)
                        {
                            draw_sprite(bmp, (BITMAP*)data[SHADE].dat, overlay.tiles[overlay.map_overlay[i]].x+X_OFFSET-shift_x, overlay.tiles[overlay.map_overlay[i]].y+Y_OFFSET-shift_y);              
                        }
                    }
                    textprintf_centre(bmp, font, overlay.tiles[overlay.map_overlay[i]].x+X_OFFSET-shift_x+32, overlay.tiles[overlay.map_overlay[i]].y+12+Y_OFFSET-shift_y, 255, "%d", tiles[l].level[v]);
                }
                if (v==4) j=NUMBER_LAYER;
            }
        }     
    }
}  


void MAP_CLASS::draw(BITMAP *bmp)
{
    clear(bmp);
    int c, l, d[NUMBER_LAYER]={FLOOR000, TILE000, ROOF000, 0, 0};
    BITMAP *b;
    int x, y;

    x=y=-1;

    for (int j=0; j<NUMBER_LAYER-2; j++)
    {
        for (int i=0; i<GRID_X*GRID_Y; i++)
        {
            if (overlay.tiles[overlay.map_overlay[i]].x+X_OFFSET-shift_x>-64 &&
                overlay.tiles[overlay.map_overlay[i]].x+X_OFFSET-shift_x<BUFFER_WIDTH &&
                overlay.tiles[overlay.map_overlay[i]].y+Y_OFFSET-shift_y>-32 &&
                overlay.tiles[overlay.map_overlay[i]].y+Y_OFFSET-shift_y<BUFFER_HEIGHT)
            {
                l=scroll+(overlay.map_overlay[i]/GRID_X*WORLD_X)+(overlay.map_overlay[i]%GRID_X); 
                if (l<0) l+=WORLD_X*WORLD_Y;
                if (l>=WORLD_X*WORLD_Y) l-=WORLD_X*WORLD_Y;

                
                if (j!=2 || ((j==2) && (tiles[player.pos].level[4]!=tiles[l].level[4])))
                {
                    c=tiles[l].level[j];

                    if ((j!=0 && c!=0) || (j==0))
                    {
                        b=(BITMAP*)data[c+d[j]].dat;
                        draw_sprite(bmp, b, overlay.tiles[overlay.map_overlay[i]].x+X_OFFSET-shift_x, overlay.tiles[overlay.map_overlay[i]].y+32-(b->h)+Y_OFFSET-shift_y);              
                    }
    
                    if (j==1 && l==player.pos)
                    {
                        x=overlay.tiles[overlay.map_overlay[i]].x+X_OFFSET-shift_x;
                        y=overlay.tiles[overlay.map_overlay[i]].y+Y_OFFSET-shift_y;
                        player.draw(bmp, x, y);                
                    }
                }
            }
        }
    }

    if (x!=-1 && y!=-1)
        player.draw2(bmp, x, y);
}  

void MAP_CLASS::save()
{
	PACKFILE *pfile;

   
	pfile=pack_fopen("map.dat", "wp");

	for (int i=0; i<WORLD_X*WORLD_Y; i++)
	{
        for (int j=0; j<NUMBER_LAYER; j++)
        {
		    pack_iputl(tiles[i].level[j], pfile);            
        }
	}
	pack_fclose(pfile);

}

void MAP_CLASS::load()
{
    PACKFILE *pfile;

	pfile=pack_fopen("map.dat", "rp");

	for (int i=0; i<WORLD_X*WORLD_Y; i++)
	{
        for (int j=0; j<NUMBER_LAYER; j++)
        {
		    tiles[i].level[j]=pack_igetl(pfile);   
        }        
/*
        if (i%WORLD_X<=10 || i%WORLD_X>=189 || i/WORLD_X<=10 || i/WORLD_X>=189)
        {
            tiles[i].level[0]=5;
            tiles[i].level[3]=1;
        }
        if (i==0 || i==WORLD_X*WORLD_Y-1) tiles[i].level[2]=1;
*/
	}
	pack_fclose(pfile);
}

void MAP_CLASS::move(int dir)
{
    if (dir==LEFT)
    {
        shift_x-=4;
        if (shift_x<0)
        {
            scroll=scroll+WORLD_X-1;
            shift_x+=64;
        }
    }
    if (dir==RIGHT)
    {
        shift_x+=4;
        if (shift_x>=64)
        {
            scroll=scroll-(WORLD_X-1);
            shift_x-=64;
        }
    }
    if (dir==UP)
    {
        shift_y-=4;
        if (shift_y<0)
        {
            scroll=scroll-WORLD_X-1;
            shift_y+=32;
        }
    }

    if (dir==DOWN)
    {
        shift_y+=4;
        if (shift_y>=32)
        {
            scroll=scroll+WORLD_X+1;
            shift_y-=32;
        }
    }
    if (scroll<0) scroll+=WORLD_X*WORLD_Y;
    if (scroll>=WORLD_X*WORLD_Y) scroll-=WORLD_X*WORLD_Y;
}