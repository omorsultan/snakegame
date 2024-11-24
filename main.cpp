#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <SDL2/SDL_mixer.h>
#include <SDL2/SDL_image.h>
#include <bits/stdc++.h>
#undef main

const int SCREEN_WIDTH = 1080;
const int SCREEN_HEIGHT = 720;
const int TILE_SIZE = 20;
int score = 0;
int timer = 0;
int bonusFoodTimer = 0;
bool pause;
bool gameOver = false;
bool quit = false;
Mix_Music *bgm;
Mix_Chunk *eatSound;

class Snake
{
public:
    Snake();
    void handleInput(SDL_Event &e);
    void move();
    void render(SDL_Renderer *renderer);
    bool checkCollision();
    std::vector<SDL_Point> recentPositions;
    void spawnFood();
    void drawWall();

private:
    SDL_Rect food;
    std::vector<SDL_Rect> body;
    int direction; // 0: up, 1: down, 2: left, 3: right
    bool bonusFoodActive;
    SDL_Rect bonusFood;
};

Snake::Snake()
{
    SDL_Rect head = {60, 60, TILE_SIZE, TILE_SIZE};//start position
    body.push_back(head);
    SDL_Rect body1 = {head.x, head.y, TILE_SIZE, TILE_SIZE};
    body.push_back(body1);
    SDL_Rect body2 = {head.x, head.y, TILE_SIZE, TILE_SIZE};
    body.push_back(body2);
    direction =3; //  right
    spawnFood();
    bonusFoodActive = false;
}

void Snake::handleInput(SDL_Event &e)
{
    if (e.type == SDL_KEYDOWN)
    {
        switch (e.key.keysym.sym)
        {
        case SDLK_UP:
            if (direction != 1)
            {
                direction = 0;
                if (pause) pause = !pause;
            }
            break;
         case SDLK_KP_8:    
            if (direction != 1)
            {
                direction = 0; 
                if (pause) pause = !pause; 
            }
            break;
        case SDLK_DOWN:
            if (direction != 0)
            {
                direction = 1;
                if (pause) pause = !pause;
            }
            break;
        case SDLK_LEFT:
            if (direction != 3)
            {
                direction = 2;
                if (pause)   pause = !pause;
            }
            break;
        case SDLK_RIGHT:
            if (direction != 2)
            {
                direction = 3;
                if (pause) pause = !pause;
            }
            break;
        case SDLK_SPACE:
            pause = !pause;
            break;
        }
    }
}

void Snake::move()
{
    SDL_Rect newHead = body.front();

    switch (direction)
    {
    case 0: // Up
        newHead.y -= TILE_SIZE;
        break;
    case 1: // Down
        newHead.y += TILE_SIZE;
        break;
    case 2: // Left
        newHead.x -= TILE_SIZE;
        break;
    case 3: // Right
        newHead.x += TILE_SIZE;
        break;
    }

    // if (newHead.x < 0)
    // {
    //     newHead.x = SCREEN_WIDTH - TILE_SIZE;
    // }
    // else if (newHead.x >= SCREEN_WIDTH)
    // {
    //     newHead.x = 0;
    // }

    body.insert(body.begin(), newHead);

    if (newHead.x == food.x && newHead.y == food.y)
    {
        score += 1;
        Mix_PauseAudio(0);
        Mix_PlayChannel(-1, eatSound, 0);
        spawnFood();
    }
    else
    {
        body.pop_back();
    }

    // Check for collision with bonus food
    if (bonusFoodActive && newHead.x < bonusFood.x + bonusFood.w && newHead.x + newHead.w > bonusFood.x &&
        newHead.y < bonusFood.y + bonusFood.h && newHead.y + newHead.h > bonusFood.y)
    {
        score += 10;
        body.push_back(SDL_Rect{-20, -20, TILE_SIZE, TILE_SIZE});
        bonusFoodActive = false;
    }

    if (bonusFoodActive && SDL_GetTicks() > bonusFoodTimer)
    {
        bonusFoodActive = false;
    }

    if (checkCollision())
    {
        std::cout << "Your Score is " << score << std::endl;
        std::cout << "Game Over!" << std::endl;
    }
}

