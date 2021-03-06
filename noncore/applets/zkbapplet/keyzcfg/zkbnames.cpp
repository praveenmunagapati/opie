#include <qmap.h>

#include "zkbnames.h"

QString Null_String((const char*) 0);

// Implementation of KeyNames
struct Key_Names_t {
	int key;
	char *name;
};

static const Key_Names_t Key_Names[] = {
	{ 32, "Space" },
	{ 39, "Apostrophe" },
	{ 44, "Comma" },
	{ 46, "Period" },
	{ 47, "Slash" },
	{ 65, "A" },
	{ 66, "B" },
	{ 67, "C" },
	{ 68, "D" },
	{ 69, "E" },
	{ 70, "F" },
	{ 71, "G" },
	{ 72, "H" },
	{ 73, "I" },
	{ 74, "J" },
	{ 75, "K" },
	{ 76, "L" },
	{ 77, "M" },
	{ 78, "N" },
	{ 79, "O" },
	{ 80, "P" },
	{ 81, "Q" },
	{ 82, "R" },
	{ 83, "S" },
	{ 84, "T" },
	{ 85, "U" },
	{ 86, "V" },
	{ 87, "W" },
	{ 88, "X" },
	{ 89, "Y" },
	{ 90, "Z" },
	{ 4096, "Cancel" },
	{ 4097, "Tab" },
	{ 4099, "Backspace" },
	{ 4100, "Enter" },
	{ 4114, "Left" },
	{ 4115, "Up" },
	{ 4116, "Right" },
	{ 4117, "Down" },
	{ 4128, "Left Shift" },
	{ 4130, "Right Shift" },
	{ 4152, "Calendar" },
	{ 4153, "Addressbook" },
	{ 4154, "Menu" },
	{ 4155, "Home" },
	{ 4156, "Mail" },
	{ 4165, "Fn" },
	{ 4173, "Middle" },
	{ 4176, "OK" },
	{ 4177, "Off" },
	{ 4178, "Light" },
	{ 0, 0 }
};

static QMap<QString, int> kn_map;
static QMap<int, QString> kn_rmap;

void init_kn_maps() {
	int i = 0;
	while (Key_Names[i].name != 0) {
		int key = Key_Names[i].key;
		QString name(Key_Names[i].name);

		kn_map.insert(name, key);
		kn_rmap.insert(key, name);
		i++;
	}
}

int KeyNames::find(const QString& key) {
	if (kn_map.isEmpty()) {
		init_kn_maps();
	}

	QMap<QString, int>::Iterator it = kn_map.find(key);
	if (it == kn_map.end()) {
		return -1;
	} else {
		return it.data();
	}
}

const QString& KeyNames::find(int k) {
	if (kn_map.isEmpty()) {
		init_kn_maps();
	}

	QMap<int, QString>::Iterator it = kn_rmap.find(k);
	if (it == kn_rmap.end()) {
		return Null_String;
	} else {
		return it.data();
	}
}

// Implementation of ModifierNames
struct Modifier_Names_t {
	int value;
	char* name;
};

static const Modifier_Names_t Modifier_Names[] = {
	{ 8, "Shift" },
	{ 16, "Control" },
	{ 32, "Alt" },
	{ 0x4000, "Keypad" },
	{ 0, 0 }
};

static QMap<QString, int> mn_map;
static QMap<int, QString> mn_rmap;

void init_mn_maps() {
	int i = 0;
	while (Modifier_Names[i].name != 0) {
		int value = Modifier_Names[i].value;
		QString name(Modifier_Names[i].name);

		mn_map.insert(name, value);
		mn_rmap.insert(value, name);
		i++;
	}
}

int ModifierNames::find(const QString& key) {
	if (mn_map.isEmpty()) {
		init_mn_maps();
	}

	QMap<QString, int>::Iterator it = mn_map.find(key);
	if (it == mn_map.end()) {
		return -1;
	} else {
		return it.data();
	}
}

const QString& ModifierNames::find(int k) {
	if (mn_map.isEmpty()) {
		init_mn_maps();
	}

	QMap<int, QString>::Iterator it = mn_rmap.find(k);
	if (it == mn_rmap.end()) {
		return Null_String;
	} else {
		return it.data();
	}
}

// Implementation of KeycodeNames

struct Keycode_Names_t {
	char* name;
	int keycode;
};

