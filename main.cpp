#include "SDL\SDL.h"
#include "SDL\SDL_image.h"
#include <iostream>
#include <fstream>
#include <stdio.h>      
#include <stdlib.h>
#include <time.h>
using namespace std;

//timers 
Uint32 sdltimer;
Uint32 dsdltimer;
int rplayerswing;
int lplayerswing;

bool quit;
bool game_ready = true;
bool in_game = false;

//keys
bool adown;
bool sup;
bool ddown;
bool larrowdown;
bool rarrowdown;
bool darrowup;
bool spacedown;

//screen values
const int SCREEN_WIDTH = 640;
const int SCREEN_HEIGHT = 360;
const int SCREEN_BPP = 32;

//sdl stuff
SDL_Event event;

//game stuff
int lscore = 0;
int rscore = 0;

//object coords
float rx = SCREEN_WIDTH - 85.0;
float lx = 21.0;
float y = SCREEN_HEIGHT - 85.0;
float bx = SCREEN_WIDTH - 200.0;
//float bx = /*SCREEN_WIDTH -*/ 160.0;
float by = 100.0;
float bx_temp;
float by_temp;
float bxv = .1;
float coll_x;
float coll_y;

bool grounded = true;
float v = 0.0;
float a = .015;

SDL_Surface *screen = NULL;
SDL_Surface *bg = NULL;
SDL_Surface *ori = NULL;
SDL_Surface *ori2 = NULL;
SDL_Surface *ball = NULL;

SDL_Rect clips[2];

SDL_Surface *load_image( std::string filename )
{
    //The image that's loaded
    SDL_Surface* loadedImage = NULL;

    //The optimized image that will be used
    SDL_Surface* optimizedImage = NULL;

    //Load the image using SDL_image
    loadedImage = IMG_Load( filename.c_str() );

    //If the image loaded
    if( loadedImage != NULL )
    {
        //Create an optimized image
        //optimizedImage = SDL_DisplayFormat( loadedImage );
        optimizedImage = SDL_DisplayFormatAlpha( loadedImage );

        //Free the old image
        SDL_FreeSurface( loadedImage );
    }

    //Return the optimized image
    return optimizedImage;
}

void apply_surface( int x, int y, SDL_Surface* source, SDL_Surface* destination, SDL_Rect* clip = NULL )
{
    //Rectangle to hold the offsets
    SDL_Rect offset;

    //Get offsets
    offset.x = x;
    offset.y = y;

    //Blit the surface
    SDL_BlitSurface( source, clip, destination, &offset );
}

bool init()
{
    //Initialize all SDL subsystems
    if( SDL_Init( SDL_INIT_EVERYTHING ) == -1 )
    {
        return false;
    }

    //Set up the screen
    screen = SDL_SetVideoMode( SCREEN_WIDTH, SCREEN_HEIGHT, SCREEN_BPP, SDL_SWSURFACE | SDL_FULLSCREEN );

    //If there was an error in setting up the screen
    if( screen == NULL )
    {
        return false;
    }

    //Set the window caption
    SDL_WM_SetCaption( "Move Test.NOT", NULL );
    
    srand (time(NULL));

    //If everything initialized fine
    return true;
}

bool load_files()
{
	bg = load_image( "img/bg.png" );
	ball = load_image( "img/ball.png" );
	ori = load_image( "img/ori.png" );
	ori2 = load_image( "img/ori2.png" );
    
    //If everything loaded fine
    return true;    
}

void clean_up()
{
    //Free the surface
    SDL_FreeSurface( bg );
    SDL_FreeSurface( ori );

    //Quit SDL
    SDL_Quit();
}	

void extra_init(){
	clips[0].x = 0;
	clips[0].y = 0;
	clips[0].w = 64;
	clips[0].h = 64;
	
	clips[1].x = 0;
	clips[1].y = 64;
	clips[1].w = 64;
	clips[1].h = 64;
}

/*bool check_coll( float a, float b, float c, float d,
				 float e, float f, float g, float h )
{
	float xx;
	float yy;
	if ( c - a == 0.0 ){
		//check top edge
		xx = a;
		yy = f;
		if ( xx <= g && xx >= e
			&& yy <= d && xx >= b )
			{
				coll_y = yy - 1.0;
				coll_x = xx;
				return true;
			} 
			
		//check bot edge
		yy = h;
		if ( xx <= g && xx >= e
			&& yy <= d && xx >= b )
			{
				coll_y = yy + 1.0;
				coll_x = xx;
				return true;
			} 
			
		return false;
	}
	else if ( ( ( d - b ) / ( c - a ) ) == 0 ){
		//check left edge
		xx = e;
		yy = b;
		if ( xx <= c && xx >= a
			&& yy <= h && xx >= f )
			{
				coll_y = yy;
				coll_x = xx - 1.0;
				return true;
			} 
			
		//check right edge
		xx = g;
		if ( xx <= c && xx >= a
			&& yy <= h && xx >= f )
			{
				coll_y = yy;
				coll_x = xx + 1.0;
				return true;
			} 
			
		return false;
	}
	else{
		float m1 = ( ( d - b ) / ( c - a ) );
		float b1 = b - ( m1 * a );
	
		//check left edge
		yy = m1*e + b1;
		if ( yy <= d && yy >= b
			&& yy <= h && yy >= f )
			{
				coll_y = yy;
				coll_x = ( yy - b1 ) / m1 - 1.0;
				return true;
			} 
			
		//check right edge
		yy = m1*( g ) + b1;
		if ( yy <= d && yy >= b
			&& yy <= h && yy >= f )
			{
				coll_y = yy;
				coll_x = ( yy - b1 ) / m1 + 1.0;
				return true;
			} 
			
		//check top edge
		xx = ( f - b1 ) / m1;
		if ( xx <= c && xx >= a
			&& xx <= g && xx >= e )
			{
				coll_y = m1*( xx ) + b1 - 1.0;
				coll_x = xx;
				return true;
			} 
			
		//check bot edge
		xx = ( h - b1 ) / m1;
		if ( xx <= c && xx >= a
			&& xx <= g && xx >= e )
			{
				coll_y = m1*( xx ) + b1 + 1.0;
				coll_x = xx;
				return true;
			} 
			
		return false;
	}
}*/

bool check_coll( float a, float b, float c, float d,
				 float e, float f, float g, float h 
				 /*float xv, float yv*/ )
{	
	if ( a > g || b > h || c < e || d < f ){
		return false;
	}
	return true;
	
	/*int s;
	
	if ( c > a ){
		for ( int r = a; r <= c; r++ ){
			s = ((d-b)/(c-a))*(r-a) + b;
			if ( r >= e && r <= g && s >= f && s <= h )
			{
				return true;
				coll_x = r;
				coll_y = s;
			}
		}			
	}
	
	else if ( c < a ){
		for ( int r = a; r >= c; r-- ){
			s = ((d-b)/(c-a))*(r-a) + b;
			if ( r >= e && r <= g 
				 && s >= f 
				 && s <= h )
			{
				return true;
				coll_x = r;
				coll_y = s;
			}
		}	
	}
	
	else if ( c = a ){
		if ( d > b ){
			for ( int r = b; r <= d; r++ ){
				if ( a >= e && a <= g 
					 && r >= f 
					 && r <= h )
				{
					return true;
					coll_x = a;
					coll_y = r;
				}
			}	
		}
		else if ( d < b ){
			for ( int r = b; r >= d; r-- ){
				if ( a >= e && a <= g 
					 && r >= f 
					 && r <= h )
				{
					return true;
					coll_x = a;
					coll_y = r;
				}
			}	
		}	
	}
	
	return false;*/
}

