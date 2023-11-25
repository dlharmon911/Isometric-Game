#include "main.h"

#define MAX_BUTTON      12

RGB_MAP rgb_table;
COLOR_MAP light_table;
COLOR_MAP trans_table;

BITMAP *buffer;
DATAFILE *data;
FONT *my_font;
int frame;
bool game_pause,gameover,sound_on;
int FONT_H;
BUTTON_CLASS button[MAX_BUTTON];
char status[40];
bool DAN;
volatile int timer=1;

void inc_timer(void)
{
    if (!game_pause)
    {
        timer++;
    }
}
END_OF_FUNCTION(inc_timer);

void doStatus(const char *tx)
{
    strcpy(status,tx);
}

int distance(int x,int y)
{
    return fixtoi(fixhypot(itofix(x),itofix(y)));
}

// initialize allegro and the data
int initAll()
{
    allegro_init();
    install_keyboard();
    install_timer();
    install_mouse();
	
    if (install_sound(DIGI_AUTODETECT, MIDI_AUTODETECT, NULL) != 0) 
    {
        allegro_message("Unable initialize sound module\n%s\n", allegro_error);
        return -1;
    }

    if (set_gfx_mode(GFX_AUTODETECT_WINDOWED, 640, 480, 0, 0)!=0)
    {
        allegro_message("Unable initialize graphics module\n%s\n", allegro_error);
        return -1;
    }

    packfile_password(PASSWORD);
    data=load_datafile("data.dat");
    packfile_password("");

    set_palette((RGB*)data[MY_PAL].dat);
    my_font=(FONT*)data[FONT01].dat;
    FONT_H=text_height(my_font);

    create_rgb_table(&rgb_table,(RGB*)data[MY_PAL].dat,NULL);
    rgb_map = &rgb_table;

    create_light_table(&light_table,(RGB*)data[MY_PAL].dat, 0, 0, 0,NULL);

    create_trans_table(&trans_table,(RGB*)data[MY_PAL].dat, 128, 128, 128,NULL);
	
    buffer=create_bitmap(SCREEN_W,SCREEN_H);
    clear(buffer);

    srand(time(NULL));
    text_mode(-1);

	LOCK_VARIABLE(timer);
	LOCK_FUNCTION(inc_timer);

 	install_int(inc_timer,1000);

    doStatus("");

    map.setup();
    player.setup();

    button[0].create(10,388,32,32,(BITMAP*)data[BUTTON01].dat,"Move Cursor");
    button[1].create(46,388,32,32,(BITMAP*)data[BUTTON02].dat,"Use/Grab Item");
    button[2].create(82,388,32,32,(BITMAP*)data[BUTTON03].dat,"Fight");
    button[3].create(118,388,32,32,(BITMAP*)data[BUTTON04].dat,"Talk");
    button[4].create(154,388,32,32,(BITMAP*)data[BUTTON05].dat,"Cast Spell");
    button[5].create(190,388,32,32,(BITMAP*)data[BUTTON06].dat,"Check Inventory");

    button[6].create(10,424,32,32,(BITMAP*)data[BUTTON07].dat,"Sound Is On");
    button[7].create(46,424,32,32,(BITMAP*)data[BUTTON08].dat,"Change Options");
    button[8].create(82,424,32,32,(BITMAP*)data[BUTTON09].dat,"Load Saved Game");
    button[9].create(118,424,32,32,(BITMAP*)data[BUTTON10].dat,"Save Current Game");
    button[10].create(154,424,32,32,(BITMAP*)data[BUTTON11].dat,"Game Help");
    button[11].create(190,424,32,32,(BITMAP*)data[BUTTON12].dat,"Quit Game");

    mouse.init();

    frame=0;
    game_pause=false;
    gameover=false;
    sound_on=true;

    DAN=false;
    if (exists("fake.bat")) DAN=true;

    astar = new AstarPathFinderClass(map);

    return 0;
}

// free all memory used
void deleteAll()
{
    mouse.uninit();
    player.uninit();
    destroy_bitmap(buffer);
    unload_datafile(data);
    if (astar)
        delete astar;
}        

// draw everything
void drawAll()
{    
    doStatus("");
    map.draw(buffer);    

    //draw_sprite(buffer,(BITMAP*)data[BACK].dat,0,0);
    drawWindow(buffer,0,WINDOW_TOP,SCREEN_W-1,SCREEN_H-1,UP);

    for (int i=0; i<MAX_BUTTON; i++)
    {
        button[i].draw(buffer);
    }

    drawWindow(buffer,10,458,221,476,DOWN);
    dtextout(buffer,(FONT*)data[FONT02].dat,status,10,459,COLOR_UP,COLOR_DOWN);
   
    text_mode(0);
    textprintf(buffer,font,20,20,255,"FPS: %d",frame/timer);
    text_mode(-1);

    drawScreen(buffer);
    frame++;
}

