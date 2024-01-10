/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2023 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "main.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "stm32f429i_discovery_lcd.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
ADC_HandleTypeDef hadc1;
ADC_HandleTypeDef hadc2;

DMA2D_HandleTypeDef hdma2d;

I2C_HandleTypeDef hi2c3;

LTDC_HandleTypeDef hltdc;

RNG_HandleTypeDef hrng;

RTC_HandleTypeDef hrtc;

SPI_HandleTypeDef hspi5;

TIM_HandleTypeDef htim3;

SDRAM_HandleTypeDef hsdram1;

/* USER CODE BEGIN PV */

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_DMA2D_Init(void);
static void MX_FMC_Init(void);
static void MX_I2C3_Init(void);
static void MX_LTDC_Init(void);
static void MX_SPI5_Init(void);
static void MX_ADC1_Init(void);
static void MX_RNG_Init(void);
static void MX_RTC_Init(void);
static void MX_ADC2_Init(void);
static void MX_TIM3_Init(void);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */
#include <assert.h>
#include <stdlib.h>
#include <string.h>

#define SZER_EKRANU 240
#define WYS_EKRANU 320

#define OCZKA_W_POZIOMIE 10
#define OCZKA_W_PIONIE 20

#define SZER_OCZKA 16
#define WYS_OCZKA 16

#define SZER_RAMKI_OCZKA 3

#define POCZATEK_PLANSZY_X 0
#define POCZATEK_PLANSZY_Y 0

#define LICZBA_RODZAJOW_KLOCKOW 7
#define MAKSYMALNY_BOK_MASKI_KLOCKA 4

#define LICZBA_KOLOROW_OCZEK 7

#define COOLDOWN_NA_RUCH 225
#define COOLDOWN_NA_OBROT 300
#define COOLDOWN_NA_PRZESUNIECIE 700

#define MULTIPLIKATOR_RUCHU 8

#define MAX(x, y) ((x) > (y) ? (x) : (y))
#define MIN(x, y) ((x) < (y) ? (x) : (y))

#define ROZMIAR_BUFORA_KLOCKOW 2

#define NASTEPNY_KLOCEK()	(&klocek_buf[((klocek_buf_i)+1) % ROZMIAR_BUFORA_KLOCKOW])
#define POPRZEDNI_KLOCEK()	(&klocek_buf[((klocek_buf_i)+ROZMIAR_BUFORA_KLOCKOW-1) % ROZMIAR_BUFORA_KLOCKOW])
#define BIEZACY_KLOCEK()	(&klocek_buf[klocek_buf_i])
#define AKTUALIZUJ_KLOCKI()	(klocek_buf_i++)

#define CEL_KLATKAZU 30
#define INTERWAL_AKTUALIZACJI_RZADKICH 1000

#define LINIA_NAPISU_FPS 0
#define NAPIS_FPS_LICZBA_START 15
static char napis_fps[24] = "               00 FPS";

#define DLUGOSC_LICZNIKA_PUNKTOW 4
#define LINIA_NAPISU_PKT 1
#define NAPIS_PKT_LICZBA_START 15
static char napis_pkt[24] = "               0000 P";

#define LINIA_NAPISU_OSX 2
#define NAPIS_OSX_LICZBA_START 17
static char napis_osx[24] = "               X 0000";

#define LINIA_NAPISU_OSY 3
#define NAPIS_OSY_LICZBA_START 17
static char napis_osy[24] = "               Y 0000";

#define LINIA_NAPISU_GAME_OVER 7
#define LICZBA_NAPISOW_GAME_OVER 6

static char napisy_game_over[LICZBA_NAPISOW_GAME_OVER][24] = {
	"=====================",
	"      GAME OVER      ",
	"                     ",
	" Nacisnij RESET zeby ",
	" sprobowac ponownie! ",
	"=====================",
};

#define LICZBA_LINII_TEKSTU 4

typedef enum Paleta {
	J_CZERW,
	J_ZOLTY,
	J_POMAR,
	J_ZIELO,
	J_TURKU,
	J_NIEBI,
	J_FIOLE,
	J_SZARY,
	C_CZERW,
	C_ZOLTY,
	C_POMAR,
	C_ZIELO,
	C_TURKU,
	C_NIEBI,
	C_FIOLE,
	C_SZARY,
	ROZMIAR_PALETY
} Paleta;

static unsigned int paleta[ROZMIAR_PALETY] = {
	LCD_COLOR_RED,
	LCD_COLOR_YELLOW,
	LCD_COLOR_ORANGE,
	LCD_COLOR_GREEN,
	LCD_COLOR_CYAN,
	LCD_COLOR_BLUE,
	LCD_COLOR_MAGENTA,
	LCD_COLOR_WHITE,
	LCD_COLOR_DARKRED,
	LCD_COLOR_DARKYELLOW,
	LCD_COLOR_BROWN,
	LCD_COLOR_DARKGREEN,
	LCD_COLOR_DARKCYAN,
	LCD_COLOR_DARKBLUE,
	LCD_COLOR_DARKMAGENTA,
	LCD_COLOR_BLACK,
};

static uint8_t plansza[OCZKA_W_PIONIE][OCZKA_W_POZIOMIE] = {{0}};

static uint8_t kolory_oczek[LICZBA_KOLOROW_OCZEK+1] = {
	0,
	J_CZERW,
	J_POMAR,
	J_ZOLTY,
	J_ZIELO,
	J_TURKU,
	J_NIEBI,
	J_FIOLE
};

static uint8_t maski_klockow[LICZBA_RODZAJOW_KLOCKOW][MAKSYMALNY_BOK_MASKI_KLOCKA][MAKSYMALNY_BOK_MASKI_KLOCKA] = {
	{
		{0, 1, 0, 0},
		{0, 1, 0, 0},
		{0, 1, 0, 0},
		{0, 1, 0, 0},
	},
	{
		{1, 1, 0, 0},
		{1, 1, 0, 0},
		{0, 0, 0, 0},
		{0, 0, 0, 0},
	},
	{
		{1, 0, 0, 0},
		{1, 0, 0, 0},
		{1, 1, 0, 0},
		{0, 0, 0, 0},
	},
	{
		{1, 0, 0, 0},
		{1, 1, 0, 0},
		{1, 0, 0, 0},
		{0, 0, 0, 0},
	},
	{
		{0, 1, 0, 0},
		{0, 1, 0, 0},
		{1, 1, 0, 0},
		{0, 0, 0, 0},
	},
	{
		{0, 1, 0, 0},
		{1, 1, 0, 0},
		{1, 0, 0, 0},
		{0, 0, 0, 0},
	},
	{
		{1, 0, 0, 0},
		{1, 1, 0, 0},
		{0, 1, 0, 0},
		{0, 0, 0, 0},
	}
};

