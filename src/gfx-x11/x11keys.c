 /*
  * UAE - The Un*x Amiga Emulator
  *
  * X keysym support
  *
  * Copyright 1995, 1996 Bernd Schmidt
  * Copyright 1996 Ed Hanway, Andre Beck, Samuel Devulder, Bruno Coste
  * Copyright 1998 Marcus Sundberg
  * DGA support by Kai Kollmorgen
  * X11/DGA merge, hotkeys and grabmouse by Marcus Sundberg
  * Copyright 2003-2004 Richard Drummond
  */

#include "sysconfig.h"
#include "sysdeps.h"

#include <X11/Xlib.h>
#include <X11/keysym.h>

#include "options.h"
#include "inputdevice.h"
#include "keyboard.h"
#include "hotkeys.h"

#ifdef USE_XKB
# include <X11/XKBlib.h>
# include <X11/extensions/XKBrules.h>

static const char xkb_rules_path[] = XKB_PATH;


/*
 * Use XKB extension and libxkbfile to discover the type of keycodes
 * generated by the X keyboard
 */
const char *get_xkb_keycodes (Display *display)
{
    char *result = 0;
    char *rulesfile = 0;

    int major = XkbMajorVersion;
    int minor = XkbMinorVersion;
    int error = 0;

    XkbRF_VarDefsRec vd;

    if (!XkbLibraryVersion (&major, &minor)) {
	write_log ("X11GFX: Xkb version conflict!\n");
	return 0;
    }

    if (!XkbQueryExtension (display, 0, 0, &error, &major, &minor)) {
	printf ("X11GFX: XKB error: %d\n", error);
	return 0;
    }

    if (XkbRF_GetNamesProp (display, &rulesfile, &vd) && rulesfile != 0) {

	if (rulesfile[0] == '/') {
	    char *tmp = malloc (strlen (rulesfile));

	    if (tmp) {
		strcpy (tmp, rulesfile);
		rulesfile = tmp;
	    }
	} else {
	    char *tmp = malloc (strlen (xkb_rules_path) + strlen (rulesfile) + 1);

	    if (tmp) {
		strcpy (tmp, xkb_rules_path);
		strcat (tmp, rulesfile);
		rulesfile = tmp;
	    }
	}
    }

    if (rulesfile) {
	XkbRF_RulesPtr rules;

	if ((rules = XkbRF_Load (rulesfile, 0, True, True))) {
	    XkbComponentNamesRec names;

	    XkbRF_GetComponents (rules, &vd, &names);

	    if (names.keycodes)
		result = names.keycodes;
	}
    }

    return result;
}
#endif


/* Decode KeySyms. This function knows about all keys that are common
 * between different keyboard languages. */
