====================
  GUI Library (SDL2)
====================

This library simplifies creating graphical interfaces using SDL2 and SDL2_ttf.  
It includes window management, text input, nested widgets, buttons, labels,  
and container boxes with relative positioning.

---------------------
 CLASSES & FUNCTIONS
---------------------

== Class: Window ==
Constructor:
  Window(const std::string& title, int w = 640, int h = 480)

Destructor:
  ~Window()

Methods:
  void bg(int r, int g, int b)
      // Sets the background color of the window.

  void addWidget(Widget& w)
      // Adds a widget to the window (does not take ownership).

  void clearWidgets()
      // Removes all registered widgets from the window (does not destroy them).

  std::pair<int,int> getSize() const
      // Returns {width, height} of the window.

  void size(int w, int h)
      // Changes window dimensions.

  void close()
      // Closes the window programmatically (update() will return false).

  bool update()
      // Handles events (mouse, keyboard, text input), redraws all widgets, and presents the frame.
      // Typically used like: while (win.update()) { ... }


== Class: Widget (abstract base class for all widgets) ==
Main fields/properties:
  - Local rectangle (x, y, w, h)
  - Background color (bg_r, bg_g, bg_b) — NOTE: for Label this is the text color
  - Pointer to parent (Widget* parent)
  - SDL_Color text_color (used by Button and TextInput)
  - int value (user-defined value that can be assigned to a widget)

Common methods:
  void setRect(int x, int y, int w, int h)
  void pos(int x, int y)              // sets local position
  void size(int w, int h)             // sets dimensions
  void bg(int r, int g, int b)        // sets background color (for Label = text color)
  void setParent(Widget* p)
  Widget* getParent() const

  virtual void setTextColor(Uint8 r, Uint8 g, Uint8 b, Uint8 a = 255)
      // Sets the text color for widgets that use it (Button, TextInput).
      // Label does NOT use this: for Label, bg(...) changes text color.

  void setValue(int v)
  int  getValue() const
  bool operator==(int v) const

  std::pair<int,int> getAbsolutePos() const
      // Returns absolute position by walking up the parent chain.

  virtual void center(const Window& win)
      // Centers the widget in the window.

  virtual void center(const Widget& parent)
      // Centers the widget inside the parent widget's rectangle (e.g., inside a Box).

  virtual void draw(SDL_Renderer*) = 0
  virtual bool handleClick(int x, int y) { return false; }
  virtual void setLabel(const std::string&) {}
  virtual ~Widget() {}


== Class: Box (container, inherits Widget) ==
Constructor:
  Box(int x, int y, int w, int h)

Methods:
  void addWidget(Widget& w)
      // Adds a child widget and sets its parent = this.

  void draw(SDL_Renderer* renderer) override
      // Draws the background, then all child widgets.

  bool handleClick(int x, int y) override
      // Forwards clicks to child widgets (reverse order, topmost-first).


== Class: Button (inherits Widget) ==
Constructors:
  Button()                                         // default (100x40)
  Button(int x, int y, int w, int h, const std::string& text)

Methods:
  void setLabel(const std::string& newText)
  std::string getLabel() const
  void setTextColor(Uint8 r, Uint8 g, Uint8 b, Uint8 a = 255)  // uses text_color
  void bg(int r, int g, int b)                                 // background color
  bool is_clicked()
      // Returns true once after a click (then resets).

  void draw(SDL_Renderer* renderer) override
  bool handleClick(int x, int y) override


== Class: Label (inherits Widget) ==
Constructor:
  Label(const std::string& txt, int fontSize = 18)

Methods:
  void draw(SDL_Renderer* renderer) override

NOTES:
  - Label uses the color set with bg(r,g,b) as TEXT COLOR (there is no background).
  - The widget’s (w,h) is initialized to the size of the text at creation.


== Class: TextInput (inherits Widget) ==
Constructor:
  TextInput()

Methods:
  void insert()
      // Activates text input (SDL_StartTextInput) and makes the field active.

  bool handleClick(int x, int y) override
      // Activates/deactivates the field on click (handles Start/StopTextInput).

  bool handleTextInput(const SDL_Event& e)
      // Usually not called directly: Window::update() calls it for all active TextInputs.

  void setTextColor(Uint8 r, Uint8 g, Uint8 b, Uint8 a = 255)
      // Sets the text color displayed in the field.

  const std::string& getText() const
  void clear()

  void draw(SDL_Renderer* renderer) override


---------------------------------
 HOW IT WORKS & USAGE
---------------------------------

-- Creating a window and loop --
    Window win("Title");
    win.size(640, 480);
    win.bg(30,30,30);

    Button btn(100, 100, 150, 40, "Click me");
    btn.bg(0, 150, 200);
    btn.setTextColor(255,255,255);
    win.addWidget(btn);

    while (win.update()) {
        if (btn.is_clicked()) {
            // button action
        }
    }

-- Box and relative positioning --
    Box panel(50, 50, 300, 200);
    panel.bg(200,200,200);

    Button ok(0,0,120,40,"OK");
    panel.addWidget(ok);
    ok.center(panel);  // center in the box

    win.addWidget(panel);

-- TextInput --
    TextInput input;
    input.pos(50, 300);
    input.size(200, 40);
    input.bg(60, 60, 160);      // background color
    input.setTextColor(255,255,255);
    input.insert();             // activate input
    win.addWidget(input);

-- Label --
    Label title("Hello world", 24);
    title.bg(255,255,0);        // text color (Label only)
    title.pos(20, 20);
    win.addWidget(title);

-- User values (setValue/getValue) --
    btn.setValue(42);
    if (btn == 42) { /* ... */ }

-- Scene/page management --
    // You can use clearWidgets() to change the "screen":
    // win.clearWidgets(); then add new widgets.


---------------------------------
 IMPORTANT NOTES
---------------------------------

- Ownership: Window/Box do not destroy the widgets you add; you must manage their lifetime.
- Label: uses bg(r,g,b) as text color (no background).
- Font: code loads "C:/Windows/Fonts/arial.ttf". Ensure the font exists on your system.
- TextInput: Window::update() automatically forwards SDL_TEXTINPUT/SDL_KEYDOWN to active inputs.
- center(): for widgets inside a Box, use center(box) after adding them (or set pos(x,y)).


---------------------------------
 REQUIREMENTS
---------------------------------

- SDL2
- SDL2_ttf
- A valid TrueType font file (e.g., Arial on Windows)


----------------------------------------
 AUTHOR : Andrii Pidkaminetskyi (Akael4)
----------------------------------------
