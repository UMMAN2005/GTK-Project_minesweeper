#ifndef APP_H
#define APP_H

#include <gtk/gtk.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <glib.h>
#include <ctype.h>
#include <unistd.h>
#include <time.h>

extern GtkApplication* app;

void printTitle();
void printSlowly(const char *str);
void buttonClicked(GtkWidget* widget, GdkEventButton* event, gpointer userData);
void activate(GtkApplication* app, gpointer userData);
void revealSquare(int row, int col);
void revealNumbers(char label[2], int row, int col);
void revealAllBombs();
void revealEmptySquares(int row, int col);
void initializeArrays();
void plantBombs();
void countTouchingSquares();
void* playAudioThread(void* data);
void playAudio(const char* mp3FilePath);
void showResultDialog(const char* message);
void showInfoDialog(const char* infoMessage);
void restartGame();
void quitApplication();
void setColor(GtkWidget*** widgets, gpointer userData);
void checkWin();
void startGame();
void clearScreen();
void cleanup();


#endif // APP_H