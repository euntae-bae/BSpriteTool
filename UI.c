#include <allegro5/allegro.h>
#include <allegro5/allegro_font.h>
#include <allegro5/allegro_ttf.h>
#include <allegro5/allegro_primitives.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include "UI.h"

static void initMenuBar(UIMenuBar* ui);
static void drawMenuBar(UIMenuBar* ui);
static void cleanupMenuBar(UIMenuBar* ui);
static void initMenu(UIMenu* ui, int x1, int y1, int width, const char* label);
static int addMenu(UIMenuBar* ui, const char* label);
static void drawMenu(UIMenu* ui);
static void cleanupMenu(UIMenu* ui);
static void initMenuItem(UIMenuItem* ui, int x1, int y1, int width, const char* label);
static int addMenuItem(UIMenu* ui, const char* label);
static void drawMenuItems(UIMenu* ui); // 메뉴가 선택(clicked)됐을 때 호출됨

static bool isMouseOn(const RECT *area);
static const char* getLabel(unsigned int id);

Size screenSize;
extern ALLEGRO_KEYBOARD_STATE kbState;
extern ALLEGRO_MOUSE_STATE mouseState;

unsigned int uiCnt = 0;
UIReference uiRef[BUF_SIZE]; // UI의 ID에 의해 접근할 수 있는 UI객체들의 주소 값
UIMenuBar mainMenuBar;

ALLEGRO_FONT* uiFontR;
ALLEGRO_COLOR uiColorBackground;
ALLEGRO_COLOR uiColorFont;
ALLEGRO_COLOR uiColorFontDisabled;
ALLEGRO_COLOR uiColorMenu;
ALLEGRO_COLOR uiColorMenuHighlighted;
ALLEGRO_COLOR uiColorMenuClicked;
ALLEGRO_COLOR uiColorMenuShadow;
ALLEGRO_COLOR uiColorFrame;

void initMenuBar(UIMenuBar* ui)
{
	ui->width = screenSize.width;
	ui->height = UI_MENU_BAR_HEIGHT;
	ui->x1 = 0;
	ui->y1 = 0;
	ui->menuHead = NULL;
	ui->menuTail = NULL;
}

void drawMenuBar(UIMenuBar* ui)
{
	al_draw_filled_rectangle(ui->x1, ui->y1, ui->x1 + ui->width, ui->height, uiColorMenu);

	UIMenu* node = ui->menuHead;
	while (node) {
		drawMenu(node);
		if (node->clicked) {
			al_draw_line(node->x1, node->y1 + UI_MARGIN_V, node->x1 + node->width, node->y1 + UI_MARGIN_V, uiColorFrame, 1); // top
			al_draw_line(node->x1, node->y1 + UI_MARGIN_V, node->x1, node->y1 + node->height, uiColorFrame, 1); // left
			al_draw_line(node->x1 + node->width, node->y1 + UI_MARGIN_V, node->x1 + node->width, node->y1 + ui->height, uiColorFrame, 1); // right
			al_draw_line(node->x1, node->y1 + node->height, node->x1 + node->width, node->y1 + node->height, uiColorFrame, 1); // bottom
			drawMenuItems(node);
		}
		node = node->next;
	}
}

void cleanupMenuBar(UIMenuBar* ui)
{
	UIMenu* node = ui->menuHead;
	UIMenu* rmNode;
	while (node) {
		rmNode = node;
		node = node->next;
		cleanupMenu(rmNode);
		free(rmNode);
	}
}

void initMenu(UIMenu* ui, int x1, int y1, int width, const char *label)
{
	ui->x1 = x1;
	ui->y1 = y1;
	ui->width = width;
	ui->height = UI_MENU_BAR_HEIGHT;
	strcpy(ui->label, label);
	ui->enabled = true;
	ui->highlighted = false;
	ui->clicked = false;
	ui->prev = NULL;
	ui->next = NULL;
	ui->menuItemHead = NULL;
	ui->menuItemTail = NULL;
}

int addMenu(UIMenuBar* ui, const char *label)
{
	printf("addMenu()\n");
	UIMenu* node = (UIMenu*)malloc(sizeof(UIMenu));
	int x1 = ui->x1, y1 = ui->y1;
	int width = strlen(label) * UI_FONT_SIZE_PX + UI_MARGIN_H * 2;

	if (!node) {
		fprintf(stderr, "failed to create UIMenu node.\n");
		return -1;
	}

	printf("width: %d\n", width);
	// 첫 번째 노드 삽입
	if (!ui->menuHead) {
		initMenu(node, x1, y1, width, label);
		ui->menuHead = node;
		ui->menuTail = ui->menuHead;
	}
	else {
		const UIMenu* temp = ui->menuHead;
		while (temp) {
			x1 += temp->width;
			temp = temp->next;
		}
		initMenu(node, x1, y1, width, label);
		node->prev = ui->menuTail;
		node->next = NULL;
		ui->menuTail->next = node;
		ui->menuTail = node;
	}
	printf("x1: %d, y1: %d\n\n", x1, y1);
	node->id = uiCnt++;
	uiRef[node->id].ptr = node;
	uiRef[node->id].type = UI_MENU;
	return node->id;
}