static unsigned int dlugosci_bokow_masek[LICZBA_RODZAJOW_KLOCKOW] = {4, 2, 3, 3, 3, 3, 3};

typedef struct Klocek {
	uint8_t maska	  [MAKSYMALNY_BOK_MASKI_KLOCKA][MAKSYMALNY_BOK_MASKI_KLOCKA];
	uint8_t maska_tmp [MAKSYMALNY_BOK_MASKI_KLOCKA][MAKSYMALNY_BOK_MASKI_KLOCKA];
	int x;
	int y;
	unsigned int dlugosc_boku_maski;
} Klocek;

static Klocek klocek_buf[ROZMIAR_BUFORA_KLOCKOW];
static unsigned int klocek_buf_i = 0;

static int cooldown_na_przesuniecie = 700;
static int czas_przesuniecia = 0;
static int cooldown_na_obrot_w_prawo = 0;
static int cooldown_na_obrot_w_lewo = 0;
static int cooldown_na_ruch_w_prawo = 0;
static int cooldown_na_ruch_w_lewo = 0;

static uint8_t ruch_w_prawo = 0;
static uint8_t ruch_w_lewo = 0;
static uint8_t ruch_w_dol = 0;
static uint8_t obrot_w_prawo = 0;
static uint8_t obrot_w_lewo = 0;

static uint8_t gra_jest_przegrana = 0;
uint8_t muzyka_wlaczona = 1;

static unsigned int licznik_punktow = 0;

static void rysujLiniePionowa(int x, int y1, int h, Paleta p) {
	BSP_LCD_SetTextColor(paleta[p]);
	BSP_LCD_DrawVLine(x, y1, h);
}

static void wypelnijProstokat(int y, int x, int h, int w, Paleta p) {
	assert(w >= 0);
	assert(h >= 0);

	if (x + w > SZER_EKRANU) {
		w = SZER_EKRANU - x;
	}
	if (y + h > WYS_EKRANU) {
		h = WYS_EKRANU - y;
	}
	if (x < 0) {
		w -= x;
		x = 0;
	}
	if (y < 0) {
		h -= y;
		y = 0;
	}

	BSP_LCD_SetTextColor(paleta[p]);
	BSP_LCD_FillRect(x, y, w, h);
}

static void liczbaNaTekst(uint32_t liczba, char * tekst, unsigned int dlugosc_tekstu) {
	for (int i = 0; i < dlugosc_tekstu; i++) {
		char cyfra = '0' + liczba % 10;
		tekst[dlugosc_tekstu-1-i] = cyfra;
		liczba /= 10;
	}
}

void rysujLicznikFPS(int liczba_fps) {
	napis_fps[NAPIS_FPS_LICZBA_START+0] = '0' + liczba_fps / 10;
	napis_fps[NAPIS_FPS_LICZBA_START+1] = '0' + liczba_fps % 10;
	BSP_LCD_SetTextColor(LCD_COLOR_WHITE);
	BSP_LCD_SetBackColor(LCD_COLOR_BLACK);
	BSP_LCD_DisplayStringAtLine(LINIA_NAPISU_FPS, (uint8_t *)napis_fps);
}

static void rysujLicznikPunktow(void) {
	liczbaNaTekst(licznik_punktow, napis_pkt+NAPIS_PKT_LICZBA_START, DLUGOSC_LICZNIKA_PUNKTOW);
	BSP_LCD_SetTextColor(LCD_COLOR_WHITE);
	BSP_LCD_SetBackColor(LCD_COLOR_BLACK);
	BSP_LCD_DisplayStringAtLine(LINIA_NAPISU_PKT, (uint8_t *)napis_pkt);
}

void rysujPozycjeGalek(uint32_t analogowe_pomiary[2]) {
	liczbaNaTekst(analogowe_pomiary[0], napis_osx+NAPIS_OSX_LICZBA_START, 4);
	liczbaNaTekst(analogowe_pomiary[1], napis_osy+NAPIS_OSY_LICZBA_START, 4);
	BSP_LCD_SetTextColor(LCD_COLOR_WHITE);
	BSP_LCD_SetBackColor(LCD_COLOR_BLACK);
	BSP_LCD_DisplayStringAtLine(LINIA_NAPISU_OSX, (uint8_t *)napis_osx);
	BSP_LCD_DisplayStringAtLine(LINIA_NAPISU_OSY, (uint8_t *)napis_osy);
}

void rysujNapisyGameOver(void) {
	BSP_LCD_SetTextColor(LCD_COLOR_WHITE);
	BSP_LCD_SetBackColor(LCD_COLOR_BLACK);
	for (int i = 0; i < LICZBA_NAPISOW_GAME_OVER; i++) {
		BSP_LCD_DisplayStringAtLine(LINIA_NAPISU_GAME_OVER+i, (uint8_t *)napisy_game_over[i]);
	}
}

static void rysujOczko(int y, int x, Paleta p) {
	wypelnijProstokat(POCZATEK_PLANSZY_Y + y * WYS_OCZKA, POCZATEK_PLANSZY_X + x * SZER_OCZKA, WYS_OCZKA, SZER_OCZKA, p+ROZMIAR_PALETY/2);
	wypelnijProstokat(POCZATEK_PLANSZY_Y + y * WYS_OCZKA + SZER_RAMKI_OCZKA, POCZATEK_PLANSZY_X + x * SZER_OCZKA + SZER_RAMKI_OCZKA,
				   	  WYS_OCZKA - SZER_RAMKI_OCZKA * 2, SZER_OCZKA - SZER_RAMKI_OCZKA * 2, p);
}

static void czyscOczko(int y, int x) {
	wypelnijProstokat(POCZATEK_PLANSZY_Y + y * WYS_OCZKA, POCZATEK_PLANSZY_X + x * SZER_OCZKA, WYS_OCZKA, SZER_OCZKA, C_SZARY);
}


static void rysujKlocek(Klocek * k) {
	for (int iy = 0; iy < k->dlugosc_boku_maski; iy++) {
		for (int ix = 0; ix < k->dlugosc_boku_maski; ix++) {
			if (k->y+iy >= OCZKA_W_PIONIE ||
				k->x+ix >= OCZKA_W_POZIOMIE ||
				k->y+iy < 0 ||
				k->x+ix < 0) {
				continue;
			}
			int ik = k->maska[iy][ix];
			if (ik == 0) {
				continue;
			}
			Paleta p = kolory_oczek[ik];
			rysujOczko(k->y+iy, k->x+ix, p);
		}
	}
}

