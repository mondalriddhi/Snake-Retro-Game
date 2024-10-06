#include <raylib.h>
#include<iostream>
#include <bits/stdc++.h>
#include <deque>
#include<raymath.h>

using namespace std;
Color green={173, 204, 96, 255};
Color darkGreen={43, 51, 24, 255};
int cellSize=30;
int cellCount=20;
int offset=75;

float lastUpdate=0;


bool eventTriggered(double interval) {
    double currentTime=GetTime();
    if(currentTime-lastUpdate>=interval) {
        lastUpdate=currentTime;
        return true;
    }
    return false;
}

bool ElementInDeque(Vector2 position, deque<Vector2> deque) {
    for(unsigned int i=0;i<deque.size();i++) {
        if(Vector2Equals(position, deque[i])) {
            return true;
        }
    }
    return false;
}

class Snake {
    public:
    
    deque<Vector2> body={Vector2{5,6}, Vector2{4,6}, Vector2{3,6}};
    Vector2 direction={1,0};
    bool addSegment=false;

    void Draw() {
        for(long long unsigned i=0;i<body.size();i++) {
            float x=body[i].x;
            float y=body[i].y;   
            Rectangle segment=Rectangle{offset + x*cellSize, offset + y*cellSize, (float)cellSize, (float)cellSize};
            DrawRectangleRounded(segment, 0.5, 6, darkGreen);
        }   
    }

    void Update()
    {
        body.push_front(Vector2Add(body[0], direction));
        if(addSegment) {
            addSegment=false; 
        }
        else {
            body.pop_back();
        }
    }
    void Reset() {
        body={Vector2{5,6}, Vector2{4,6}, Vector2{3,6}};
        direction={1, 0};
    }
};

class Food{
    public:
    Vector2 position;
    Texture2D texture;
    Food(deque<Vector2> snakeBody)
    {
        Image image=LoadImage("G:/snakeGame/graphics/food.png");
        texture=LoadTextureFromImage(image);
        UnloadImage(image);
        position=generateRandomPos(snakeBody);
    }

    ~Food() 
    {
        UnloadTexture(texture);
    }

    void Draw() 
    {
        DrawTexture(texture, offset + position.x * cellSize, offset + position.y * cellSize, WHITE);
    }

    Vector2 generateRandomCell() {
        float x = GetRandomValue(0, cellCount-1);
        float y = GetRandomValue(0, cellCount-1);
        return Vector2{x, y};
    }

    Vector2 generateRandomPos(deque<Vector2> snakeBody)
    {
        Vector2 position=generateRandomCell();
        while (ElementInDeque(position, snakeBody))
        {
            position=generateRandomCell();
        }
        return position;
    }
};

class Game {
    public:
    
    Snake snake=Snake();
    Food food=Food(snake.body);
    bool running=true;
    int score=0;
    int highScore=0;
    Sound eatSound;
    Sound wallSound;

    Game() {
        InitAudioDevice();
        eatSound=LoadSound("G:/snakeGame/graphics/eat.mp3");
        wallSound=LoadSound("G:/snakeGame/graphics/wall.mp3");
    }

    ~Game() {
        UnloadSound(eatSound);
        UnloadSound(wallSound);
        CloseAudioDevice();
    }

    void Draw() {
        snake.Draw();
        food.Draw();
    }

    void Update() {
        if(running==true) {
            snake.Update();
            checkCollisionWithFood();
            checkCollisionWithEdge();
            checkCollisionWithTail();
        }
    }

    void checkCollisionWithFood() {
        if(Vector2Equals(snake.body[0], food.position)) {
            // cout<<"Food ate"<<endl;
            PlaySound(eatSound);
            food.position=food.generateRandomPos(snake.body);
            snake.addSegment=true;
            score++;
        }
    }

    void checkCollisionWithEdge() {
        if(snake.body[0].x==cellCount || snake.body[0].x==-1) {
            gameOver();
        }
        if(snake.body[0].y==cellCount || snake.body[0].y==-1) {
            gameOver();
        }
    }

    void gameOver() {
        PlaySound(wallSound);
        snake.Reset();
        food.position=food.generateRandomPos(snake.body);
        running=false;
        if(score>highScore) {
            highScore=score;
        }
        score=0;
    }
    //this method for collision with tail also works the same, but time complexity will be more(using loops)
    // void checkCollisionWithTail() {
    //     unsigned int n=snake.body.size();
    //     for(unsigned int i=1;i<n;i++) {
    //         if(Vector2Equals(snake.body[0], snake.body[i])){
    //             gameOver();
    //         }
    //     }      
    // }

    void checkCollisionWithTail() {
        deque<Vector2> headlessBody=snake.body;
        headlessBody.pop_front();
        if(ElementInDeque(snake.body[0], headlessBody)) {
            gameOver();
        }  
    }
};

int main(void)
{
    InitWindow(2*offset + cellSize*cellCount, 2*(offset-15) + cellSize*cellCount, "Snake Game");
    SetTargetFPS(60);
    Game game=Game();

    while (!WindowShouldClose())
    {
        BeginDrawing();
        if(eventTriggered(0.2)) {
            game.Update();
        }
        if(IsKeyPressed(KEY_UP) && game.snake.direction.y!=1) {
            game.snake.direction={0, -1};
            game.running=true;
        }
        if(IsKeyPressed(KEY_DOWN) && game.snake.direction.y!=-1) {
            game.snake.direction={0, 1};
            game.running=true;
        }
        if(IsKeyPressed(KEY_LEFT) && game.snake.direction.x!=1) {
            game.snake.direction={-1, 0};
            
        }
        if(IsKeyPressed(KEY_RIGHT) && game.snake.direction.x!=-1) {
            game.snake.direction={1, 0};
            game.running=true;
        }
        ClearBackground(green);
        DrawRectangleLinesEx(Rectangle{(float)offset-5, (float)offset-5, (float)cellSize*cellCount+10, (float)cellSize*cellCount+10}, 5, darkGreen);
        DrawText("Classic Snake", offset-5, 20, 40, darkGreen);
        DrawText(TextFormat("Score: %i", game.score), offset-5, offset + cellSize*cellCount+10, 40, darkGreen);
        DrawText(TextFormat("Highest: %i", game.highScore), cellSize*cellCount-2*offset, offset + cellSize*cellCount+10, 40, darkGreen);
        game.Draw();
        EndDrawing();
    }

    CloseWindow();

    return 0;
}
