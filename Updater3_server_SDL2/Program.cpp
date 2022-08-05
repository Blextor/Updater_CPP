#include "Program.h"
#include "common.h"
#include "updater.h"

using namespace std;

Updaters_data data;

mutex   plannedB, /// data változóinak lock-ja
        plannedF,
        sentB,
        sentF,
        startT,
        boolO;
#define WINDOW_WIDTH 300
#define WINDOW_HEIGHT (WINDOW_WIDTH)

void Draw_Text(SDL_Window &window, SDL_Renderer &renderer, string text, int pos_x, int pos_y, int fontS, int fontC){
    TTF_Font* Sans = TTF_OpenFont("FeeSans.ttf", fontS); //this opens a font style and sets a size

    SDL_Color White = {0, 0, 0};  // this is the color in rgb format, maxing out all would give you the color white, and it will be your text's color

    SDL_Surface* surfaceMessage = TTF_RenderText_Solid(Sans, text.c_str(), White); // as TTF_RenderText_Solid could only be used on SDL_Surface then you have to create the surface first

    SDL_Texture* Message = SDL_CreateTextureFromSurface(&renderer, surfaceMessage); //now you can convert it into a texture

    SDL_Rect Message_rect; //create a rect
    Message_rect.x = pos_x;  //controls the rect's x coordinate
    Message_rect.y = pos_y; // controls the rect's y coordinte
    Message_rect.w = 100; // controls the width of the rect
    Message_rect.h = 100; // controls the height of the rect

    //Mind you that (0,0) is on the top left of the window/screen, think a rect as the text's box, that way it would be very simple to understand

    //Now since it's a texture, you have to put RenderCopy in your game loop area, the area where the whole code executes

    SDL_RenderCopy(&renderer, Message, NULL, &Message_rect); //you put the renderer's name first, the Message, the crop size(you can ignore this if you don't want to dabble with cropping), and the rect which is the size and coordinate of your texture

    //Don't forget to free your surface and texture
    SDL_FreeSurface(surfaceMessage);
    SDL_DestroyTexture(Message);
}

void get_text_and_rect(SDL_Renderer *renderer, int x, int y, char *text,
    TTF_Font *font, SDL_Texture **texture, SDL_Rect *rect) {
    int text_width;
    int text_height;
    SDL_Surface *surface;
    SDL_Color textColor = {0, 0, 0, 0};
    stringRGBA(renderer,x,y,text,0,0,0,255);
    /*surface = TTF_RenderText_Solid(font, text, textColor);
    cout<<"er2"<<endl;
    *texture = SDL_CreateTextureFromSurface(renderer, surface);
    text_width = surface->w;
    text_height = surface->h;
    SDL_FreeSurface(surface);
    cout<<surface->refcount<<endl;
    rect->x = x;
    rect->y = y;
    rect->w = text_width;
    rect->h = text_height;
    SDL_RenderCopy(renderer, *texture, NULL, rect);*/
}

void get_client1_texts(SDL_Renderer *renderer,SDL_Texture **client1T,SDL_Rect *client1R,TTF_Font *font){
    boolO.lock();
    if (data.online[0]){
        sentB.lock();
        char str[20], str2[20];
        stringstream ss; ss<<data.sent_bytes[0]; ss>>str;
        stringstream ss1; ss1<<(data.sent_bytes[0]/1000)*100/(data.all_bytes/1000); ss1>>str2; strcat(str2,"%");
        get_text_and_rect(renderer, 240, 0, str, font, &client1T[0], &client1R[0]);
        stringRGBA(renderer,240,46,str2,(100-(data.sent_bytes[0]/1000)*100/(data.all_bytes/1000))*2.55,(data.sent_bytes[0]/1000)*100/(data.all_bytes/1000)*2.55,0,255);
        sentB.unlock();

        sentF.lock();

        char str3[20], str4[20];
        stringstream ss2; ss2<<data.sent_files[0]; ss2>>str3;
        stringstream ss3; ss3<<data.sent_files[0]*100/data.all_files; ss3>>str4; strcat(str4,"%");
        get_text_and_rect(renderer, 240, 0, str, font, &client1T[0], &client1R[0]);
        stringRGBA(renderer,240,46,str2,(100-(data.sent_bytes[0]/1000)*100/(data.all_bytes/1000))*2.55,(data.sent_bytes[0]/1000)*100/(data.all_bytes/1000)*2.55,0,255);

        sentF.unlock();
    }
    else {
        get_text_and_rect(renderer, 240, 0, "0", font, &client1T[0], &client1R[0]);

    }
    char str[20], str1[20];
    stringstream ss; ss<<data.all_bytes; ss>>str;
    stringstream ss1; ss1<<data.all_files; ss1>>str1;
    get_text_and_rect(renderer, 240, 20, str, font, &client1T[0], &client1R[0]);
    get_text_and_rect(renderer, 105, 20, str1, font, &client1T[0], &client1R[0]);
    boolO.unlock();



}

