/***************************************************************************
 *             __________               __   ___.
 *   Open      \______   \ ____   ____ |  | _\_ |__   _______  ___
 *   Source     |       _//  _ \_/ ___\|  |/ /| __ \ /  _ \  \/  /
 *   Jukebox    |    |   (  <_> )  \___|    < | \_\ (  <_> > <  <
 *   Firmware   |____|_  /\____/ \___  >__|_ \|___  /\____/__/\_ \
 *                     \/            \/     \/    \/            \/
 * $Id$
 *
 * Copyright © Amaury Pouly 2017
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This software is distributed on an "AS IS" basis, WITHOUT WARRANTY OF ANY
 * KIND, either express or implied.
 *
 ****************************************************************************/

#include "config.h"
#include "action.h"
#include "button.h"
#include "settings.h"

/* {Action Code,    Button code,    Prereq button code } */

/*
 * The format of the list is as follows
 * { Action Code,   Button code,    Prereq button code }
 * if there's no need to check the previous button's value, use BUTTON_NONE
 * Insert LAST_ITEM_IN_LIST at the end of each mapping
 */
static const struct button_mapping button_context_standard[]  = {
    { ACTION_STD_PREV,                  BUTTON_FF,                         BUTTON_NONE },
    { ACTION_STD_PREVREPEAT,            BUTTON_FF|BUTTON_REPEAT,           BUTTON_NONE },
    { ACTION_STD_NEXT,                  BUTTON_REW,                        BUTTON_NONE },
    { ACTION_STD_NEXTREPEAT,            BUTTON_REW|BUTTON_REPEAT,          BUTTON_NONE },

    { ACTION_STD_CONTEXT,               BUTTON_PLAY|BUTTON_REPEAT,         BUTTON_PLAY },

    { ACTION_STD_MENU,                  BUTTON_BACK,                       BUTTON_NONE },
    { ACTION_STD_MENU,                  BUTTON_BACK|BUTTON_REPEAT,         BUTTON_NONE },
    { ACTION_STD_OK,                    BUTTON_PLAY|BUTTON_REL,            BUTTON_PLAY },

    LAST_ITEM_IN_LIST
}; /* button_context_standard */

static const struct button_mapping button_context_wps[]  = {
    { ACTION_WPS_PLAY,                  BUTTON_PLAY|BUTTON_REL,            BUTTON_PLAY },
    { ACTION_WPS_STOP,                  BUTTON_PLAY|BUTTON_REPEAT,         BUTTON_NONE },

    { ACTION_WPS_SKIPNEXT,              BUTTON_FF|BUTTON_REL,              BUTTON_RIGHT },
    { ACTION_WPS_SEEKFWD,               BUTTON_FF|BUTTON_REPEAT,           BUTTON_NONE },
    { ACTION_WPS_SKIPPREV,              BUTTON_REW|BUTTON_REL,             BUTTON_LEFT },
    { ACTION_WPS_SEEKBACK,              BUTTON_REW|BUTTON_REPEAT,          BUTTON_NONE },
    { ACTION_WPS_STOPSEEK,              BUTTON_REW|BUTTON_REL,             BUTTON_REW|BUTTON_REPEAT },
    { ACTION_WPS_STOPSEEK,              BUTTON_FF|BUTTON_REL,              BUTTON_FF|BUTTON_REPEAT },

    { ACTION_WPS_VOLUP,                 BUTTON_VOL_UP,                     BUTTON_NONE },
    { ACTION_WPS_VOLUP,                 BUTTON_VOL_UP|BUTTON_REPEAT,       BUTTON_NONE },
    { ACTION_WPS_VOLDOWN,               BUTTON_VOL_DOWN,                   BUTTON_NONE },
    { ACTION_WPS_VOLDOWN,               BUTTON_VOL_DOWN|BUTTON_REPEAT,     BUTTON_NONE },

    { ACTION_WPS_MENU,                  BUTTON_BACK|BUTTON_REPEAT,         BUTTON_NONE },
    { ACTION_WPS_BROWSE,                BUTTON_BACK|BUTTON_REL,            BUTTON_BACK },

    LAST_ITEM_IN_LIST__NEXTLIST(CONTEXT_STD)
}; /* button_context_wps */

