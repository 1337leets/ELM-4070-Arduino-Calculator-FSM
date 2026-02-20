// SPDX-License-Identifier: Apache-2.0
// Copyright (c) 2026 Kerem Önol
// Copyright (c) 2026 Samet Erhan Sarı

// Libs __________________________________________________________________________________________________________________________________________________________________
#include <string.h>
#include <Wire.h>
#include <Keypad.h>
#include <LiquidCrystal_I2C.h>
#include <math.h>
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


// Object Declarations ___________________________________________________________________________________________________________________________________________________
const byte rp[] = { 9, 8, 7, 6 }; const byte cp[] = { 5, 4, 3, 2 }; // Statically allocates arrays of the row & col pin numbers (required by the 'Keypad' function)
char keys[4][4] = { { '1', '2', '3', '/' }, { '4', '5', '6', '*' }, { '7', '8', '9', '-' }, { 'C', '0', '=', '+' } }; // Key-Char assignment
Keypad keypad = Keypad(makeKeymap(keys), rp, cp, 4, 4); // Creates the 'keypad' object
LiquidCrystal_I2C lcd(0x27, 16, 2); // Creates the 'lcd' object
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


// Math Core ___________________________________________________________________________________________________________________________________________________________
constexpr double div(double a, double b) { return b ? a / b : NAN; }
constexpr double mul(double a, double b) { return a * b; }
constexpr double sub(double a, double b) { return a - b; }
constexpr double add(double a, double b) { return a + b; }
constexpr double calc(double a, double b, double (*op)(double, double)) { return op(a, b); } // Passes two numbers into a specified math function
double (*func[128])(double, double) = { 0 }; // Lookup table of 128 function pointers (ASCII characters will point to the math functions)
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


// Logic Setup ___________________________________________________________________________________________________________________________________________________________
void setup() {
  lcd.init(); lcd.backlight(); // Initializes the LCD screen and lights up the LCD screen
  func['/'] = div; func['*'] = mul; func['-'] = sub; func['+'] = add; // Assigns chars to functions
}
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


// Global Vars ___________________________________________________________________________________________________________________________________________________________
char expr[64]; // Declares the char arr that will hold the math expression in a C-String
uint8_t loc = 0; // Declares the 8-bit variable that will keep track of the current element of 'expr' where new key presses will be applied to
char ans[32]; // Declares the char arr that will hold to answer in a C-String
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


// Parse Expression ______________________________________________________________________________________________________________________________________________________
double par(char *expr) {
  double nums[32], val; char ops[32], *q, *s = expr; int numi=0, opi=0, neg; // Declares the local vars

  while (*expr) {
    neg = (*expr == '-' && (expr == s || strchr("*/+-", expr[-1]))); // Detects the presence of unary minus (1 if negative, 0 if positive)
    val = strtod(expr + neg, &q) * (neg ? -1 : 1); expr = q; // Parses the number and applies unary minus if needed, then advances the expression ptr

    while (opi && (ops[opi - 1] == '*' || ops[opi - 1] == '/' || !(*expr == '*' || *expr == '/'))) val = calc(nums[--numi], val, func[(int)ops[--opi]]); // Calc exec
    if (*expr) ops[opi++] = *expr++, nums[numi++] = val; // If expr isn't over yet, pushes curr op, advances expr ptr, and pushes curr val onto the number stack
  }
  return val; // Returns the result
}
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


// Compose Display _______________________________________________________________________________________________________________________________________________________
void comp(double res, char ans[]) {
  (res && (fabs(res) <= 1e-6 || fabs(res) >= 1e7)) ? dtostre(res, ans, 6, 0) : dtostrf(res, 0, fabs(res) >= 1 ? 6 - (int)log10(fabs(res)) : 7, ans); // dtostr / dtostrf
  
  if (*ans == ' ') memmove(ans, ans + 1, strlen(ans) + 1); // Trims the extra space reserved for the sign
  if (char *e = strchr(ans, 'e')) while (e[2] == '0' && strlen(e + 2) > 1) memmove(e + 2, e + 3, strlen(e + 3) + 1); // Trims the leading zeros in the exponent

  char *e = strchr(ans, 'e'), *t= e ? e : strchr(ans, 0), *q = t; // Determines where the trimming should begin in the mantissa
  if (strchr(ans, '.')) { while (q > ans && *(q - 1) == '0') --q; if (*(q - 1) == '.') --q; } // Trims the trailing zeros in the mantissa
  memmove(q, t, strlen(t) + 1); // Shifts the exponent to avoid spaces
}
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


// Print Expression & Answer _____________________________________________________________________________________________________________________________________________
void prx(char expr[]) { expr[loc] = 0; lcd.clear(); if (loc <= 16) lcd.print(expr); else lcd.print(expr + (loc - 16)); } // Completes the C-String and fits expr to screen
void pra(char ans[]) { lcd.setCursor(16 - strlen(ans), 1); lcd.print(ans); } // Anchors ans to the bottom-right of the screen
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


// Logic _________________________________________________________________________________________________________________________________________________________________
void loop() {
  char key = keypad.waitForKey(); // Pauses the exec flow until a key is pressed

  // Hit: Number
  if (!strchr("/*-+=C", key)) { // Checks if the key pressed is something other than "/*-+=C", meaning it's a number (0123456789)
    loc -= (expr[loc - 1] == '0' && (loc < 2 || strchr("/*-+", expr[loc - 2]))); // Manipulates loc to have the new numbers replace the leading 0
    expr[loc++] = key; prx(expr); return; // Extends the C-String expr holds, then increments the tracked index, updates screen and early exits
  }

  // Hit: All Clear
  if (key == 'C') { // Checks if the key pressed is All Clear
    loc = *expr = *ans = 0; lcd.clear(); return; // Resets loc, discards ans and expr by terminating at the start (making them empty), clears the screen and early exits
  }

  // Hit: Calculate
  if (key == '=') { // Checks if the key pressed is Calculate
    if (*ans && (!loc || strchr("*/+-", expr[loc - 1]))) { // Checks if ans is valid and if either loc is 0 or expr ends with an op
    int s = (*ans == '-' && loc && strchr("+-", expr[loc - 1])) ? (expr[loc - 1] == '-' ? (expr[loc - 1] = '+', 1) : (expr[--loc] = 0, 0)) : 0; // Handles sign
    loc = strlen(strcat(expr, ans + s)); prx(expr); return; // Extends the C-String expr holds with the latest valid answer, updates screen and early exits
    }
    while (loc && strchr("/*-+", expr[loc - 1])) expr[--loc] = 0, prx(expr); // Trims the trailing operators if any, and updates screen
    if (loc) { comp(par(expr), ans); pra(ans); loc = *expr = 0; if (ans == 'N') *ans = 0; } // Fills ans, updates screen, resets loc, discards expr, discards ans if NAN
    return; // Early exits
  }

  // Hit: Operator
  if (!expr[loc - 2] && expr[loc - 1] == '-') return; if (!loc && key != '-') expr[loc++] = '0'; // Auto-adds '0' if the first key press is an op, unless it's '-'
  if (strchr("/*-+", expr[loc - 1]) && !(strchr("/*", expr[loc - 1]) && key == '-')) loc--; expr[loc++] = key; prx(expr); // Extends C-String or replaces prev op
  if (strchr("/*", expr[loc - 2]) && key != '-') expr[loc - 2] = expr[loc - 1], loc--, prx(expr); // Replaces "/-" or "*-" with the new op
}
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