// draw the buffer to the screen
void drawScreen(BITMAP *bmp)
{
    int x,y;
    x=mouse_x;
    y=mouse_y;

    blit(bmp,mouse.under,x,y,0,0,32,32);
    mouse.draw(bmp);

    acquire_screen();
    blit(bmp,screen,0,0,0,0,SCREEN_W,SCREEN_H);
    release_screen();

    blit(mouse.under,bmp,0,0,x,y,32,32);
}

void doScreenSave()
{
    char name[40];
    int p=0;

    do
    {
        sprintf(name,"sshot%02d.bmp",p);
        p++;
    } while (exists(name));

    save_bmp(name,buffer,(RGB*)data[MY_PAL].dat);
}

void check_buttons(int x, int y)
{
    if (button[0].isMouse(x,y))
    {
        mouse.type=1;
    }
    if (button[1].isMouse(x,y))
    {
        mouse.type=2;
    }

    if (button[6].isMouse(x,y))
    {
        if (sound_on)
        {
            sound_on=false;
            strcpy(button[6].alt,"Sound Is Off");
        }
        else
        {
            sound_on=true;
            strcpy(button[6].alt,"Sound Is On");
        }
    }

    if (button[8].isMouse(x,y))
    {
        if (doQuestion(buffer,-1,-1,4," Yes "," No ",
            "Are you sure that you want",
            "to load a saved game?",
            "You will lose current",
            "information!"))
        {
            player.load();
        }
    }
    if (button[9].isMouse(x,y))
    {
        player.save();
    }


    if (button[10].isMouse(x,y))
    {
        doGameHelp();
    }
    if (button[11].isMouse(x,y))
    {
        gameover=true;
    }
}

void goodbye()
{
    fade_out(4);
    clear(screen);
    set_palette((RGB*)data[MY_PAL].dat);
    acquire_screen();
    textout_centre(screen,my_font,"Good Bye!",320,240-FONT_H/2,255);
    release_screen();
    rest(1000);
    fade_out(4);
    rest(1000);
}

// main loop
int main(int32_t argc, char** argv)
{
    bool done=false;
    int cnt=0,destX,destY;

    if (initAll()<0) return -1;

    destX=player.x_pos;
    destY=player.y_pos;

    while (!done)
    {               
        drawAll();

        if (key[KEY_F1]) 
        {
            game_pause=true;
            if (DAN || pass())
            {
                map.edit();
            }
            frame = 0;
            game_pause=false;
        }

            if (mouse_x==0)
            {
                map.move(LEFT);
            }
            if (mouse_y==0)
            {
                map.move(UP);
            }
            if (mouse_x==(SCREEN_W-1))
            {
                map.move(RIGHT);
            }
            if (mouse_y==(SCREEN_H-1))
            {
                map.move(DOWN);
            }

        if (mouse.click())
        {  
            if (mouse_y<WINDOW_TOP)
            {
                if (mouse.type==1)
                {
                    player.set_movement(mouse_x+9,mouse_y+6);
                }
            }
            else
            {
                check_buttons(mouse_x,mouse_y);
            }
        }

        player.move();

        if (key[KEY_UP]) player.do_move(UP);
        if (key[KEY_DOWN]) player.do_move(DOWN);
        if (key[KEY_LEFT]) player.do_move(LEFT);
        if (key[KEY_RIGHT]) player.do_move(RIGHT);
/*
        if (mouse_b & 1)
        {
            if (astar->NewPath(player.x_pos, player.y_pos, mouse_x,mouse_y))
            {
                astar->PathNextNode();
                destX = astar->NodeGetX();
                destY = astar->NodeGetY();
            }
        }

        if ( !astar->ReachedGoal() && (destX==player.x_pos) && (destY==player.y_pos) )
        {
            astar->PathNextNode();
            destX = astar->NodeGetX();
            destY = astar->NodeGetY();
        }

        if ( destX > player.x_pos ) 
        {
            player.do_move(RIGHT);
        }

        if ( destY > player.y_pos ) 
        {
            player.do_move(DOWN);
        }

        if ( destX < player.x_pos )
        {
            player.do_move(LEFT);
        }

        if ( destY < player.y_pos ) 
        {
            player.do_move(UP);
        }
*/
        if (key[KEY_F10] || (DAN && key[KEY_ESC]))
        {            
            done=true;
            gameover=false;
        }

        if (key[KEY_LCONTROL] && key[KEY_S])
        {
            game_pause=true;
            while (key[KEY_LCONTROL] && key[KEY_S]) {}   
            doScreenSave();
            game_pause=false;
        }

        if (gameover)
        {
            gameover=false;
            if (doQuestion(buffer,-1,-1,4," Yes ","No",
                "Are you sure that you",
                "want to end your game?",
                "You will lose any unsaved",
                "information!"))
            {
                done=true;
            }
        }
	}

//    goodbye();

    deleteAll();
    return 1;
}
END_OF_MAIN()