static int kc_decode (KeySym ks)
{
    switch (ks) {
     case XK_B: case XK_b: return AK_B;
     case XK_C: case XK_c: return AK_C;
     case XK_D: case XK_d: return AK_D;
     case XK_E: case XK_e: return AK_E;
     case XK_F: case XK_f: return AK_F;
     case XK_G: case XK_g: return AK_G;
     case XK_H: case XK_h: return AK_H;
     case XK_I: case XK_i: return AK_I;
     case XK_J: case XK_j: return AK_J;
     case XK_K: case XK_k: return AK_K;
     case XK_L: case XK_l: return AK_L;
     case XK_N: case XK_n: return AK_N;
     case XK_O: case XK_o: return AK_O;
     case XK_P: case XK_p: return AK_P;
     case XK_R: case XK_r: return AK_R;
     case XK_S: case XK_s: return AK_S;
     case XK_T: case XK_t: return AK_T;
     case XK_U: case XK_u: return AK_U;
     case XK_V: case XK_v: return AK_V;
     case XK_X: case XK_x: return AK_X;

     case XK_0: return AK_0;
     case XK_1: return AK_1;
     case XK_2: return AK_2;
     case XK_3: return AK_3;
     case XK_4: return AK_4;
     case XK_5: return AK_5;
     case XK_6: return AK_6;
     case XK_7: return AK_7;
     case XK_8: return AK_8;
     case XK_9: return AK_9;

	/* You never know which Keysyms might be missing on some workstation
	 * This #ifdef should be enough. */
#if defined(XK_KP_Prior) && defined(XK_KP_Left) && defined(XK_KP_Insert) && defined (XK_KP_End)
     case XK_KP_0: case XK_KP_Insert: return AK_NP0;
     case XK_KP_1: case XK_KP_End: return AK_NP1;
     case XK_KP_2: case XK_KP_Down: return AK_NP2;
     case XK_KP_3: case XK_KP_Next: return AK_NP3;
     case XK_KP_4: case XK_KP_Left: return AK_NP4;
     case XK_KP_5: case XK_KP_Begin: return AK_NP5;
     case XK_KP_6: case XK_KP_Right: return AK_NP6;
     case XK_KP_7: case XK_KP_Home: return AK_NP7;
     case XK_KP_8: case XK_KP_Up: return AK_NP8;
     case XK_KP_9: case XK_KP_Prior: return AK_NP9;
#else
     case XK_KP_0: return AK_NP0;
     case XK_KP_1: return AK_NP1;
     case XK_KP_2: return AK_NP2;
     case XK_KP_3: return AK_NP3;
     case XK_KP_4: return AK_NP4;
     case XK_KP_5: return AK_NP5;
     case XK_KP_6: return AK_NP6;
     case XK_KP_7: return AK_NP7;
     case XK_KP_8: return AK_NP8;
     case XK_KP_9: return AK_NP9;
#endif
     case XK_KP_Divide: return AK_NPDIV;
     case XK_KP_Multiply: return AK_NPMUL;
     case XK_KP_Subtract: return AK_NPSUB;
     case XK_KP_Add: return AK_NPADD;
     case XK_KP_Decimal: return AK_NPDEL;
     case XK_KP_Enter: return AK_ENT;

     case XK_F1: return AK_F1;
     case XK_F2: return AK_F2;
     case XK_F3: return AK_F3;
     case XK_F4: return AK_F4;
     case XK_F5: return AK_F5;
     case XK_F6: return AK_F6;
     case XK_F7: return AK_F7;
     case XK_F8: return AK_F8;
     case XK_F9: return AK_F9;
     case XK_F10: return AK_F10;

     case XK_BackSpace: return AK_BS;
     case XK_Delete: return AK_DEL;
     case XK_Control_L: return AK_CTRL;
     case XK_Control_R: return AK_RCTRL;
     case XK_Tab: return AK_TAB;
     case XK_Alt_L: return AK_LALT;
     case XK_Alt_R: return AK_RALT;
     case XK_Meta_R: case XK_Hyper_R: return AK_RAMI;
     case XK_Meta_L: case XK_Hyper_L: return AK_LAMI;
     case XK_Return: return AK_RET;
     case XK_space: return AK_SPC;
     case XK_Shift_L: return AK_LSH;
     case XK_Shift_R: return AK_RSH;
     case XK_Escape: return AK_ESC;

     case XK_Insert: return AK_HELP;
     case XK_Home: return AK_NPLPAREN;
     case XK_End: return AK_NPRPAREN;
     case XK_Caps_Lock: return AK_CAPSLOCK;

     case XK_Up: return AK_UP;
     case XK_Down: return AK_DN;
     case XK_Left: return AK_LF;
     case XK_Right: return AK_RT;

#ifdef XK_Page_Up /* These are missing occasionally */
     case XK_Page_Up: return AK_RAMI;          /* PgUp mapped to right amiga */
     case XK_Page_Down: return AK_LAMI;        /* PgDn mapped to left amiga */
#endif

#ifdef XK_Super_L
     case XK_Super_L: return AK_LAMI;
     case XK_Super_R: return AK_RAMI;
#endif
     case XK_Scroll_Lock: return AKS_INHIBITSCREEN;
     case XK_Print: return AKS_SCREENSHOT;
    }
    return -1;
}

