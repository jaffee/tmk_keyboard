/*
Copyright 2012 Jun Wako <wakojun@gmail.com>

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

#include "ch.h"
#include "hal.h"
#include "timer.h"
/*
 * scan matrix
 */
#include "print.h"
#include "debug.h"
#include "util.h"
#include "matrix.h"
#include "wait.h"

#ifndef DEBOUNCE
#   define DEBOUNCE 5
#endif
static uint8_t debouncing = DEBOUNCE;
/* matrix state(1:on, 0:off) */
static matrix_row_t matrix[MATRIX_ROWS];
static matrix_row_t matrix_debouncing[MATRIX_ROWS];

static matrix_row_t read_cols(void);
static void init_cols(void);
static void unselect_row(uint8_t row);
static void select_row(uint8_t row);


inline
uint8_t matrix_rows(void)
{
    return MATRIX_ROWS;
}

inline
uint8_t matrix_cols(void)
{
    return MATRIX_COLS;
}

#define LED_ON()    do { palSetPad(TEENSY_PIN13_IOPORT, TEENSY_PIN13) ;} while (0)
#define LED_OFF()   do { palClearPad(TEENSY_PIN13_IOPORT, TEENSY_PIN13); } while (0)
#define LED_TGL()   do { palTogglePad(TEENSY_PIN13_IOPORT, TEENSY_PIN13); } while (0)

void matrix_init(void)
{
    // initialize row and col
    for (uint8_t i = 0; i < MATRIX_ROWS; i++) {
      unselect_row(i);
    }
    init_cols();

    // initialize matrix state: all keys off
    for (uint8_t i=0; i < MATRIX_ROWS; i++) {
        matrix[i] = 0;
        matrix_debouncing[i] = 0;
    }

    debug_enable = 1;

    //debug
    debug_matrix = true;
    LED_ON();
    wait_ms(500);
    LED_OFF();
    timer_init();
}

uint8_t matrix_scan(void)
{

  uint32_t last = timer_read32();
  // dprintf("start time: %d\n", last);
    for (uint8_t i = 0; i < MATRIX_ROWS; i++) {
        select_row(i);
        wait_us(20);  // without this wait read unstable value.
        matrix_row_t cols = read_cols();
        matrix[i] = cols;
        /* if (matrix_debouncing[i] != cols) { */
        /*     matrix_debouncing[i] = cols; */
        /*     if (debouncing) { */
        /*       dprintf("bounce!: %d", debouncing); dprint("\n"); */
        /*     } */
        /*     debouncing = DEBOUNCE; */
        /* } */
        unselect_row(i);
    }

    /* if (debouncing) { */
    /*     if (--debouncing) { */
    /*       dprintf("wait for debounce 1ms"); */
    /*         wait_ms(1); */
    /*     } else { */
    /*         for (uint8_t i = 0; i < MATRIX_ROWS; i++) { */
    /*             matrix[i] = matrix_debouncing[i]; */
    /*         } */
    /*     } */
    /* } */
    // dprintf("elapsed time: %d\n", timer_elapsed32(last));
    return 1;
}

inline
bool matrix_is_on(uint8_t row, uint8_t col)
{
    return (matrix[row] & ((matrix_row_t)1<<col));
}

inline
matrix_row_t matrix_get_row(uint8_t row)
{
    return matrix[row];
}

void matrix_print(void)
{
    print("\nr/c 0123456789ABCDEF\n");
    for (uint8_t row = 0; row < MATRIX_ROWS; row++) {
        phex(row); print(": ");
        pbin_reverse16(matrix_get_row(row));
        print("\n");
    }
}

/* Column pin configuration
 */
static void  init_cols(void)
{
    // internal pull-up
  palSetPadMode(TEENSY_PIN11_IOPORT, TEENSY_PIN11, PAL_MODE_INPUT_PULLUP);
  palSetPadMode(TEENSY_PIN14_IOPORT, TEENSY_PIN14, PAL_MODE_INPUT_PULLUP);
  palSetPadMode(TEENSY_PIN15_IOPORT, TEENSY_PIN15, PAL_MODE_INPUT_PULLUP);
  palSetPadMode(TEENSY_PIN18_IOPORT, TEENSY_PIN18, PAL_MODE_INPUT_PULLUP);
  palSetPadMode(TEENSY_PIN19_IOPORT, TEENSY_PIN19, PAL_MODE_INPUT_PULLUP);
  palSetPadMode(TEENSY_PIN20_IOPORT, TEENSY_PIN20, PAL_MODE_INPUT_PULLUP);
  palSetPadMode(TEENSY_PIN23_IOPORT, TEENSY_PIN23, PAL_MODE_INPUT_PULLUP);
  palSetPadMode(TEENSY_PIN33_IOPORT, TEENSY_PIN33, PAL_MODE_INPUT_PULLUP);
}

