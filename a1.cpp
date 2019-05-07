// You may use any or all of this code in your assignment!
// See makefile for compiling instructions

#include <cstdlib>
#include <iostream>
#include <unistd.h>
#include <sys/time.h>
#include <vector>
#include <string>
#include <sstream>

#include <X11/Xlib.h>
#include <X11/Xutil.h>
using namespace std;

Colormap cmap;
XColor blue,red,magenta,yellow,green;

// Define Displayable sturctures
struct XInfo{
  Display* display;
  Window window;
  GC gc;
  Pixmap pm;
};

class Displayable {
  public: 
  	virtual void paint(XInfo &xinfo)=0;
};

class Ball: public Displayable{
  public:
    int size;
    int x; 
    int y;
    int dirx;
    int diry;
    virtual void paint(XInfo &xinfo){
   	XFillArc(xinfo.display,xinfo.pm,xinfo.gc,
		 x-size/2,y-size/2,size,size,0,360*64);
  }   	
    Ball(int size, int x, int y, int dirx, int diry):
	size(size), x(x), y(y), dirx(dirx), diry(diry){}
};

class Rec: public Displayable{
  public:
    int x;
    int y;
    int width;
    int height;
    XColor* color;
    virtual void paint(XInfo &xinfo){
	if (color != NULL)
	XSetForeground(xinfo.display,xinfo.gc,color->pixel);
	XFillRectangle(xinfo.display,xinfo.pm,xinfo.gc,
		       x,y,width,height);
  }  
    Rec(int x, int y, int width, int height, XColor* color):
 	x(x), y(y), width(width), height(height), color(color){}
};

class Text: public Displayable {
public:
  int x;
  int y;
  string s;
  int value;
  virtual void paint(XInfo &xinfo){
    string str = s;
    if(value != -1){
      ostringstream ss;
      ss<<value;
      str = s + ss.str();
    }
    XSetForeground(xinfo.display,xinfo.gc,BlackPixel(xinfo.display, DefaultScreen(xinfo.display)));
    XDrawImageString(xinfo.display,xinfo.pm,xinfo.gc,x,y,str.c_str(),str.length());
  }
  Text(int x, int y, string s, int v):
	x(x),y(y),s(s),value(v){};
};

// X11 structures
Display* display;
Window window;

// fixed frames per second animation
int FPS = 60;
int speed = 5;

// get current time
unsigned long now() {
	timeval tv;
	gettimeofday(&tv, NULL);
	return tv.tv_sec * 1000000 + tv.tv_usec;
}

// withInRange
bool withInRange(int a, int b){
  if(a<=b+speed/2 && a>=b-speed/2){
    return true;
  }
  return false;
}