static void czyscKlocek(Klocek * k) {
	for (int iy = 0; iy < k->dlugosc_boku_maski; iy++) {
		for (int ix = 0; ix < k->dlugosc_boku_maski; ix++) {
			if (k->y+iy >= OCZKA_W_PIONIE ||
				k->x+ix >= OCZKA_W_POZIOMIE ||
				k->y+iy < 0 ||
				k->x+ix < 0) {
				continue;
			}
			int ik = k->maska[iy][ix];
			if (ik == 0) {
				continue;
			}
			czyscOczko(k->y+iy, k->x+ix);
		}
	}
}

static void rysujPlansze(unsigned int ile_rzedow) {
	for (int iy = 0; iy < ile_rzedow; iy++) {
		for (int ix = 0; ix < OCZKA_W_POZIOMIE; ix++) {
			int ik = plansza[iy][ix];
			if (ik == 0) {
				czyscOczko(iy, ix);
			} else {
				Paleta p = kolory_oczek[ik];
				rysujOczko(iy, ix, p);
			}
		}
	}
}

static int losujLiczbeNieujemna(void);

static void losujNowyRuchomyKlocek(void) {
	int irk = losujLiczbeNieujemna() % LICZBA_RODZAJOW_KLOCKOW;
	int ikk = (losujLiczbeNieujemna() % LICZBA_KOLOROW_OCZEK) + 1;

	BIEZACY_KLOCEK()->dlugosc_boku_maski = dlugosci_bokow_masek[irk];
	BIEZACY_KLOCEK()->x = (OCZKA_W_POZIOMIE - (BIEZACY_KLOCEK()->dlugosc_boku_maski & (~1))) / 2;
	BIEZACY_KLOCEK()->y = -BIEZACY_KLOCEK()->dlugosc_boku_maski;
	czas_przesuniecia += cooldown_na_przesuniecie;

	for (int iy = 0; iy < MAKSYMALNY_BOK_MASKI_KLOCKA; iy++) {
		for (int ix = 0; ix < MAKSYMALNY_BOK_MASKI_KLOCKA; ix++) {
			BIEZACY_KLOCEK()->maska[iy][ix] = maski_klockow[irk][iy][ix] * ikk;
		}
	}
}

static int czyRuchomyKlocekKoliduje(void) {
	for (int iy = 0; iy < BIEZACY_KLOCEK()->dlugosc_boku_maski; iy++) {
		for (int ix = 0; ix < BIEZACY_KLOCEK()->dlugosc_boku_maski; ix++) {
			if (BIEZACY_KLOCEK()->maska[iy][ix] &&
				(BIEZACY_KLOCEK()->x+ix >= OCZKA_W_POZIOMIE || BIEZACY_KLOCEK()->x+ix < 0 ||
				 BIEZACY_KLOCEK()->y+iy >= OCZKA_W_PIONIE ||
				 (BIEZACY_KLOCEK()->y+iy >= 0 && plansza[BIEZACY_KLOCEK()->y+iy][BIEZACY_KLOCEK()->x+ix]))) {
				return 1;
			}
		}
	}
	return 0;
}

static void obrocKlocekWPrawo() {
	for (int iy = 0; iy < BIEZACY_KLOCEK()->dlugosc_boku_maski; iy++) {
		for (int ix = 0; ix < BIEZACY_KLOCEK()->dlugosc_boku_maski; ix++) {
			BIEZACY_KLOCEK()->maska_tmp[iy][ix] = BIEZACY_KLOCEK()->maska[iy][ix];
		}
	}
	for (int iy = 0; iy < BIEZACY_KLOCEK()->dlugosc_boku_maski; iy++) {
		for (int ix = 0; ix < BIEZACY_KLOCEK()->dlugosc_boku_maski; ix++) {
			BIEZACY_KLOCEK()->maska[iy][ix] = BIEZACY_KLOCEK()->maska_tmp[BIEZACY_KLOCEK()->dlugosc_boku_maski-1-ix][iy];
		}
	}
}

static void obrocKlocekWLewo() {
	for (int iy = 0; iy < BIEZACY_KLOCEK()->dlugosc_boku_maski; iy++) {
		for (int ix = 0; ix < BIEZACY_KLOCEK()->dlugosc_boku_maski; ix++) {
			BIEZACY_KLOCEK()->maska_tmp[iy][ix] = BIEZACY_KLOCEK()->maska[iy][ix];
		}
	}
	for (int iy = 0; iy < BIEZACY_KLOCEK()->dlugosc_boku_maski; iy++) {
		for (int ix = 0; ix < BIEZACY_KLOCEK()->dlugosc_boku_maski; ix++) {
			BIEZACY_KLOCEK()->maska[iy][ix] = BIEZACY_KLOCEK()->maska_tmp[ix][BIEZACY_KLOCEK()->dlugosc_boku_maski-1-iy];
		}
	}
}

static void odwrocObrot() {
	for (int iy = 0; iy < BIEZACY_KLOCEK()->dlugosc_boku_maski; iy++) {
		for (int ix = 0; ix < BIEZACY_KLOCEK()->dlugosc_boku_maski; ix++) {
			BIEZACY_KLOCEK()->maska[iy][ix] = BIEZACY_KLOCEK()->maska_tmp[iy][ix];
		}
	}
}

static void aktualizujPlansze(void) {
	int liczba_wypelnionych_linii = 0;
	for (int iy = 0; iy < OCZKA_W_PIONIE; iy++) {
		int wypelniona = 1;
		for (int ix = 0; ix < OCZKA_W_POZIOMIE; ix++) {
			if (!plansza[iy][ix]) {
				wypelniona = 0;
				break;
			}
		}
		if (wypelniona) {
			liczba_wypelnionych_linii++;
			for (int jy = iy; jy > 0; jy--) {
				for (int jx = 0; jx < OCZKA_W_POZIOMIE; jx++) {
					plansza[jy][jx] = plansza[jy-1][jx];
				}
			}
			for (int jx = 0; jx < OCZKA_W_POZIOMIE; jx++) {
				plansza[0][jx] = 0;
			}
		}
	}
	if (liczba_wypelnionych_linii != 0) {
		licznik_punktow += liczba_wypelnionych_linii * liczba_wypelnionych_linii;
		rysujLicznikPunktow();
	}
	rysujPlansze(OCZKA_W_PIONIE);
}

