#pragma once
#include <stdio.h>

// Define the lookup table for the characters in a 6x8 bitmap font
#define FONT_BITMAP "ASSETS/white.bmp"		 // Standard font bitmap
#define FONT_BITMAP_REV "ASSETS/black.bmp"	// Inverted bitmap

typedef struct CHAR_SYM {
	char sym;			// The alphanumeric symbol that this bitmap represents
	unsigned int x;		// x-offset into the bitmap by x * FONT_W pixels
	unsigned int y; 	// y-offset into the bitmap by x * FONT_H pixels
} CHAR_SYM;

// 32 * 6px wide
// 8 * 8px high
static CHAR_SYM CHAR_LIST[] = {

	// This array defines the coordinates into the bitmap file for the individual characters for our font.
	// Char, x-index, y-index
	{ ' ', 0, 0 }, // Space
	{ '@', 0, 2 }, // @
	{ 'A', 1, 2 }, // A
	{ 'B', 2, 2 }, // B
	{ 'C', 3, 2 }, // C
	{ 'D', 4, 2 }, // D
	{ 'E', 5, 2 }, // E
	{ 'F', 6, 2 }, // F
	{ 'G', 7, 2 }, // G
	{ 'H', 8, 2 }, // H
	{ 'I', 9, 2 }, // I
	{ 'J', 10, 2 }, // J
	{ 'K', 11, 2 }, // K
	{ 'L', 12, 2 }, // L
	{ 'M', 13, 2 }, // M
	{ 'N', 14, 2 }, // N
	{ 'O', 15, 2 }, // O
	{ 'P', 16, 2 }, // P
	{ 'Q', 17, 2 }, // Q
	{ 'R', 18, 2 }, // R
	{ 'S', 19, 2 }, // S
	{ 'T', 20, 2 }, // T
	{ 'U', 21, 2 }, // U
	{ 'V', 22, 2 }, // V
	{ 'W', 23, 2 }, // W
	{ 'X', 24, 2 }, // X
	{ 'Y', 25, 2 }, // Y
	{ 'Z', 26, 2 }, // Z
	{ '[', 27, 2 }, // [
	{ '\\', 28, 2 }, // Backslash
	{ ']', 29, 2 }, // ]
	{ '^', 30, 2 }, // ^
	{ '_', 31, 2 }, // _ Large underscore
	{ '`', 0, 3 }, // Right single quote/ "99"
	{ 'a', 1, 3 }, // a
	{ 'b', 2, 3 }, // b
	{ 'c', 3, 3 }, // c
	{ 'd', 4, 3 }, // d
	{ 'e', 5, 3 }, // e
	{ 'f', 6, 3 }, // f
	{ 'g', 7, 3 }, // g
	{ 'h', 8, 3 }, // h
	{ 'i', 9, 3 }, // i
	{ 'j', 10, 3 }, // j
	{ 'k', 11, 3 }, // k
	{ 'l', 12, 3 }, // l
	{ 'm', 13, 3 }, // m
	{ 'n', 14, 3 }, // n
	{ 'o', 15, 3 }, // o
	{ 'p', 16, 3 }, // p
	{ 'q', 17, 3 }, // q
	{ 'r', 18, 3 }, // r
	{ 's', 19, 3 }, // s
	{ 't', 20, 3 }, // t
	{ 'u', 21, 3 }, // u
	{ 'v', 22, 3 }, // v
	{ 'w', 23, 3 }, // w
	{ 'x', 24, 3 }, // x
	{ 'y', 25, 3 }, // y
	{ 'z', 26, 3 }, // z
	{ '{', 27, 3 }, // {
	{ '|', 28, 3 }, // Pipe
	{ '}', 29, 3 }, // } 
	{ '~', 30, 3 }, // ~ tilde
	{ '_', 31, 3 }, // _ Small underscore
	{ '!', 1, 1}, // ! exclamation
	{ '"', 2, 1}, // " double quote
	{ '#', 3, 1}, // # hash
	{ '$', 4, 1}, // $ dollar
	{ '%', 5, 1}, // % percentage
	{ '&', 6, 1}, // & amperesand
	{ '\'', 7, 1}, // single quote
	{ '(', 8, 1}, // ( left parenthesis
	{ ')', 9, 1}, // ) right parenthesis
	{ '*', 10, 1}, // * star
	{ '+', 11, 1}, // + plus
	{ ',', 12, 1}, // , comma
	{ '-', 13, 1}, // - minus 
	{ '.', 14, 1}, // . full stop
	{ '/', 15, 1}, // / forward slash
	{ '0', 16, 1}, // 0
	{ '1', 17, 1}, // 1
	{ '2', 18, 1}, // 2
	{ '3', 19, 1}, // 3
	{ '4', 20, 1}, // 4
	{ '5', 21, 1}, // 5
	{ '6', 22, 1}, // 6
	{ '7', 23, 1}, // 7
	{ '8', 24, 1}, // 8
	{ '9', 25, 1}, // 9
	{ ':', 26, 1}, // : colon
	{ ';', 27, 1}, // ; semi colon
	{ '<', 28, 1}, // < left chevron
	{ '=', 29, 1}, // = equals
	{ '>', 30, 1}, // > right chevron
	{ '?', 31, 1}, // ? question mark
};

// The index to character to use when we dont find a match
static unsigned int FONT_PLACEHOLDER = 1; // @

static unsigned int CHAR_LIST_SIZE = 96;