static int decode_fr (KeySym ks)
{
    switch(ks) {        /* FR specific */
     case XK_A: case XK_a: return AK_Q;
     case XK_M: case XK_m: return AK_SEMICOLON;
     case XK_Q: case XK_q: return AK_A;
     case XK_Y: case XK_y: return AK_Y;
     case XK_W: case XK_w: return AK_Z;
     case XK_Z: case XK_z: return AK_W;
#if 0
     case XK_bracketleft: return AK_LBRACKET;
     case XK_bracketright: return AK_RBRACKET;
     case XK_comma: return AK_M;
     case XK_less: case XK_greater: return AK_LTGT;
     case XK_period: return AK_COMMA;
     case XK_parenright: return AK_MINUS;
     case XK_equal: return AK_SLASH;
     case XK_numbersign: return AK_NUMBERSIGN;
     case XK_slash: return AK_PERIOD;
     case XK_minus: return AK_EQUAL;
     case XK_backslash: return AK_BACKSLASH;
#else
     /* not sure for this one: my X 3.3 server doesn't handle this key always
      * correctly... But anyway, on new french keyboards, no more bracket key
      * at this place.
      */
     case XK_dead_circumflex:
     case XK_dead_diaeresis: return AK_LBRACKET;
     case XK_dollar:
     case XK_sterling: return AK_RBRACKET;
     case XK_comma: case XK_question: return AK_M;
     case XK_less: case XK_greater: return AK_LTGT;
     case XK_semicolon: case XK_period: return AK_COMMA;
     case XK_parenright: case XK_degree: return AK_MINUS;
     case XK_equal: case XK_plus: return AK_SLASH;
     case XK_numbersign: return AK_NUMBERSIGN;
     case XK_colon: case XK_slash: return AK_PERIOD;
     case XK_minus: case XK_6: return AK_6;
     case XK_ugrave: case XK_percent: return AK_QUOTE;
     /* found a spare key - I hope it deserves this place. */
     case XK_asterisk: case XK_mu: return AK_BACKSLASH;
     case XK_exclam: case XK_section: return AK_EQUAL;
     case XK_twosuperior: case XK_asciitilde: return AK_BACKQUOTE;
     case XK_Multi_key: return AK_RAMI;
     case XK_Mode_switch: return AK_RALT;
#endif
    }

    return -1;
}

static int decode_us (KeySym ks)
{
    switch(ks) {	/* US specific */
     case XK_A: case XK_a: return AK_A;
     case XK_M: case XK_m: return AK_M;
     case XK_Q: case XK_q: return AK_Q;
     case XK_Y: case XK_y: return AK_Y;
     case XK_W: case XK_w: return AK_W;
     case XK_Z: case XK_z: return AK_Z;
     case XK_bracketleft: return AK_LBRACKET;
     case XK_bracketright: return AK_RBRACKET;
     case XK_comma: return AK_COMMA;
     case XK_period: return AK_PERIOD;
     case XK_slash: return AK_SLASH;
     case XK_semicolon: return AK_SEMICOLON;
     case XK_minus: return AK_MINUS;
     case XK_equal: return AK_EQUAL;
	/* this doesn't work: */
     case XK_quoteright: return AK_QUOTE;
     case XK_quoteleft: return AK_BACKQUOTE;
     case XK_backslash: return AK_BACKSLASH;
    }

    return -1;
}

static int decode_de (KeySym ks)
{
    switch(ks) {
	/* DE specific */
     case XK_A: case XK_a: return AK_A;
     case XK_M: case XK_m: return AK_M;
     case XK_Q: case XK_q: return AK_Q;
     case XK_W: case XK_w: return AK_W;
     case XK_Y: case XK_y: return AK_Z;
     case XK_Z: case XK_z: return AK_Y;
     case XK_Odiaeresis: case XK_odiaeresis: return AK_SEMICOLON;
     case XK_Adiaeresis: case XK_adiaeresis: return AK_QUOTE;
     case XK_Udiaeresis: case XK_udiaeresis: return AK_LBRACKET;
     case XK_plus: case XK_asterisk: return AK_RBRACKET;
     case XK_comma: return AK_COMMA;
     case XK_period: return AK_PERIOD;
     case XK_less: case XK_greater: return AK_LTGT;
     case XK_numbersign: return AK_NUMBERSIGN;
     case XK_ssharp: return AK_MINUS;
     case XK_apostrophe: return AK_EQUAL;
     case XK_asciicircum: return AK_BACKQUOTE;
     case XK_minus: return AK_SLASH;
    }

    return -1;
}