static void aktualizujRuchomyKlocek(int czas_od_ostatniej_aktualizacji) {
	memcpy(POPRZEDNI_KLOCEK(), BIEZACY_KLOCEK(), sizeof(*BIEZACY_KLOCEK()));
	int narysuj_ponownie = 0;

	cooldown_na_obrot_w_prawo = MAX(0, cooldown_na_obrot_w_prawo-czas_od_ostatniej_aktualizacji);
	cooldown_na_obrot_w_lewo = MAX(0, cooldown_na_obrot_w_lewo-czas_od_ostatniej_aktualizacji);
	cooldown_na_ruch_w_prawo = MAX(0, cooldown_na_ruch_w_prawo-czas_od_ostatniej_aktualizacji);
	cooldown_na_ruch_w_lewo = MAX(0, cooldown_na_ruch_w_lewo-czas_od_ostatniej_aktualizacji);

	if (ruch_w_prawo && cooldown_na_ruch_w_prawo == 0) {
		BIEZACY_KLOCEK()->x++;
		int l_narysuj_ponownie = 1;
		if (czyRuchomyKlocekKoliduje()) {
			BIEZACY_KLOCEK()->x--;
			l_narysuj_ponownie = 0;
		} else {
			cooldown_na_ruch_w_prawo = COOLDOWN_NA_RUCH;
		}
		narysuj_ponownie |= l_narysuj_ponownie;
	}

	if (ruch_w_lewo && cooldown_na_ruch_w_lewo == 0) {
		BIEZACY_KLOCEK()->x--;
		int l_narysuj_ponownie = 1;
		if (czyRuchomyKlocekKoliduje()) {
			BIEZACY_KLOCEK()->x++;
			l_narysuj_ponownie = 0;
		} else {
			cooldown_na_ruch_w_lewo = COOLDOWN_NA_RUCH;
		}
		narysuj_ponownie |= l_narysuj_ponownie;
	}

	if (obrot_w_prawo && cooldown_na_obrot_w_prawo == 0) {
		obrocKlocekWPrawo();
		int l_narysuj_ponownie = 1;
		if (czyRuchomyKlocekKoliduje()) {
			odwrocObrot();
			l_narysuj_ponownie = 0;
		} else {
			cooldown_na_obrot_w_prawo = COOLDOWN_NA_OBROT;
		}
		narysuj_ponownie |= l_narysuj_ponownie;
	}

	if (obrot_w_lewo && cooldown_na_obrot_w_lewo == 0) {
		obrocKlocekWLewo();
		int l_narysuj_ponownie = 1;
		if (czyRuchomyKlocekKoliduje()) {
			odwrocObrot();
			l_narysuj_ponownie = 0;
		} else {
			cooldown_na_obrot_w_lewo = COOLDOWN_NA_OBROT;
		}
		narysuj_ponownie |= l_narysuj_ponownie;
	}

	if (ruch_w_dol) {
		czas_przesuniecia += czas_od_ostatniej_aktualizacji * MULTIPLIKATOR_RUCHU;
	} else {
		czas_przesuniecia += czas_od_ostatniej_aktualizacji;
	}

	while (czas_przesuniecia >= cooldown_na_przesuniecie) {
		czas_przesuniecia -= cooldown_na_przesuniecie;
		BIEZACY_KLOCEK()->y++;
		int l_narysuj_ponownie = 1;
		if (czyRuchomyKlocekKoliduje()) {
			BIEZACY_KLOCEK()->y--;
			if (BIEZACY_KLOCEK()->y <= 0) {
				gra_jest_przegrana = 1;
				rysujNapisyGameOver();
				return;
			}
			l_narysuj_ponownie = 0;
			for (int iy = 0; iy < BIEZACY_KLOCEK()->dlugosc_boku_maski; iy++) {
				for (int ix = 0; ix < BIEZACY_KLOCEK()->dlugosc_boku_maski; ix++) {
					if (!BIEZACY_KLOCEK()->maska[iy][ix]) {
						continue;
					}
					assert(!plansza[BIEZACY_KLOCEK()->y+iy][BIEZACY_KLOCEK()->x+ix]);
					plansza[BIEZACY_KLOCEK()->y+iy][BIEZACY_KLOCEK()->x+ix] = BIEZACY_KLOCEK()->maska[iy][ix];
				}
			}
			aktualizujPlansze();
			losujNowyRuchomyKlocek();
			memcpy(POPRZEDNI_KLOCEK(), BIEZACY_KLOCEK(), sizeof(*BIEZACY_KLOCEK()));
			rysujKlocek(BIEZACY_KLOCEK());
		}
		narysuj_ponownie |= l_narysuj_ponownie;
	}

	if (narysuj_ponownie) {
		czyscKlocek(POPRZEDNI_KLOCEK());
		rysujKlocek(BIEZACY_KLOCEK());
	}
}

static void inicjujRNG(void) {
	uint32_t seed;
	HAL_RNG_GenerateRandomNumber(&hrng, &seed);
	srand(seed);
}

static int losujLiczbeNieujemna(void) {
	return rand();
}

static int liczbaMilisekundOdStartuProgramu() {
	return HAL_GetTick();
}

static void czekaj(int ile_milisekund) {
	HAL_Delay(ile_milisekund);
}