static const Keycode_Names_t Keycode_Names[] = {
	{ "Escape", 0x1000 },
	{ "Tab", 0x1001 },
	{ "Backtab", 0x1002 },
	{ "Backspace", 0x1003 },
	{ "BackSpace", 0x1003 },
	{ "Return", 0x1004 },
	{ "Enter", 0x1005 },
	{ "Insert", 0x1006 },
	{ "Delete", 0x1007 },
	{ "Pause", 0x1008 },
	{ "Print", 0x1009 },
	{ "SysReq", 0x100a },
	{ "Home", 0x1010 },
	{ "End", 0x1011 },
	{ "Left", 0x1012 },
	{ "Up", 0x1013 },
	{ "Right", 0x1014 },
	{ "Down", 0x1015 },
	{ "Prior", 0x1016 },
	{ "PageUp", 0x1016 },
	{ "Next", 0x1017 },
	{ "PageDown", 0x1017 },
	{ "Shift", 0x1020 },
	{ "Control", 0x1021 },
	{ "Meta", 0x1022 },
	{ "Alt", 0x1023 },
	{ "CapsLock", 0x1024 },
	{ "NumLock", 0x1025 },
	{ "ScrollLock", 0x1026 },
	{ "F1", 0x1030 },
	{ "F2", 0x1031 },
	{ "F3", 0x1032 },
	{ "F4", 0x1033 },
	{ "F5", 0x1034 },
	{ "F6", 0x1035 },
	{ "F7", 0x1036 },
	{ "F8", 0x1037 },
	{ "F9", 0x1038 },
	{ "F10", 0x1039 },
	{ "F11", 0x103a },
	{ "F12", 0x103b },
	{ "F13", 0x103c },
	{ "F14", 0x103d },
	{ "F15", 0x103e },
	{ "F16", 0x103f },
	{ "F17", 0x1040 },
	{ "F18", 0x1041 },
	{ "F19", 0x1042 },
	{ "F20", 0x1043 },
	{ "F21", 0x1044 },
	{ "F22", 0x1045 },
	{ "F23", 0x1046 },
	{ "F24", 0x1047 },
	{ "F25", 0x1048 },
	{ "F26", 0x1049 },
	{ "F27", 0x104a },
	{ "F28", 0x104b },
	{ "F29", 0x104c },
	{ "F30", 0x104d },
	{ "F31", 0x104e },
	{ "F32", 0x104f },
	{ "F33", 0x1050 },
	{ "F34", 0x1051 },
	{ "F35", 0x1052 },
	{ "Super_L", 0x1053 },
	{ "Super_R", 0x1054 },
	{ "Menu", 0x1055 },
	{ "Hyper_L", 0x1056 },
	{ "Hyper_R", 0x1057 },
	{ "Help", 0x1058 },
	{ "Space", 0x20 },
	{ "Any", 0x20 },
	{ "Exclam", 0x21 },
	{ "QuoteDbl", 0x22 },
	{ "NumberSign", 0x23 },
	{ "Dollar", 0x24 },
	{ "Percent", 0x25 },
	{ "Ampersand", 0x26 },
	{ "Apostrophe", 0x27 },
	{ "ParenLeft", 0x28 },
	{ "ParenRight", 0x29 },
	{ "Asterisk", 0x2a },
	{ "Plus", 0x2b },
	{ "Comma", 0x2c },
	{ "Minus", 0x2d },
	{ "Period", 0x2e },
	{ "Slash", 0x2f },
	{ "0", 0x30 },
	{ "1", 0x31 },
	{ "2", 0x32 },
	{ "3", 0x33 },
	{ "4", 0x34 },
	{ "5", 0x35 },
	{ "6", 0x36 },
	{ "7", 0x37 },
	{ "8", 0x38 },
	{ "9", 0x39 },
	{ "Colon", 0x3a },
	{ "Semicolon", 0x3b },
	{ "Less", 0x3c },
	{ "Equal", 0x3d },
	{ "Greater", 0x3e },
	{ "Question", 0x3f },
	{ "At", 0x40 },
	{ "A", 0x41 },
	{ "B", 0x42 },
	{ "C", 0x43 },
	{ "D", 0x44 },
	{ "E", 0x45 },
	{ "F", 0x46 },
	{ "G", 0x47 },
	{ "H", 0x48 },
	{ "I", 0x49 },
	{ "J", 0x4a },
	{ "K", 0x4b },
	{ "L", 0x4c },
	{ "M", 0x4d },
	{ "N", 0x4e },
	{ "O", 0x4f },
	{ "P", 0x50 },
	{ "Q", 0x51 },
	{ "R", 0x52 },
	{ "S", 0x53 },
	{ "T", 0x54 },
	{ "U", 0x55 },
	{ "V", 0x56 },
	{ "W", 0x57 },
	{ "X", 0x58 },
	{ "Y", 0x59 },
	{ "Z", 0x5a },
	{ "BracketLeft", 0x5b },
	{ "Backslash", 0x5c },
	{ "BracketRight", 0x5d },
	{ "AsciiCircum", 0x5e },
	{ "Underscore", 0x5f },
	{ "QuoteLeft", 0x60 },
	{ "BraceLeft", 0x7b },
	{ "Bar", 0x7c },
	{ "BraceRight", 0x7d },
	{ "AsciiTilde", 0x7e },
	{ "nobreakspace", 0x0a0 },
	{ "exclamdown", 0x0a1 },
	{ "cent", 0x0a2 },
	{ "sterling", 0x0a3 },
	{ "currency", 0x0a4 },
	{ "yen", 0x0a5 },
	{ "brokenbar", 0x0a6 },
	{ "section", 0x0a7 },
	{ "diaeresis", 0x0a8 },
	{ "copyright", 0x0a9 },
	{ "ordfeminine", 0x0aa },
	{ "guillemotleft", 0x0ab },
	{ "notsign", 0x0ac },
	{ "hyphen", 0x0ad },
	{ "registered", 0x0ae },
	{ "macron", 0x0af },
	{ "degree", 0x0b0 },
	{ "plusminus", 0x0b1 },
	{ "twosuperior", 0x0b2 },
	{ "threesuperior", 0x0b3 },
	{ "acute", 0x0b4 },
	{ "mu", 0x0b5 },
	{ "paragraph", 0x0b6 },
	{ "periodcentered", 0x0b7 },
	{ "cedilla", 0x0b8 },
	{ "onesuperior", 0x0b9 },
	{ "masculine", 0x0ba },
	{ "guillemotright", 0x0bb },
	{ "onequarter", 0x0bc },
	{ "onehalf", 0x0bd },
	{ "threequarters", 0x0be },
	{ "questiondown", 0x0bf },
	{ "Agrave", 0x0c0 },
	{ "Aacute", 0x0c1 },
	{ "Acircumflex", 0x0c2 },
	{ "Atilde", 0x0c3 },
	{ "Adiaeresis", 0x0c4 },
	{ "Aring", 0x0c5 },
	{ "AE", 0x0c6 },
	{ "Ccedilla", 0x0c7 },
	{ "Egrave", 0x0c8 },
	{ "Eacute", 0x0c9 },
	{ "Ecircumflex", 0x0ca },
	{ "Ediaeresis", 0x0cb },
	{ "Igrave", 0x0cc },
	{ "Iacute", 0x0cd },
	{ "Icircumflex", 0x0ce },
	{ "Idiaeresis", 0x0cf },
	{ "ETH", 0x0d0 },
	{ "Ntilde", 0x0d1 },
	{ "Ograve", 0x0d2 },
	{ "Oacute", 0x0d3 },
	{ "Ocircumflex", 0x0d4 },
	{ "Otilde", 0x0d5 },
	{ "Odiaeresis", 0x0d6 },
	{ "multiply", 0x0d7 },
	{ "Ooblique", 0x0d8 },
	{ "Ugrave", 0x0d9 },
	{ "Uacute", 0x0da },
	{ "Ucircumflex", 0x0db },
	{ "Udiaeresis", 0x0dc },
	{ "Yacute", 0x0dd },
	{ "THORN", 0x0de },
	{ "ssharp", 0x0df },
	{ "agrave", 0x0e0 },
	{ "aacute", 0x0e1 },
	{ "acircumflex", 0x0e2 },
	{ "atilde", 0x0e3 },
	{ "adiaeresis", 0x0e4 },
	{ "aring", 0x0e5 },
	{ "ae", 0x0e6 },
	{ "ccedilla", 0x0e7 },
	{ "egrave", 0x0e8 },
	{ "eacute", 0x0e9 },
	{ "ecircumflex", 0x0ea },
	{ "ediaeresis", 0x0eb },
	{ "igrave", 0x0ec },
	{ "iacute", 0x0ed },
	{ "icircumflex", 0x0ee },
	{ "idiaeresis", 0x0ef },
	{ "eth", 0x0f0 },
	{ "ntilde", 0x0f1 },
	{ "ograve", 0x0f2 },
	{ "oacute", 0x0f3 },
	{ "ocircumflex", 0x0f4 },
	{ "otilde", 0x0f5 },
	{ "odiaeresis", 0x0f6 },
	{ "division", 0x0f7 },
	{ "oslash", 0x0f8 },
	{ "ugrave", 0x0f9 },
	{ "uacute", 0x0fa },
	{ "ucircumflex", 0x0fb },
	{ "udiaeresis", 0x0fc },
	{ "yacute", 0x0fd },
	{ "thorn", 0x0fe },
	{ "ydiaeresis", 0x0ff },
	{ "unknown", 0xffff },
	{ 0, 0}
};

static QMap<QString, int> kcn_map;
static QMap<int, QString> kcn_rmap;

void init_kcn_maps() {
	int i = 0;
	while (Keycode_Names[i].name != 0) {
		int keycode = Keycode_Names[i].keycode;
		QString name(Keycode_Names[i].name);

		kcn_map.insert(name, keycode);
		kcn_rmap.insert(keycode, name);
		i++;
	}
}

int KeycodeNames::find(const QString& key) {
	if (kcn_map.isEmpty()) {
		init_kcn_maps();
	}

	QMap<QString, int>::Iterator it = kcn_map.find(key);
	if (it == kcn_map.end()) {
		return -1;
	} else {
		return it.data();
	}
}

const QString& KeycodeNames::find(int k) {
	if (kcn_map.isEmpty()) {
		init_kcn_maps();
	}

	QMap<int, QString>::Iterator it = kcn_rmap.find(k);
	if (it == kcn_rmap.end()) {
		return Null_String;
	} else {
		return it.data();
	}
}