// bounce function
int bounce(Ball* ball, Rec* rec){
  int radius = ball->size/2;
  int result = 0;
  int odirx = ball->dirx;
  int odiry = ball->diry;
  int by = ball->y;
  int bx = ball->x;
  int ry = rec->y;
  int rx = rec->x;
  int rwidth = rec->width;
  int rheight = rec->height;

  //if (bx > rx && bx < rx+rwidth && by > ry && by < ry+rheight){
    // ball is inside block
    //ball->dirx = -ball->dirx;
   // return 1;
  //}

  // bounce upper panel
  if (withInRange(by+radius,ry) && bx + radius >= rx && bx - radius <= rx + rwidth ){
    ball->diry = -(ball->diry);
    result = 1;
  }
  //bounce left side panel
  if (withInRange(bx+radius,rx) && by + radius >= ry && by - radius <= ry + rheight ){
    ball->dirx = -(ball->dirx);
    result = 1;
  }
  //bounce bottom panel
  if (ball->diry == odiry &&  withInRange(by-radius,ry+rheight) && bx + radius >= rx && bx - radius <= rx + rwidth){
    ball->diry = -(ball->diry);
    result = 1;
  }
  //bounce right side panel
  if (ball->dirx == odirx && withInRange(bx-radius,rx+rwidth) && by + radius >= ry && by - radius <= ry + rheight){
    ball->dirx = -(ball->dirx);
    result = 1;
  }

  if (odirx == -ball->dirx && odiry == -ball->diry){
    if (withInRange(by+radius,ry)){
      if (bx - radius < rx && bx + radius >= rx){
        //top left corner
	if (odirx>0 && odiry<0){
          ball->diry = odiry;
        } else if (odirx<0&&odiry>0){
          ball->dirx = odirx;
        }
      } else if (bx + radius > rx + rwidth && bx - radius <= rx + rwidth){
	//top right corner
	if (odirx>0 && odiry>0){
	  ball->dirx = odirx;
        } else if (odirx<0 && odiry<0){
	  ball->diry = odiry;
        }
      } else {
	cout<<"UPPER PANEL ERROR"<<endl;
      }
    } else if (withInRange(by-radius,ry+rheight)){
       if (bx + radius > rx + rwidth && bx - radius <= rx + rwidth){
	//bottom right corner
	if (odirx<0 && odiry>0){
	  ball->diry = odiry;
	} else if (odirx>0 && odiry<0){
	  ball->dirx = odirx;
	}
       } else if (bx - radius < rx && bx + radius >= rx){
	//bottom left cornor
	if (odirx>0 && odiry>0){
     	  ball->diry = odiry;
        } else if (odirx<0 && odiry<0){
	  ball->dirx = odirx;
	}
      } else {
        cout<<"BOTTOM PANEL ERROR"<<endl;
      }
    } else {
      cout<<"I DON'T KNOW WHERE I AM!!!"<<endl;
    }
  }  
  return result;
}

// function to repaint a list of Rec
void repaint (vector<Rec*> rlist, XInfo xinfo){
  vector<Rec*>::const_iterator begin = rlist.begin();
  vector<Rec*>::const_iterator end = rlist.end();
  
  while(begin!=end){
    Rec* d = *begin;
    d->paint(xinfo);
    begin++;
  }
}

// function to repaint a list of Displayable
void repaint (vector<Displayable*> dlist, XInfo xinfo){
  vector<Displayable*>::const_iterator begin = dlist.begin();
  vector<Displayable*>::const_iterator end = dlist.end();

  while(begin!=end){
    Displayable* d = *begin;
    d->paint(xinfo);
    begin++;
  }
}

// function to randomly return a color
XColor* roll_color(){
  int roll = rand()%5;
  XColor * xc;
  if (roll == 0){
    xc = &blue;
  } else if (roll == 1){
    xc = &red;
  } else if (roll == 2){
    xc = &magenta;
  } else if (roll == 3){
    xc = &yellow;
  } else {
    xc = &green;
  }
  return xc;
}

// function to create a new 5*10 block matrix 
vector<Rec*> createBlockMatrix(){
  vector<Rec*> reclist;
  int roll = rand()%3;
  XColor* xc;
  int x,y;
  if (roll == 0){
    x = 150;
    y = 200;
    for (int i = 0;i < 5;i++){
      for (int j = 0; j < 10; j++){
        xc = roll_color();
        reclist.push_back(new Rec(x,y,80,30,xc));
        x += 91;
      }
      y += 41;
      if (i%2==0){ 
        x = 230;
      } else {
        x = 150;
      }
    }
  } else if (roll == 1){
    x = 190;
    y = 200;
    for (int i = 0; i < 5;i++){
      xc = roll_color();
      for (int j = 0;j<10;j++){
        reclist.push_back(new Rec(x,y,80,30,xc));
        x += 91;
      }
      x = 190;
      y += 41;
    }
  } else if (roll == 2){
    x = 190;
    y = 150;
    for (int i = 0;i<2;i++){
      xc = roll_color();
      for (int j = 0;j<10;j++){
        reclist.push_back(new Rec(x,y,80,30,xc));
        x += 91;
      }
      x = 190;
      y += 6*41;
    }
    x = 190;
    y = 150+41;
    for (int i = 0;i<2;i++){
      xc = roll_color();
      for (int j = 0;j<5;j++){
        reclist.push_back(new Rec(x,y,80,30,xc));
        y+=41;
      }
      x += 9*91;
      y = 191;
    }
    x = 190+91;
    y = 150+41;
    int dis = 7*91;
    xc = roll_color();
    for (int j = 0;j<2;j++){
      for (int i = 0;i<4;i++){
          reclist.push_back(new Rec(x,y,80,30,xc));
          reclist.push_back(new Rec(x+dis,y,80,30,xc));
          x += 91;
          y += 41;
          dis -= 2*91;
      }
      x = 190+91;
      y = 150+82;
      dis = 7*91;
    }
    xc = roll_color();
    x = 190+ 2*91;
    y = 100;
    reclist.push_back(new Rec(x,y,80,30,xc));
    reclist.push_back(new Rec(x,y-41,80,30,xc));
    reclist.push_back(new Rec(x+5*91,y,80,30,xc));
    reclist.push_back(new Rec(x+5*91,y-41,80,30,xc));
  }
  return reclist;
}  

