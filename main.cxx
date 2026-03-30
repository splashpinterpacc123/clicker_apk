#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_mixer.h>
#include <SDL2/SDL_ttf.h>
#include <string>

int main(int argc, char* argv[]) {
    SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO);
    IMG_Init(IMG_INIT_PNG);
    Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048);
    TTF_Init();

    SDL_Window* window = SDL_CreateWindow("Coin Clicker", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, 0, 0, SDL_WINDOW_FULLSCREEN | SDL_WINDOW_ALLOW_HIGHDPI);
    SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);

    SDL_Texture* coinTex = IMG_LoadTexture(renderer, "coin.png");
    Mix_Chunk* clickSound = Mix_LoadWAV("coin.wav");
    TTF_Font* font = TTF_OpenFont("font.ttf", 64);

    bool running = true;
    SDL_Event event;
    int score = 0;
    float scale = 1.0f; // Для эффекта вздрагивания

    while (running) {
        int w, h;
        SDL_GetWindowSize(window, &w, &h);

        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) running = false;
            
            if (event.type == SDL_FINGERDOWN || event.type == SDL_MOUSEBUTTONDOWN) {
                int mouseX, mouseY;
                if (event.type == SDL_FINGERDOWN) {
                    mouseX = event.tfinger.x * w;
                    mouseY = event.tfinger.y * h;
                } else {
                    SDL_GetMouseState(&mouseX, &mouseY);
                }

                // Расчет текущей области монетки (центр)
                int coinSize = (w < h ? w : h) / 2;
                SDL_Rect coinRect = { (w - coinSize) / 2, (h - coinSize) / 2, coinSize, coinSize };

                // Проверка: попали ли в квадрат монетки
                if (mouseX >= coinRect.x && mouseX <= (coinRect.x + coinRect.w) &&
                    mouseY >= coinRect.y && mouseY <= (coinRect.y + coinRect.h)) {
                    score++;
                    Mix_PlayChannel(-1, clickSound, 0);
                    scale = 1.2f; // Увеличиваем при нажатии
                }
            }
        }

        // Плавное возвращение размера монетки
        if (scale > 1.0f) scale -= 0.05f;

        SDL_SetRenderDrawColor(renderer, 20, 20, 25, 255);
        SDL_RenderClear(renderer);

        // 1. Рисуем монету
        int coinBaseSize = (w < h ? w : h) / 2;
        int currentSize = (int)(coinBaseSize * scale);
        SDL_Rect dest = { (w - currentSize) / 2, (h - currentSize) / 2, currentSize, currentSize };
        SDL_RenderCopy(renderer, coinTex, NULL, &dest);

        // 2. Рисуем счет (над монетой)
        SDL_Color white = {255, 255, 255, 255};
        std::string scoreStr = "Score: " + std::to_string(score);
        SDL_Surface* scoreSurf = TTF_RenderText_Blended(font, scoreStr.c_str(), white);
        SDL_Texture* scoreTex = SDL_CreateTextureFromSurface(renderer, scoreSurf);
        
        int tw, th;
        SDL_QueryTexture(scoreTex, NULL, NULL, &tw, &th);
        SDL_Rect textRect = { (w - tw) / 2, dest.y - th - 20, tw, th }; // Чуть выше монетки
        
        SDL_RenderCopy(renderer, scoreTex, NULL, &textRect);
        
        SDL_FreeSurface(scoreSurf);
        SDL_DestroyTexture(scoreTex);

        SDL_RenderPresent(renderer);
        SDL_Delay(16); // ~60 FPS
    }

    TTF_CloseFont(font);
    Mix_FreeChunk(clickSound);
    SDL_DestroyTexture(coinTex);
    TTF_Quit();
    Mix_Quit();
    IMG_Quit();
    SDL_Quit();
    return 0;
}
