#ifndef APP_H
#define APP_H

#include <gtk/gtk.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <glib.h>
#include <ctype.h>

extern GtkApplication* app;

void customMode();
void challengeMode(int challengeNumber);
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
void showModeDialog(const char* message, const char* option1, const char* option2);
void showResultDialog(const char* message, const char* option1, const char* option2);
void showChallengeDialog(const char* message, const char* option1, const char* option2, const char* option3, const char* option4);
void showInfoDialog(const char* infoMessage);
void resultDialogResponse(GtkDialog *dialog, gint response_id, gpointer user_data);
void modeDialogResponse(GtkDialog *dialog, gint response_id, gpointer user_data);
void challengeDialogResponse(GtkDialog *dialog, gint response_id, gpointer user_data);
void restartGame();
void quitApplication();
void setColor(GtkWidget*** widgets, gpointer userData);
void checkWin();
void startGame();
void cleanupButton(GtkWidget* button);
void cleanupButtons();
void cleanup();


#endif // APP_H