/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{
  /* USER CODE BEGIN 1 */

  /* USER CODE END 1 */

  /* MCU Configuration--------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

  /* USER CODE BEGIN SysInit */

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_DMA2D_Init();
  MX_FMC_Init();
  MX_I2C3_Init();
  MX_LTDC_Init();
  MX_SPI5_Init();
  MX_ADC1_Init();
  MX_RNG_Init();
  MX_RTC_Init();
  MX_ADC2_Init();
  MX_TIM3_Init();
  /* USER CODE BEGIN 2 */

  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  	HAL_TIM_Base_Start_IT(&htim3);
  	HAL_TIM_PWM_Start(&htim3, TIM_CHANNEL_1);
  	__HAL_TIM_SET_AUTORELOAD(&htim3, 1);

	BSP_LCD_Init();
	BSP_LCD_LayerDefaultInit(LCD_BACKGROUND_LAYER, LCD_FRAME_BUFFER);
	BSP_LCD_SetBackColor(LCD_COLOR_BLACK);
	BSP_LCD_SetTextColor(LCD_COLOR_WHITE);
	BSP_LCD_SetFont(&Font16);
	BSP_LCD_Clear(LCD_COLOR_BLACK);

	int licznik_klatek_w_sekundzie = 0;
	int moment_poprzedniej_aktualizacji = 0;
	int moment_poprzedniej_sekundy = 0;
	int reszta_czasu_s16 = 0;

	inicjujRNG();
	losujNowyRuchomyKlocek();
	czekaj(500);
	rysujLicznikFPS(00);
	rysujLicznikPunktow();
	rysujLiniePionowa(160, 0, 320, J_SZARY);

	for (int i = 0; i < 10; i++) {
		int ii = (1 << (i>>1)) ^ (i & 1);
		wypelnijProstokat(64+4 + ii, 168 + ii, 320-64-8-ii*2, 80-14-ii*2, (i & 1) == 0 ? J_SZARY : C_SZARY);
	}

	moment_poprzedniej_aktualizacji = liczbaMilisekundOdStartuProgramu();
	moment_poprzedniej_sekundy = moment_poprzedniej_aktualizacji;

	int ostatni_moment_zmiany_stanu_muzyki = 0;
	uint32_t analogowe_pomiary[2];

	while (1) {
		int poczatek_obliczen_w_obecnej_klatce = liczbaMilisekundOdStartuProgramu();

		// odczyt sterowania
		HAL_ADC_Start(&hadc1);
		HAL_ADC_Start(&hadc2);
		HAL_ADC_PollForConversion(&hadc1, HAL_MAX_DELAY);
		HAL_ADC_PollForConversion(&hadc2, HAL_MAX_DELAY);
		analogowe_pomiary[0] = 3 * HAL_ADC_GetValue(&hadc1);
		analogowe_pomiary[1] = 3 * HAL_ADC_GetValue(&hadc2);
		HAL_ADC_Stop(&hadc1);
		HAL_ADC_Stop(&hadc2);

		if (analogowe_pomiary[0] < 3 || (analogowe_pomiary[0] < 30 && analogowe_pomiary[0] < analogowe_pomiary[1] / 50)) {
			ruch_w_lewo = 1;
		} else {
			ruch_w_lewo = 0;
		}

		if (analogowe_pomiary[0] > 500) {
			ruch_w_prawo = 1;
		} else {
			ruch_w_prawo = 0;
		}

		if (analogowe_pomiary[1] > 500) {
			ruch_w_dol = 1;
		} else {
			ruch_w_dol = 0;
		}

		if (HAL_GPIO_ReadPin(GPIOD, GPIO_PIN_11) == GPIO_PIN_SET &&
			poczatek_obliczen_w_obecnej_klatce - ostatni_moment_zmiany_stanu_muzyki >= 500) {
			muzyka_wlaczona = !muzyka_wlaczona;
			ostatni_moment_zmiany_stanu_muzyki = poczatek_obliczen_w_obecnej_klatce;
		}

		if (HAL_GPIO_ReadPin(GPIOD, GPIO_PIN_13) == GPIO_PIN_SET) {
			obrot_w_lewo = 1;
		} else {
			obrot_w_lewo = 0;
		}

		if (HAL_GPIO_ReadPin(GPIOD, GPIO_PIN_12) == GPIO_PIN_SET) {
			obrot_w_prawo = 1;
		} else {
			obrot_w_prawo = 0;
		}

		// aktualizacje rzadkie
		int moment_obecnej_aktualizacji = liczbaMilisekundOdStartuProgramu();
		int czas_od_ostatniej_aktualizacji = moment_obecnej_aktualizacji - moment_poprzedniej_aktualizacji;
		if (moment_obecnej_aktualizacji / INTERWAL_AKTUALIZACJI_RZADKICH > moment_poprzedniej_aktualizacji / INTERWAL_AKTUALIZACJI_RZADKICH) {
			cooldown_na_przesuniecie -= 2;
			int moment_obecnej_sekundy = moment_obecnej_aktualizacji;
			int czas_poprzedniej_sekundy = moment_obecnej_sekundy - moment_poprzedniej_sekundy;
			int l_licznik_klatek_w_sekundzie = (licznik_klatek_w_sekundzie * INTERWAL_AKTUALIZACJI_RZADKICH + INTERWAL_AKTUALIZACJI_RZADKICH/2) / czas_poprzedniej_sekundy;
			rysujLicznikFPS(l_licznik_klatek_w_sekundzie);
			rysujPozycjeGalek(analogowe_pomiary);
			rysujPlansze(LICZBA_LINII_TEKSTU);
			rysujKlocek(BIEZACY_KLOCEK());
			rysujLiniePionowa(160, 0, 320, J_SZARY);
			licznik_klatek_w_sekundzie = 0;
			moment_poprzedniej_sekundy = moment_obecnej_sekundy;
			if (gra_jest_przegrana) {
				char * ruchomy_napis = napisy_game_over[1];
				int n = strlen(ruchomy_napis) - 2;
				for (int i = n-1; i >= 0; i--) {
					ruchomy_napis[1 + i] = ruchomy_napis[1 + (i+n-1)%n];
				}
				rysujNapisyGameOver();
			}
		}
		licznik_klatek_w_sekundzie++;
		moment_poprzedniej_aktualizacji = moment_obecnej_aktualizacji;

		// aktualizuj i ewentualnie renderuj potrzebne elementy
		if (!gra_jest_przegrana) {
			aktualizujRuchomyKlocek(czas_od_ostatniej_aktualizacji);
		}

		// kontrola klatkazu
		int koniec_obliczen_w_obecnej_klatce = liczbaMilisekundOdStartuProgramu();
		int idealny_czas_klatki_s16 = (1000 << 16) / CEL_KLATKAZU;
		int czas_obliczen_w_obecnej_klatce_s16 = (koniec_obliczen_w_obecnej_klatce - poczatek_obliczen_w_obecnej_klatce) << 16;
		int czas_bezczynnosci_s16 = idealny_czas_klatki_s16 + reszta_czasu_s16 - czas_obliczen_w_obecnej_klatce_s16;
		if (czas_bezczynnosci_s16 > 0) {
			int czas_bezczynnosci = czas_bezczynnosci_s16 >> 16;
			czekaj(czas_bezczynnosci);
			reszta_czasu_s16 = czas_bezczynnosci_s16 - (czas_bezczynnosci << 16);
		} else {
			reszta_czasu_s16 = 0;
		}
	}
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
  /* USER CODE END 3 */
}

/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

  /** Configure the main internal regulator output voltage
  */
  __HAL_RCC_PWR_CLK_ENABLE();
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_LSI|RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.LSIState = RCC_LSI_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLM = 16;
  RCC_OscInitStruct.PLL.PLLN = 180;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;
  RCC_OscInitStruct.PLL.PLLQ = 8;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  /** Activate the Over-Drive mode
  */
  if (HAL_PWREx_EnableOverDrive() != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV4;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV2;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_5) != HAL_OK)
  {
    Error_Handler();
  }
}

/**
  * @brief ADC1 Initialization Function
  * @param None
  * @retval None
  */
