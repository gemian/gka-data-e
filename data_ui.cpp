//
// Created by adam on 24/11/17.
//

#include <sqlite_file.h>
#include <Elementary.h>
#include <regex>
#include "data_ui.h"

static sqlite_file db;
data_ui ui;

static char *right_list_text_get(void *data, Evas_Object *obj, const char *part) {
    if (strcmp(part, "elm.text") == 0) {
        auto *buf = static_cast<char *>(malloc(MAX_LIST_LENGTH));
        auto row = db.readRowTitle((int) (uintptr_t) data);
        snprintf(buf, MAX_LIST_LENGTH, "%s", row.c_str());

        return buf;
    } else return NULL;
}

static void row_selected_cb(void *data, Evas_Object *obj, void *event_info) {
    Elm_Object_Item *it = (Elm_Object_Item *) event_info;
    auto i = (int) (uintptr_t) data;

    ui.rowSelected(i);
}

static void window_cb_key_down(void *data EINA_UNUSED, Evas *e EINA_UNUSED, Evas_Object *obj EINA_UNUSED, void *event_info) {
    ui.handleKeyDown(event_info);
}

void data_ui::handleKeyDown(void *event_info) {
    auto *ev = static_cast<Evas_Event_Key_Down *>(event_info);
    Eina_Bool ctrl, alt, shift;
    Elm_Object_Item *it;

    ctrl = evas_key_modifier_is_set(ev->modifiers, "Control");
    alt = evas_key_modifier_is_set(ev->modifiers, "Alt");
    shift = evas_key_modifier_is_set(ev->modifiers, "Shift");

//    EINA_LOG_ERR("KeyDown: %s - %s - %s", ev->key, ev->compose, ev->string);

    if (ctrl && shift) {
        if (!strcmp(ev->key, "N")) {
            newRow();
        } else if (!strcmp(ev->key, "E")) {
            editRow();
        }
    } else if (ctrl) {
        if (!strcmp(ev->key, "n")) {
            newFile();
        } else if (!strcmp(ev->key, "o")) {
            openFile();
        } else if (!strcmp(ev->key, "e")) {
            elm_exit();
        }
    }

    if (!strcmp(ev->key, "Escape")) {
        elm_object_text_set(searchEntry,"");
    } else if (!strcmp(ev->key, "Up")) {
    } else if (!strcmp(ev->key, "Down")) {
    } else if (!strcmp(ev->key, "Left")) {
        auto pos = elm_entry_cursor_pos_get(searchEntry);
        if (pos == oldSearchEntryPos) {
            auto sIt = elm_genlist_selected_item_get(rightList);
            it = elm_genlist_item_prev_get(sIt);
            if (!it) it = elm_genlist_last_item_get(rightList);
            elm_genlist_item_selected_set(it, EINA_TRUE);
            elm_genlist_item_show(it, ELM_GENLIST_ITEM_SCROLLTO_IN);
            clearFocus();
        }
        oldSearchEntryPos = pos;
    } else if (!strcmp(ev->key, "Right")) {
        auto pos = elm_entry_cursor_pos_get(searchEntry);
        if (pos == oldSearchEntryPos) {
            auto sIt = elm_genlist_selected_item_get(rightList);
            it = elm_genlist_item_next_get(sIt);
            if (!it) it = elm_genlist_first_item_get(rightList);
            elm_genlist_item_selected_set(it, EINA_TRUE);
            elm_genlist_item_show(it, ELM_GENLIST_ITEM_SCROLLTO_IN);
            clearFocus();
        }
        oldSearchEntryPos = pos;
    }
}

