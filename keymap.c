#include QMK_KEYBOARD_H

static bool jiggler_active = false;
static uint32_t jiggler_start_time = 0;
static deferred_token jiggler_token = INVALID_DEFERRED_TOKEN;

enum custom_keycodes {
    JIGGLER_TOGGLE = QFAILSAFE,
};

static uint32_t jiggler_callback(uint32_t trigger_time, void *cb_arg) {
    if (!jiggler_active) {
        return 0;
    }

    if (timer_elapsed32(jiggler_start_time) > 28800000) {
        jiggler_active = false;
        return 0;
    }

    tap_code16(MS_MS_RIGHT);
    tap_code16(MS_MS_LEFT);

    uint32_t random_delay = 25000 + (rand() % 5000);
    return random_delay;
}

bool process_record_user(uint16_t keycode, keyrecord_t *record) {
    switch (keycode) {
        case JIGGLER_TOGGLE:
            if (record->event.pressed) {
                jiggler_active = !jiggler_active;
                if (jiggler_active) {
                    jiggler_start_time = timer_read32();
                    jiggler_token = defer_exec(5000, jiggler_callback, NULL);
                } else {
                    if (jiggler_token != INVALID_DEFERRED_TOKEN) {
                        cancel_deferred_exec(jiggler_token);
                        jiggler_token = INVALID_DEFERRED_TOKEN;
                    }
                }
            }
            return false;
    }
    return true;
}