void Snake::render(SDL_Renderer *renderer)
{

    SDL_SetRenderDrawColor(renderer, 173, 216, 230, 255);
    for (int i = 1; i < body.size(); ++i)
    {

        SDL_RenderFillRect(renderer, &body[i]);
    }

    SDL_SetRenderDrawColor(renderer,80, 120, 200, 255);
    SDL_RenderFillRect(renderer, &body[0]);

    SDL_SetRenderDrawColor(renderer, 255, 178, 102, 255);
    SDL_RenderFillRect(renderer, &food);

    if (bonusFoodActive)
    {
        SDL_SetRenderDrawColor(renderer, 0, 255, 0, 255);
        SDL_RenderFillRect(renderer, &bonusFood);
    }

    // draw walls
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    SDL_Rect wall1 = {SCREEN_WIDTH/3+200, SCREEN_HEIGHT/3-100, 20, 340};
    SDL_Rect wall2 = {SCREEN_HEIGHT/3+160, SCREEN_WIDTH/3-60, 340, 20};
    SDL_Rect wall3 = {100, 150, 20, 380};
    SDL_Rect wall4 = {1060-100, 150, 20 ,380};

    SDL_RenderFillRect(renderer, &wall1);
    SDL_RenderFillRect(renderer, &wall2);
    SDL_RenderFillRect(renderer, &wall3);
    SDL_RenderFillRect(renderer, &wall4);

    SDL_SetRenderDrawColor(renderer, 165, 42, 42, 255);
    SDL_Rect wall6 = {0, 720-4*TILE_SIZE, SCREEN_WIDTH, TILE_SIZE*4};
    SDL_Rect wall7={0,0,20,SCREEN_HEIGHT};
    SDL_Rect wall8={1060,0,20,SCREEN_HEIGHT};
    SDL_Rect wall9={0,0,SCREEN_WIDTH,20};
    // SDL_Rect wall7 ={}
    SDL_RenderFillRect(renderer, &wall6);
     SDL_RenderFillRect(renderer, &wall7);
      SDL_RenderFillRect(renderer, &wall8);
       SDL_RenderFillRect(renderer, &wall9);

}

bool Snake::checkCollision()
{
    SDL_Rect head = body.front();

    for (auto it = body.begin() + 1; it != body.end(); ++it)
    {
        if (head.x == it->x && head.y == it->y)
        {
            return true;
        }
    }

    if (head.y < 0 || head.y >= SCREEN_HEIGHT-4*TILE_SIZE)
    {
        return true;
    }
    if (head.x <  TILE_SIZE || head.x >= SCREEN_WIDTH-TILE_SIZE)
    {
        return true;
    }

    SDL_Rect wall1 = {SCREEN_WIDTH/3+200, SCREEN_HEIGHT/3-100, 20, 340};
    SDL_Rect wall2 = {SCREEN_HEIGHT/3+160, SCREEN_WIDTH/3-60, 340, 20};
    SDL_Rect wall3 = {100, 150, 20, 380};
    SDL_Rect wall4 = {1060-100, 150, 20 ,380};

    SDL_Rect wall6 = {0, 10, SCREEN_WIDTH, TILE_SIZE / 4};

    if ((head.x < wall1.x + wall1.w && head.x + head.w > wall1.x && head.y < wall1.y + wall1.h && head.y + head.h > wall1.y) ||
        (head.x < wall2.x + wall2.w && head.x + head.w > wall2.x && head.y < wall2.y + wall2.h && head.y + head.h > wall2.y) ||
        (head.x < wall3.x + wall3.w && head.x + head.w > wall3.x && head.y < wall3.y + wall3.h && head.y + head.h > wall3.y) ||
        (head.x < wall4.x + wall4.w && head.x + head.w > wall4.x && head.y < wall4.y + wall4.h && head.y + head.h > wall4.y) ||
        (head.x < wall6.x + wall6.w && head.x + head.w > wall6.x && head.y < wall6.y + wall6.h && head.y + head.h > wall6.y))
    {
        return true;
    }

    return false;
}

bool operator==(const SDL_Point& p1, const SDL_Point& p2)
{
    return p1.x == p2.x && p1.y == p2.y;
}

