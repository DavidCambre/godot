/*************************************************************************/
/*  visual_script_property_selector.h                                    */
/*************************************************************************/
/*                       This file is part of:                           */
/*                           GODOT ENGINE                                */
/*                      https://godotengine.org                          */
/*************************************************************************/
/* Copyright (c) 2007-2021 Juan Linietsky, Ariel Manzur.                 */
/* Copyright (c) 2014-2021 Godot Engine contributors (cf. AUTHORS.md).   */
/*                                                                       */
/* Permission is hereby granted, free of charge, to any person obtaining */
/* a copy of this software and associated documentation files (the       */
/* "Software"), to deal in the Software without restriction, including   */
/* without limitation the rights to use, copy, modify, merge, publish,   */
/* distribute, sublicense, and/or sell copies of the Software, and to    */
/* permit persons to whom the Software is furnished to do so, subject to */
/* the following conditions:                                             */
/*                                                                       */
/* The above copyright notice and this permission notice shall be        */
/* included in all copies or substantial portions of the Software.       */
/*                                                                       */
/* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,       */
/* EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF    */
/* MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.*/
/* IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY  */
/* CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,  */
/* TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE     */
/* SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.                */
/*************************************************************************/

#ifndef VISUALSCRIPT_PROPERTYSELECTOR_H
#define VISUALSCRIPT_PROPERTYSELECTOR_H

#include "../visual_script.h"
#include "editor/editor_help.h"
#include "editor/property_editor.h"
#include "scene/gui/rich_text_label.h"

class VisualScriptPropertySelector : public ConfirmationDialog {
	GDCLASS(VisualScriptPropertySelector, ConfirmationDialog);

	enum SearchFlags {
		SEARCH_CLASSES = 1 << 0,
		SEARCH_CONSTRUCTORS = 1 << 1,
		SEARCH_METHODS = 1 << 2,
		SEARCH_OPERATORS = 1 << 3,
		SEARCH_SIGNALS = 1 << 4,
		SEARCH_CONSTANTS = 1 << 5,
		SEARCH_PROPERTIES = 1 << 6,
		SEARCH_THEME_ITEMS = 1 << 7,
		SEARCH_ALL = SEARCH_CLASSES | SEARCH_CONSTRUCTORS | SEARCH_METHODS | SEARCH_OPERATORS | SEARCH_SIGNALS | SEARCH_CONSTANTS | SEARCH_PROPERTIES | SEARCH_THEME_ITEMS,
		SEARCH_CASE_SENSITIVE = 1 << 29,
		SEARCH_SHOW_HIERARCHY = 1 << 30,
	};

	enum ScopeFlags {
		SCOPE_BASE = 1 << 0,
		SCOPE_INHERITERS = 1 << 1,
		SCOPE_UNRELATED = 1 << 2,
		SCOPE_RELATED = SCOPE_BASE | SCOPE_INHERITERS
	};

	LineEdit *search_box;
	Button *search_base_button;
	Button *search_inheritors_button;
	Button *search_unrelated_button;
	Button *case_sensitive_button;
	Button *hierarchy_button;
	OptionButton *filter_combo;
	OptionButton *scope_combo;
	Tree *results_tree;

	class DocRunner;
	Ref<DocRunner> doc_runner;
	Vector<Ref<VisualScriptNode>> result_nodes;
	Map<String, DocData::ClassDoc> result_class_list;

	void _sbox_input(const Ref<InputEvent> &p_ie);
	void _update_results_i(int p_int);
	void _update_results_s(String p_string);
	void _update_results();
	void _update_search();

	void create_visualscript_item(const String &name, TreeItem *const root, const String &search_input, const String &text);
	void get_visual_node_names(const String &root_filter, const Set<String> &p_modifiers, bool &found, TreeItem *const root, LineEdit *const search_box);

	void _confirmed();
	void _item_selected();
	void _hide_requested();

	EditorHelpBit *help_bit;

	bool properties;
	bool visual_script_generic;
	bool connecting;
	String selected;
	Variant::Type type;
	String base_type;
	ObjectID script;
	Object *instance;
	bool virtuals_only;
	bool seq_connect;
	VBoxContainer *vbox;

	Vector<Variant::Type> type_filter;

protected:
	void _notification(int p_what);
	static void _bind_methods();

public:
	void select_method_from_base_type(const String &p_base, const String &p_current = "", const bool p_virtuals_only = false, const bool p_connecting = true, bool clear_text = true);
	void select_from_base_type(const String &p_base, const String &p_current = "", bool p_virtuals_only = false, bool p_seq_connect = false, const bool p_connecting = true, bool clear_text = true);
	void select_from_script(const Ref<Script> &p_script, const String &p_current = "", const bool p_connecting = true, bool clear_text = true);
	void select_from_basic_type(Variant::Type p_type, const String &p_current = "", const bool p_connecting = true, bool clear_text = true);
	void select_from_action(const String &p_type, const String &p_current = "", const bool p_connecting = true, bool clear_text = true);
	void select_from_instance(Object *p_instance, const String &p_current = "", const bool p_connecting = true, const String &p_basetype = "", bool clear_text = true);
	void select_from_visual_script(const String &p_base, const bool p_connecting = true, bool clear_text = true);

	void show_window(float p_screen_ratio);

	void set_type_filter(const Vector<Variant::Type> &p_type_filter);

	VisualScriptPropertySelector();
};

class VisualScriptPropertySelector::DocRunner : public RefCounted {
	enum Phase {
		PHASE_INIT_SEARCH,
		PHASE_GET_ALL_NODE_CLASS_DOCS,
		PHASE_GET_ALL_FOLDER_PATHS,
		PHASE_GET_ALL_FILE_PATHS,
		PHASE_MAX
	};
	int phase = 0;

	Vector<Ref<VisualScriptNode>> *result_nodes;
	//Vector<Ref<Script>> *result_scripts;
	Map<String, DocData::ClassDoc> *result_class_list;
	List<String> visual_script_nodes;

	// Config
	Vector<String> _extension_filter;

	// State
	String _current_dir;
	Vector<PackedStringArray> _folders_stack;
	Vector<String> _files_to_scan;
	int _initial_files_count = 0;

	bool _slice();
	bool _phase_init_search();
	bool _phase_get_all_node_class_docs();
	bool _phase_get_all_folder_paths();
	bool _phase_get_all_file_paths();

	void _scan_dir(String path, PackedStringArray &out_folders);
	void _scan_file(String fpath);
	DocData::MethodDoc _get_method_doc(MethodInfo method_info);

public:
	bool work(uint64_t slot = 100000);

	DocRunner(Vector<Ref<VisualScriptNode>> *p_result_nodes, Map<String, DocData::ClassDoc> *p_result_class_list);
};

#endif // VISUALSCRIPT_PROPERTYSELECTOR_H