/* Returns status of switches(1:on, 0:off) */
static matrix_row_t read_cols(void)
{
  return ((palReadPad(TEENSY_PIN11_IOPORT, TEENSY_PIN11)==PAL_HIGH) ? 0 : (1<<0)) |
    ((palReadPad(TEENSY_PIN14_IOPORT, TEENSY_PIN14)==PAL_HIGH) ? 0 : (1<<1)) |
    ((palReadPad(TEENSY_PIN15_IOPORT, TEENSY_PIN15)==PAL_HIGH) ? 0 : (1<<2)) |
    ((palReadPad(TEENSY_PIN18_IOPORT, TEENSY_PIN18)==PAL_HIGH) ? 0 : (1<<3)) |
    ((palReadPad(TEENSY_PIN19_IOPORT, TEENSY_PIN19)==PAL_HIGH) ? 0 : (1<<4)) |
    ((palReadPad(TEENSY_PIN20_IOPORT, TEENSY_PIN20)==PAL_HIGH) ? 0 : (1<<5)) |
    ((palReadPad(TEENSY_PIN23_IOPORT, TEENSY_PIN23)==PAL_HIGH) ? 0 : (1<<6)) |
    ((palReadPad(TEENSY_PIN33_IOPORT, TEENSY_PIN33)==PAL_HIGH) ? 0 : (1<<7));
    //    | ((palReadPad(...)==PAL_HIGH) ? 0 : (1<<1))
}

/* Row pin configuration
 */
static void unselect_row(uint8_t row)
{
  (void)row;
  switch(row) {
  case 0:
    palSetPadMode(TEENSY_PIN1_IOPORT, TEENSY_PIN1, PAL_MODE_INPUT); break; // hi-Z
  case 1:
    palSetPadMode(TEENSY_PIN2_IOPORT, TEENSY_PIN2, PAL_MODE_INPUT); break; // hi-Z
  case 2:
    palSetPadMode(TEENSY_PIN3_IOPORT, TEENSY_PIN3, PAL_MODE_INPUT); break; // hi-Z
  case 3:
    palSetPadMode(TEENSY_PIN4_IOPORT, TEENSY_PIN4, PAL_MODE_INPUT); break; // hi-Z
  case 4:
    palSetPadMode(TEENSY_PIN5_IOPORT, TEENSY_PIN5, PAL_MODE_INPUT); break; // hi-Z
  case 5:
    palSetPadMode(TEENSY_PIN6_IOPORT, TEENSY_PIN6, PAL_MODE_INPUT); break; // hi-Z
  case 6:
    palSetPadMode(TEENSY_PIN7_IOPORT, TEENSY_PIN7, PAL_MODE_INPUT); break; // hi-Z
  case 7:
    palSetPadMode(TEENSY_PIN8_IOPORT, TEENSY_PIN8, PAL_MODE_INPUT); break; // hi-Z
  case 8:
    palSetPadMode(TEENSY_PIN9_IOPORT, TEENSY_PIN9, PAL_MODE_INPUT); break; // hi-Z
  case 9:
    palSetPadMode(TEENSY_PIN10_IOPORT, TEENSY_PIN10, PAL_MODE_INPUT); break; // hi-Z
  case 10:
    palSetPadMode(TEENSY_PIN16_IOPORT, TEENSY_PIN16, PAL_MODE_INPUT); break; // hi-Z
  case 11:
    palSetPadMode(TEENSY_PIN17_IOPORT, TEENSY_PIN17, PAL_MODE_INPUT); break; // hi-Z
  case 12:
    palSetPadMode(TEENSY_PIN21_IOPORT, TEENSY_PIN21, PAL_MODE_INPUT); break; // hi-Z
  case 13:
    palSetPadMode(TEENSY_PIN22_IOPORT, TEENSY_PIN22, PAL_MODE_INPUT); break; // hi-Z
  case 14:
    palSetPadMode(TEENSY_PIN29_IOPORT, TEENSY_PIN29, PAL_MODE_INPUT); break; // hi-Z
  case 15:
    palSetPadMode(TEENSY_PIN30_IOPORT, TEENSY_PIN30, PAL_MODE_INPUT); break; // hi-Z
  case 16:
    palSetPadMode(TEENSY_PIN31_IOPORT, TEENSY_PIN31, PAL_MODE_INPUT); break; // hi-Z
  case 17:
    palSetPadMode(TEENSY_PIN32_IOPORT, TEENSY_PIN32, PAL_MODE_INPUT); break; // hi-Z
  }
}

