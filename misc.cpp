#include "main.h"

void picFill(BITMAP *source,BITMAP *dest,int sx, int sy, int dx, int dy, int w, int h)
{
    int aw,ah,pw,ph,x,y,l;
    aw=pw=source->w;
    ah=ph=source->h;

    x=w/pw+1;
    y=h/ph+1;
    l=h;

    for (int i=0; i<x; i++)
    {
        pw=aw;
        if (pw>w) pw=w;
        w-=pw;
        h=l;
        for (int j=0; j<y; j++)
        {
            ph=ah;
            if (ph>h) ph=h;
            h-=ph;
            blit(source,dest,0,0,dx+aw*i,dy+ah*j,pw,ph);
        }
    }
}

void drawWindow(BITMAP *bmp, int x, int y, int x2, int y2,int flag)
{
    BITMAP *b=(BITMAP*)data[PAPER1].dat;

    if (flag==DOWN)
    {
        b=(BITMAP*)data[PAPER2].dat;
    }

    picFill(b,bmp,0,0,x,y,x2-x,y2-y);
    drawBorder(bmp,x,y,x2,y2,flag);
}

void drawBorder(BITMAP *bmp, int x, int y, int x2, int y2,int flag)
{
    int c1,c2;
    c1=COLOR_UP;
    c2=COLOR_DOWN;

    if (flag==DOWN)
    {
        c2=COLOR_UP;
        c1=COLOR_DOWN;
    }

    hline(bmp,x ,y ,x2,c1);
    vline(bmp,x ,y ,y2,c1);
    hline(bmp,x ,y2,x2,c2);
    vline(bmp,x2,y ,y2,c2);    
}

void dtextout(BITMAP *bmp, const FONT *f, const char *tx, int x, int y, int fg,int bg)
{
    textout(bmp,f,tx,x+1,y+1,bg);
    textout(bmp,f,tx,x,y,fg);
}

void dtextout_centre(BITMAP *bmp, const FONT *f, const char *tx, int x, int y, int fg,int bg)
{
    textout_centre(bmp,f,tx,x+1,y+1,bg);
    textout_centre(bmp,f,tx,x,y,fg);
}

void dtextprintf(BITMAP *bmp, const FONT *f, int x,int y,int fg,int bg, const char *format, ...)
{
    char buf[512];

    va_list ap;
    va_start(ap, format);
    vsprintf(buf,format,ap); 
    va_end(ap);

    dtextout(bmp, f, buf, x, y, fg,bg);
}

void dtextprintf_centre(BITMAP *bmp, const FONT *f, int x,int y,int fg,int bg, const char *format, ...)
{
    char buf[512];

    va_list ap;
    va_start(ap, format);
    vsprintf(buf,format,ap); 
    va_end(ap);

    dtextout_centre(bmp, f, buf, x, y, fg,bg);
}

bool pass()
{
    char str[80]="Please Enter Password to Edit Map!",
        funny[5]={'h','u','m','o','r'},ch;
    int i=0,l;
    BITMAP *under;

    l=text_length(my_font,"Please Enter Password to Edit Map");
    under=getBehind(buffer,SCREEN_W/2-l/2-10,SCREEN_H/2-20,SCREEN_W/2+l/2+10,SCREEN_H/2+20);
    drawWindow(buffer,SCREEN_W/2-l/2-10,SCREEN_H/2-20,SCREEN_W/2+l/2+10,SCREEN_H/2+20,UP);
    dtextout_centre(buffer,my_font,str,SCREEN_W/2,(SCREEN_H-FONT_H)/2,COLOR_UP,COLOR_DOWN);

    drawScreen(buffer);
    putBehind(under,buffer,SCREEN_W/2-l/2-10,SCREEN_H/2-20);
    clear_keybuf();
    do
    {
        if (keypressed())
        {
            ch=readkey() & 0xFF;
            
            if (ch!=funny[i] || ch==27)
            {
                i=99;
            }
            else
            {
                i++;
            }
            while (keypressed()) {}
            clear_keybuf();
        }
    } while (i<=4);

    if (i!=99) 
        return true;
    l=text_length(my_font,"Password Incorrect!");
    under=getBehind(buffer,SCREEN_W/2-l/2-10,SCREEN_H/2-20,SCREEN_W/2+l/2+10,SCREEN_H/2+20);
    drawWindow(buffer,SCREEN_W/2-l/2-10,SCREEN_H/2-20,SCREEN_W/2+l/2+10,SCREEN_H/2+20,UP);
    dtextout_centre(buffer,my_font,"Password Incorrect!",SCREEN_W/2,(SCREEN_H-FONT_H)/2,COLOR_UP,COLOR_DOWN);

    drawScreen(buffer);
    putBehind(under,buffer,SCREEN_W/2-l/2-10,SCREEN_H/2-20);
    clear_keybuf();    


    
    while (!keypressed()) {}

    return false;
}