// list clear
void clear_list(vector<Rec*> rlist){
  vector<Rec*>::const_iterator begin = rlist.begin();
  vector<Rec*>::const_iterator end = rlist.end();

  while(begin!=end){
    delete *begin;
    begin++;
  }
}

// flag that whether the ball start to fly
int start = 0;


// flag that which stage is the game in
// =0 introduction stage
// =1 game starts
// =2 game over
int stage = 0;

// flag that inditate hardness level
int mode = 0;

// entry point
int main( int argc, char *argv[] ) {
	double s;
	if (argc == 3) {
		FPS = atoi(argv[1]);
		s = atoi(argv[2]);
	} else if (argc == 2){
		FPS = atoi(argv[1]);
		cout<<"Missing speed parameter. Speed is set to default"<<endl;
  	} else if (argc == 1){
		cout<<"Missing FPS parameter. FPS is set to default"<<endl;
		cout<<"Missing speed parameter. Speed is set to default"<<endl;
	}
	if (FPS>60) FPS = 60;
	if (FPS<10) FPS = 10;
	if (s > 10) s = 10;
	if (s < 1) s = 1;
	Text speedText(20,60,"Speed: ",(int)s);
	s = s* 60 /FPS;
	if(s!=1) s /= 2;
	speed = (int)s;
	
	// create window
	XInfo xinfo;
	xinfo.display = XOpenDisplay("");
	if (xinfo.display == NULL) exit (-1);
	int screennum = DefaultScreen(xinfo.display);
	long background = WhitePixel(xinfo.display, screennum);
	long foreground = BlackPixel(xinfo.display, screennum);
	xinfo.window = XCreateSimpleWindow(xinfo.display, DefaultRootWindow(xinfo.display),
                            10, 10, 1280, 800, 2, foreground, background);
	
	// prevent resize action
	XSizeHints* sh = XAllocSizeHints();
	sh->flags = PMinSize | PMaxSize;
	sh->min_width = sh->max_width = 1280;
	sh->min_height = sh->max_height = 800;
	XSetWMNormalHints(xinfo.display,xinfo.window,sh);
	XFree(sh);

	// set events to monitor and display window
	XSelectInput(xinfo.display, xinfo.window, ButtonPressMask | KeyPressMask);
	XMapRaised(xinfo.display, xinfo.window);
	XFlush(xinfo.display);
	
	// create gc for drawing
	xinfo.gc = XCreateGC(xinfo.display, xinfo.window, 0, 0);
	XWindowAttributes w;
	XGetWindowAttributes(xinfo.display, xinfo.window, &w);
	XSetForeground(xinfo.display,xinfo.gc,foreground);
	XSetBackground(xinfo.display,xinfo.gc,background);
	
	// color initialization
	cmap = DefaultColormap(xinfo.display,DefaultScreen(xinfo.display));
	XAllocNamedColor(xinfo.display,cmap,"blue",&blue,&blue);
	XAllocNamedColor(xinfo.display,cmap,"red",&red,&red);
	XAllocNamedColor(xinfo.display,cmap,"magenta",&magenta,&magenta);
	XAllocNamedColor(xinfo.display,cmap,"yellow",&yellow,&yellow);
	XAllocNamedColor(xinfo.display,cmap,"green",&green,&green);

	// double buffer
	int depth = DefaultDepth(xinfo.display,DefaultScreen(xinfo.display));
	Pixmap buffer = XCreatePixmap(xinfo.display,xinfo.window,w.width,w.height,depth);
	xinfo.pm = buffer;

	// save time of last window paint
	unsigned long lastRepaint = 0;

	// event handle for current event
	XEvent event;
	
	// creating objects in stage 0
	vector<Displayable *> dlist0;
	Text welcome1(400,290,"Welcome to the World of BreakOut!",-1);
	Text welcome2(400,310,"This is a game that you use a pad to control a ball to break ALL BRICKS THAT BLOCKS YOU",-1);
	Text welcome3(400,330,"Please press <e> to start this game in easy mode",-1);
	Text welcome4(400,350,"Please press <n> to start this game in normal mode",-1);
	Text welcome5(400,370,"Please press <h> to start this game in hard mode",-1);
	Text welcome6(400,390,"Please press <space> to start this game in TA mode",-1);
	Text welcome7(400,410,"Please press <q> to quit this game",-1);
	Text welcome8(400,430,"Developed by Mingkun Ni(userid:m8ni)",-1);
	Text controlMenu1(400,450,"Game control instruction",-1);
	Text controlMenu2(400,470,"Press <a> to move pad leftword",-1);
	Text controlMenu3(400,490,"Press <d> to move pad rightword",-1);
	Text controlMenu4(400,510,"Press <r> to restart the game",-1);
	dlist0.push_back(&welcome1);
	dlist0.push_back(&welcome2);
	dlist0.push_back(&welcome3);
	dlist0.push_back(&welcome4);
	dlist0.push_back(&welcome5);
	dlist0.push_back(&welcome6);
	dlist0.push_back(&welcome7);
	dlist0.push_back(&welcome8);
	dlist0.push_back(&controlMenu1);
	dlist0.push_back(&controlMenu2);
	dlist0.push_back(&controlMenu3);
	dlist0.push_back(&controlMenu4);
	
	// creating objects in stage 1
	Ball ball(12,640,690,0,0);
	Rec rec(590,700,100,5,NULL);
     	vector<Rec*> rlist = createBlockMatrix();
	Text score(20,20,"Score: ",0);
	Text fps(20,40,"FPS: ",FPS);
	Text control1(600,600,"Press <a> to move pad leftword",-1);
	Text control2(600,620,"Press <d> to move pad rightword",-1);
	Text control3(600,640,"Press <r> to recreate all blocks",-1);
	Text control4(600,660,"Press <q> to quit the game",-1);
	vector<Displayable*> dlist;
	vector<Displayable*> dlist1;
	dlist.push_back(&ball);
	dlist.push_back(&rec);
	dlist.push_back(&score);
	dlist.push_back(&fps);
	dlist.push_back(&speedText);
	dlist1.push_back(&control1);
	dlist1.push_back(&control2);
	dlist1.push_back(&control3);
	dlist1.push_back(&control4);

	// creating objects in stage2
	Text quit(600,400,"GAME OVER!!!",-1);
	Text restart(600,440,"Press <r> to retry",-1);
	Text end(600,460,"Press <q> to quit the game",-1);
	vector<Displayable*> dlist2;
	dlist2.push_back(&quit);
	dlist2.push_back(&restart);
	dlist2.push_back(&end);

	
	// event loop
	while ( true ) {

		// process if we have any events
		if (XPending(xinfo.display) > 0) { 
			XNextEvent( xinfo.display, &event ); 
			switch ( event.type ) {
				// mouse button press
				case ButtonPress:
					cout << "CLICK" << endl;
					break;

				case KeyPress: // any keypress
					KeySym key;
					char text[10];
					int i = XLookupString( (XKeyEvent*)&event, text, 10, &key, 0 );
					if (stage == 0){
						if (i == 1 && text[0]== 'n'){
							stage = 1;
							rec.width = 100;
							rec.x = 590;
							mode = 1;
						} else if (i == 1 && text[0]== 'e'){
							stage = 1;
							rec.width = 200;
							rec.x = 540;
							mode = 0;
						} else if (i == 1 && text[0]=='h'){
							stage = 1;
							rec.width = 50;
							rec.x = 615;
							mode = 2;
						} else if (i == 1 && text[0]==' '){
							stage = 1;
							rec.width = 1200;
							rec.x = 40;
							mode = 3;
						}
					} else if (stage == 1){
						// move right
						if ( i == 1 && text[0] == 'd' ) {
							if(rec.x+rec.width<=1240){
								rec.x += 40;
								if(!start) ball.x+=40;
							}
						}

						// move left
						if ( i == 1 && text[0] == 'a' ) {
							if (rec.x>=40){
								rec.x -= 40;
								if(!start) ball.x-=40;
							}
						}
					
						// recreate blocks
						if (i == 1 && text[0]=='r'){
							clear_list(rlist);
							rlist = createBlockMatrix();
						}
					
						// ball starts to fly
						if (i==1 && text[0]==' '){
							if (!start){
								start=1;
								ball.dirx = (speed>0)? -speed:speed;
								ball.diry = (speed>0)? -speed:speed;
							}
						}
					} else if (stage == 2){
						// restart game
						if (i==1 && text[0]== 'r'){
							stage = 0;
							score.value = 0;
							ball.x = 640;
							ball.y = 690;
							ball.dirx = 0;
							ball.diry = 0;
							if (mode == 0){
								rec.x = 540;
							} else if (mode == 1){
								rec.x = 590;
							} else if (mode == 2){
								rec.x = 615;
							} else if (mode == 3){
								rec.x = 40;
							}
							start = 0;
							clear_list(rlist);
							rlist = createBlockMatrix();
						}
					}
					// quit game
					if ( i == 1 && text[0] == 'q' ) {
						XCloseDisplay(xinfo.display);
						exit(0);
					}
					break;
				}
			}
			

		unsigned long end = now();	// get current time in microsecond

		if (end - lastRepaint > 1000000 / FPS) {
 
			XSetForeground(xinfo.display,xinfo.gc,WhitePixel(xinfo.display,screennum));
			XFillRectangle(xinfo.display,xinfo.pm,xinfo.gc,
					0,0,w.width,w.height);
				

			// clear background
			XClearWindow(xinfo.display, xinfo.window);
			XSetForeground(xinfo.display,xinfo.gc,foreground);

			if (stage == 0){
				repaint(dlist0,xinfo);
			} else if (stage == 1){	
				repaint(dlist,xinfo);
				if (!start) repaint(dlist1,xinfo);
				
				// update ball position
				ball.x += ball.dirx;
				ball.y += ball.diry;
				if (ball.y+ball.size/2>800){
					stage = 2;
				}

				// bounce ball
				if (ball.x + ball.size/2 > w.width ||
					ball.x - ball.size/2 < 0){
					ball.dirx = -ball.dirx;
					if (rlist.size()==0) rlist=createBlockMatrix();
				}
				if (ball.y + ball.size/2 > w.height ||
					ball.y - ball.size/2 < 0){
					ball.diry = -ball.diry;
					if (rlist.size()==0) rlist=createBlockMatrix();
				}
				// ball hits the panel
				if(start){
					bounce(&ball,&rec);
					for(int i = 0;i<rlist.size();i++){
						int j = bounce(&ball,rlist[i]);
						if (j) {
							Rec* r = *(rlist.begin()+i);
							delete r;
							rlist.erase(rlist.begin()+i);
							score.value++;
							if (score.value % 50==0){
								XColor* color = roll_color();
								rec.color = color;
							}
						}
					}	
				}
				repaint(rlist,xinfo);	 
			} else if (stage == 2){
				repaint(dlist2,xinfo);
			}
		
			XCopyArea(xinfo.display,xinfo.pm,xinfo.window,xinfo.gc,
				0,0,w.width,w.height,0,0);
	
			XFlush( xinfo.display );
			
			lastRepaint = now(); // remember when the paint happened
		}

		// IMPORTANT: sleep for a bit to let other processes work
		if (XPending(xinfo.display) == 0) {
			usleep(1000000 / FPS - (now() - lastRepaint));
		}
	}
}
