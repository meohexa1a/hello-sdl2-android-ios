#include <SDL.h>
#undef main
#include "narc/core/narc_core.h"

void init()
{
    SDL_Rect r;
    r.x = 50;
    r.y = 50;
    r.w = 50;
    r.h = 50;

    NarcDraw::setDrawCallback(
        [r](SDL_Renderer *renderer) mutable
        {
            // Xoá màn hình (màu đen)
            SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
            SDL_RenderClear(renderer);

            // Vẽ hình chữ nhật đỏ đặc
            SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);
            SDL_RenderFillRect(renderer, &r);

            // Vẽ viền xanh quanh hình chữ nhật
            SDL_SetRenderDrawColor(renderer, 0, 255, 0, 255);
            SDL_RenderDrawRect(renderer, &r);

            // Vẽ 1 đường chéo trắng
            SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
            SDL_RenderDrawLine(renderer, r.x, r.y, r.x + r.w, r.y + r.h);

            // Vẽ vài điểm xanh dương
            SDL_SetRenderDrawColor(renderer, 0, 0, 255, 255);
            for (int i = 0; i < 20; i++)
                SDL_RenderDrawPoint(renderer, r.x + i, r.y + i);
        });

    NarcCoreAppScheduler::postRepeated(
        []()
        {
            SDL_Event e;

            while (SDL_PollEvent(&e) != 0)
            {
                std::cout << e.type << std::endl;

                if (e.type == SDL_QUIT)
                    NarcCoreApplication::shutdown();
            }
        },
        std::chrono::milliseconds(1000 / 240), -1);
}

int main()
{
    NarcCoreApplication::init(init);
    return 0;
}