void drawMenu(UIMenu* ui)
{
	if (ui->highlighted)
		al_draw_filled_rectangle(ui->x1, ui->y1 + UI_MARGIN_V, ui->x1 + ui->width, ui->y1 + ui->height - UI_MARGIN_V, uiColorMenuHighlighted);
	
	if (ui->clicked)
		al_draw_filled_rectangle(ui->x1, ui->y1 + UI_MARGIN_V, ui->x1 + ui->width, ui->y1 + ui->height - UI_MARGIN_V, uiColorMenuClicked);
	
	if (ui->enabled)
		al_draw_text(uiFontR, uiColorFont, ui->x1 + UI_MARGIN_H, ui->y1 + UI_MARGIN_V, 0, ui->label);
	else
		al_draw_text(uiFontR, uiColorFontDisabled, ui->x1 + UI_MARGIN_H, ui->y1 + UI_MARGIN_V, 0, ui->label);
}

void cleanupMenu(UIMenu* ui)
{
	printf("cleanupMenu()\n");
	UIMenuItem* node = ui->menuItemHead;
	UIMenuItem* rmNode;
	while (node) {
		printf("cleanup menu item\n");
		rmNode = node;
		node = node->next;
		free(rmNode);
	}
}

void initMenuItem(UIMenuItem* ui, int x1, int y1, int width, const char* label)
{
	ui->x1 = x1;
	ui->y1 = y1;
	ui->width = width;
	ui->height = UI_MENU_BAR_HEIGHT;
	ui->enabled = true;
	ui->highlighted = false;
	ui->clicked = false;
	ui->prev = NULL;
	ui->next = NULL;
	strcpy(ui->label, label);
}

int addMenuItem(UIMenu* ui, const char* label)
{
	printf("addMenuItem()\n");
	UIMenuItem* node = (UIMenuItem*)malloc(sizeof(UIMenuItem));
	int x1 = ui->x1;
	int y1 = ui->y1 + UI_MARGIN_V + UI_MENU_HEIGHT;

	if (!node) {
		fprintf(stderr, "failed to create UIMenuItem node.\n");
		return -1;
	}

	if (!ui->menuItemHead) {
		initMenuItem(node, x1, y1, UI_MENU_ITEM_WIDTH, label);
		ui->menuItemHead = node;
		ui->menuItemTail = ui->menuItemHead;
	}
	else {
		const UIMenuItem* temp = ui->menuItemHead;
		while (temp) {
			y1 += temp->height;
			temp = temp->next;
		}
		initMenuItem(node, x1, y1, UI_MENU_ITEM_WIDTH, label);
		node->prev = ui->menuItemTail;
		node->next = NULL;
		ui->menuItemTail->next = node;
		ui->menuItemTail = node;
	}
	node->id = uiCnt++;
	uiRef[node->id].ptr = node;
	uiRef[node->id].type = UI_MENU_ITEM;
	return node->id;
}

void drawMenuItems(UIMenu* ui)
{
	UIMenuItem* node = ui->menuItemHead;
	int uiHeight = 0;

	if (!node)
		return;

	while (node) {
		uiHeight += node->height;
		node = node->next;
	}
	node = ui->menuItemHead;
	
	// 메뉴 아이템 전체 영역
	al_draw_filled_rectangle(node->x1 + UI_SHADOW_OFFSET, node->y1 + UI_SHADOW_OFFSET, node->x1 + node->width + UI_SHADOW_OFFSET, node->y1 + uiHeight + UI_SHADOW_OFFSET, uiColorMenuClicked);
	al_draw_filled_rectangle(node->x1, node->y1, node->x1 + node->width, node->y1 + uiHeight, uiColorMenuClicked);
	
	// 테두리
	al_draw_line(node->x1 + ui->width, node->y1, node->x1 + node->width, node->y1, uiColorFrame, 1); // top
	al_draw_line(node->x1, node->y1, node->x1, node->y1 + uiHeight, uiColorFrame, 1); // left
	al_draw_line(node->x1 + node->width, node->y1, node->x1 + node->width, node->y1 + uiHeight, uiColorFrame, 1); // right
	al_draw_line(node->x1, node->y1 + uiHeight, node->x1 + node->width, node->y1 + uiHeight, uiColorFrame, 1); // bottom

	while (node) {
		if (node->highlighted) {
			al_draw_filled_rectangle(node->x1 + UI_MARGIN_H_MENU_ITEM, node->y1 + UI_MARGIN_V, node->x1 + node->width - UI_MARGIN_H_MENU_ITEM, node->y1 + node->height - UI_MARGIN_V, uiColorMenuHighlighted);
		}

		if (node->enabled) {
			al_draw_text(uiFontR, uiColorFont, node->x1 + UI_MARGIN_H, node->y1 + UI_MARGIN_V, 0, node->label);
		}
		else {
			al_draw_text(uiFontR, uiColorFontDisabled, node->x1 + UI_MARGIN_H, node->y1 + UI_MARGIN_V, 0, node->label);
		}
		node = node->next;
	}
}

