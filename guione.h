#ifndef GUIONE_H
#define GUIONE_H
#define SDL_MAIN_HANDLED

#include <SDL.h>
#include <string>
#include <vector>
#include <iostream>
#include <utility> 
#include <SDL_ttf.h>

class TextInput;
class Window;


class Widget {
protected:
    SDL_Rect rect = {0, 0, 0, 0};
    int bg_r = 255, bg_g = 255, bg_b = 255;
    Widget* parent = nullptr; 
    SDL_Color text_color = {255, 255, 255, 255};  // Bianco di default
    int value = 0;

public:
    Widget() {
        rect = {0, 0, 100, 40};
    }

    Widget(int x, int y, int w, int h) {
        rect = {x, y, w, h};
    }

    virtual void draw(SDL_Renderer* renderer) = 0;
    virtual bool handleClick(int x, int y) { return false; }

    void setRect(int x, int y, int w, int h) { rect = {x, y, w, h}; }
    int getX() const { return rect.x; }
    int getY() const { return rect.y; }
    std::pair<int, int> getSize() const { return {rect.w, rect.h}; }

    void pos(int x, int y) { rect.x = x; rect.y = y; }
    void size(int w, int h) { rect.w = w; rect.h = h; }

    void bg(int r, int g, int b) { bg_r = r; bg_g = g; bg_b = b; }
    void setParent(Widget* p) { parent = p; }
    Widget* getParent() const { return parent; }

     virtual void setTextColor(Uint8 r, Uint8 g, Uint8 b, Uint8 a = 255) {
        text_color = {r, g, b, a};
    }

    void setValue(int v) { value = v; }
    int getValue() const { return value; }
    bool operator==(int v) const { return value == v; }
    virtual void setLabel(const std::string&) {}

    // Calcola posizione assoluta sommando ricorsivamente genitori
    std::pair<int,int> getAbsolutePos() const {
        if (!parent) return {rect.x, rect.y};
        auto [px, py] = parent->getAbsolutePos();
        return {px + rect.x, py + rect.y};
    }

    virtual void center(const Window& win);
    virtual void center(const Widget& parent);

    virtual ~Widget() {}
};


class Box : public Widget {
    std::vector<Widget*> children;

public:
    Box(int x, int y, int w, int h) {
        setRect(x, y, w, h);
        bg(200, 200, 200);
    }

    void addWidget(Widget& w) {
        w.setParent(this);  // <-- assegna il genitore
        children.push_back(&w);
    }

    void draw(SDL_Renderer* renderer) override {
    auto [absx, absy] = getAbsolutePos();
    SDL_Rect r = {absx, absy, rect.w, rect.h};
    SDL_SetRenderDrawColor(renderer, bg_r, bg_g, bg_b, 255);
    SDL_RenderFillRect(renderer, &r);

    for (auto* child : children)
        child->draw(renderer);
}

bool handleClick(int x, int y) override {
    for (auto it = children.rbegin(); it != children.rend(); ++it) {
        if ((*it)->handleClick(x, y))
            return true;
    }
    return false;
}
};


class Button : public Widget {
    std::string text; 
    std::string label;
    bool clicked = false;
    TTF_Font* font = nullptr;

public:
    // Costruttore di default (solo UNO!)
    Button() : Widget(0, 0, 100, 40), label("Button") {if (TTF_WasInit() == 0) TTF_Init();
    font = TTF_OpenFont("C:/Windows/Fonts/arial.ttf", 18);
    if (!font) std::cerr << "Errore caricamento font: " << TTF_GetError() << "\n";}

    // Costruttore parametrico
    Button(int x, int y, int w, int h, const std::string& text)
        : Widget(x, y, w, h), label(text)
    {
        if (TTF_WasInit() == 0) TTF_Init();
        font = TTF_OpenFont("C:/Windows/Fonts/arial.ttf", 18);
        if (!font) std::cerr << "Errore caricamento font: " << TTF_GetError() << "\n";
    }

    Button(const Button&) = default;
Button& operator=(const Button&) = default;

    ~Button() {
        if (font) TTF_CloseFont(font);
        font = nullptr;
    }

    void bg(int r, int g, int b)  {
        Widget::bg(r, g, b);
    }