void draw_clients_bg(SDL_Renderer *renderer){
    SDL_Rect c1 = {0,0,320,240};
    SDL_SetRenderDrawColor(renderer,200,200,200,255);
    SDL_RenderFillRect(renderer, &c1);

    c1 = {0,78,320,4};
    SDL_SetRenderDrawColor(renderer,100,100,100,255);
    SDL_RenderFillRect(renderer, &c1);

    c1 = {0,158,320,4};
    SDL_SetRenderDrawColor(renderer,100,100,100,255);
    SDL_RenderFillRect(renderer, &c1);



    boolO.lock();
    if (data.online[0]){
        filledCircleRGBA(renderer,10,10,5,0,255,0,255);
    } else {
        filledCircleRGBA(renderer,10,10,5,255,0,0,255);
    }
    if (data.online[1]){
        filledCircleRGBA(renderer,10,90,5,0,255,0,255);
    } else {
        filledCircleRGBA(renderer,10,90,5,255,0,0,255);
    }
    if (data.online[2]){
        filledCircleRGBA(renderer,10,170,5,0,255,0,255);
    } else {
        filledCircleRGBA(renderer,10,170,5,255,0,0,255);
    }
    boolO.unlock();
    stringRGBA(renderer,152,6,"Sent bytes:",0,0,0,255);
    stringRGBA(renderer,160,26,"All bytes:",0,0,0,255);
    stringRGBA(renderer,184,46,"bytes%:",0,0,0,255);

    stringRGBA(renderer,17,6,"Sent files:",0,0,0,255);
    stringRGBA(renderer,25,26,"All files:",0,0,0,255);
    stringRGBA(renderer,49,46,"files%:",0,0,0,255);

}

void ServerProc_4(SDL_Renderer *renderer, SDL_Texture *client1T[10], SDL_Rect client1R[10],TTF_Font *font){
    SDL_Event ev;
    clock_t t1 = clock();
    clock_t delay=30;
    while(true){
        if (clock() < t1 + CLOCKS_PER_SEC/delay){
            Sleep(1);
        } else {
            t1=clock();
            SDL_SetRenderDrawColor(renderer,0,0,0,255);
            SDL_RenderClear(renderer);
            draw_clients_bg(renderer);
            SDL_RenderPresent( renderer );
            SDL_DestroyTexture(client1T[0]);
            if (SDL_PollEvent(&ev)){
            }
        }
    }
}

void jatek( SDL_Window *window, SDL_Renderer *renderer){


    SDL_Rect rect1, rect2;
    SDL_Texture *texture1, *texture2;
    SDL_Rect client1R[10];
    SDL_Texture *client1T[10];

    char *font_path = "FreeSans.ttf";
    TTF_Font *font = TTF_OpenFont(font_path, 12);

    SDL_SetRenderDrawColor( renderer, 0, 0, 0, 255 );
    SDL_RenderClear( renderer );



    SDL_RenderPresent( renderer );
    get_text_and_rect(renderer, 0, 0, "hello", font, &texture1, &rect1);


    get_text_and_rect(renderer, 0, rect1.y + rect1.h, "world", font, &texture2, &rect2);

    SDL_RenderPresent( renderer );

    HANDLE hServerProc_1 = CreateThread(NULL, 0, ServerProc_1, NULL, 0, 0);
    HANDLE hServerProc_2 = CreateThread(NULL, 0, ServerProc_2, NULL, 0, 0);
    HANDLE hServerProc_3 = CreateThread(NULL, 0, ServerProc_3, NULL, 0, 0);

    bool stop=false, jatek_kezdes=true;
    std::thread t(ServerProc_4,renderer, client1T, client1R,font);
    SDL_Event ev;
    clock_t t1 = clock();
    clock_t delay=60;
    while(true){
        if (clock() < t1 + CLOCKS_PER_SEC/delay){
            Sleep(1);
        } else {
            t1=clock();
            if (SDL_PollEvent(&ev)){
            }
        }
    }
    t.join();
    return ;
}
