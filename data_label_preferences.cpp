//
// Created by adam on 26/11/17.
//

#include <sqlite_file.h>
#include "Elementary.h"
#include "data_label_preferences.h"
#include "data_ui.h"

static void label_pref_selection_cb(void *data, Evas_Object *obj, void *event_info) {
    auto index = (int)(uintptr_t) data;
    ui.setEditColumnSelection(index);
}

static void populate_list(Evas_Object *list) {
    auto cols = ui.getEditableColumns();
    elm_list_clear(list);
    for (int i = 0; i < cols.size(); i++) {
        auto item = elm_list_item_append(list, cols[i].c_str(), nullptr, nullptr, label_pref_selection_cb, (void *) (uintptr_t) i);
        if (i == ui.getEditColumnSelection()) {
            elm_list_item_selected_set(item, EINA_TRUE);
        }
    }
}

static void label_pref_up_cb(void *data, Evas_Object *obj, void *event_info) {
    auto list = (Evas_Object *) data;
    ui.editColumnMoveUp();
    populate_list(list);
}

static void label_pref_down_cb(void *data, Evas_Object *obj, void *event_info) {
    auto list = (Evas_Object *) data;
    ui.editColumnMoveDown();
    populate_list(list);
}

static void label_pref_update_label_key_up_cb(void *data, Evas *e, Evas_Object *obj, void *event_info) {
    std::string label = elm_object_text_get(obj);
    ui.updateEditLabel(label);
}

static void on_elm_popup_event_dismissed(void *data, Evas *e, Evas_Object *obj, void *event_info) {
    auto list = (Evas_Object *) data;
    populate_list(list);
}

static void label_pref_addedit_label_exit_cb(void *data, Evas_Object *obj, void *event_info) {
    elm_popup_dismiss((Evas_Object *) data);
}

static void label_pref_edit_label_ok_cb(void *data, Evas_Object *obj, void *event_info) {
    elm_popup_dismiss((Evas_Object *) data);
    ui.saveEditableLabel();
}

static void label_pref_edit_cb(void *data, Evas_Object *obj, void *event_info) {
    auto list = (Evas_Object *) data;

    Evas_Object *popup = elm_popup_add(list);
    elm_object_part_text_set(popup, "title,text", _("Edit Label"));

    Evas_Object *input = elm_entry_add(popup);
    elm_entry_single_line_set(input, EINA_TRUE);
    elm_entry_editable_set(input, EINA_TRUE);
    elm_entry_scrollable_set(input, EINA_TRUE);
    auto inputValue = ui.getEditLabel();
    elm_object_text_set(input, inputValue.c_str());
    evas_object_size_hint_weight_set(input, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
    evas_object_size_hint_align_set(input, EVAS_HINT_FILL, EVAS_HINT_FILL);
    evas_object_event_callback_add(input, EVAS_CALLBACK_KEY_UP, label_pref_update_label_key_up_cb, NULL);
    evas_object_show(input);
    elm_object_content_set(popup, input);

    Evas_Object *button = elm_button_add(popup);
    elm_object_text_set(button, _("Cancel"));
    elm_object_part_content_set(popup, "button1", button);
    evas_object_smart_callback_add(button, "clicked", label_pref_addedit_label_exit_cb, popup);

    button = elm_button_add(popup);
    elm_object_text_set(button, _("OK"));
    elm_object_part_content_set(popup, "button2", button);
    evas_object_smart_callback_add(button, "clicked", label_pref_edit_label_ok_cb, popup);

    evas_object_show(popup);
    elm_object_focus_set(input, EINA_TRUE);

    evas_object_event_callback_add(popup, EVAS_CALLBACK_HIDE, on_elm_popup_event_dismissed, list);

    populate_list(list);
}

static void label_pref_add_label_ok_cb(void *data, Evas_Object *obj, void *event_info) {
    elm_popup_dismiss((Evas_Object *) data);
    ui.addEditableLabel();
}

static void label_pref_add_cb(void *data, Evas_Object *obj, void *event_info) {
    auto list = (Evas_Object *) data;

    Evas_Object *popup = elm_popup_add(list);
    elm_object_part_text_set(popup, "title,text", _("Add Label"));

    Evas_Object *input = elm_entry_add(popup);
    elm_entry_single_line_set(input, EINA_TRUE);
    elm_entry_editable_set(input, EINA_TRUE);
    elm_entry_scrollable_set(input, EINA_TRUE);
    evas_object_size_hint_weight_set(input, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
    evas_object_size_hint_align_set(input, EVAS_HINT_FILL, EVAS_HINT_FILL);
    evas_object_event_callback_add(input, EVAS_CALLBACK_KEY_UP, label_pref_update_label_key_up_cb, NULL);
    evas_object_show(input);
    elm_object_content_set(popup, input);

    Evas_Object *button = elm_button_add(popup);
    elm_object_text_set(button, _("Cancel"));
    elm_object_part_content_set(popup, "button1", button);
    evas_object_smart_callback_add(button, "clicked", label_pref_addedit_label_exit_cb, popup);

    button = elm_button_add(popup);
    elm_object_text_set(button, _("OK"));
    elm_object_part_content_set(popup, "button2", button);
    evas_object_smart_callback_add(button, "clicked", label_pref_add_label_ok_cb, popup);

    evas_object_show(popup);
    elm_object_focus_set(input, EINA_TRUE);

    evas_object_event_callback_add(popup, EVAS_CALLBACK_HIDE, on_elm_popup_event_dismissed, list);

    populate_list(list);
}

static void label_pref_delete_cb(void *data, Evas_Object *obj, void *event_info) {
    auto list = (Evas_Object *) data;
    ui.editColumnDelete();
    populate_list(list);
}

static void label_pref_exit_cb(void *data, Evas_Object *obj, void *event_info) {
    elm_popup_dismiss((Evas_Object *) data);
    ui.clearFocus();
}

static void label_pref_ok_cb(void *data, Evas_Object *obj, void *event_info) {
    if (ui.labelPreferencesAreValid()) {
        elm_popup_dismiss((Evas_Object *) data);
        ui.saveLabelPreferences();
        ui.clearFocus();
    }
}

void data_label_preferences::show(Evas_Object *window) {
    Evas_Object *popup = elm_popup_add(window);
    elm_object_part_text_set(popup, "title,text", _("Label preferences"));
    elm_popup_scrollable_set(popup, EINA_TRUE);

    auto hbox = elm_box_add(popup);
    elm_box_horizontal_set(hbox, EINA_TRUE);
    evas_object_size_hint_weight_set(hbox, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
    evas_object_size_hint_align_set(hbox, EVAS_HINT_FILL, EVAS_HINT_FILL);
    elm_object_content_set(popup, hbox);
    evas_object_show(hbox);

    auto list = elm_list_add(popup);
    evas_object_size_hint_weight_set(list, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
    evas_object_size_hint_align_set(list, EVAS_HINT_FILL, EVAS_HINT_FILL);
    populate_list(list);
    elm_list_go(list);
    evas_object_show(list);
    elm_box_pack_end(hbox, list);

    auto box = elm_box_add(popup);
    evas_object_size_hint_weight_set(box, 0, 0);
    evas_object_size_hint_align_set(box, 1, 0.5);
    elm_box_pack_end(hbox, box);
    evas_object_show(box);

    Evas_Object *button = elm_button_add(popup);
    elm_object_text_set(button, _("Move up"));
    evas_object_size_hint_weight_set(button, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
    evas_object_size_hint_align_set(button, EVAS_HINT_FILL, EVAS_HINT_FILL);
    evas_object_smart_callback_add(button, "clicked", label_pref_up_cb, list);
    elm_box_pack_end(box, button);
    evas_object_show(button);

    button = elm_button_add(popup);
    elm_object_text_set(button, _("Move down"));
    evas_object_size_hint_weight_set(button, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
    evas_object_size_hint_align_set(button, EVAS_HINT_FILL, EVAS_HINT_FILL);
    evas_object_smart_callback_add(button, "clicked", label_pref_down_cb, list);
    elm_box_pack_end(box, button);
    evas_object_show(button);

    button = elm_button_add(popup);
    elm_object_text_set(button, _("Edit"));
    evas_object_size_hint_weight_set(button, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
    evas_object_size_hint_align_set(button, EVAS_HINT_FILL, EVAS_HINT_FILL);
    evas_object_smart_callback_add(button, "clicked", label_pref_edit_cb, list);
    elm_box_pack_end(box, button);
    evas_object_show(button);

    button = elm_button_add(popup);
    elm_object_text_set(button, _("Add"));
    evas_object_size_hint_weight_set(button, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
    evas_object_size_hint_align_set(button, EVAS_HINT_FILL, EVAS_HINT_FILL);
    evas_object_smart_callback_add(button, "clicked", label_pref_add_cb, list);
    elm_box_pack_end(box, button);
    evas_object_show(button);

    button = elm_button_add(popup);
    elm_object_text_set(button, _("Delete"));
    evas_object_size_hint_weight_set(button, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
    evas_object_size_hint_align_set(button, EVAS_HINT_FILL, EVAS_HINT_FILL);
    evas_object_smart_callback_add(button, "clicked", label_pref_delete_cb, list);
    elm_box_pack_end(box, button);
    evas_object_show(button);

    button = elm_button_add(popup);
    elm_object_text_set(button, _("Cancel"));
    elm_object_part_content_set(popup, "button1", button);
    evas_object_smart_callback_add(button, "clicked", label_pref_exit_cb, popup);

    button = elm_button_add(popup);
    elm_object_text_set(button, _("OK"));
    elm_object_part_content_set(popup, "button2", button);
    evas_object_smart_callback_add(button, "clicked", label_pref_ok_cb, popup);

    evas_object_show(popup);

}
