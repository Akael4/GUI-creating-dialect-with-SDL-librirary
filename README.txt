====================
  GUI Library (SDL2)
====================

Questa libreria semplifica la creazione di interfacce grafiche con SDL2 e SDL2_ttf.
Include gestione finestra, input testuale, widget annidati, pulsanti, label e box
contenitori con posizionamento relativo.

---------------------
 CLASSI E FUNZIONI
---------------------

== Classe: Window ==
Costruttore:
  Window(const std::string& title, int w = 640, int h = 480)

Distruttore:
  ~Window()

Metodi:
  void bg(int r, int g, int b)
      // Imposta il colore di sfondo della finestra.

  void addWidget(Widget& w)
      // Aggiunge un widget alla finestra (non prende ownership).

  void clearWidgets()
      // Rimuove tutti i widget registrati nella finestra (non li distrugge).

  std::pair<int,int> getSize() const
      // Restituisce {larghezza, altezza} della finestra.

  void size(int w, int h)
      // Cambia dimensioni della finestra.

  void close()
      // Chiude programmaticamente la finestra (update() restituirà false).

  bool update()
      // Gestisce eventi (mouse, tastiera, testo), ridisegna tutti i widget e presenta il frame.
      // Da usare tipicamente in: while (win.update()) { ... }


== Classe: Widget (astratta, base di tutti i widget) ==
Campi/proprietà principali:
  - rettangolo locale (x, y, w, h)
  - colore di sfondo (bg_r, bg_g, bg_b) — NOTA: per Label questo è il colore del testo
  - puntatore al genitore (Widget* parent)
  - SDL_Color text_color (usato da Button e TextInput)
  - int value (valore utente associabile al widget)

Metodi comuni:
  void setRect(int x, int y, int w, int h)
  void pos(int x, int y)              // imposta posizione locale
  void size(int w, int h)             // imposta dimensioni
  void bg(int r, int g, int b)        // imposta colore di sfondo (per Label = colore testo)
  void setParent(Widget* p)
  Widget* getParent() const

  virtual void setTextColor(Uint8 r, Uint8 g, Uint8 b, Uint8 a = 255)
      // Colore del testo per i widget che lo usano (Button, TextInput).
      // Label NON lo usa: per Label il colore del testo è bg(...).

  void setValue(int v)
  int  getValue() const
  bool operator==(int v) const

  std::pair<int,int> getAbsolutePos() const
      // Posizione assoluta calcolata risalendo i genitori.

  virtual void center(const Window& win)
      // Centra il widget nella finestra.

  virtual void center(const Widget& parent)
      // Centra il widget nel rettangolo del genitore (es. dentro un Box).

  virtual void draw(SDL_Renderer*) = 0
  virtual bool handleClick(int x, int y) { return false; }
  virtual void setLabel(const std::string&) {}
  virtual ~Widget() {}


== Classe: Box (contenitore, eredita da Widget) ==
Costruttore:
  Box(int x, int y, int w, int h)

Metodi:
  void addWidget(Widget& w)
      // Aggiunge figlio e imposta il suo parent = this.

  void draw(SDL_Renderer* renderer) override
      // Disegna lo sfondo e poi tutti i figli.

  bool handleClick(int x, int y) override
      // Propaga i click ai figli (in ordine inverso, topmost-first).


== Classe: Button (eredita da Widget) ==
Costruttori:
  Button()                                         // default (100x40)
  Button(int x, int y, int w, int h, const std::string& text)

Metodi:
  void setLabel(const std::string& newText)
  std::string getLabel() const
  void setTextColor(Uint8 r, Uint8 g, Uint8 b, Uint8 a = 255)  // eredita/usa text_color
  void bg(int r, int g, int b)                                 // sfondo del bottone
  bool is_clicked()
      // True una sola volta dopo il click (si resetta).

  void draw(SDL_Renderer* renderer) override
  bool handleClick(int x, int y) override


== Classe: Label (eredita da Widget) ==
Costruttore:
  Label(const std::string& txt, int fontSize = 18)

Metodi:
  void draw(SDL_Renderer* renderer) override

NOTE:
  - La Label usa il colore impostato con bg(r,g,b) come COLORE DEL TESTO (non come sfondo).
  - Il suo rettangolo (w,h) viene inizializzato alla dimensione del testo al momento della creazione.


== Classe: TextInput (eredita da Widget) ==
Costruttore:
  TextInput()

Metodi:
  void insert()
      // Attiva l’input testuale (SDL_StartTextInput) e rende attivo il campo.

  bool handleClick(int x, int y) override
      // Attiva/disattiva il campo in base al click (gestisce Start/StopTextInput).

  bool handleTextInput(const SDL_Event& e)
      // Da non chiamare direttamente: Window::update() lo invoca per tutti i TextInput.

  void setTextColor(Uint8 r, Uint8 g, Uint8 b, Uint8 a = 255)
      // Colore del testo mostrato nel campo.

  const std::string& getText() const
  void clear()

  void draw(SDL_Renderer* renderer) override


---------------------------------
 FUNZIONAMENTO E UTILIZZO
---------------------------------

-- Creazione e ciclo finestra --
    Window win("Titolo");
    win.size(640, 480);
    win.bg(30,30,30);

    Button btn(100, 100, 150, 40, "Cliccami");
    btn.bg(0, 150, 200);
    btn.setTextColor(255,255,255);
    win.addWidget(btn);

    while (win.update()) {
        if (btn.is_clicked()) {
            // azione pulsante
        }
    }

-- Box e posizionamento relativo --
    Box panel(50, 50, 300, 200);
    panel.bg(200,200,200);

    Button ok(0,0,120,40,"OK");
    panel.addWidget(ok);
    ok.center(panel);  // centra nel box

    win.addWidget(panel);

-- TextInput --
    TextInput input;
    input.pos(50, 300);
    input.size(200, 40);
    input.bg(60, 60, 160);      // colore sfondo del campo
    input.setTextColor(255,255,255);
    input.insert();             // attiva input
    win.addWidget(input);

-- Label --
    Label title("Ciao mondo", 24);
    title.bg(255,255,0);        // colore TESTO (per Label)
    title.pos(20, 20);
    win.addWidget(title);

-- Valori utente (setValue/getValue) --
    btn.setValue(42);
    if (btn == 42) { /* ... */ }

-- Gestione scena/pagine --
    // Puoi usare clearWidgets() per cambiare "schermata":
    // win.clearWidgets(); poi aggiungi i nuovi widget.


---------------------------------
 NOTE IMPORTANTI
---------------------------------

- Ownership: Window/Box non distruggono i widget aggiunti; devi gestirne il ciclo di vita.
- Label: usa bg(r,g,b) come colore del testo (non c'è sfondo della label).
- Font: il codice apre "C:/Windows/Fonts/arial.ttf". Verifica che il font esista sul sistema.
- TextInput: Window::update() inoltra automaticamente SDL_TEXTINPUT/SDL_KEYDOWN ai TextInput attivi.
- center(): per i widget dentro Box, usa center(box) dopo averli aggiunti (o imposta pos(x,y)).


---------------------------------
 REQUISITI
---------------------------------

- SDL2
- SDL2_ttf
- Un file di font TrueType valido (es. Arial su Windows)


----------------------------------------
 AUTORE : Andrii Pidkaminetskyi (Akael4)
----------------------------------------
