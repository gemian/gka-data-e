//
// Created by adam on 24/11/17.
//

#ifndef GKA_DATA_E_DATA_UI_H
#define GKA_DATA_E_DATA_UI_H

#include <string>
#include <vector>
#include <map>
#include <stack>
#include "data_menu.h"

#ifdef ENABLE_NLS
# include <libintl.h>
# define _(x) gettext(x)
#else
# define _(x) (x)
#endif

static const int MAX_LIST_LENGTH = 255;

static const int DEFAULT_APP_WIDTH = 216 * 3;
static const int DEFAULT_APP_HEIGHT = 108 * 3;
static const int DEFAULT_DIALOG_WIDTH = 216 * 2;
static const int DEFAULT_DIALOG_HEIGHT = DEFAULT_APP_HEIGHT;

static const int  STACKMAXSIZE = 10;
struct stack {
    Evas_Object *sObject[STACKMAXSIZE];
    Evas_Object *sFocus[STACKMAXSIZE];
    int topIndex = -1;
};

typedef struct stack STACK;

#define MENU_ELLIPSIS(S) eina_slstr_printf("%s...", S)

class data_ui {

public:
    Evas_Object *searchEntry;
    Evas_Object *rightList;
    Evas_Object *fieldsTable;
    Evas_Object *window;
    Evas_Object *scroller;
    Elm_Genlist_Item_Class *right_list_itc;
    int selectedRow;
    std::string newFileName;
    std::vector<std::string> currentRowValues;
    data_menu menu;
    int oldSearchEntryPos;
    std::vector<std::string> editableColumns;
    int editableColumnsIndex;
    std::string editableColumnsEditLabel;
    std::map<std::string, std::string> renames;
    STACK popupStack;

public:
    void init();

    void setFile(std::string fileName);

    void rowSelected(int i);

    void setNewFile();

    void repopulateFieldsTable();

    void repopulateRightList(int selected=0) const;

    void openFile();

    void newFile();

    void updateNewFileName(std::string fileName);

    void clearFocus();

    void newEntry();

    void editRow();

    void deleteCurrentRow();

    void updateCurrentRowValue(int i, std::string value);

    void saveCurrentRow();

    std::string getTitleForFileName(const std::string &fileName) const;

    void populateAndShowEntryPopup(Evas_Object *popup, const std::vector<std::string> &cols);

    void handleKeyDown(void *event_info);

    void prevButton();

    void nextButton();

    void prevItem();

    void nextItem();

    void repopulateUI();

    void labelPreferences();

    void saveLabelPreferences();

    std::vector<std::string> getEditableColumns();

    void editColumnMoveUp();

    void editColumnMoveDown();

    void editColumnDelete();

    void setEditColumnSelection(int i);

    int getEditColumnSelection();

    void updateEditLabel(std::string label);

    void addEditableLabel();

    std::string getEditLabel();

    void saveEditableLabel();

    Eina_Bool labelPreferencesAreValid();

    void deleteEntry();

    void clearActivePopup();

    void showPopup(Evas_Object *popup, Evas_Object *focusOn = nullptr);

    void popupStackPush(Evas_Object *toPush, Evas_Object *focusOn);

    Evas_Object *popupStackPop();

    bool popupStackEmpty();

    Evas_Object *popupStackTopFocus();

    void clearLabelPreferences();
};

extern data_ui ui;

#endif //GKA_DATA_E_DATA_UI_H