static const struct button_mapping button_context_keyboard[]  = {
    LAST_ITEM_IN_LIST
}; /* button_context_keyboard */

static const struct button_mapping button_context_quickscreen[]  = {
    { ACTION_STD_CONTEXT,               BUTTON_PLAY|BUTTON_REPEAT,         BUTTON_PLAY },
    { ACTION_STD_CANCEL,                BUTTON_PLAY|BUTTON_REL,            BUTTON_PLAY },
    { ACTION_STD_CANCEL,                BUTTON_BACK,                       BUTTON_NONE },

    LAST_ITEM_IN_LIST
}; /* button_context_quickscreen */

static const struct button_mapping button_context_tree[]  = {
    { ACTION_TREE_WPS,                  BUTTON_PLAY|BUTTON_REL,            BUTTON_PLAY },
    { ACTION_TREE_STOP,                 BUTTON_PLAY|BUTTON_REPEAT,         BUTTON_NONE },

    LAST_ITEM_IN_LIST__NEXTLIST(CONTEXT_LIST)
}; /* button_context_tree */

static const struct button_mapping button_context_list[]  = {
    { ACTION_LIST_VOLUP,                BUTTON_VOL_UP,                     BUTTON_NONE },
    { ACTION_LIST_VOLUP,                BUTTON_VOL_UP|BUTTON_REPEAT,       BUTTON_NONE },
    { ACTION_LIST_VOLDOWN,              BUTTON_VOL_DOWN,                   BUTTON_NONE },
    { ACTION_LIST_VOLDOWN,              BUTTON_VOL_DOWN|BUTTON_REPEAT,     BUTTON_NONE },

    LAST_ITEM_IN_LIST__NEXTLIST(CONTEXT_STD)
}; /* button_context_list */

#ifdef CONFIG_TUNER
static const struct button_mapping button_context_radio[]  = {
    { ACTION_FM_PLAY,                  BUTTON_PLAY|BUTTON_REL,           BUTTON_PLAY },
    { ACTION_FM_STOP,                  BUTTON_PLAY|BUTTON_REPEAT,        BUTTON_NONE },

    { ACTION_FM_EXIT,                  BUTTON_BACK,                      BUTTON_NONE },

    LAST_ITEM_IN_LIST__NEXTLIST(CONTEXT_SETTINGS)
}; /* button_context_radio */
#endif

#ifdef HAVE_RECORDING
static const struct button_mapping button_context_recscreen[]  = {
    { ACTION_REC_PAUSE,                BUTTON_PLAY,                        BUTTON_NONE },
    { ACTION_SETTINGS_INC,             BUTTON_FF,                          BUTTON_NONE },
    { ACTION_SETTINGS_INCREPEAT,       BUTTON_FF|BUTTON_REPEAT,            BUTTON_NONE },
    { ACTION_SETTINGS_DEC,             BUTTON_REW,                        BUTTON_NONE },
    { ACTION_SETTINGS_DECREPEAT,       BUTTON_REW|BUTTON_REPEAT,          BUTTON_NONE },

    LAST_ITEM_IN_LIST__NEXTLIST(CONTEXT_STD)
}; /* button_context_recscreen */
#endif

static const struct button_mapping button_context_settings[]  = {
    { ACTION_SETTINGS_INC,              BUTTON_VOL_UP,                     BUTTON_NONE },
    { ACTION_SETTINGS_INCREPEAT,        BUTTON_VOL_UP|BUTTON_REPEAT,       BUTTON_NONE },
    { ACTION_SETTINGS_DEC,              BUTTON_VOL_DOWN,                   BUTTON_NONE },
    { ACTION_SETTINGS_DECREPEAT,        BUTTON_VOL_DOWN|BUTTON_REPEAT,     BUTTON_NONE },

    LAST_ITEM_IN_LIST__NEXTLIST(CONTEXT_STD)
}; /* button_context_settings */

static const struct button_mapping button_context_settings_right_is_inc[]  = {

    LAST_ITEM_IN_LIST__NEXTLIST(CONTEXT_SETTINGS)
}; /* button_context_settings */

static const struct button_mapping button_context_time[]  = {

    { ACTION_STD_CANCEL,               BUTTON_BACK,                        BUTTON_NONE },
    { ACTION_STD_OK,                   BUTTON_PLAY,                        BUTTON_NONE },

    LAST_ITEM_IN_LIST__NEXTLIST(CONTEXT_SETTINGS)
}; /* button_context_time */

static const struct button_mapping button_context_colorchooser[]  = {
    { ACTION_STD_OK,                    BUTTON_PLAY|BUTTON_REL,            BUTTON_PLAY },
    { ACTION_STD_CANCEL,                BUTTON_BACK,                       BUTTON_NONE },

    LAST_ITEM_IN_LIST__NEXTLIST(CONTEXT_CUSTOM|CONTEXT_SETTINGS),
}; /* button_context_colorchooser */

static const struct button_mapping button_context_eq[]  = {
    { ACTION_STD_CANCEL,                BUTTON_BACK,                       BUTTON_NONE },
    { ACTION_STD_OK,                    BUTTON_PLAY|BUTTON_REL,            BUTTON_PLAY },

    LAST_ITEM_IN_LIST__NEXTLIST(CONTEXT_CUSTOM|CONTEXT_SETTINGS),
}; /* button_context_eq */

/* Bookmark Screen */
static const struct button_mapping button_context_bmark[]  = {
    { ACTION_BMS_DELETE,                BUTTON_PLAY|BUTTON_REPEAT,         BUTTON_PLAY },

    LAST_ITEM_IN_LIST__NEXTLIST(CONTEXT_LIST),
}; /* button_context_bmark */

static const struct button_mapping button_context_pitchscreen[]  = {
    { ACTION_PS_RESET,          BUTTON_PLAY,                               BUTTON_NONE },
    { ACTION_PS_EXIT,           BUTTON_BACK,                               BUTTON_NONE },

    LAST_ITEM_IN_LIST
}; /* button_context_pitchcreen */

static const struct button_mapping button_context_yesno[]  = {
    { ACTION_YESNO_ACCEPT,              BUTTON_PLAY,                     BUTTON_NONE },

    LAST_ITEM_IN_LIST__NEXTLIST(CONTEXT_STD)
}; /* button_context_yesno */

#ifdef USB_ENABLE_HID
static const struct button_mapping button_context_usb_hid[] = {

    LAST_ITEM_IN_LIST
}; /* button_context_usb_hid */

static const struct button_mapping button_context_usb_hid_mode_multimedia[] = {

    { ACTION_USB_HID_MULTIMEDIA_VOLUME_DOWN,         BUTTON_VOL_DOWN,               BUTTON_NONE },
    { ACTION_USB_HID_MULTIMEDIA_VOLUME_DOWN,         BUTTON_VOL_DOWN|BUTTON_REPEAT, BUTTON_NONE },
    { ACTION_USB_HID_MULTIMEDIA_VOLUME_UP,           BUTTON_VOL_UP,                 BUTTON_NONE },
    { ACTION_USB_HID_MULTIMEDIA_VOLUME_UP,           BUTTON_VOL_UP|BUTTON_REPEAT,   BUTTON_NONE },
    { ACTION_USB_HID_MULTIMEDIA_VOLUME_MUTE,         BUTTON_BACK|BUTTON_REL,        BUTTON_BACK },
    { ACTION_USB_HID_MULTIMEDIA_PLAYBACK_PLAY_PAUSE, BUTTON_PLAY|BUTTON_REL,        BUTTON_PLAY },
    { ACTION_USB_HID_MULTIMEDIA_PLAYBACK_STOP,       BUTTON_PLAY|BUTTON_REPEAT,     BUTTON_PLAY },
    { ACTION_USB_HID_MULTIMEDIA_PLAYBACK_TRACK_PREV, BUTTON_REW|BUTTON_REL,         BUTTON_LEFT },
    { ACTION_USB_HID_MULTIMEDIA_PLAYBACK_TRACK_NEXT, BUTTON_FF|BUTTON_REL,          BUTTON_RIGHT },

    LAST_ITEM_IN_LIST__NEXTLIST(CONTEXT_USB_HID)
}; /* button_context_usb_hid_mode_multimedia */

static const struct button_mapping button_context_usb_hid_mode_presentation[] = {
    { ACTION_USB_HID_PRESENTATION_SLIDESHOW_START, BUTTON_PLAY|BUTTON_REL,          BUTTON_PLAY },
    { ACTION_USB_HID_PRESENTATION_SLIDESHOW_LEAVE, BUTTON_PLAY|BUTTON_REPEAT,       BUTTON_PLAY },
    { ACTION_USB_HID_PRESENTATION_SLIDE_PREV,      BUTTON_REW|BUTTON_REL,           BUTTON_REW },
    { ACTION_USB_HID_PRESENTATION_SLIDE_NEXT,      BUTTON_FF|BUTTON_REL,            BUTTON_FF },
    { ACTION_USB_HID_PRESENTATION_SLIDE_FIRST,     BUTTON_REW|BUTTON_REPEAT,        BUTTON_NONE },
    { ACTION_USB_HID_PRESENTATION_SLIDE_LAST,      BUTTON_FF|BUTTON_REPEAT,         BUTTON_NONE },
    { ACTION_USB_HID_PRESENTATION_MOUSE_CLICK,     BUTTON_PLAY,                     BUTTON_PLAY },
    { ACTION_USB_HID_PRESENTATION_MOUSE_OVER,      BUTTON_PLAY|BUTTON_REPEAT,       BUTTON_PLAY },

    LAST_ITEM_IN_LIST__NEXTLIST(CONTEXT_USB_HID)
}; /* button_context_usb_hid_mode_presentation */

static const struct button_mapping button_context_usb_hid_mode_browser[] = {

    LAST_ITEM_IN_LIST__NEXTLIST(CONTEXT_USB_HID)
}; /* button_context_usb_hid_mode_browser */

#ifdef HAVE_USB_HID_MOUSE
static const struct button_mapping button_context_usb_hid_mode_mouse[] = {
    LAST_ITEM_IN_LIST__NEXTLIST(CONTEXT_USB_HID)
}; /* button_context_usb_hid_mode_mouse */
#endif
#endif

const struct button_mapping* target_get_context_mapping(int context)
{
    switch (context)
    {
        case CONTEXT_STD:
            return button_context_standard;
        case CONTEXT_MAINMENU:
            return button_context_tree;
        case CONTEXT_SETTINGS:
            return button_context_settings;
        case CONTEXT_WPS:
            return button_context_wps; 
        case CONTEXT_YESNOSCREEN:
            return button_context_yesno;
        case CONTEXT_SETTINGS_TIME:
            return button_context_time;
        case CONTEXT_KEYBOARD:
        case CONTEXT_MORSE_INPUT:
            return button_context_keyboard;
#ifdef CONFIG_TUNER
        case CONTEXT_FM:
             return button_context_radio;
#endif
        case CONTEXT_LIST:
            return button_context_list;
        case CONTEXT_TREE:
            return button_context_tree;
        case CONTEXT_SETTINGS_EQ:
            return button_context_eq;
#ifdef HAVE_RECORDING
        case CONTEXT_RECSCREEN:
            return button_context_recscreen;
#endif
        case CONTEXT_QUICKSCREEN:
            return button_context_quickscreen;
        case CONTEXT_BOOKMARKSCREEN:
            return button_context_bmark;
        case CONTEXT_PITCHSCREEN:
            return button_context_pitchscreen;
        case CONTEXT_SETTINGS_COLOURCHOOSER:
            return button_context_colorchooser;
        case CONTEXT_SETTINGS_RECTRIGGER:
            return button_context_settings_right_is_inc;
    case CONTEXT_CUSTOM|CONTEXT_SETTINGS:
            return button_context_settings_right_is_inc;
#ifdef USB_ENABLE_HID
        case CONTEXT_USB_HID:
            return button_context_usb_hid;
        case CONTEXT_USB_HID_MODE_MULTIMEDIA:
            return button_context_usb_hid_mode_multimedia;
        case CONTEXT_USB_HID_MODE_PRESENTATION:
            return button_context_usb_hid_mode_presentation;
        case CONTEXT_USB_HID_MODE_BROWSER:
            return button_context_usb_hid_mode_browser;
#ifdef HAVE_USB_HID_MOUSE
        case CONTEXT_USB_HID_MODE_MOUSE:
            return button_context_usb_hid_mode_mouse;
#endif
#endif
        default:
            return button_context_standard;
    } 
    return button_context_standard;
}