bool isMouseOn(const RECT* area)
{
	return (
		mouseState.x >= area->x &&
		mouseState.y >= area->y &&
		mouseState.x <= area->x + area->width &&
		mouseState.y <= area->y + area->height
	);
}

const char* getLabel(unsigned int id)
{
	if (id < 0 || id >= uiCnt)
		return NULL;

	if (uiRef[id].type == UI_MENU) {
		return ((UIMenu*)uiRef[id].ptr)->label;
	}
	else if (uiRef[id].type == UI_MENU_ITEM) {
		return ((UIMenuItem*)uiRef[id].ptr)->label;
	}

	return NULL;
}

void initUI(void)
{
	uiFontR = al_load_ttf_font("fonts/UbuntuMono-R.ttf", 18, 0);

	uiColorBackground = al_map_rgb(199, 209, 224);
	uiColorFont = al_map_rgb(0, 0, 0);
	uiColorFontDisabled = al_map_rgb(80, 80, 80);
	uiColorMenu = al_map_rgb(236, 236, 236);
	uiColorMenuHighlighted = al_map_rgb(144, 200, 246);
	uiColorMenuClicked = al_map_rgb(192, 192, 192);
	uiColorMenuShadow = al_map_rgba(192, 192, 192, 128);
	uiColorFrame = al_map_rgb(210, 210, 210);
	//uiColorFrame = al_map_rgb(255, 0, 0);

	int id;
	UIMenu* pmenu = NULL;
	UIMenuItem* pitem = NULL;

	initMenuBar(&mainMenuBar);
	
	id = addMenu(&mainMenuBar, "File");
	pmenu = (UIMenu*)uiRef[id].ptr;
	addMenuItem(pmenu, "Save");
	addMenuItem(pmenu, "Save as");
	addMenuItem(pmenu, "Load");
	addMenuItem(pmenu, "Close");

	id = addMenu(&mainMenuBar, "Edit");
	pmenu = (UIMenu*)uiRef[id].ptr;
	pmenu->enabled = false;

	addMenu(&mainMenuBar, "About");
}

void updateUI(const ALLEGRO_EVENT* ev)
{
	UIMenu* pmenu = NULL;
	UIMenuItem* pitem = NULL;
	bool menuOpened = false;
	unsigned int lastSelectedMenuID = -1;
	RECT uiArea = { 0, };
	unsigned int i;

	mainMenuBar.width = screenSize.width;

	for (i = 0; i < uiCnt; i++) {
		pmenu = NULL;
		pitem = NULL;
		if (uiRef[i].type == UI_MENU) {
			pmenu = (UIMenu*)uiRef[i].ptr;
			uiArea.x = pmenu->x1;
			uiArea.y = pmenu->y1;
			uiArea.width = pmenu->width;
			uiArea.height = pmenu->height;

			if (ev->type == ALLEGRO_EVENT_MOUSE_BUTTON_DOWN) {
				if (isMouseOn(&uiArea) && pmenu->enabled) {
					printf("menu[id=%d, label=%s] is clicked.\n", i, getLabel(i));
					pmenu->clicked = true;
					menuOpened = true;
					lastSelectedMenuID = i;
				}
				else {
					menuOpened = false;
				}
			}
			else {
				if (isMouseOn(&uiArea)) { // 마우스를 올려놓기만 한 경우
					if (menuOpened) { // 메뉴가 열려있는 상태: 마우스 포인터가 놓여있는 메뉴를 연다.
						if (lastSelectedMenuID != i) {
							pmenu->clicked = true;
							((UIMenu*)uiRef[lastSelectedMenuID].ptr)->clicked = false;
							lastSelectedMenuID = i;
						}
					}
					else { // 메뉴가 닫혀있는 상태: 강조 표시만
						pmenu->highlighted = true;
					}
				}
				else {
					if (!menuOpened)
						pmenu->highlighted = false;
				}
			}
		}
		else if (uiRef[i].type == UI_MENU_ITEM) {
			pitem = (UIMenuItem*)uiRef[i].ptr;
			uiArea.x = pitem->x1;
			uiArea.y = pitem->y1;
			uiArea.width = pitem->width;
			uiArea.height = pitem->height;

			if (ev->type == ALLEGRO_EVENT_MOUSE_BUTTON_UP) {
				if (isMouseOn(&uiArea) && pitem->enabled) {
					printf("menu item[id=%d, label=%s] is selected.\n", i, getLabel(i));
				}
				else {
					menuOpened = false;
				}
			}
			else {
				if (isMouseOn(&uiArea)) {
					pitem->highlighted = true;
				}
				else {
					pitem->highlighted = false;
				}
			}
		}
	}
}

void drawUI(void)
{
	drawMenuBar(&mainMenuBar);
}

void cleanupUI(void)
{
	cleanupMenuBar(&mainMenuBar);
}