static void select_row(uint8_t row)
{
    (void)row;
    // Output low to select
    switch (row) {
    case 0:
      palSetPadMode(TEENSY_PIN1_IOPORT, TEENSY_PIN1, PAL_MODE_OUTPUT_PUSHPULL);
      palClearPad(TEENSY_PIN1_IOPORT, TEENSY_PIN1);
      break;
    case 1:
      palSetPadMode(TEENSY_PIN2_IOPORT, TEENSY_PIN2, PAL_MODE_OUTPUT_PUSHPULL);
      palClearPad(TEENSY_PIN2_IOPORT, TEENSY_PIN2);
      break;
    case 2:
      palSetPadMode(TEENSY_PIN3_IOPORT, TEENSY_PIN3, PAL_MODE_OUTPUT_PUSHPULL);
      palClearPad(TEENSY_PIN3_IOPORT, TEENSY_PIN3);
      break;
    case 3:
      palSetPadMode(TEENSY_PIN4_IOPORT, TEENSY_PIN4, PAL_MODE_OUTPUT_PUSHPULL);
      palClearPad(TEENSY_PIN4_IOPORT, TEENSY_PIN4);
      break;
    case 4:
      palSetPadMode(TEENSY_PIN5_IOPORT, TEENSY_PIN5, PAL_MODE_OUTPUT_PUSHPULL);
      palClearPad(TEENSY_PIN5_IOPORT, TEENSY_PIN5);
      break;
    case 5:
      palSetPadMode(TEENSY_PIN6_IOPORT, TEENSY_PIN6, PAL_MODE_OUTPUT_PUSHPULL);
      palClearPad(TEENSY_PIN6_IOPORT, TEENSY_PIN6);
      break;
    case 6:
      palSetPadMode(TEENSY_PIN7_IOPORT, TEENSY_PIN7, PAL_MODE_OUTPUT_PUSHPULL);
      palClearPad(TEENSY_PIN7_IOPORT, TEENSY_PIN7);
      break;
    case 7:
      palSetPadMode(TEENSY_PIN8_IOPORT, TEENSY_PIN8, PAL_MODE_OUTPUT_PUSHPULL);
      palClearPad(TEENSY_PIN8_IOPORT, TEENSY_PIN8);
      break;
    case 8:
      palSetPadMode(TEENSY_PIN9_IOPORT, TEENSY_PIN9, PAL_MODE_OUTPUT_PUSHPULL);
      palClearPad(TEENSY_PIN9_IOPORT, TEENSY_PIN9);
      break;
    case 9:
      palSetPadMode(TEENSY_PIN10_IOPORT, TEENSY_PIN10, PAL_MODE_OUTPUT_PUSHPULL);
      palClearPad(TEENSY_PIN10_IOPORT, TEENSY_PIN10);
      break;
    case 10:
      palSetPadMode(TEENSY_PIN16_IOPORT, TEENSY_PIN16, PAL_MODE_OUTPUT_PUSHPULL);
      palClearPad(TEENSY_PIN16_IOPORT, TEENSY_PIN16);
      break;
    case 11:
      palSetPadMode(TEENSY_PIN17_IOPORT, TEENSY_PIN17, PAL_MODE_OUTPUT_PUSHPULL);
      palClearPad(TEENSY_PIN17_IOPORT, TEENSY_PIN17);
      break;
    case 12:
      palSetPadMode(TEENSY_PIN21_IOPORT, TEENSY_PIN21, PAL_MODE_OUTPUT_PUSHPULL);
      palClearPad(TEENSY_PIN21_IOPORT, TEENSY_PIN21);
      break;
    case 13:
      palSetPadMode(TEENSY_PIN22_IOPORT, TEENSY_PIN22, PAL_MODE_OUTPUT_PUSHPULL);
      palClearPad(TEENSY_PIN22_IOPORT, TEENSY_PIN22);
      break;
    case 14:
      palSetPadMode(TEENSY_PIN29_IOPORT, TEENSY_PIN29, PAL_MODE_OUTPUT_PUSHPULL);
      palClearPad(TEENSY_PIN29_IOPORT, TEENSY_PIN29);
      break;
    case 15:
      palSetPadMode(TEENSY_PIN30_IOPORT, TEENSY_PIN30, PAL_MODE_OUTPUT_PUSHPULL);
      palClearPad(TEENSY_PIN30_IOPORT, TEENSY_PIN30);
      break;
    case 16:
      palSetPadMode(TEENSY_PIN31_IOPORT, TEENSY_PIN31, PAL_MODE_OUTPUT_PUSHPULL);
      palClearPad(TEENSY_PIN31_IOPORT, TEENSY_PIN31);
      break;
    case 17:
      palSetPadMode(TEENSY_PIN32_IOPORT, TEENSY_PIN32, PAL_MODE_OUTPUT_PUSHPULL);
      palClearPad(TEENSY_PIN32_IOPORT, TEENSY_PIN32);
      break;
    }
}