static int decode_dk (KeySym ks)
{
    switch(ks) {
	/* DK specific */
    case XK_A: case XK_a: return AK_A;
    case XK_M: case XK_m: return AK_M;
    case XK_Q: case XK_q: return AK_Q;
    case XK_W: case XK_w: return AK_W;
    case XK_Y: case XK_y: return AK_Y;
    case XK_Z: case XK_z: return AK_Z;
    case XK_AE: case XK_ae: return AK_SEMICOLON;
    case XK_Ooblique: case XK_oslash: return AK_QUOTE;
    case XK_Aring: case XK_aring: return AK_LBRACKET;
    case XK_apostrophe: case XK_asterisk: return AK_NUMBERSIGN;
    case XK_dead_diaeresis: case XK_dead_circumflex: return AK_RBRACKET;
    case XK_dead_acute: case XK_dead_grave: return AK_BACKSLASH;
    case XK_onehalf: case XK_section: return AK_BACKQUOTE;
    case XK_comma: return AK_COMMA;
    case XK_period: return AK_PERIOD;
    case XK_less: case XK_greater: return AK_LTGT;
    case XK_numbersign: return AK_NUMBERSIGN;
    case XK_plus: return AK_MINUS;
    case XK_asciicircum: return AK_BACKQUOTE;
    case XK_minus: return AK_SLASH;
    }

    return -1;
}

static int decode_se (KeySym ks)
{
    switch(ks) {
	/* SE specific */
     case XK_A: case XK_a: return AK_A;
     case XK_M: case XK_m: return AK_M;
     case XK_Q: case XK_q: return AK_Q;
     case XK_W: case XK_w: return AK_W;
     case XK_Y: case XK_y: return AK_Y;
     case XK_Z: case XK_z: return AK_Z;
     case XK_Odiaeresis: case XK_odiaeresis: return AK_SEMICOLON;
     case XK_Adiaeresis: case XK_adiaeresis: return AK_QUOTE;
     case XK_Aring: case XK_aring: return AK_LBRACKET;
     case XK_comma: return AK_COMMA;
     case XK_period: return AK_PERIOD;
     case XK_minus: return AK_SLASH;
     case XK_less: case XK_greater: return AK_LTGT;
     case XK_plus: case XK_question: return AK_EQUAL;
     case XK_at: case XK_onehalf: return AK_BACKQUOTE;
     case XK_asciitilde: case XK_asciicircum: return AK_RBRACKET;
     case XK_backslash: case XK_bar: return AK_MINUS;

     case XK_numbersign: return AK_NUMBERSIGN;
    }

    return -1;
 }

static int decode_it (KeySym ks)
{
    switch(ks) {
	/* IT specific */
     case XK_A: case XK_a: return AK_A;
     case XK_M: case XK_m: return AK_M;
     case XK_Q: case XK_q: return AK_Q;
     case XK_W: case XK_w: return AK_W;
     case XK_Y: case XK_y: return AK_Y;
     case XK_Z: case XK_z: return AK_Z;
     case XK_Ograve: case XK_ograve: return AK_SEMICOLON;
     case XK_Agrave: case XK_agrave: return AK_QUOTE;
     case XK_Egrave: case XK_egrave: return AK_LBRACKET;
     case XK_plus: case XK_asterisk: return AK_RBRACKET;
     case XK_comma: return AK_COMMA;
     case XK_period: return AK_PERIOD;
     case XK_less: case XK_greater: return AK_LTGT;
     case XK_backslash: case XK_bar: return AK_BACKQUOTE;
     case XK_apostrophe: return AK_MINUS;
     case XK_Igrave: case XK_igrave: return AK_EQUAL;
     case XK_minus: return AK_SLASH;
     case XK_numbersign: return AK_NUMBERSIGN;
    }

    return -1;
}

static int decode_es (KeySym ks)
{
    switch(ks) {
	/* ES specific */
     case XK_A: case XK_a: return AK_A;
     case XK_M: case XK_m: return AK_M;
     case XK_Q: case XK_q: return AK_Q;
     case XK_W: case XK_w: return AK_W;
     case XK_Y: case XK_y: return AK_Y;
     case XK_Z: case XK_z: return AK_Z;
     case XK_ntilde: case XK_Ntilde: return AK_SEMICOLON;
#ifdef XK_dead_acute
     case XK_dead_acute: case XK_dead_diaeresis: return AK_QUOTE;
     case XK_dead_grave: case XK_dead_circumflex: return AK_LBRACKET;
#endif
     case XK_plus: case XK_asterisk: return AK_RBRACKET;
     case XK_comma: return AK_COMMA;
     case XK_period: return AK_PERIOD;
     case XK_less: case XK_greater: return AK_LTGT;
     case XK_backslash: case XK_bar: return AK_BACKQUOTE;
     case XK_apostrophe: return AK_MINUS;
     case XK_Igrave: case XK_igrave: return AK_EQUAL;
     case XK_minus: return AK_SLASH;
     case XK_numbersign: return AK_NUMBERSIGN;
    }

    return -1;
}

