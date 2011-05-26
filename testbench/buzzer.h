#ifndef BUZZER_H
# define BUZZER_H

//# define BUZZER_ENABLED
//# define BUZZER_INIT_SOUND
# define BUZZER_VOLUME_STEPS 512
# define BUZZER_CNT_MAX 1000
# define BUZZER_MIN_PSC 0
# define BUZZER_MAX_PSC (BUZZER_CNT_MAX / 4)

enum Notes {
  C3 = 262, D3f = 277, D3 = 294, E3f = 311, E3 = 330, F3 = 349,
  G3f = 370, G3 = 392, A3f = 415, A3 = 440, B3f = 466, B3 = 494,
  C4 = 523, D4f = 554, D4 = 587, E4f = 622, E4 = 659, F4 = 699,
  G4f = 740, G4 = 784, A4f = 831, A4 = 880, B4f = 932, B4 = 988,
  C5 = 1047
};

void vBuzzerInit();
void vBuzzerOn();
void vBuzzerOff();
void vBuzzerFrequencySet(int f);
void vBuzzerVolumeSet(int v, int show_progressbar);
void vBuzzerTask(void *pvParameters);

#endif
