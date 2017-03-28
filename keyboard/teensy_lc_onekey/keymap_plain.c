/*
Copyright 2012,2013 Jun Wako <wakojun@gmail.com>

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 2 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#include "keycode.h"
#include "action.h"
#include "action_macro.h"
#include "report.h"
#include "host.h"
#include "print.h"
#include "debug.h"
#include "keymap.h"

static const uint8_t keymaps[][MATRIX_ROWS][MATRIX_COLS] = {
  {
    {KC_F6, KC_NO, KC_8, KC_EQL, KC_K, KC_RBRC, KC_COMM, KC_I},
    {KC_LALT, KC_FN0, KC_PSCR, KC_NO, KC_NO, KC_NO, KC_NO, KC_SLCK},
    {KC_NO, KC_MENU, KC_9, KC_F8, KC_L, KC_F7, KC_DOT, KC_O},
    {KC_H, KC_N, KC_7, KC_6, KC_J, KC_Y, KC_M, KC_U},
    {KC_G, KC_B, KC_4, KC_5, KC_F, KC_T, KC_V, KC_R},
    {KC_F4, KC_NO, KC_3, KC_F2, KC_D, KC_F3, KC_C, KC_E},
    {KC_NO, KC_NO, KC_2, KC_F1, KC_S, KC_LCTRL, KC_X, KC_W},
    {KC_ESC, KC_NO, KC_1, KC_GRV, KC_A, KC_TAB, KC_Z, KC_Q},
    {KC_NO, KC_NO, KC_F5, KC_LCTRL, KC_NO, KC_NO, KC_RCTRL, KC_PAUS},
    {KC_F11, KC_F12, KC_F10, KC_F9, KC_BSLS, KC_BSPC, KC_ENT, KC_NO},
    {KC_KP_0, KC_RIGHT, KC_NO, KC_INSERT, KC_KP_2, KC_KP_5, KC_KP_SLASH, KC_KP_8},
    {KC_FN1, KC_DOWN, KC_NO, KC_DEL, KC_KP_1, KC_KP_4, KC_NUMLOCK, KC_KP_7},
    {KC_QUOT, KC_SLSH, KC_0, KC_MINUS, KC_SCOLON, KC_LBRC, KC_NO, KC_P},
    {KC_KP_DOT, KC_KP_MINUS, KC_PGDOWN, KC_PGUP, KC_KP_3, KC_KP_6, KC_KP_ASTERISK, KC_KP_9},
    {KC_NO, KC_NO, KC_NO, KC_NO, KC_RSHIFT, KC_LSHIFT, KC_NO, KC_NO},
    {KC_NO, KC_NO, KC_NO, KC_NO, KC_NO, KC_LGUI, KC_NO, KC_NO},
    {KC_NO, KC_NO, KC_NO, KC_NO, KC_RGUI, KC_NO, KC_NO, KC_NO},
    {KC_UP, KC_LEFT, KC_END, KC_HOME, KC_KP_ENTER, KC_NO, KC_NO, KC_KP_PLUS},
  },
  {
    {KC_F6, KC_NO, KC_8, KC_EQL, KC_K, KC_RBRC, KC_COMM, KC_I},
    {KC_LALT, KC_TRNS, KC_PSCR, KC_NO, KC_NO, KC_NO, KC_NO, KC_SLCK},
    {KC_NO, KC_MENU, KC_9, KC_F8, KC_L, KC_F7, KC_DOT, KC_O},
    {KC_H, KC_N, KC_7, KC_6, KC_J, KC_Y, KC_M, KC_U},
    {KC_G, KC_B, KC_4, KC_5, KC_RIGHT, KC_T, KC_V, KC_R},
    {KC_F4, KC_NO, KC_3, KC_F2, KC_DOWN, KC_F3, KC_C, KC_UP},
    {KC_NO, KC_NO, KC_2, KC_F1, KC_LEFT, KC_LCTRL, KC_X, KC_W},
    {KC_ESC, KC_NO, KC_1, KC_GRV, KC_A, KC_TAB, KC_Z, KC_Q},
    {KC_NO, KC_NO, KC_F5, KC_LCTRL, KC_NO, KC_NO, KC_RCTRL, KC_PAUS},
    {KC_F11, KC_F12, KC_F10, KC_F9, KC_BSLS, KC_BSPC, KC_ENT, KC_NO},
    {KC_KP_0, KC_RIGHT, KC_NO, KC_INSERT, KC_KP_2, KC_KP_5, KC_KP_SLASH, KC_KP_8},
    {KC_FN1, KC_DOWN, KC_NO, KC_DEL, KC_KP_1, KC_KP_4, KC_NUMLOCK, KC_KP_7},
    {KC_QUOT, KC_SLSH, KC_0, KC_MINUS, KC_SCOLON, KC_LBRC, KC_NO, KC_P},
    {KC_KP_DOT, KC_KP_MINUS, KC_PGDOWN, KC_PGUP, KC_KP_3, KC_KP_6, KC_KP_ASTERISK, KC_KP_9},
    {KC_NO, KC_NO, KC_NO, KC_NO, KC_RSHIFT, KC_LSHIFT, KC_NO, KC_NO},
    {KC_NO, KC_NO, KC_NO, KC_NO, KC_NO, KC_LGUI, KC_NO, KC_NO},
    {KC_NO, KC_NO, KC_NO, KC_NO, KC_RGUI, KC_NO, KC_NO, KC_NO},
    {KC_UP, KC_LEFT, KC_END, KC_HOME, KC_KP_ENTER, KC_NO, KC_NO, KC_KP_PLUS},
  },
  {
    {KC_F6, KC_NO, KC_8, KC_EQL, KC_8, KC_RBRC, KC_COMM, KC_I},
    {KC_LALT, KC_TRNS, KC_PSCR, KC_NO, KC_NO, KC_NO, KC_NO, KC_SLCK},
    {KC_NO, KC_MENU, KC_9, KC_F8, KC_9, KC_F7, KC_DOT, KC_O},
    {KC_6, KC_N, KC_7, KC_6, KC_7, KC_Y, KC_M, KC_U},
    {KC_5, KC_B, KC_4, KC_5, KC_4, KC_T, KC_V, KC_R},
    {KC_F4, KC_NO, KC_3, KC_F2, KC_3, KC_F3, KC_C, KC_UP},
    {KC_NO, KC_NO, KC_2, KC_F1, KC_2, KC_LCTRL, KC_X, KC_W},
    {KC_ESC, KC_NO, KC_1, KC_GRV, KC_1, KC_TAB, KC_Z, KC_Q},
    {KC_NO, KC_NO, KC_F5, KC_LCTRL, KC_NO, KC_NO, KC_RCTRL, KC_PAUS},
    {KC_F11, KC_F12, KC_F10, KC_F9, KC_BSLS, KC_BSPC, KC_ENT, KC_NO},
    {KC_KP_0, KC_RIGHT, KC_NO, KC_INSERT, KC_KP_2, KC_KP_5, KC_KP_SLASH, KC_KP_8},
    {KC_FN1, KC_DOWN, KC_NO, KC_DEL, KC_KP_1, KC_KP_4, KC_NUMLOCK, KC_KP_7},
    {KC_QUOT, KC_SLSH, KC_0, KC_MINUS, KC_0, KC_LBRC, KC_NO, KC_P},
    {KC_KP_DOT, KC_KP_MINUS, KC_PGDOWN, KC_PGUP, KC_KP_3, KC_KP_6, KC_KP_ASTERISK, KC_KP_9},
    {KC_NO, KC_NO, KC_NO, KC_NO, KC_RSHIFT, KC_LSHIFT, KC_NO, KC_NO},
    {KC_NO, KC_NO, KC_NO, KC_NO, KC_NO, KC_LGUI, KC_NO, KC_NO},
    {KC_NO, KC_NO, KC_NO, KC_NO, KC_RGUI, KC_NO, KC_NO, KC_NO},
    {KC_UP, KC_LEFT, KC_END, KC_HOME, KC_KP_ENTER, KC_NO, KC_NO, KC_KP_PLUS},
  },
};  // to test: KC_CAPS, KT_BTLD, KC_A

static const action_t fn_actions[] = {
  [0] = ACTION_LAYER_MOMENTARY(1),
  [1] = ACTION_LAYER_TAP_KEY(2, KC_SPC),
};

/* translates key to keycode */
uint8_t keymap_key_to_keycode(uint8_t layer, keypos_t key)
{
    return keymaps[(layer)][(key.row)][(key.col)];
}

/* translates Fn keycode to action */
action_t keymap_fn_to_action(uint8_t keycode)
{
    return fn_actions[FN_INDEX(keycode)];
}