void data_ui::init() {
    window = elm_win_util_standard_add("Main", _("Data"));
    elm_win_autodel_set(window, EINA_TRUE);
    evas_object_resize(window, DEFAULT_APP_WIDTH, DEFAULT_APP_HEIGHT);

    right_list_itc = elm_genlist_item_class_new();
    right_list_itc->item_style = "default";
    right_list_itc->func.text_get = right_list_text_get;
    right_list_itc->func.content_get = NULL;
    right_list_itc->func.state_get = NULL;
    right_list_itc->func.del = NULL;

    Evas_Object *panes;

    // Add an elm_panes
    panes = elm_panes_add(window);
    evas_object_size_hint_weight_set(panes, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
    elm_win_resize_object_add(window, panes);
    evas_object_show(panes);

    // Create right hand pane
    rightList = elm_genlist_add(window);

    elm_genlist_homogeneous_set(rightList, EINA_TRUE);
    elm_genlist_multi_select_set(rightList, EINA_FALSE);
    evas_object_size_hint_weight_set(rightList, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
    evas_object_size_hint_align_set(rightList, EVAS_HINT_FILL, EVAS_HINT_FILL);
    evas_object_show(rightList);

    elm_object_part_content_set(panes, "right", rightList);

    // Created fields and search box
    Evas_Object *leftBox = elm_box_add(window);
    elm_object_part_content_set(panes, "left", leftBox);

    // Scroller for fields table
    Evas_Object *scroller = elm_scroller_add(window);
    evas_object_size_hint_weight_set(scroller, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
    evas_object_size_hint_align_set(scroller, EVAS_HINT_FILL, EVAS_HINT_FILL);
    elm_scroller_bounce_set(scroller, EINA_FALSE, EINA_FALSE);
    elm_scroller_policy_set(scroller, ELM_SCROLLER_POLICY_OFF, ELM_SCROLLER_POLICY_AUTO);
    evas_object_show(scroller);
    elm_box_pack_end(leftBox, scroller);

    // Create fields table
    fieldsTable = elm_table_add(window);
    evas_object_size_hint_weight_set(fieldsTable, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
    evas_object_size_hint_align_set(fieldsTable, EVAS_HINT_FILL, EVAS_HINT_FILL);
    evas_object_size_hint_padding_set(fieldsTable, 5, 5, 5, 5);
    elm_table_padding_set(fieldsTable, 6, 0);
    elm_table_align_set(fieldsTable, 0, 0);
    elm_table_homogeneous_set(fieldsTable, EINA_FALSE);
    evas_object_show(fieldsTable);
    elm_object_content_set(scroller, fieldsTable);

    // Create search box
    Evas_Object *searchBox = elm_box_add(window);
    elm_box_horizontal_set(searchBox, EINA_TRUE);
    elm_bg_color_set(searchBox, 100, 1, 1);
    evas_object_size_hint_weight_set(searchBox, EVAS_HINT_EXPAND, 0);
    evas_object_size_hint_align_set(searchBox, 0, 1);
    evas_object_show(searchBox);

    Evas_Object *searchLabel = elm_label_add(window);
    elm_object_text_set(searchLabel, _("Find: "));
    evas_object_size_hint_weight_set(searchLabel, 0, EVAS_HINT_EXPAND);
    evas_object_size_hint_align_set(searchLabel, 0, EVAS_HINT_FILL);
    elm_box_pack_end(searchBox, searchLabel);
    evas_object_show(searchLabel);

    searchEntry = elm_entry_add(window);
    elm_entry_single_line_set(searchEntry, EINA_TRUE);
    elm_entry_editable_set(searchEntry, EINA_TRUE);
    evas_object_size_hint_weight_set(searchEntry, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
    evas_object_size_hint_align_set(searchEntry, EVAS_HINT_FILL, EVAS_HINT_FILL);
    elm_box_pack_end(searchBox, searchEntry);
    elm_object_focus_set(searchEntry, EINA_TRUE);
    evas_object_show(searchEntry);

    Evas_Object *searchButton = elm_button_add(window);
    elm_object_text_set(searchButton, _("Search"));
    evas_object_size_hint_weight_set(searchButton, 0, 0);
    evas_object_size_hint_align_set(searchButton, 1, 1);
    elm_box_pack_end(searchBox, searchButton);
    evas_object_show(searchButton);

    elm_box_pack_end(leftBox, searchBox);

    elm_panes_content_left_size_set(panes, 0.7);

    evas_object_event_callback_add(searchEntry, EVAS_CALLBACK_KEY_DOWN, window_cb_key_down, searchEntry);
    elm_object_focus_allow_set(window, EINA_FALSE);
    elm_object_focus_allow_set(rightList, EINA_FALSE);
    elm_object_focus_allow_set(fieldsTable, EINA_FALSE);
    elm_object_focus_allow_set(leftBox, EINA_FALSE);
    elm_object_focus_allow_set(searchBox, EINA_FALSE);
    elm_object_focus_allow_set(searchLabel, EINA_FALSE);
    elm_object_focus_allow_set(searchButton, EINA_FALSE);

    elm_win_center(window, EINA_TRUE, EINA_TRUE);
    evas_object_show(window);

    menu.init(window);

}

void data_ui::setFile(std::string fileName) {
    db.file(fileName);
    elm_win_title_set(window, getTitleForFileName(fileName).c_str());

    repopulateFieldsTable();
    repopulateRightList();

    clearFocus();
}

std::string data_ui::getTitleForFileName(const std::string &fileName) const {
    return (_("Data: ") + fileName);
}

void data_ui::setNewFile() {
    if (newFileName.empty()) {
        newFileName = "data.db";
    }
    db.newFile(newFileName);
    elm_win_title_set(window, getTitleForFileName(newFileName).c_str());

    repopulateFieldsTable();
    repopulateRightList();

    clearFocus();
}

void data_ui::repopulateFieldsTable() const {
    elm_table_clear(fieldsTable, EINA_TRUE);
    auto cols = db.listColumns();

    for (int i = 0; i < cols.size(); i++) {
        auto field_name = elm_label_add(fieldsTable);
        elm_object_text_set(field_name, cols[i].c_str());
        evas_object_size_hint_align_set(field_name, 1, 0);
        evas_object_show(field_name);
        elm_table_pack(fieldsTable, field_name, 0, i, 1, 1);

        if (selectedRow) {
            auto row = db.readRow(selectedRow - 1);
            auto field_value = elm_label_add(fieldsTable);
            elm_object_text_set(field_value, row[i].c_str());
            evas_object_size_hint_weight_set(field_value, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
            evas_object_size_hint_align_set(field_value, 0, EVAS_HINT_FILL);
//            elm_label_line_wrap_set(field_value, ELM_WRAP_WORD); //TODO: We would like wordwrap, but this line makes the text disapear
            evas_object_show(field_value);
            elm_table_pack(fieldsTable, field_value, 1, i, 1, 1);
        }
    }
}

void data_ui::repopulateRightList() const {
    elm_genlist_clear(rightList);

    for (int i = 0; i < db.rowCount(); i++) {
        elm_genlist_item_append(rightList,
                                right_list_itc,
                                (void *) (uintptr_t) i,   // Item data
                                NULL,                    // Parent item for trees, NULL if none
                                ELM_GENLIST_ITEM_NONE,   // Item type; this is the common one
                                row_selected_cb,        // Callback on selection of the item
                                (void *) (uintptr_t) (i + 1)    // Data for that callback function
        );
    }
}

void data_ui::rowSelected(int i) {
    selectedRow = i;

    repopulateFieldsTable();
}

static void file_open_exit_cb(void *data, Evas_Object *obj, void *event_info) {
    evas_object_del((Evas_Object *) data);

    ui.clearFocus();
}

static void file_open_ok_cb(void *data, Evas_Object *obj, void *event_info) {
    evas_object_del((Evas_Object *) data);

    if (event_info) {
        struct stat s;
        if (stat((const char *) event_info, &s) == 0) {
            if (s.st_mode & S_IFREG) {
                ui.setFile((const char *) event_info);
            }
        }
    }
}

void data_ui::openFile() {
    Evas_Object *win = elm_win_add(window, "settings", ELM_WIN_BASIC);
    if (!win) return;

    elm_win_title_set(win, _("Open File"));
    elm_win_focus_highlight_enabled_set(win, EINA_TRUE);
    evas_object_smart_callback_add(win, "delete,request", file_open_exit_cb, win);

    Evas_Object *fs = elm_fileselector_add(win);
    evas_object_size_hint_weight_set(fs, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
    evas_object_size_hint_align_set(fs, EVAS_HINT_FILL, EVAS_HINT_FILL);
    evas_object_smart_callback_add(fs, "delete,request", file_open_exit_cb, win);
    evas_object_smart_callback_add(fs, "done", file_open_ok_cb, win);
    evas_object_show(fs);

    elm_fileselector_expandable_set(fs, EINA_TRUE);
    elm_fileselector_folder_only_set(fs, EINA_FALSE);
    elm_fileselector_path_set(fs, eina_environment_home_get());
    //http://fileformats.archiveteam.org/wiki/DB_(SQLite)
    elm_fileselector_mime_types_filter_append(fs, "application/x-sqlite3", "");
    elm_fileselector_sort_method_set(fs, ELM_FILESELECTOR_SORT_BY_FILENAME_ASC);

    elm_win_resize_object_add(win, fs);
    evas_object_resize(win, DEFAULT_DIALOG_WIDTH, DEFAULT_DIALOG_HEIGHT);
    elm_win_center(win, EINA_TRUE, EINA_TRUE);
    evas_object_show(win);
}

static void file_new_exit_cb(void *data, Evas_Object *obj, void *event_info) {
    elm_popup_dismiss((Evas_Object *) data);

    ui.clearFocus();
}

static void file_new_ok_cb(void *data, Evas_Object *obj, void *event_info) {
    elm_popup_dismiss((Evas_Object *) data);

    ui.setNewFile();
}

static void file_new_key_up_cb(void *data, Evas *e, Evas_Object *obj, void *event_info) {
    std::string newFilePath = eina_environment_home_get();
    newFilePath += "/";
    newFilePath += elm_object_text_get(obj);
    std::regex fileType("(db$|sqlite$|sqlite3$)");
    if (!std::regex_search(newFilePath, fileType)) {
        newFilePath += ".db";
    }
    ui.updateNewFileName(newFilePath);
}

void data_ui::newFile() {
    Evas_Object *popup = elm_popup_add(window);
    elm_object_part_text_set(popup, "title,text", _("New File"));

    Evas_Object *input = elm_entry_add(popup);
    elm_entry_single_line_set(input, EINA_TRUE);
    elm_entry_editable_set(input, EINA_TRUE);
    elm_entry_scrollable_set(input, EINA_TRUE);
    evas_object_size_hint_weight_set(input, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
    evas_object_size_hint_align_set(input, EVAS_HINT_FILL, EVAS_HINT_FILL);
    evas_object_event_callback_add(input, EVAS_CALLBACK_KEY_UP, file_new_key_up_cb, NULL);
    evas_object_show(input);
    elm_object_content_set(popup, input);

    Evas_Object *button = elm_button_add(popup);
    elm_object_text_set(button, _("Cancel"));
    elm_object_part_content_set(popup, "button1", button);
    evas_object_smart_callback_add(button, "clicked", file_new_exit_cb, popup);

    button = elm_button_add(popup);
    elm_object_text_set(button, _("OK"));
    elm_object_part_content_set(popup, "button2", button);
    evas_object_smart_callback_add(button, "clicked", file_new_ok_cb, popup);

    evas_object_show(popup);
    elm_object_focus_set(input, EINA_TRUE);
}

void data_ui::updateNewFileName(std::string fileName) {
    newFileName = std::move(fileName);
    clearFocus();
}

void data_ui::clearFocus() {
    elm_object_focus_set(searchEntry, EINA_TRUE);
}

static void edit_entry_key_up_cb(void *data, Evas *e, Evas_Object *obj, void *event_info) {
    std::string entryValue = elm_object_text_get(obj);
    ui.updateCurrentRowValue((int) (uintptr_t) data, entryValue);
}


static void edit_entry_exit_cb(void *data, Evas_Object *obj, void *event_info) {
    elm_popup_dismiss((Evas_Object *) data);
    ui.clearFocus();
}

static void edit_entry_ok_cb(void *data, Evas_Object *obj, void *event_info) {
    elm_popup_dismiss((Evas_Object *) data);
    ui.clearFocus();
    ui.saveCurrentRow();
}

void data_ui::newRow() {
    Evas_Object *popup = elm_popup_add(window);
    elm_object_part_text_set(popup, "title,text", _("New Entry"));

    auto cols = db.listColumns();
    currentRowValues.clear();
    for (int i = 0; i < cols.size(); i++) {
        currentRowValues.emplace_back("");
    }

    populateAndShowEntryPopup(popup, cols);
}

void data_ui::populateAndShowEntryPopup(Evas_Object *popup, const std::vector<std::string> &cols) const {
    elm_popup_scrollable_set(popup, EINA_TRUE);

    Evas_Object *popupTable = elm_table_add(window);
    evas_object_size_hint_weight_set(popupTable, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
    evas_object_size_hint_align_set(popupTable, EVAS_HINT_FILL, EVAS_HINT_FILL);
    evas_object_size_hint_padding_set(popupTable, 5, 5, 5, 5);
    elm_table_padding_set(popupTable, 6, 0);
    elm_table_align_set(popupTable, 0, 0);
    elm_table_homogeneous_set(popupTable, EINA_FALSE);
    elm_object_content_set(popup, popupTable);
    evas_object_show(popupTable);

    for (int i = (db.intPrimaryKey ? 1 : 0); i < cols.size(); i++) {
        auto field_name = elm_label_add(popupTable);
        elm_object_text_set(field_name, cols[i].c_str());
        evas_object_size_hint_align_set(field_name, 1, EVAS_HINT_FILL);
        elm_table_pack(popupTable, field_name, 0, i, 1, 1);
        evas_object_show(field_name);

        Evas_Object *input = elm_entry_add(popupTable);
        if (!currentRowValues[i].empty()) {
            elm_object_text_set(input, currentRowValues[i].c_str());
        }
        elm_entry_single_line_set(input, EINA_FALSE);
        elm_entry_editable_set(input, EINA_TRUE);
        evas_object_size_hint_weight_set(input, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
        evas_object_size_hint_align_set(input, EVAS_HINT_FILL, EVAS_HINT_FILL);
        evas_object_event_callback_add(input, EVAS_CALLBACK_KEY_UP, edit_entry_key_up_cb, (void *) (uintptr_t) i);
        elm_table_pack(popupTable, input, 1, i, 1, 1);
        evas_object_show(input);

    }

    Evas_Object *button = elm_button_add(popup);
    elm_object_text_set(button, _("Cancel"));
    elm_object_part_content_set(popup, "button1", button);
    evas_object_smart_callback_add(button, "clicked", edit_entry_exit_cb, popup);
//    evas_object_smart_callback_add(button, "activated", file_new_exit_cb, popup); //TODO: Figure out keyboard enter/space activation on focused button

    button = elm_button_add(popup);
    elm_object_text_set(button, _("OK"));
    elm_object_part_content_set(popup, "button2", button);
    evas_object_smart_callback_add(button, "clicked", edit_entry_ok_cb, popup);
//    evas_object_smart_callback_add(button, "activated", file_new_ok_cb, popup); //TODO: Figure out keyboard enter/space activation on focused button

    evas_object_show(popup);
}

void data_ui::editRow() {
    Evas_Object *popup = elm_popup_add(window);
    elm_object_part_text_set(popup, "title,text", _("Edit Entry"));

    auto cols = db.listColumns();
    auto rows = db.readRow(selectedRow - 1);
    currentRowValues.clear();
    for (int i = 0; i < cols.size(); i++) {
        currentRowValues.emplace_back(rows[i]);
    }

    populateAndShowEntryPopup(popup, cols);
}

void data_ui::updateCurrentRowValue(int i, std::string value) {
    currentRowValues[i] = std::move(value);
}

void data_ui::saveCurrentRow() {
    db.addRow(currentRowValues);

    repopulateFieldsTable();
    repopulateRightList();
}