void Snake::spawnFood()
{
    static std::random_device rd; // random vlaue
    static std::mt19937 gen(rd());
   static std::uniform_int_distribution<> disX(80 / TILE_SIZE, 1000 / TILE_SIZE - 1);
static std::uniform_int_distribution<> disY(80 / TILE_SIZE, 620 / TILE_SIZE - 1);

    SDL_Rect wall1 = {SCREEN_WIDTH/3+200, SCREEN_HEIGHT/3-100, 20, 340};
    SDL_Rect wall2 = {SCREEN_HEIGHT/3+160, SCREEN_WIDTH/3-60, 340, 20};
    SDL_Rect wall3 = {100, 160, 20, 380};
    SDL_Rect wall4 = {1060-100, 160, 20 ,380};
    SDL_Rect wall6 = {0, 55, SCREEN_WIDTH, TILE_SIZE / 4};

    SDL_Point foodPosition;

    do
    {
        foodPosition.x = disX(gen) * TILE_SIZE;
        foodPosition.y = disY(gen) * TILE_SIZE;
    } while ((foodPosition.x >= wall1.x && foodPosition.x < wall1.x + wall1.w && foodPosition.y >= wall1.y && foodPosition.y < wall1.y + wall1.h) ||
             (foodPosition.x >= wall2.x && foodPosition.x < wall2.x + wall2.w && foodPosition.y >= wall2.y && foodPosition.y < wall2.y + wall2.h) ||
             (foodPosition.x >= wall3.x && foodPosition.x < wall3.x + wall3.w && foodPosition.y >= wall3.y && foodPosition.y < wall3.y + wall3.h) ||
             (foodPosition.x >= wall4.x && foodPosition.x < wall4.x + wall4.w && foodPosition.y >= wall4.y && foodPosition.y < wall4.y + wall4.h) ||
             (foodPosition.x >= 200 && foodPosition.x < SCREEN_WIDTH-200 && foodPosition.y >= 600 && foodPosition.y < 3000) ||
             std::any_of(body.begin(), body.end(), [foodPosition](const SDL_Rect &segment)
                         { return foodPosition.x == segment.x && foodPosition.y == segment.y; }) ||
             std::find(recentPositions.begin(), recentPositions.end(), foodPosition) != recentPositions.end());

    // Update food position
    food.x = foodPosition.x;
    food.y = foodPosition.y;
    food.w = TILE_SIZE;
    food.h = TILE_SIZE;

    // Add to recent positions to avoid repeats
    recentPositions.push_back(foodPosition);
    if (recentPositions.size() > 5) // Limit recent positions to last 5
        recentPositions.erase(recentPositions.begin());
     if (score % 4 == 0)
    {
        bonusFoodActive = true;
        bonusFoodTimer = SDL_GetTicks() + 70000;

        SDL_Point bonusFoodPosition;

        do
        {
            bonusFoodPosition.x = disX(gen) * TILE_SIZE;
            bonusFoodPosition.y = disY(gen) * TILE_SIZE;
        } while ((bonusFoodPosition.x >= wall1.x && bonusFoodPosition.x < wall1.x + wall1.w && bonusFoodPosition.y >= wall1.y && bonusFoodPosition.y < wall1.y + wall1.h) ||
                 (bonusFoodPosition.x >= wall2.x && bonusFoodPosition.x < wall2.x + wall2.w && bonusFoodPosition.y >= wall2.y && bonusFoodPosition.y < wall2.y + wall2.h) ||
                 (bonusFoodPosition.x >= wall3.x && bonusFoodPosition.x < wall3.x + wall3.w && bonusFoodPosition.y >= wall3.y && bonusFoodPosition.y < wall3.y + wall3.h) ||
                 (bonusFoodPosition.x >= wall4.x && bonusFoodPosition.x < wall4.x + wall4.w && bonusFoodPosition.y >= wall4.y && bonusFoodPosition.y < wall4.y + wall4.h) ||
                 (bonusFoodPosition.x >= 0 && bonusFoodPosition.x < SCREEN_WIDTH && bonusFoodPosition.y >= 0 && bonusFoodPosition.y < 60) ||
                 std::any_of(body.begin(), body.end(), [bonusFoodPosition](const SDL_Rect &segment)
                             { return bonusFoodPosition.x == segment.x && bonusFoodPosition.y == segment.y; }));

        bonusFood.x = bonusFoodPosition.x;
        bonusFood.y = bonusFoodPosition.y;
        bonusFood.w = TILE_SIZE ;
        bonusFood.h = TILE_SIZE;
    }
}
void renderScore(SDL_Renderer *renderer, TTF_Font *font, int score)
{
    SDL_Color fontColor = {255, 255, 102, 255};
    SDL_Surface *surface1 = TTF_RenderText_Solid(font, ("Score: " + std::to_string(score)).c_str(), fontColor);
    if (surface1)
    {
        SDL_Texture *text1 = SDL_CreateTextureFromSurface(renderer, surface1);
        if (text1)
        {
            SDL_Rect textRect = {800, 720-4*TILE_SIZE, 7*TILE_SIZE, 4*TILE_SIZE};
            SDL_RenderCopy(renderer, text1, nullptr, &textRect);
            SDL_DestroyTexture(text1);
        }
    }
        
}