int main( int argc, char* args[] ) {
	init();
	load_files();
	extra_init();
	
	while( quit == false ){
		while( SDL_PollEvent( &event ) )
        {
			//If the user has Xed out the window
            if( event.type == SDL_QUIT )
            {
                //Quit the program
                quit = true;
            }
            
            if( event.type == SDL_KEYDOWN )
            {
				switch( event.key.keysym.sym )
                {
					case SDLK_ESCAPE: 
						quit = true;
                    	break;
                    	
                    case SDLK_a: 
						adown = true;
                    	break;
                    
                    case SDLK_d: 
						ddown = true;
                    	break;
                    	
                    case SDLK_SPACE: 
						spacedown = true;
                    	break;
                    	
                    case SDLK_LEFT: 
						larrowdown = true;
                    	break;
                    	
                    case SDLK_RIGHT: 
						rarrowdown = true;
                    	break;
				}
			}
			if( event.type == SDL_KEYUP )
            {
				switch( event.key.keysym.sym )
                {
                    case SDLK_a: 
						adown = false;
                    	break;
                    	
                    case SDLK_s: 
						sup = true;
						if ( lplayerswing <= -250 ){
							lplayerswing = 500;
						}
                    	break;
                    
                    case SDLK_d: 
						ddown = false;
                    	break;
                    	
                    case SDLK_SPACE: 
						spacedown = false;
						if ( game_ready ){
							game_ready = false;
							in_game = true;
							bxv = .1;
						}
                    	break;
                    	
					case SDLK_DOWN: 
						darrowup = true;
						if ( rplayerswing <= -250 ){
							rplayerswing = 500;
						}
                    	break;
                    	
                    case SDLK_LEFT: 
						larrowdown = false;
                    	break;
                    	
                    case SDLK_RIGHT: 
						rarrowdown = false;
                    	break;
				}
			}
		}
		
		dsdltimer = SDL_GetTicks() - sdltimer;
		sdltimer = SDL_GetTicks();
		if ( lplayerswing > -10000 ){
			lplayerswing -= dsdltimer;
		}
		if ( rplayerswing > -10000 ){
			rplayerswing -= dsdltimer;
		}
        
		if ( adown ){
			lx -= dsdltimer/4.0;
		}
		if ( ddown ){
			lx += dsdltimer/4.0;
		}
		
		if ( rarrowdown ){
			rx += dsdltimer/4.0;
		}
		if ( larrowdown ){
			rx -= dsdltimer/4.0;
		}
		
		if( in_game ){
			v += a*dsdltimer/4.0;
			
			if ( v > 1.0 ){
				//v = 1.0;
			}
			
			//by_temp = by + v*dsdltimer/4.0;
			//bx_temp = bx + bxv*dsdltimer/3.0;
			
			by = by + v*dsdltimer/4.0;
			bx = bx + bxv*dsdltimer/3.0;
	
			//check collide left
			if ( check_coll( bx, by, bx + 16, by + 16,
							 lx + 25, y + 20, lx + 51, y + 30 ) 
							 && lplayerswing > 0 )
			{
				v = -3.0;
				bxv = .2 + ( rand()%50 )/100.0;
				//bx = coll_x;
				//by = coll_y;
			}
			
			//check collide right
			else if ( check_coll( bx, by, bx + 16, by + 16,
							 rx - 3, y + 20, rx + 23, y + 30 ) 
							 && rplayerswing > 0 )
			{
				v = -3.0;
				bxv = -.2 - ( rand()%50 )/100.0;
				//bx = coll_x;
				//by = coll_y;
			}
			
			//check collide pole
			else if ( check_coll( bx, by, bx + 16, by + 16,
							 (SCREEN_WIDTH/2) - 10, 208, (SCREEN_WIDTH/2), 213 ) )
			{
				if ( v > 0 ) v = -v;
				//bx = coll_x;
				//by = coll_y;
			}
			else if ( check_coll( bx, by, bx + 16, by + 16,
							 (SCREEN_WIDTH/2) - 10, 213, (SCREEN_WIDTH/2), SCREEN_HEIGHT ) )
			{
				bxv = -bxv;
				//bx = coll_x;
				//by = coll_y;
			}
			
			//no collision -> just move
			/*else{
				bx = bx_temp;
				by = by_temp;
			}*/
			
			//check bounds
			if ( by > y + 64 ){
				by = 100.0;
				v = 0;
				bxv = 0;
				//bx = /*SCREEN_WIDTH -*/ 160.0;
				bx = SCREEN_WIDTH - 200.0;
				in_game = false;
				game_ready = true;
			}
	
			if ( by > SCREEN_HEIGHT - 16.0 ){
				by = SCREEN_HEIGHT - 16.0;
				v = 0;
			} 
		}
		
		if ( rx < 320.0 ) rx = 320.0;
		if ( rx > SCREEN_WIDTH - 64.0 ) rx = SCREEN_WIDTH - 64.0;
		
		if ( lx < 0.0 ) lx = 0.0;
		if ( lx > -64.0 + SCREEN_WIDTH/2 ) lx = -64.0 + SCREEN_WIDTH/2;
		
	        
	    //rendering
	    //bg
	    apply_surface( 0, 0, bg, screen );
	    //debug coll rects
	    //SDL_FillRect( screen, &coll_rect[0], SDL_MapRGB( screen->format, 100, 100, 100 ) );
	    //ball
	    apply_surface( int(bx), int(by), ball, screen );
	    //oris
	    if ( lplayerswing > 0 ) apply_surface( int(lx), int(y), ori2, screen, &clips[1] );
	    else apply_surface( int(lx), int(y), ori2, screen, &clips[0] );
	    
	    if ( rplayerswing > 0 ) apply_surface( int(rx), int(y), ori, screen, &clips[1] );
	    else apply_surface( int(rx), int(y), ori, screen, &clips[0] );
		
		SDL_Flip( screen );
		
		darrowup = false;
		sup = false;
	}
	
	clean_up();
	return 0;
	
}