int xkeysym2amiga (KeySym ks)
{
    int as = kc_decode (ks);

    if (as == -1) {
	switch (currprefs.keyboard_lang) {
	    case KBD_LANG_FR:
		as = decode_fr (ks); break;
	    case KBD_LANG_US:
		as = decode_us (ks); break;
	    case KBD_LANG_DE:
		as = decode_de (ks); break;
	    case KBD_LANG_DK:
		as = decode_dk (ks); break;
	    case KBD_LANG_SE:
		as = decode_se (ks); break;
	    case KBD_LANG_IT:
		as = decode_it (ks); break;
	    case KBD_LANG_ES:
		as = decode_es (ks); break;
	}
    }
    return as;
}

/*
 * Default hotkeys
 *
 * We need a better way of doing this. ;-)
 */
static struct uae_hotkeyseq x11_hotkeys[] =
{
    { MAKE_HOTKEYSEQ (XK_F12, XK_q, -1, -1,           INPUTEVENT_SPC_QUIT) },
    { MAKE_HOTKEYSEQ (XK_F12, XK_r, -1, -1,           INPUTEVENT_SPC_SOFTRESET) },
    { MAKE_HOTKEYSEQ (XK_F12, XK_Shift_L, XK_r, -1,   INPUTEVENT_SPC_HARDRESET) },
    { MAKE_HOTKEYSEQ (XK_F12, XK_d, -1, -1,           INPUTEVENT_SPC_ENTERDEBUGGER) },
    { MAKE_HOTKEYSEQ (XK_F12, XK_s, -1, -1,           INPUTEVENT_SPC_TOGGLEFULLSCREEN) },
    { MAKE_HOTKEYSEQ (XK_F12, XK_g, -1, -1,           INPUTEVENT_SPC_TOGGLEMOUSEGRAB) },
    { MAKE_HOTKEYSEQ (XK_F12, XK_i, -1, -1,           INPUTEVENT_SPC_INHIBITSCREEN) },
    { MAKE_HOTKEYSEQ (XK_F12, XK_p, -1, -1,           INPUTEVENT_SPC_SCREENSHOT) },
    { MAKE_HOTKEYSEQ (XK_F12, XK_a, -1, -1,           INPUTEVENT_SPC_SWITCHINTERPOL) },
    { MAKE_HOTKEYSEQ (XK_F12, XK_F1, -1, -1,	      INPUTEVENT_SPC_FLOPPY0) },
    { MAKE_HOTKEYSEQ (XK_F12, XK_F2, -1, -1,	      INPUTEVENT_SPC_FLOPPY1) },
    { MAKE_HOTKEYSEQ (XK_F12, XK_F3, -1, -1,	      INPUTEVENT_SPC_FLOPPY2) },
    { MAKE_HOTKEYSEQ (XK_F12, XK_F4, -1, -1,	      INPUTEVENT_SPC_FLOPPY3) },
    { MAKE_HOTKEYSEQ (XK_F12, XK_Shift_L, XK_F1, -1,  INPUTEVENT_SPC_EFLOPPY0) },
    { MAKE_HOTKEYSEQ (XK_F12, XK_Shift_L, XK_F2, -1,  INPUTEVENT_SPC_EFLOPPY1) },
    { MAKE_HOTKEYSEQ (XK_F12, XK_Shift_L, XK_F3, -1,  INPUTEVENT_SPC_EFLOPPY2) },
    { MAKE_HOTKEYSEQ (XK_F12, XK_Shift_L, XK_F4, -1,  INPUTEVENT_SPC_EFLOPPY3) },
    { MAKE_HOTKEYSEQ (XK_F12, XK_Return, -1, -1,      INPUTEVENT_SPC_ENTERGUI) },
    { MAKE_HOTKEYSEQ (XK_F12, XK_f, -1, -1,           INPUTEVENT_SPC_FREEZEBUTTON) },
    { HOTKEYS_END }
};

struct uae_hotkeyseq *get_x11_default_hotkeys (void)
{
    return x11_hotkeys;
}