    void draw(SDL_Renderer* renderer) override {
        auto [absx, absy] = getAbsolutePos();
        SDL_Rect drawRect = {absx, absy, rect.w, rect.h};
        SDL_SetRenderDrawColor(renderer, bg_r, bg_g, bg_b, 255);
        SDL_RenderFillRect(renderer, &drawRect);

        if (font) {
            SDL_Color textColor = {255, 255, 255, 255};
            SDL_Surface* textSurface = TTF_RenderText_Blended(font, label.c_str(), text_color);
            if (textSurface) {
                SDL_Texture* textTexture = SDL_CreateTextureFromSurface(renderer, textSurface);
                int tw = textSurface->w;
                int th = textSurface->h;
                SDL_Rect textRect = {absx + (rect.w - tw)/2, absy + (rect.h - th)/2, tw, th};
                SDL_FreeSurface(textSurface);
                SDL_RenderCopy(renderer, textTexture, nullptr, &textRect);
                SDL_DestroyTexture(textTexture);
            }
        }
    }

    void setLabel(const std::string& newText) {
        label = newText;
    }

    std::string getLabel() const {
        return label;
    }

    bool handleClick(int x, int y) override {
        auto [absx, absy] = getAbsolutePos();
        clicked = (x >= absx && x <= absx + rect.w &&
                   y >= absy && y <= absy + rect.h);
        return clicked;
    }

    bool is_clicked() {
        bool was = clicked;
        clicked = false;
        return was;
    }
};

class TextInput : public Widget {
    std::string text;
    bool active = false;
    TTF_Font* font = nullptr;
    SDL_Color text_color = {255, 255, 255, 255};

public:
    TextInput() {
        if (!TTF_WasInit()) TTF_Init();
        font = TTF_OpenFont("C:/Windows/Fonts/arial.ttf", 18);  // Usa una dimensione leggibile
        if (!font) std::cerr << "Errore font: " << TTF_GetError() << "\n";
    }

    ~TextInput() {
        if (font) TTF_CloseFont(font);
    }

    void setTextColor(Uint8 r, Uint8 g, Uint8 b, Uint8 a = 255) {
        text_color = {r, g, b, a};
    }

    void insert() {
        active = true;
        SDL_StartTextInput();
        std::cout << "Input attivato\n";
    }

    void draw(SDL_Renderer* renderer) override {
        auto [absx, absy] = getAbsolutePos();
        SDL_Rect r = {absx, absy, rect.w, rect.h};

        // Sfondo
        SDL_SetRenderDrawColor(renderer, bg_r, bg_g, bg_b, 255);
        SDL_RenderFillRect(renderer, &r);

        // Bordo
        SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
        SDL_RenderDrawRect(renderer, &r);

        if (font && !text.empty()) {
            SDL_Color color = text_color;
            SDL_Surface* surf = TTF_RenderText_Blended(font, text.c_str(), color);
            if (surf) {
                SDL_Texture* tex = SDL_CreateTextureFromSurface(renderer, surf);
                SDL_Rect dst = {absx + 5, absy + (rect.h - surf->h) / 2, surf->w, surf->h};
                SDL_FreeSurface(surf);
                SDL_RenderCopy(renderer, tex, nullptr, &dst);
                SDL_DestroyTexture(tex);
            }
        }
    }

 bool handleClick(int x, int y) override {
    auto [absx, absy] = getAbsolutePos();
    bool inside = (x >= absx && x <= absx + rect.w &&
                   y >= absy && y <= absy + rect.h);
    active = inside;
    if (!inside) SDL_StopTextInput();
    else SDL_StartTextInput();
    return inside;
}


    bool handleTextInput(const SDL_Event& e) {
        if (!active) return false;

        if (e.type == SDL_TEXTINPUT) {
            text += e.text.text;
            std::cout << "Digitato: " << text << "\n";
            return true;
        } else if (e.type == SDL_KEYDOWN && e.key.keysym.sym == SDLK_BACKSPACE) {
            if (!text.empty()) {
                text.pop_back();
                std::cout << "Backspace: " << text << "\n";
            }
            return true;
        }

        return false;
    }

    const std::string& getText() const {
        return text;
    }

    void clear() {
        text.clear();
    }
};