static void MX_ADC1_Init(void)
{

  /* USER CODE BEGIN ADC1_Init 0 */

  /* USER CODE END ADC1_Init 0 */

  ADC_ChannelConfTypeDef sConfig = {0};

  /* USER CODE BEGIN ADC1_Init 1 */

  /* USER CODE END ADC1_Init 1 */

  /** Configure the global features of the ADC (Clock, Resolution, Data Alignment and number of conversion)
  */
  hadc1.Instance = ADC1;
  hadc1.Init.ClockPrescaler = ADC_CLOCK_SYNC_PCLK_DIV4;
  hadc1.Init.Resolution = ADC_RESOLUTION_10B;
  hadc1.Init.ScanConvMode = DISABLE;
  hadc1.Init.ContinuousConvMode = DISABLE;
  hadc1.Init.DiscontinuousConvMode = DISABLE;
  hadc1.Init.ExternalTrigConvEdge = ADC_EXTERNALTRIGCONVEDGE_NONE;
  hadc1.Init.ExternalTrigConv = ADC_SOFTWARE_START;
  hadc1.Init.DataAlign = ADC_DATAALIGN_RIGHT;
  hadc1.Init.NbrOfConversion = 1;
  hadc1.Init.DMAContinuousRequests = DISABLE;
  hadc1.Init.EOCSelection = ADC_EOC_SINGLE_CONV;
  if (HAL_ADC_Init(&hadc1) != HAL_OK)
  {
    Error_Handler();
  }

  /** Configure for the selected ADC regular channel its corresponding rank in the sequencer and its sample time.
  */
  sConfig.Channel = ADC_CHANNEL_1;
  sConfig.Rank = 1;
  sConfig.SamplingTime = ADC_SAMPLETIME_15CYCLES;
  if (HAL_ADC_ConfigChannel(&hadc1, &sConfig) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN ADC1_Init 2 */

  /* USER CODE END ADC1_Init 2 */

}

/**
  * @brief ADC2 Initialization Function
  * @param None
  * @retval None
  */
static void MX_ADC2_Init(void)
{

  /* USER CODE BEGIN ADC2_Init 0 */

  /* USER CODE END ADC2_Init 0 */

  ADC_ChannelConfTypeDef sConfig = {0};

  /* USER CODE BEGIN ADC2_Init 1 */

  /* USER CODE END ADC2_Init 1 */

  /** Configure the global features of the ADC (Clock, Resolution, Data Alignment and number of conversion)
  */
  hadc2.Instance = ADC2;
  hadc2.Init.ClockPrescaler = ADC_CLOCK_SYNC_PCLK_DIV4;
  hadc2.Init.Resolution = ADC_RESOLUTION_10B;
  hadc2.Init.ScanConvMode = DISABLE;
  hadc2.Init.ContinuousConvMode = DISABLE;
  hadc2.Init.DiscontinuousConvMode = DISABLE;
  hadc2.Init.ExternalTrigConvEdge = ADC_EXTERNALTRIGCONVEDGE_NONE;
  hadc2.Init.ExternalTrigConv = ADC_SOFTWARE_START;
  hadc2.Init.DataAlign = ADC_DATAALIGN_RIGHT;
  hadc2.Init.NbrOfConversion = 1;
  hadc2.Init.DMAContinuousRequests = DISABLE;
  hadc2.Init.EOCSelection = ADC_EOC_SINGLE_CONV;
  if (HAL_ADC_Init(&hadc2) != HAL_OK)
  {
    Error_Handler();
  }

  /** Configure for the selected ADC regular channel its corresponding rank in the sequencer and its sample time.
  */
  sConfig.Channel = ADC_CHANNEL_2;
  sConfig.Rank = 1;
  sConfig.SamplingTime = ADC_SAMPLETIME_15CYCLES;
  if (HAL_ADC_ConfigChannel(&hadc2, &sConfig) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN ADC2_Init 2 */

  /* USER CODE END ADC2_Init 2 */

}

/**
  * @brief DMA2D Initialization Function
  * @param None
  * @retval None
  */
static void MX_DMA2D_Init(void)
{

  /* USER CODE BEGIN DMA2D_Init 0 */

  /* USER CODE END DMA2D_Init 0 */

  /* USER CODE BEGIN DMA2D_Init 1 */

  /* USER CODE END DMA2D_Init 1 */
  hdma2d.Instance = DMA2D;
  hdma2d.Init.Mode = DMA2D_M2M;
  hdma2d.Init.ColorMode = DMA2D_OUTPUT_RGB565;
  hdma2d.Init.OutputOffset = 0;
  hdma2d.LayerCfg[1].InputOffset = 0;
  hdma2d.LayerCfg[1].InputColorMode = DMA2D_INPUT_RGB565;
  hdma2d.LayerCfg[1].AlphaMode = DMA2D_NO_MODIF_ALPHA;
  hdma2d.LayerCfg[1].InputAlpha = 0;
  if (HAL_DMA2D_Init(&hdma2d) != HAL_OK)
  {
    Error_Handler();
  }
  if (HAL_DMA2D_ConfigLayer(&hdma2d, 1) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN DMA2D_Init 2 */

  /* USER CODE END DMA2D_Init 2 */

}

/**
  * @brief I2C3 Initialization Function
  * @param None
  * @retval None
  */
static void MX_I2C3_Init(void)
{

  /* USER CODE BEGIN I2C3_Init 0 */

  /* USER CODE END I2C3_Init 0 */

  /* USER CODE BEGIN I2C3_Init 1 */

  /* USER CODE END I2C3_Init 1 */
  hi2c3.Instance = I2C3;
  hi2c3.Init.ClockSpeed = 100000;
  hi2c3.Init.DutyCycle = I2C_DUTYCYCLE_2;
  hi2c3.Init.OwnAddress1 = 0;
  hi2c3.Init.AddressingMode = I2C_ADDRESSINGMODE_7BIT;
  hi2c3.Init.DualAddressMode = I2C_DUALADDRESS_DISABLE;
  hi2c3.Init.OwnAddress2 = 0;
  hi2c3.Init.GeneralCallMode = I2C_GENERALCALL_DISABLE;
  hi2c3.Init.NoStretchMode = I2C_NOSTRETCH_DISABLE;
  if (HAL_I2C_Init(&hi2c3) != HAL_OK)
  {
    Error_Handler();
  }

  /** Configure Analogue filter
  */
  if (HAL_I2CEx_ConfigAnalogFilter(&hi2c3, I2C_ANALOGFILTER_ENABLE) != HAL_OK)
  {
    Error_Handler();
  }

  /** Configure Digital filter
  */
  if (HAL_I2CEx_ConfigDigitalFilter(&hi2c3, 0) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN I2C3_Init 2 */

  /* USER CODE END I2C3_Init 2 */

}

/**
  * @brief LTDC Initialization Function
  * @param None
  * @retval None
  */
static void MX_LTDC_Init(void)
{

  /* USER CODE BEGIN LTDC_Init 0 */

  /* USER CODE END LTDC_Init 0 */

  LTDC_LayerCfgTypeDef pLayerCfg = {0};
  LTDC_LayerCfgTypeDef pLayerCfg1 = {0};

  /* USER CODE BEGIN LTDC_Init 1 */

  /* USER CODE END LTDC_Init 1 */
  hltdc.Instance = LTDC;
  hltdc.Init.HSPolarity = LTDC_HSPOLARITY_AL;
  hltdc.Init.VSPolarity = LTDC_VSPOLARITY_AL;
  hltdc.Init.DEPolarity = LTDC_DEPOLARITY_AL;
  hltdc.Init.PCPolarity = LTDC_PCPOLARITY_IPC;
  hltdc.Init.HorizontalSync = 7;
  hltdc.Init.VerticalSync = 3;
  hltdc.Init.AccumulatedHBP = 14;
  hltdc.Init.AccumulatedVBP = 5;
  hltdc.Init.AccumulatedActiveW = 254;
  hltdc.Init.AccumulatedActiveH = 325;
  hltdc.Init.TotalWidth = 260;
  hltdc.Init.TotalHeigh = 327;
  hltdc.Init.Backcolor.Blue = 0;
  hltdc.Init.Backcolor.Green = 0;
  hltdc.Init.Backcolor.Red = 0;
  if (HAL_LTDC_Init(&hltdc) != HAL_OK)
  {
    Error_Handler();
  }
  pLayerCfg.WindowX0 = 0;
  pLayerCfg.WindowX1 = 0;
  pLayerCfg.WindowY0 = 0;
  pLayerCfg.WindowY1 = 0;
  pLayerCfg.PixelFormat = LTDC_PIXEL_FORMAT_ARGB8888;
  pLayerCfg.Alpha = 0;
  pLayerCfg.Alpha0 = 0;
  pLayerCfg.BlendingFactor1 = LTDC_BLENDING_FACTOR1_CA;
  pLayerCfg.BlendingFactor2 = LTDC_BLENDING_FACTOR2_CA;
  pLayerCfg.FBStartAdress = 0;
  pLayerCfg.ImageWidth = 0;
  pLayerCfg.ImageHeight = 0;
  pLayerCfg.Backcolor.Blue = 0;
  pLayerCfg.Backcolor.Green = 0;
  pLayerCfg.Backcolor.Red = 0;
  if (HAL_LTDC_ConfigLayer(&hltdc, &pLayerCfg, 0) != HAL_OK)
  {
    Error_Handler();
  }
  pLayerCfg1.WindowX0 = 0;
  pLayerCfg1.WindowX1 = 0;
  pLayerCfg1.WindowY0 = 0;
  pLayerCfg1.WindowY1 = 0;
  pLayerCfg1.PixelFormat = LTDC_PIXEL_FORMAT_ARGB8888;
  pLayerCfg1.Alpha = 0;
  pLayerCfg1.Alpha0 = 0;
  pLayerCfg1.BlendingFactor1 = LTDC_BLENDING_FACTOR1_CA;
  pLayerCfg1.BlendingFactor2 = LTDC_BLENDING_FACTOR2_CA;
  pLayerCfg1.FBStartAdress = 0;
  pLayerCfg1.ImageWidth = 0;
  pLayerCfg1.ImageHeight = 0;
  pLayerCfg1.Backcolor.Blue = 0;
  pLayerCfg1.Backcolor.Green = 0;
  pLayerCfg1.Backcolor.Red = 0;
  if (HAL_LTDC_ConfigLayer(&hltdc, &pLayerCfg1, 1) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN LTDC_Init 2 */

  /* USER CODE END LTDC_Init 2 */

}

/**
  * @brief RNG Initialization Function
  * @param None
  * @retval None
  */
static void MX_RNG_Init(void)
{

  /* USER CODE BEGIN RNG_Init 0 */

  /* USER CODE END RNG_Init 0 */

  /* USER CODE BEGIN RNG_Init 1 */

  /* USER CODE END RNG_Init 1 */
  hrng.Instance = RNG;
  if (HAL_RNG_Init(&hrng) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN RNG_Init 2 */

  /* USER CODE END RNG_Init 2 */

}

/**
  * @brief RTC Initialization Function
  * @param None
  * @retval None
  */
static void MX_RTC_Init(void)
{

  /* USER CODE BEGIN RTC_Init 0 */

  /* USER CODE END RTC_Init 0 */

  /* USER CODE BEGIN RTC_Init 1 */

  /* USER CODE END RTC_Init 1 */

  /** Initialize RTC Only
  */
  hrtc.Instance = RTC;
  hrtc.Init.HourFormat = RTC_HOURFORMAT_24;
  hrtc.Init.AsynchPrediv = 127;
  hrtc.Init.SynchPrediv = 255;
  hrtc.Init.OutPut = RTC_OUTPUT_DISABLE;
  hrtc.Init.OutPutPolarity = RTC_OUTPUT_POLARITY_HIGH;
  hrtc.Init.OutPutType = RTC_OUTPUT_TYPE_OPENDRAIN;
  if (HAL_RTC_Init(&hrtc) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN RTC_Init 2 */

  /* USER CODE END RTC_Init 2 */

}

/**
  * @brief SPI5 Initialization Function
  * @param None
  * @retval None
  */
static void MX_SPI5_Init(void)
{

  /* USER CODE BEGIN SPI5_Init 0 */

  /* USER CODE END SPI5_Init 0 */

  /* USER CODE BEGIN SPI5_Init 1 */

  /* USER CODE END SPI5_Init 1 */
  /* SPI5 parameter configuration*/
  hspi5.Instance = SPI5;
  hspi5.Init.Mode = SPI_MODE_MASTER;
  hspi5.Init.Direction = SPI_DIRECTION_2LINES;
  hspi5.Init.DataSize = SPI_DATASIZE_8BIT;
  hspi5.Init.CLKPolarity = SPI_POLARITY_LOW;
  hspi5.Init.CLKPhase = SPI_PHASE_1EDGE;
  hspi5.Init.NSS = SPI_NSS_SOFT;
  hspi5.Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_2;
  hspi5.Init.FirstBit = SPI_FIRSTBIT_MSB;
  hspi5.Init.TIMode = SPI_TIMODE_DISABLE;
  hspi5.Init.CRCCalculation = SPI_CRCCALCULATION_DISABLE;
  hspi5.Init.CRCPolynomial = 10;
  if (HAL_SPI_Init(&hspi5) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN SPI5_Init 2 */

  /* USER CODE END SPI5_Init 2 */

}

/**
  * @brief TIM3 Initialization Function
  * @param None
  * @retval None
  */
static void MX_TIM3_Init(void)
{

  /* USER CODE BEGIN TIM3_Init 0 */

  /* USER CODE END TIM3_Init 0 */

  TIM_ClockConfigTypeDef sClockSourceConfig = {0};
  TIM_MasterConfigTypeDef sMasterConfig = {0};
  TIM_OC_InitTypeDef sConfigOC = {0};

  /* USER CODE BEGIN TIM3_Init 1 */

  /* USER CODE END TIM3_Init 1 */
  htim3.Instance = TIM3;
  htim3.Init.Prescaler = 2224;
  htim3.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim3.Init.Period = 999;
  htim3.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  htim3.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
  if (HAL_TIM_Base_Init(&htim3) != HAL_OK)
  {
    Error_Handler();
  }
  sClockSourceConfig.ClockSource = TIM_CLOCKSOURCE_INTERNAL;
  if (HAL_TIM_ConfigClockSource(&htim3, &sClockSourceConfig) != HAL_OK)
  {
    Error_Handler();
  }
  if (HAL_TIM_PWM_Init(&htim3) != HAL_OK)
  {
    Error_Handler();
  }
  sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
  sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
  if (HAL_TIMEx_MasterConfigSynchronization(&htim3, &sMasterConfig) != HAL_OK)
  {
    Error_Handler();
  }
  sConfigOC.OCMode = TIM_OCMODE_PWM1;
  sConfigOC.Pulse = 499;
  sConfigOC.OCPolarity = TIM_OCPOLARITY_LOW;
  sConfigOC.OCFastMode = TIM_OCFAST_DISABLE;
  if (HAL_TIM_PWM_ConfigChannel(&htim3, &sConfigOC, TIM_CHANNEL_1) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN TIM3_Init 2 */

  /* USER CODE END TIM3_Init 2 */
  HAL_TIM_MspPostInit(&htim3);

}

/* FMC initialization function */
static void MX_FMC_Init(void)
{

  /* USER CODE BEGIN FMC_Init 0 */

  /* USER CODE END FMC_Init 0 */

  FMC_SDRAM_TimingTypeDef SdramTiming = {0};

  /* USER CODE BEGIN FMC_Init 1 */

  /* USER CODE END FMC_Init 1 */

  /** Perform the SDRAM1 memory initialization sequence
  */
  hsdram1.Instance = FMC_SDRAM_DEVICE;
  /* hsdram1.Init */
  hsdram1.Init.SDBank = FMC_SDRAM_BANK2;
  hsdram1.Init.ColumnBitsNumber = FMC_SDRAM_COLUMN_BITS_NUM_8;
  hsdram1.Init.RowBitsNumber = FMC_SDRAM_ROW_BITS_NUM_12;
  hsdram1.Init.MemoryDataWidth = FMC_SDRAM_MEM_BUS_WIDTH_16;
  hsdram1.Init.InternalBankNumber = FMC_SDRAM_INTERN_BANKS_NUM_4;
  hsdram1.Init.CASLatency = FMC_SDRAM_CAS_LATENCY_1;
  hsdram1.Init.WriteProtection = FMC_SDRAM_WRITE_PROTECTION_DISABLE;
  hsdram1.Init.SDClockPeriod = FMC_SDRAM_CLOCK_DISABLE;
  hsdram1.Init.ReadBurst = FMC_SDRAM_RBURST_DISABLE;
  hsdram1.Init.ReadPipeDelay = FMC_SDRAM_RPIPE_DELAY_0;
  /* SdramTiming */
  SdramTiming.LoadToActiveDelay = 16;
  SdramTiming.ExitSelfRefreshDelay = 16;
  SdramTiming.SelfRefreshTime = 16;
  SdramTiming.RowCycleDelay = 16;
  SdramTiming.WriteRecoveryTime = 16;
  SdramTiming.RPDelay = 16;
  SdramTiming.RCDDelay = 16;

  if (HAL_SDRAM_Init(&hsdram1, &SdramTiming) != HAL_OK)
  {
    Error_Handler( );
  }

  /* USER CODE BEGIN FMC_Init 2 */

  /* USER CODE END FMC_Init 2 */
}

/**
  * @brief GPIO Initialization Function
  * @param None
  * @retval None
  */
static void MX_GPIO_Init(void)
{
  GPIO_InitTypeDef GPIO_InitStruct = {0};
/* USER CODE BEGIN MX_GPIO_Init_1 */
/* USER CODE END MX_GPIO_Init_1 */

  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOF_CLK_ENABLE();
  __HAL_RCC_GPIOH_CLK_ENABLE();
  __HAL_RCC_GPIOC_CLK_ENABLE();
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();
  __HAL_RCC_GPIOG_CLK_ENABLE();
  __HAL_RCC_GPIOE_CLK_ENABLE();
  __HAL_RCC_GPIOD_CLK_ENABLE();

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOG, GPIO_PIN_13|GPIO_PIN_14, GPIO_PIN_RESET);

  /*Configure GPIO pins : PD11 PD12 PD13 */
  GPIO_InitStruct.Pin = GPIO_PIN_11|GPIO_PIN_12|GPIO_PIN_13;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(GPIOD, &GPIO_InitStruct);

  /*Configure GPIO pins : PG13 PG14 */
  GPIO_InitStruct.Pin = GPIO_PIN_13|GPIO_PIN_14;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOG, &GPIO_InitStruct);

/* USER CODE BEGIN MX_GPIO_Init_2 */
/* USER CODE END MX_GPIO_Init_2 */
}

/* USER CODE BEGIN 4 */

/*
// Called when first half of buffer is filled
void HAL_ADC_ConvHalfCpltCallback(ADC_HandleTypeDef* hadc) {
  HAL_GPIO_WritePin(LD2_GPIO_Port, LD2_Pin, GPIO_PIN_SET);
}

// Called when buffer is completely filled
void HAL_ADC_ConvCpltCallback(ADC_HandleTypeDef* hadc) {
  HAL_GPIO_WritePin(LD2_GPIO_Port, LD2_Pin, GPIO_PIN_RESET);
}
*/

/* USER CODE END 4 */

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */
  __disable_irq();
  while (1)
  {
  }
  /* USER CODE END Error_Handler_Debug */
}

#ifdef  USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t *file, uint32_t line)
{
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */
