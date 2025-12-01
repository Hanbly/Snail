#pragma once

#ifdef SNL_PLATFORM_WINDOWS
	/* From glfw3.h */
	/* Keycodes */
	#define SNL_KEY_UNKNOWN            -1
	#define SNL_KEY_SPACE              32
	#define SNL_KEY_APOSTROPHE         39  /* ' */
	#define SNL_KEY_COMMA              44  /* , */
	#define SNL_KEY_MINUS              45  /* - */
	#define SNL_KEY_PERIOD             46  /* . */
	#define SNL_KEY_SLASH              47  /* / */
	#define SNL_KEY_0                  48
	#define SNL_KEY_1                  49
	#define SNL_KEY_2                  50
	#define SNL_KEY_3                  51
	#define SNL_KEY_4                  52
	#define SNL_KEY_5                  53
	#define SNL_KEY_6                  54
	#define SNL_KEY_7                  55
	#define SNL_KEY_8                  56
	#define SNL_KEY_9                  57
	#define SNL_KEY_SEMICOLON          59  /* ; */
	#define SNL_KEY_EQUAL              61  /* = */
	#define SNL_KEY_A                  65
	#define SNL_KEY_B                  66
	#define SNL_KEY_C                  67
	#define SNL_KEY_D                  68
	#define SNL_KEY_E                  69
	#define SNL_KEY_F                  70
	#define SNL_KEY_G                  71
	#define SNL_KEY_H                  72
	#define SNL_KEY_I                  73
	#define SNL_KEY_J                  74
	#define SNL_KEY_K                  75
	#define SNL_KEY_L                  76
	#define SNL_KEY_M                  77
	#define SNL_KEY_N                  78
	#define SNL_KEY_O                  79
	#define SNL_KEY_P                  80
	#define SNL_KEY_Q                  81
	#define SNL_KEY_R                  82
	#define SNL_KEY_S                  83
	#define SNL_KEY_T                  84
	#define SNL_KEY_U                  85
	#define SNL_KEY_V                  86
	#define SNL_KEY_W                  87
	#define SNL_KEY_X                  88
	#define SNL_KEY_Y                  89
	#define SNL_KEY_Z                  90
	#define SNL_KEY_LEFT_BRACKET       91  /* [ */
	#define SNL_KEY_BACKSLASH          92  /* \ */
	#define SNL_KEY_RIGHT_BRACKET      93  /* ] */
	#define SNL_KEY_GRAVE_ACCENT       96  /* ` */
	#define SNL_KEY_WORLD_1            161 /* non-US #1 */
	#define SNL_KEY_WORLD_2            162 /* non-US #2 */
	/* Function keys */
	#define SNL_KEY_ESCAPE             256
	#define SNL_KEY_ENTER              257
	#define SNL_KEY_TAB                258
	#define SNL_KEY_BACKSPACE          259
	#define SNL_KEY_INSERT             260
	#define SNL_KEY_DELETE             261
	#define SNL_KEY_RIGHT              262
	#define SNL_KEY_LEFT               263
	#define SNL_KEY_DOWN               264
	#define SNL_KEY_UP                 265
	#define SNL_KEY_PAGE_UP            266
	#define SNL_KEY_PAGE_DOWN          267
	#define SNL_KEY_HOME               268
	#define SNL_KEY_END                269
	#define SNL_KEY_CAPS_LOCK          280
	#define SNL_KEY_SCROLL_LOCK        281
	#define SNL_KEY_NUM_LOCK           282
	#define SNL_KEY_PRINT_SCREEN       283
	#define SNL_KEY_PAUSE              284
	#define SNL_KEY_F1                 290
	#define SNL_KEY_F2                 291
	#define SNL_KEY_F3                 292
	#define SNL_KEY_F4                 293
	#define SNL_KEY_F5                 294
	#define SNL_KEY_F6                 295
	#define SNL_KEY_F7                 296
	#define SNL_KEY_F8                 297
	#define SNL_KEY_F9                 298
	#define SNL_KEY_F10                299
	#define SNL_KEY_F11                300
	#define SNL_KEY_F12                301
	#define SNL_KEY_F13                302
	#define SNL_KEY_F14                303
	#define SNL_KEY_F15                304
	#define SNL_KEY_F16                305
	#define SNL_KEY_F17                306
	#define SNL_KEY_F18                307
	#define SNL_KEY_F19                308
	#define SNL_KEY_F20                309
	#define SNL_KEY_F21                310
	#define SNL_KEY_F22                311
	#define SNL_KEY_F23                312
	#define SNL_KEY_F24                313
	#define SNL_KEY_F25                314
	#define SNL_KEY_KP_0               320
	#define SNL_KEY_KP_1               321
	#define SNL_KEY_KP_2               322
	#define SNL_KEY_KP_3               323
	#define SNL_KEY_KP_4               324
	#define SNL_KEY_KP_5               325
	#define SNL_KEY_KP_6               326
	#define SNL_KEY_KP_7               327
	#define SNL_KEY_KP_8               328
	#define SNL_KEY_KP_9               329
	#define SNL_KEY_KP_DECIMAL         330
	#define SNL_KEY_KP_DIVIDE          331
	#define SNL_KEY_KP_MULTIPLY        332
	#define SNL_KEY_KP_SUBTRACT        333
	#define SNL_KEY_KP_ADD             334
	#define SNL_KEY_KP_ENTER           335
	#define SNL_KEY_KP_EQUAL           336
	#define SNL_KEY_LEFT_SHIFT         340
	#define SNL_KEY_LEFT_CONTROL       341
	#define SNL_KEY_LEFT_ALT           342
	#define SNL_KEY_LEFT_SUPER         343
	#define SNL_KEY_RIGHT_SHIFT        344
	#define SNL_KEY_RIGHT_CONTROL      345
	#define SNL_KEY_RIGHT_ALT          346
	#define SNL_KEY_RIGHT_SUPER        347
	#define SNL_KEY_MENU               348

	#define SNL_KEY_LAST               SNL_KEY_MENU

	/* Mouse codes */
	#define SNL_MOUSE_BUTTON_1         0
	#define SNL_MOUSE_BUTTON_2         1
	#define SNL_MOUSE_BUTTON_3         2
	#define SNL_MOUSE_BUTTON_4         3
	#define SNL_MOUSE_BUTTON_5         4
	#define SNL_MOUSE_BUTTON_6         5
	#define SNL_MOUSE_BUTTON_7         6
	#define SNL_MOUSE_BUTTON_8         7
	#define SNL_MOUSE_BUTTON_LAST      SNL_MOUSE_BUTTON_8
	#define SNL_MOUSE_BUTTON_LEFT      SNL_MOUSE_BUTTON_1
	#define SNL_MOUSE_BUTTON_RIGHT     SNL_MOUSE_BUTTON_2
	#define SNL_MOUSE_BUTTON_MIDDLE    SNL_MOUSE_BUTTON_3
#endif