class Window {
    SDL_Window* window = nullptr;
    SDL_Renderer* renderer = nullptr;
    std::vector<Widget*> widgets;
    int bg_r = 255, bg_g = 255, bg_b = 255;
    bool closed = false;

public:
    Window(const std::string& title, int w = 640, int h = 480) {
        SDL_Init(SDL_INIT_VIDEO);
        window = SDL_CreateWindow(title.c_str(),
            SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, w, h, SDL_WINDOW_SHOWN);
        renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
        SDL_StartTextInput();
    }

    ~Window() {
        SDL_StopTextInput();
        SDL_DestroyRenderer(renderer);
        SDL_DestroyWindow(window);
        //SDL_Quit();
    }

    void bg(int r, int g, int b) {
        bg_r = r;
        bg_g = g;
        bg_b = b;
    }

    void addWidget(Widget& w) {
        widgets.push_back(&w);
    }

    std::pair<int, int> getSize() const {
        int w, h;
        SDL_GetWindowSize(window, &w, &h);
        return {w, h};
    }

    void size(int w, int h) {
        SDL_SetWindowSize(window, w, h);
    }


    void clearWidgets() {
        widgets.clear(); 
    }

    void close() {
        closed = true;
    }

    bool update() {
    if (closed) return false;
    SDL_Event e;
    while (SDL_PollEvent(&e)) {
        if (e.type == SDL_QUIT)
            return false;

        if (e.type == SDL_WINDOWEVENT && e.window.event == SDL_WINDOWEVENT_CLOSE) {
            if (e.window.windowID == SDL_GetWindowID(window)) {
                return false;
            }
        }

        if (e.type == SDL_MOUSEBUTTONDOWN) {
            int x = e.button.x;
            int y = e.button.y;
            for (auto it = widgets.rbegin(); it != widgets.rend(); ++it) {
                if ((*it)->handleClick(x, y))
                    break;
            }
        }

        
        if (e.type == SDL_TEXTINPUT || e.type == SDL_KEYDOWN) {
            for (auto* w : widgets) {
                if (auto* ti = dynamic_cast<TextInput*>(w)) {
                    ti->handleTextInput(e);
                }
            }
        }
    }

    SDL_SetRenderDrawColor(renderer, bg_r, bg_g, bg_b, 255);
    SDL_RenderClear(renderer);

    for (auto w : widgets)
        w->draw(renderer);

    SDL_RenderPresent(renderer);
    SDL_Delay(10);
    return true;
}
};

class Label : public Widget {
    std::string text;
    TTF_Font* font = nullptr;

public:
    Label(const std::string& txt, int fontSize = 18)
        : text(txt)
    {
        if (TTF_WasInit() == 0) TTF_Init();
        font = TTF_OpenFont("C:/Windows/Fonts/arial.ttf", fontSize);
        if (!font) {
            std::cerr << "Errore caricamento font: " << TTF_GetError() << "\n";
        } else {
            // Calcola larghezza e altezza del testo
            int tw, th;
            TTF_SizeText(font, text.c_str(), &tw, &th);
            setRect(0, 0, tw, th);
        }
    }

    ~Label() {
        if (font) TTF_CloseFont(font);
    }

    void draw(SDL_Renderer* renderer) override {
        auto [absx, absy] = getAbsolutePos();

        SDL_Color color = { Uint8(bg_r), Uint8(bg_g), Uint8(bg_b), 255 }; // Evita il warning
        SDL_Surface* surface = TTF_RenderText_Blended(font, text.c_str(), color);
        if (surface) {
            SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);
            SDL_Rect dst = { absx, absy, surface->w, surface->h };
            SDL_RenderCopy(renderer, texture, nullptr, &dst);
            SDL_DestroyTexture(texture);
            SDL_FreeSurface(surface);
        }
    }
};



inline void Widget::center(const Window& win) {
    auto [w, h] = win.getSize();
    rect.x = (w - rect.w) / 2;
    rect.y = (h - rect.h) / 2;
}

inline void Widget::center(const Widget& parent) {
    auto [pw, ph] = parent.getSize();
    rect.x = (pw - rect.w) / 2;
    rect.y = (ph - rect.h) / 2;
}

#endif 