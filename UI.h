#ifndef __UI_H__
#define __UI_H__

#define DEFAULT_SCREEN_WIDTH	720
#define DEFAULT_SCREEN_HEIGHT	480
#define UI_MENU_BAR_HEIGHT		30
#define UI_MENU_HEIGHT			20
#define UI_MENU_ITEM_WIDTH		100
#define UI_MARGIN_H_MENU_ITEM	3
#define UI_MARGIN_V				5
#define UI_MARGIN_H				10
#define LABEL_LENGTH			64

#define UI_SHADOW_OFFSET		5

// https://websemantics.uk/articles/font-size-conversion/
#define UI_FONT_SIZE_PT			20
#define UI_FONT_SIZE_PX			10

#define BUF_SIZE				256

typedef struct Size {
	unsigned int width;
	unsigned int height;
} Size;

typedef struct MenuBar {
	int x1, y1;
	int width, height;
	unsigned int selectedMenuID;
	struct Menu* menuHead;
	struct Menu* menuTail;
} UIMenuBar;

typedef struct Menu {
	unsigned int id;
	int x1, y1;
	int width, height;
	char label[LABEL_LENGTH];
	bool enabled;
	bool highlighted;
	bool clicked;
	struct Menu* prev;
	struct Menu* next;
	struct MenuItem* menuItemHead;
	struct MenuItem* menuItemTail;
} UIMenu;

typedef struct MenuItem {
	unsigned int id;
	int x1, y1;
	int width, height;
	char label[LABEL_LENGTH];
	bool enabled;
	bool highlighted;
	bool clicked;
	struct MenuItem* prev;
	struct MenuItem* next;
} UIMenuItem;

enum UIType {
	UI_MENU_BAR = 0,
	UI_MENU,
	UI_MENU_ITEM
};

typedef struct UIReference {
	int type;
	void* ptr;
} UIReference;

typedef struct RECT {
	int x, y;
	int width, height;
} RECT;

void initUI(void);
void cleanupUI(void);
void updateUI(const ALLEGRO_EVENT *ev);
void drawUI(void);

#endif