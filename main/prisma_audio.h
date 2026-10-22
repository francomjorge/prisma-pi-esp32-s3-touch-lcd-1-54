#pragma once

#include <stdbool.h>
#include <stdint.h>
#include "esp_err.h"

typedef struct {
    bool ready;
    bool mic_enabled;
    bool speaker_enabled;
    float level;
    float peak;
    int input_gain_db;
    int output_volume;
} prisma_audio_state_t;

esp_err_t prisma_audio_init(void);
void prisma_audio_get_state(prisma_audio_state_t *state);
esp_err_t prisma_audio_set_mic(bool enabled);
esp_err_t prisma_audio_set_speaker(bool enabled);
esp_err_t prisma_audio_set_input_gain(int gain_db);
esp_err_t prisma_audio_set_output_volume(int volume);
esp_err_t prisma_audio_beep(uint32_t frequency_hz, uint32_t duration_ms);
