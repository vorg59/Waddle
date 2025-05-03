#include <iostream>
#include "SDL2/SDL.h"
#include "SDL2/SDL_image.h"
#include "Movement.hpp"
#include "Entity.hpp"
#include "Math.hpp"
#include "threads.hpp"
using namespace std;

Movement::Movement(int p_speed)
{
	oldy=360;
	margin = 100;
	state = true;
	oCNT = qCNT = 1;
	OLDP1_Score = P1_Score = OLDP2_Score = P2_Score = 0; 
	Ball_Velocity_X = 8;
	Ball_Velocity_Y = 8;
	GokuSpeed = resetspeed = p_speed;
	facing = true;
	wPressed = sPressed = iPressed = kPressed = oPressed = qPressed = false;
}

void Movement::Input(SDL_Event& event)
{
	if (event.type == SDL_KEYDOWN) 
 	{
 	    switch (event.key.keysym.sym) 
 	    {
 	        case SDLK_i: // 'i' key
 	            iPressed = true;
 	            break;
 	        case SDLK_k: // 'k' key
 	            kPressed = true;
 	            break;
 	        case SDLK_s: // 's' key
 	            sPressed = true;
 	            break;
 	        case SDLK_w: // 'w' key
 	            wPressed = true;
 	            break;
 	        case SDLK_o: // 'o' key
 	            oPressed = true;
 	            break;
 	        case SDLK_q: // 'o' key
 	            qPressed = true;
 	            break;
 	        default:
 	            break;
 	    }
 	} 
 	else if (event.type == SDL_KEYUP)
 	{
 	    switch (event.key.keysym.sym) 
 	    {
 	        case SDLK_i: // 'i' key
 	            iPressed = false;
 	            break;
 	        case SDLK_k: // 'k' key
 	            kPressed = false;
 	            break;
 	        case SDLK_s: // 's' key
 	            sPressed = false;
 	            break;
 	        case SDLK_w: // 'w' key
 	            wPressed = false;
 	            break;
 	        case SDLK_o: // 'o' key
 	            oPressed = false;
 	            oCNT--;
 	            break;
 	        case SDLK_q: // 'q' key
 	            qPressed = false;
 	            qCNT--;
 	            cout<<"-------------------------Q = "<<qCNT; 	            
 	            break;
 	        default:
 	            break;
 	    }
 	}
}
void* Movement::threadInput(void *tparam)
{
    ThreadParamInput *param = static_cast<ThreadParamInput*>(tparam);
    param->M->Input(param->event);
    delete param;
    return NULL;
}



void Movement::Paddle_Move(Entity& E, bool isLeftPaddle, int Window_Height)
{
    Vector2f pos_f = E.getPos(); // Initialize pos_f with current position

    if (isLeftPaddle) 
    {
        if (wPressed && !sPressed && pos_f.y - 5 > margin) 
        {
            pos_f.y -= GokuSpeed;
        } 
        else if (sPressed && !wPressed && pos_f.y < Window_Height - 128) 
        {
            pos_f.y += GokuSpeed;
        }
    } 
    else 
    {
        if (iPressed && !kPressed && pos_f.y - 5 > margin)
        {
            pos_f.y -= GokuSpeed;
        } 
        else if (kPressed && !iPressed && pos_f.y < Window_Height - 128) 
        {
            pos_f.y += GokuSpeed;
        }
    }
    E.setPos(pos_f);
}

void* Movement::threadPaddle_Move(void *tparam)
{
    ThreadParamPaddle *param = static_cast<ThreadParamPaddle*>(tparam);
    param->M->Paddle_Move(param->P, param->iLP, param->WH);
    delete param;
    return NULL;
}

void Movement::Ball_Move(Entity& B, Entity& P1, Entity& P2, int Window_Width, int Window_Height)
{

	Vector2f pos_ball = B.getPos();
	// cout<<pos_ball.x<<endl;
	pos_ball.x += Ball_Velocity_X;
	pos_ball.y += Ball_Velocity_Y;


	if(pos_ball.y <= 0 + margin || pos_ball.y >= Window_Height - 32)
	{
		// if(oldy>pos_ball.y)
		// 	Ball_Velocity_Y-=1;
		// else
		// 	Ball_Velocity_Y+=1;
		Ball_Velocity_Y = - Ball_Velocity_Y;

	}


	if(pos_ball.x <= 0)
	{
		margin = 100;
	oCNT = qCNT = 1;

		GokuSpeed = resetspeed;
		Ball_Velocity_X = 8;
		Ball_Velocity_Y = 8;
		P2_Score++;
		if(P2_Score == 10)
		{
			state = false;
			return;
		}
		pos_ball.x = Window_Width/2;
		pos_ball.y = Window_Height/2;
		Ball_Velocity_X = -abs(Ball_Velocity_X);
	}
	if(pos_ball.x >= Window_Width)
	{
	oCNT = qCNT = 1;

		margin = 100;
		GokuSpeed = resetspeed;
		Ball_Velocity_X = 8;
		Ball_Velocity_Y = 8;
		P1_Score++;
		if(P1_Score == 10)
		{
			state = false;
			return;
		}
		pos_ball.x = Window_Width/2;
		pos_ball.y = Window_Height/2;
		Ball_Velocity_X = abs(Ball_Velocity_X);
	}
	OLDP1_Score = P1_Score;
	OLDP2_Score = P2_Score;


	if(pos_ball.x <= 32 && pos_ball.y >= P1.getPos().y && pos_ball.y < P1.getPos().y + 128)
	{
	margin++;

		GokuSpeed+=1;
		pos_ball.x = 32;
		// Ball_Velocity_X-=1;
		// Ball_Velocity_Y-=2;
		if(qPressed && qCNT>0)
		{
			Ball_Velocity_X-=7;
			Ball_Velocity_Y = - Ball_Velocity_Y;
		}
		Ball_Velocity_X = - Ball_Velocity_X;
	}
	if(pos_ball.x >= Window_Width - 32*2 && pos_ball.y >= P2.getPos().y && pos_ball.y < P2.getPos().y + 128)
	{
	margin++;

		GokuSpeed+=1;
		pos_ball.x = Window_Width - 64;
		// Ball_Velocity_X+=1;
		// Ball_Velocity_Y+=2;
		if(oPressed && oCNT>0)
		{
			Ball_Velocity_X+=7;
			Ball_Velocity_Y = - Ball_Velocity_Y;
		}
		Ball_Velocity_X = - Ball_Velocity_X;
	}
	oldy=pos_ball.y;
	B.setPos(pos_ball);
}

void* Movement::threadBall_Move(void *tparam)
{
    ThreadParamBall *param = static_cast<ThreadParamBall*>(tparam);
    param->M->Ball_Move(param->B, param->P1, param->P2, param->WW, param->WH);
    delete param;
    return NULL;
}