void displayGameOver(SDL_Renderer *renderer, TTF_Font *font, int finalScore)
{
    SDL_SetRenderDrawColor(renderer, 204, 200, 153, 255);
    SDL_RenderClear(renderer);

    SDL_Color textColor = {255, 255, 255, 255};
    SDL_Surface *surface = TTF_RenderText_Solid(font, ("Game Over! Final Score: " + std::to_string(finalScore)).c_str(), textColor);

    if (surface)
    {
        SDL_Texture *text = SDL_CreateTextureFromSurface(renderer, surface);

        if (text)
        {
            SDL_Rect textRect;
            textRect.x = SCREEN_WIDTH / 4;
            textRect.y = SCREEN_HEIGHT / 3;
            textRect.w = SCREEN_WIDTH / 2;
            textRect.h = SCREEN_HEIGHT / 5;

            SDL_RenderCopy(renderer, text, nullptr, &textRect);

            SDL_DestroyTexture(text);
        }

        SDL_FreeSurface(surface);
    }

    SDL_RenderPresent(renderer);
    SDL_Delay(3000);
    exit(0);
}

int main(int argc, char *argv[])
{
    SDL_Window *window = nullptr;
    SDL_Renderer *renderer = nullptr;

    SDL_Init(SDL_INIT_VIDEO);
    TTF_Init();
   // Mix_Init(MIX_INIT_MP3);
    IMG_Init(IMG_INIT_PNG); // Initialize SDL2_image for PNG images

  //  Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048);

  //   bgm = Mix_LoadMUS("back.mp3");
   // Mix_PlayMusic(bgm, -1);

    // eatSound = Mix_LoadWAV("eat1.mp3");
    // if (!eatSound)
    // {
    //     std::cerr << "Failed to load eat sound: " << Mix_GetError() << std::endl;
    //     Mix_Quit();
    //     SDL_Quit();
    //     return 1;
    // }

    window = SDL_CreateWindow("Snake Game", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);
    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    SDL_Surface *backgroundImageSurface = IMG_Load("gamee.png");
    SDL_Texture *backgroundImageTexture = SDL_CreateTextureFromSurface(renderer, backgroundImageSurface);

    // Load font
    TTF_Font *font = TTF_OpenFont("Atop.ttf", 30);
    if (!font)
    {
        std::cerr << "Failed to load font: " << TTF_GetError() << std::endl;
        TTF_Quit();
        return 1;
    }

    Snake snake;

    SDL_Event e;
    while (!quit)
    {
        while (SDL_PollEvent(&e) != 0)
        {
            if (e.type == SDL_QUIT)
            {
                quit = true;
            }
            snake.handleInput(e);
        }
        if (!pause && !gameOver)
        {
            snake.move();

            if (snake.checkCollision())
            {
                gameOver = true;
            }
        }

        // Render the background image

        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
        SDL_RenderClear(renderer);
        SDL_RenderCopy(renderer, backgroundImageTexture, NULL, NULL);

        if (!gameOver)
        {
            snake.render(renderer);
            renderScore(renderer, font, score);
        }
        else
        {
            displayGameOver(renderer, font, score);
        }

        SDL_RenderPresent(renderer);
        SDL_Delay(120);
    }

    SDL_FreeSurface(backgroundImageSurface);
    SDL_DestroyTexture(backgroundImageTexture);
    IMG_Quit();
    Mix_FreeMusic(bgm);
    Mix_FreeChunk(eatSound);
    Mix_CloseAudio();
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
    TTF_Quit();
    Mix_Quit();

    return 0;
}