BITMAP *getBehind(BITMAP *bmp,int x1,int y1,int x2,int y2)
{
    BITMAP *b=create_bitmap(x2-x1+1,y2-y1+1);
    
    blit(bmp,b,x1,y1,0,0,x2-x1+1,y2-y1+1);

    return b;
}

void putBehind(BITMAP *src,BITMAP *dest,int x,int y)
{
    if (src!=NULL)
    {
        blit(src,dest,0,0,x,y,src->w,src->h);
        destroy_bitmap(src);
        src=NULL;
    }
}

void doMessage(BITMAP *bmp,int x, int y,int amount,const char *tx,...)
{
    int width,height,f_width,t,b_width;
    BUTTON_CLASS button;
    char text[80],lines[15][80];
	va_list marker;
    BITMAP *under;
    bool done=false;

    if (amount>15) amount=15;

    t=mouse.type;
    mouse.type=0;

    b_width=text_length(my_font,tx);
    width=b_width;
    height=((FONT_H+10)*(amount+1))+10;

    va_start(marker,tx);
   
	for (int i=0; i<amount; i++)
    {
		strcpy(text,va_arg(marker,char *));

		f_width=text_length(my_font,text);
			
		if (f_width>width) 
        {
            width=f_width;
        }

		strcpy(lines[i],text);
	}

	va_end(marker);  

    width+=24;
    if (x==-1 || y==-1)
    {
        x=(SCREEN_W-width)/2;
        y=(SCREEN_H-height)/2;
    }

    button.create(x+(width-text_length(my_font,tx)-16)/2,y+height-FONT_H-16,b_width+8,FONT_H+6,tx);
    under=getBehind(bmp,x,y,width-1,height-1);
    drawWindow(bmp,x,y,x+width-1,y+height-1,UP);

    for (int i=0; i<amount; i++)
    {
        dtextout_centre(bmp,my_font,lines[i],x+width/2,y+(10+FONT_H)*i+10,COLOR_UP,COLOR_DOWN);
    }

    while (!done) 
    {
        button.draw(bmp);
        drawScreen(bmp);

        if (key[KEY_ESC]) 
        {
            done=true;
            while (key[KEY_ESC]) {}
        }

        if ((mouse.click() && button.isMouse(mouse_x,mouse_y)))
        {
            done=true;
        }
    }

    putBehind(under,bmp,x,y);
    mouse.type=t;
}

bool doQuestion(BITMAP* bmp, int x, int y, int amount, const char* tx1, const char* tx2, ...)
{
    int width, height, f_width, t, b_width;
    BUTTON_CLASS button[2];
    char text[80],lines[15][80];
	va_list marker;
    BITMAP *under;
    bool done=false;

    if (amount>15) amount=15;

    t=mouse.type;
    mouse.type=0;

    b_width=text_length(my_font,tx1);

    if (text_length(my_font,tx2)>b_width)
    {
        b_width=text_length(my_font,tx2);
    }

    width=b_width*2+16;
    height=((FONT_H+10)*(amount+1))+10;

    va_start(marker,tx2);
   
	for (int i=0; i<amount; i++)
    {
		strcpy(text,va_arg(marker,char *));

		f_width=text_length(my_font,text);
			
		if (f_width>width) 
        {
            width=f_width;
        }

		strcpy(lines[i],text);
	}

	va_end(marker);  

    width+=24;
    if (x==-1 || y==-1)
    {
        x=(SCREEN_W-width)/2;
        y=(SCREEN_H-height)/2;
    }

    button[0].create(x+width/2-text_length(my_font,tx1)-16,y+height-FONT_H-12,b_width+8,FONT_H+6,tx1);
    button[1].create(x+width/2+8,y+height-FONT_H-12,b_width+8,FONT_H+6,tx2);

    under=getBehind(bmp,x,y,width-1,height-1);
    drawWindow(bmp,x,y,x+width-1,y+height-1,UP);

    for (int i=0; i<amount; i++)
    {
        dtextout_centre(bmp,my_font,lines[i],x+width/2,y+(10+FONT_H)*i+10,COLOR_UP,COLOR_DOWN);
    }

    while (!done) 
    {
        button[0].draw(bmp);
        button[1].draw(bmp);
        drawScreen(bmp);

        if (key[KEY_ESC]) 
        {
            done=true;
            while (key[KEY_ESC]) {}
        }
        
        if (mouse.click())
        {
            if (button[0].isMouse(mouse_x,mouse_y) || 
                button[1].isMouse(mouse_x,mouse_y))
            done=true;
        }
    }

    putBehind(under,bmp,x,y);
    mouse.type=t;

    if (button[0].isMouse(mouse_x,mouse_y))
    {
        return true;
    }

    return false;
}

void doGameHelp()
{
    doMessage(buffer,-1,-1,10,"  Ok  ",
        "So far this is just a demo game.",
        "",
        "When the game is more complete,",
        "I will add help.",
        "",
        "In the meantime, just mess around",
        "with your guy and have fun.",
        "",
        "Email: dl_harmon@yahoo.com",
        "");
}