/*
Copyright 2022 @Yowkees
Copyright 2022 MURAOKA Taro (aka KoRoN, @kaoriya)

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

#include QMK_KEYBOARD_H

#include "quantum.h"

enum custom_keycodes {
    SNIPER_SPACE = SAFE_RANGE,
    SCROLL_RIGHT,
    VOLUME_LEFT,
};

static uint16_t sniper_saved_cpi = 0;
static uint16_t sniper_timer     = 0;
static bool     sniper_pressed   = false;
static bool     sniper_active    = false;
static uint16_t scroll_timer     = 0;
static bool     scroll_pressed   = false;
static bool     scroll_active    = false;
static uint16_t volume_timer     = 0;
static bool     volume_pressed   = false;
static bool     volume_active    = false;

// clang-format off
const uint16_t PROGMEM keymaps[][MATRIX_ROWS][MATRIX_COLS] = {
  [0] = LAYOUT_universal(
    KC_TILD  , KC_1     , KC_2     , KC_3     , KC_4     , KC_5     ,                                  KC_6     , KC_7     , KC_8     , KC_9     , KC_0     , KC_MINS  ,
    KC_TAB   , KC_Q     , KC_W     , KC_E     , KC_R     , KC_T     ,                                  KC_Y     , KC_U     , KC_I     , KC_O     , KC_P     , KC_EQL   ,
    KC_ESC   , KC_A     , KC_S     , KC_D     , KC_F     , KC_G     ,                                  KC_H     , KC_J     , KC_K     , KC_L     , KC_SCLN  , KC_QUOT  ,
    KC_LSFT  , KC_Z     , KC_X     , KC_C     , KC_V     , KC_B     , KC_LBRC  ,              KC_RBRC, KC_N     , KC_M     , KC_COMM  , KC_DOT   , KC_SLSH  , KC_RSFT  ,
    KC_LCTL  , KC_LALT  , MO(1)    , KC_LGUI,VOLUME_LEFT,SCROLL_RIGHT,SNIPER_SPACE,    KC_BSPC,LT(2,KC_ENT),LT(1,KC_LNG2),KC_RGUI, _______ , KC_RALT  , KC_BSLS
  ),

  [1] = LAYOUT_universal(
    S(KC_ESC), S(KC_1)  , KC_LBRC  , S(KC_3)  , S(KC_4)  , S(KC_5)  ,                                  KC_EQL   , S(KC_6)  ,S(KC_QUOT), S(KC_8)  , S(KC_9)  ,S(KC_INT1),
    S(KC_DEL), S(KC_Q)  , S(KC_W)  , S(KC_E)  , S(KC_R)  , S(KC_T)  ,                                  S(KC_Y)  , S(KC_U)  , S(KC_I)  , S(KC_O)  , S(KC_P)  ,S(KC_INT3),
    S(KC_TAB), S(KC_A)  , S(KC_S)  , S(KC_D)  , S(KC_F)  , S(KC_G)  ,                                  KC_LEFT  , KC_DOWN  , KC_UP    , KC_RGHT  , KC_QUOT  , S(KC_2)  ,
    _______  , S(KC_Z)  , S(KC_X)  , S(KC_C)  , S(KC_V)  , S(KC_B)  ,S(KC_RBRC),           S(KC_NUHS), S(KC_N)  , S(KC_M)  ,S(KC_COMM), S(KC_DOT),S(KC_SLSH),S(KC_RSFT),
    _______  ,S(KC_LCTL),S(KC_LALT),S(KC_LGUI), KC_DOWN  , KC_UP    , _______  ,            _______  , _______  , _______  ,S(KC_RGUI), _______  , S(KC_RALT), _______
  ),

  [2] = LAYOUT_universal(
    SSNP_FRE , KC_F1    , KC_F2    , KC_F3    , KC_F4    , KC_F5    ,                                  KC_F6    , KC_F7    , KC_F8    , KC_F9    , KC_F10   , KC_F11   ,
    SSNP_VRT , _______  , KC_7     , KC_8     , KC_9     , _______  ,                                  _______  , KC_LEFT  , KC_UP    , KC_RGHT  , _______  , KC_F12   ,
    SSNP_HOR , _______  , KC_4     , KC_5     , KC_6     ,S(KC_SCLN),                                  KC_PGUP  , MS_BTN1  , MS_BTN2  , _______  , _______  , _______  ,
    _______  , _______  , KC_1     , KC_2     , KC_3     ,S(KC_MINS), S(KC_8)  ,            S(KC_9)  , KC_PGDN  , _______  , _______  , _______  , _______  , _______  ,
    _______  , _______  , KC_0     , KC_DOT   , KC_UP    , KC_DOWN  , _______  ,             KC_DEL  , _______  , _______  , _______  , _______  , _______  , _______
  ),

  [3] = LAYOUT_universal(
    UG_TOGG  , AML_TO   , AML_I50  , AML_D50  , _______  , _______  ,                                  RGB_M_P  , RGB_M_B  , RGB_M_R  , RGB_M_SW , RGB_M_SN , RGB_M_K  ,
    UG_NEXT  , UG_HUEU  , UG_SATU  , UG_VALU  , _______  , _______  ,                                  RGB_M_X  , RGB_M_G  , RGB_M_T  , RGB_M_TW , _______  , _______  ,
    UG_PREV  , UG_HUED  , UG_SATD  , UG_VALD  , _______  , _______  ,                                  _______  , _______  , _______  , _______  , KBC_SAVE , KBC_RST  ,
    _______  , _______  , SCRL_DVD , SCRL_DVI , SCRL_MO  , SCRL_TO  , EE_CLR   ,            EE_CLR   , KC_HOME  , KC_PGDN  , KC_PGUP  , KC_END   , _______  , _______  ,
    QK_BOOT  , _______  , KC_LEFT  , KC_DOWN  , KC_UP    , KC_RGHT  , _______  ,            _______  , KC_BSPC  , _______  , _______  , _______  , _______  , QK_BOOT
  ),
};
// clang-format on

bool process_record_user(uint16_t keycode, keyrecord_t *record) {
    switch (keycode) {
        case VOLUME_LEFT:
            if (record->event.pressed) {
                volume_timer   = timer_read();
                volume_pressed = true;
                volume_active  = false;
            } else {
                volume_pressed = false;
                if (volume_active) {
                    keyball_set_scroll_mode(false);
                    volume_active = false;
                } else {
                    tap_code(KC_LEFT);
                }
            }
            return false;

        case SNIPER_SPACE:
            if (record->event.pressed) {
                sniper_timer   = timer_read();
                sniper_pressed = true;
                sniper_active  = false;
            } else {
                sniper_pressed = false;
                if (sniper_active) {
                    keyball_set_cpi(sniper_saved_cpi);
                    sniper_active = false;
                } else {
                    tap_code(KC_SPC);
                }
            }
            return false;

        case SCROLL_RIGHT:
            if (record->event.pressed) {
                scroll_timer   = timer_read();
                scroll_pressed = true;
                scroll_active  = false;
            } else {
                scroll_pressed = false;
                if (scroll_active) {
                    keyball_set_scroll_mode(false);
                    scroll_active = false;
                } else {
                    tap_code(KC_RGHT);
                }
            }
            return false;
    }
    return true;
}

void matrix_scan_user(void) {
    if (sniper_pressed && !sniper_active && timer_elapsed(sniper_timer) > TAPPING_TERM) {
        sniper_saved_cpi = keyball_get_cpi();
        keyball_set_cpi(100);
        sniper_active = true;
    }

    if (scroll_pressed && !scroll_active && timer_elapsed(scroll_timer) > TAPPING_TERM) {
        keyball_set_scroll_mode(true);
        scroll_active = true;
    }

    if (volume_pressed && !volume_active && timer_elapsed(volume_timer) > TAPPING_TERM) {
        keyball_set_scroll_mode(true);
        volume_active = true;
    }
}

report_mouse_t pointing_device_task_user(report_mouse_t mouse_report) {
    if (volume_active) {
        while (mouse_report.v > 0) {
            tap_code(KC_VOLU);
            mouse_report.v--;
        }
        while (mouse_report.v < 0) {
            tap_code(KC_VOLD);
            mouse_report.v++;
        }

        // Consume ball movement so volume mode cannot move or scroll the pointer.
        mouse_report.x = 0;
        mouse_report.y = 0;
        mouse_report.h = 0;
    }
    return mouse_report;
}

layer_state_t layer_state_set_user(layer_state_t state) {
    // Auto enable scroll mode when the highest layer is 3
    keyball_set_scroll_mode(get_highest_layer(state) == 3);
    return state;
}

#ifdef OLED_ENABLE

#    include "lib/oledkit/oledkit.h"

void oledkit_render_info_user(void) {
    oledkit_render_logo_user();
}
#endif
