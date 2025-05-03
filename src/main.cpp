#include <iostream>
#include <vector>
#include "SDL2/SDL.h"
#include "SDL2/SDL_image.h"
#include "Movement.hpp"
#include "RenderWindow.hpp"
#include "Entity.hpp"
#include "Math.hpp"
#include "Utils.hpp"
using namespace std;

int main(int argc, char *argv[])
{
    if (SDL_Init(SDL_INIT_VIDEO) > 0)
        cout << "HEY .. SDL_Init HAS FAILED. SDL_ERROR: " << SDL_GetError() << endl;

    if (! (IMG_Init(IMG_INIT_PNG)))
        cout << "IMG_init has failed. Error: " << SDL_GetError() << endl;

    int Window_Width = 1280;
    int Window_Height = 650;
    float paddle_Mag = 1;
    int paddle_Speed = paddle_Mag * 14;


    RenderWindow window("PingPong", Window_Width, Window_Height);

    vector<Entity> Numbers;
    {
        utils::Populate(Numbers, window);
    }

    SDL_Texture* Mar = window.loadTexture("res/gfx/margin.png");
    Entity murgin(Vector2f(0, 0), Mar);
    murgin.setcurrentFrame(0, 0, 1280, 100);

    SDL_Texture* Blizzard = window.loadTexture("res/gfx/pur.png");
    Entity background(Vector2f(0, 0), Blizzard);
    background.setcurrentFrame(0, 0, 1024, 576);

    SDL_Texture* ball = window.loadTexture("res/gfx/Bball.jpg");
    Entity Ball(Vector2f(Window_Width/2, Window_Height/2+50), ball);// x_cord, y_cord
    Ball.setcurrentFrame(0, 0, 32, 32);

    SDL_Texture* paddle = window.loadTexture("res/gfx/BpaddleL.jpg");
    Entity paddleL(Vector2f(0, 50 + Window_Height/2 - 64), paddle);// x_cord, y_cord
    paddleL.setcurrentFrame(0, 0, 32, 128);

    paddle = window.loadTexture("res/gfx/BpaddleL.jpg");
    Entity paddleR(Vector2f(Window_Width - 32, 50 + Window_Height/2 - 64), paddle);// x_cord, y_cord
    paddleR.setcurrentFrame(0, 0, 32, 128);

    SDL_Texture* oKey = window.loadTexture("res/gfx/G.png");
    Entity H(Vector2f(Window_Width/2 - 460, Window_Height/2 -256 + 50), oKey);// x_cord, y_cord
    H.setcurrentFrame(0, 0, 512, 512);

    oKey = window.loadTexture("res/gfx/GI.png");
    Entity F(Vector2f(Window_Width/2 - 460, Window_Height/2 -256 + 50), oKey);// x_cord, y_cord
    F.setcurrentFrame(0, 0, 512, 512);

    SDL_Texture* qKey = window.loadTexture("res/gfx/L.png");
    Entity H_(Vector2f(500, Window_Height/2 -256 + 50), qKey);// x_cord, y_cord
    H_.setcurrentFrame(0, 0, 512, 512);

    qKey = window.loadTexture("res/gfx/LI.png");
    Entity F_(Vector2f(500, Window_Height/2 -256 + 50), qKey);// x_cord, y_cordk
    F_.setcurrentFrame(0, 0, 512, 512);

    Movement M(paddle_Speed);


    // bool gameRunning = M.getState();
    SDL_Event event;
    const float timeStep = 0.01f;
    float accumalator = 0.0f;
    float currentTime = utils::hireTimeInSeconds();
    while(M.getState())
    {
        cout<<window.getRefreshRate()<<endl;
        int startTicks = SDL_GetTicks();
        float newTime = utils::hireTimeInSeconds();
        float frameTime = newTime - currentTime;
        currentTime = newTime;
        accumalator += frameTime;
        while (accumalator >= timeStep)
        {
            while(SDL_PollEvent(&event))
            {
                if(event.type == SDL_QUIT)
                    M.setState();
                    // gameRunning = false;
                //M.Input(event);
                pthread_t thread;
                ThreadParamInput* param = new ThreadParamInput;
                param->M = &M;
                param->event = event; 
                pthread_create(&thread, NULL, &Movement::threadInput, param);
            }
            accumalator -= timeStep;
        }

        window.clear();
        // window.render(background, 1);

        // M.Ball_Move(Ball, paddleL, paddleR, Window_Width, Window_Height);
        pthread_t threadBall;
        ThreadParamBall* paramB = new ThreadParamBall{&M, Ball, paddleL, paddleR, Window_Width, Window_Height};
        pthread_create(&threadBall, NULL, &Movement::threadBall_Move, paramB);
        // M.Paddle_Move(paddleL, 1, Window_Height);
        pthread_t threadP1;
        ThreadParamPaddle* paramP1 = new ThreadParamPaddle{&M, paddleL, true, Window_Height};
        pthread_create(&threadP1, NULL, &Movement::threadPaddle_Move, paramP1);
        // M.Paddle_Move(paddleR, 0, Window_Height);
        pthread_t threadP2;
        ThreadParamPaddle* paramP2 = new ThreadParamPaddle{&M, paddleR, false, Window_Height};
        pthread_create(&threadP2, NULL, &Movement::threadPaddle_Move, paramP2);

        window.render(Ball, paddle_Mag);
        // pthread_t threadrenderB;
        // ThreadParamRender* paramRenB = new ThreadParamRender{&window, Ball, paddle_Mag};
        // pthread_create(&threadrenderB, NULL, &RenderWindow::threadrender, paramRenB);

        window.render(paddleL, paddle_Mag);
        // pthread_t threadrenderP1;
        // ThreadParamRender* paramRenP1 = new ThreadParamRender{&window, paddleL, paddle_Mag};
        // pthread_create(&threadrenderP1, NULL, &RenderWindow::threadrender, paramRenP1);
        window.render(paddleR, paddle_Mag);
        utils::PrintScoresP1(Numbers, window, M, Window_Width, Window_Height);
        utils::PrintScoresP2(Numbers, window, M, Window_Width, Window_Height);


        if(M.getqPressed() && M.getqCNT()>0)
        {
            window.render(F, paddle_Mag);
        }
        else
        {
            window.render(H, paddle_Mag);
        }
        if(M.getoPressed() && M.getoCNT()>0)
        {
            window.render(F_, paddle_Mag);
        }
        else
        {
            window.render(H_, paddle_Mag);
        }
        window.render(murgin, paddle_Mag);
        //make window.render() p_thread 
        //make M.Input() p_thread XXXk
        //make M.Ball_Move() p_thread XXX
        //make M.Paddle_Move() p_thread XXX
        //make M.Paddle_Move() p_thread XXX
        


        window.display();

        int frameTicks = SDL_GetTicks() - startTicks;
        if(frameTicks < 1000 / window.getRefreshRate())
            SDL_Delay(1000 / window.getRefreshRate() - frameTicks);
    }

    window.cleanUp();
    SDL_Quit();
    return 0;
}