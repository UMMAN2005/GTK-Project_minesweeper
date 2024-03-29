#include "app.h"
#include <pthread.h>

GtkWidget*** buttons;
int restartCount = 0;
int N, M, B, F, U, R;
int** board;
int* bombsArray;
gboolean firstPlay;
int tempRow = -1;
int tempCol = -1;
clock_t startTime;
clock_t endTime;
GtkCssProvider* cssProvider = NULL;
GtkWidget* grid;
GtkWidget* window;

struct ThreadData {
    const char* mp3FilePath;
};

const char* mpg123Path = "C:\\Users\\user\\Desktop\\TOOLS\\mpg123-1.32.3-static-x86-64\\mpg123.exe";
const char* mp3FilePath_1 = ".\\Audio_files\\-1.mp3";
const char* mp3FilePath0 = ".\\Audio_files\\0.mp3";
const char* mp3FilePath1 = ".\\Audio_files\\1.mp3";
const char* mp3FilePath2 = ".\\Audio_files\\2.mp3";
const char* mp3FilePath3 = ".\\Audio_files\\3.mp3";
const char* mp3FilePath4 = ".\\Audio_files\\4.mp3";
const char* mp3FilePath5 = ".\\Audio_files\\5.mp3";
const char* mp3FilePath6 = ".\\Audio_files\\6.mp3";
const char* mp3FilePath7 = ".\\Audio_files\\7.mp3";
const char* mp3FilePath8 = ".\\Audio_files\\8.mp3";
const char* mp3FilePathFlag = ".\\Audio_files\\flag.mp3";
const char* mp3FilePathUnFlag = ".\\Audio_files\\unflag.mp3";
const char* mp3FilePathWin = ".\\Audio_files\\win.mp3";

void customMode() {
    do {
        printf("Enter the board length (N): ");
        if (scanf("%d", &N) != 1 || N < 5) {
            printf("Input is not valid! Please enter a number greater than or equal to 5!\n");
            while (getchar() != '\n');
        }
    } while (N < 5);

    do {
        printf("Enter the board width (M): ");
        if (scanf("%d", &M) != 1 || M < 5) {
            printf("Input is not valid! Please enter a number greater than or equal to 5!\n");
            while (getchar() != '\n');
        }
    } while (M < 5);

    do {
        printf("Enter the bomb count (B): ");
        if (scanf("%d", &B) != 1 || B < M * N / 10 || B > M * N / 5) {
            printf("Input is not valid! Please enter a valid number (M * N / 10 <= B <= M * N / 5)!\n");
            while (getchar() != '\n');
        }
    } while (B < M * N / 10 || B > M * N / 5);

    do {
        printf("Enter the flag count (F): ");
        if (scanf("%d", &F) != 1 || F < 0 || F > B) {
            printf("Input is not valid! Please enter a number (0 <= F <= B)!\n");
            while (getchar() != '\n');
        }
    } while (F < 0 || F > B);
}

void challengeMode(int challengeNumber) {
    switch (challengeNumber) {
        case 1:
        // Easy challenge
            N = 10;
            M = 8;
            F = 10;
            B = 10;
            break;
        case 2:
        // Medium challenge
            N = 18;
            M = 14;
            F = 40;
            B = 40;
            break;
        case 3:
        // Hard challenge
            N = 24;
            M = 20;
            F = 99;
            B = 99;
            break;
        case 4:
        // No Flag challenge
            N = 20;
            M = 16;
            F = 0;
            B = 50;
            break;
    }
}

void buttonClicked(GtkWidget* widget, GdkEventButton* event, gpointer userData) {
    int index = GPOINTER_TO_INT(userData);
    int row = index / M;
    int col = index % M;

    GtkStyleContext* context = gtk_widget_get_style_context(widget);

    if (event->type == GDK_BUTTON_PRESS && event->button == 3) {
        if (gtk_style_context_has_class(context, "right-clicked")) {
            gtk_style_context_remove_class(context, "right-clicked");
            playAudio(mp3FilePathUnFlag);
            F++;
        } else {
            if (F > 0) {
                gtk_style_context_add_class(context, "right-clicked");
                playAudio(mp3FilePathFlag);
                F--;
            }
        }
        GtkWidget* label = GTK_WIDGET(gtk_grid_get_child_at(GTK_GRID(grid), 0, 0));
        gtk_label_set_text(GTK_LABEL(label), g_strdup_printf("📕 %d  📖 %d  🚩 %d", U, R, F));
    } else if (event->type == GDK_BUTTON_PRESS && event->button == 1) {
        if (!gtk_style_context_has_class(context, "right-clicked")) {
            if (firstPlay && board[row][col] == -1) {
                tempRow = row;
                tempCol = col;
                cleanup();
                U = N * M;
                R = 0;
                startGame();
            } else {
                firstPlay = FALSE;
                revealSquare(row, col);
            }
        }
    GtkWidget* label = GTK_WIDGET(gtk_grid_get_child_at(GTK_GRID(grid), 0, 0));
    gtk_label_set_text(GTK_LABEL(label), g_strdup_printf("📕 %d  📖 %d  🚩 %d", U, R, F));
    }
}

void activate(GtkApplication* app, gpointer userData) {
    const char option1[] = "Challenge";
    const char option2[] = "Custom";
    showModeDialog("Choose a game mode you want to play", option1, option2);

    printf("Board length: %d, Board width: %d, Bomb count: %d, Flag count: %d\n", N, M, B, F);
    U = N * M;
    R = 0;

    showInfoDialog("The source code can be found at:\nhttps://github.com/UMMAN2005/GTK-Project_minesweeper\
    \n\nIf you want to change the source code, please send a pull request.");

    window = gtk_application_window_new(app);
    gtk_window_set_title(GTK_WINDOW(window), "minesweeper");
    gtk_window_set_resizable(GTK_WINDOW(window), FALSE);
    gtk_window_set_position(GTK_WINDOW(window), GTK_WIN_POS_CENTER);
    grid = gtk_grid_new();
    gtk_container_add(GTK_CONTAINER(window), grid);

    GtkWidget* label = gtk_label_new(g_strdup_printf("📕 %d  📖 %d  🚩 %d", U, R, F));
    gtk_widget_set_hexpand(label, TRUE);
    gtk_widget_set_halign(label, GTK_ALIGN_CENTER);
    gtk_style_context_add_class(gtk_widget_get_style_context(label), "area_label");
    gtk_grid_attach(GTK_GRID(grid), label, 0, 0, N, 1);

    buttons = (GtkWidget***)malloc(N * sizeof(GtkWidget**));
    for (int i = 0; i < N; i++) {
        buttons[i] = (GtkWidget**)malloc(M * sizeof(GtkWidget*));
        for (int j = 0; j < M; j++) {
            buttons[i][j] = gtk_button_new_with_label(NULL);
            gtk_style_context_add_class(gtk_widget_get_style_context(buttons[i][j]), "square");
            gtk_widget_set_size_request(buttons[i][j], 50, 50);
            gtk_style_context_add_class(gtk_widget_get_style_context(buttons[i][j]), "area_button");
            setColor(buttons, NULL);
            g_signal_connect(buttons[i][j], "button-press-event", G_CALLBACK(buttonClicked), GINT_TO_POINTER(i * M + j));
            gtk_grid_attach(GTK_GRID(grid), buttons[i][j], i, j + 1, 1, 1);
        }
    }
    startGame();
}

void startGame() {
    firstPlay = TRUE;
    initializeArrays();
    plantBombs();
    countTouchingSquares();
    if (restartCount == 0)
    startTime = clock();

    gtk_widget_show_all(window);
    if (tempCol >= 0 && tempRow >= 0)
    revealSquare(tempRow, tempCol);    
}

void revealSquare(int row, int col) {
    GtkWidget* button = GTK_WIDGET(buttons[row][col]);
    GtkStyleContext* context = gtk_widget_get_style_context(button);

    if (board[row][col] == -1) {
        playAudio(mp3FilePath_1);
        gtk_style_context_add_class(context, "bomb");
        revealAllBombs();
        char* text = (char*)malloc(100);
        sprintf(text, "Game Over! You clicked on a bomb. Try again!\n\t\t\t  Restart count: %d", restartCount);
        const char option1[] = "Restart";
        const char option2[] = "Quit";
        showResultDialog(text, option1, option2);
    } else if (board[row][col] == 0) {
        playAudio(mp3FilePath0);
        gtk_button_set_label(GTK_BUTTON(button), "");
        revealEmptySquares(row, col);
    } else {
        char label[2];
        sprintf(label, "%d", board[row][col]);
        revealNumbers(label, row, col);
        U--;
        R++;
        checkWin();
    }

    gtk_widget_set_sensitive(button, FALSE);
}

void revealAllBombs() {
    for (int i = 0; i < N; i++) {
        for (int j = 0; j < M; j++) {
            if (board[i][j] == -1) {
                GtkWidget* button = GTK_WIDGET(buttons[i][j]);
                GtkStyleContext* context = gtk_widget_get_style_context(button);
                gtk_style_context_add_class(context, "bomb");
                gtk_button_set_label(GTK_BUTTON(button), "💣");
                gtk_widget_set_sensitive(button, FALSE);
            }
        }
    }
}

void revealNumbers(char label[2], int row, int col) {
    GtkWidget* button = GTK_WIDGET(buttons[row][col]);
    GtkStyleContext* context = gtk_widget_get_style_context(button);
    if (strcmp(label, "1\0") == 0) {
        playAudio(mp3FilePath1);
        gtk_style_context_add_class(context, "one");
    }
    else if (strcmp(label, "2\0") == 0) {
        playAudio(mp3FilePath2);
        gtk_style_context_add_class(context, "two");
    }
    else if (strcmp(label, "3\0") == 0) {
        playAudio(mp3FilePath3);
        gtk_style_context_add_class(context, "three");
    }
    else if (strcmp(label, "4\0") == 0) {
        playAudio(mp3FilePath4);
        gtk_style_context_add_class(context, "four");
    }
    else if (strcmp(label, "5\0") == 0) {
        playAudio(mp3FilePath5);
        gtk_style_context_add_class(context, "five");
    }
    else if (strcmp(label, "6\0") == 0) {
        playAudio(mp3FilePath6);
        gtk_style_context_add_class(context, "six");
    }
    else if (strcmp(label, "7\0") == 0) {
        playAudio(mp3FilePath7);
        gtk_style_context_add_class(context, "seven");
    }
    else if (strcmp(label, "8\0") == 0) {
        playAudio(mp3FilePath8);
        gtk_style_context_add_class(context, "eight");
    }  
    gtk_button_set_label(GTK_BUTTON(button), label);
}

void revealEmptySquares(int row, int col) {
    if (row < 0 || col < 0 || row >= N || col >= M) {
        return;
    }

    GtkWidget* button = GTK_WIDGET(buttons[row][col]);
    GtkStyleContext* context = gtk_widget_get_style_context(button);

    if (gtk_widget_get_sensitive(button) == FALSE || 
        gtk_style_context_has_class(gtk_widget_get_style_context(button), "right-clicked")) {
        return;
    }

    gtk_widget_set_sensitive(button, FALSE);

    if (board[row][col] == 0) {
        gtk_button_set_label(GTK_BUTTON(button), "");
        U--;
        R++;
        checkWin();

        revealEmptySquares(row - 1, col);
        revealEmptySquares(row + 1, col);
        revealEmptySquares(row, col - 1);
        revealEmptySquares(row, col + 1);
        revealEmptySquares(row - 1, col - 1);
        revealEmptySquares(row - 1, col + 1);
        revealEmptySquares(row + 1, col - 1);
        revealEmptySquares(row + 1, col + 1);
    } else if (board[row][col] > 0) {
        char label[2];
        sprintf(label, "%d", board[row][col]);
        if (strcmp(label, "1\0") == 0)
            gtk_style_context_add_class(context, "one");
        else if (strcmp(label, "2\0") == 0)
            gtk_style_context_add_class(context, "two");
        else if (strcmp(label, "3\0") == 0)
            gtk_style_context_add_class(context, "three");
        else if (strcmp(label, "4\0") == 0)
            gtk_style_context_add_class(context, "four");
        else if (strcmp(label, "5\0") == 0)
            gtk_style_context_add_class(context, "five");
        else if (strcmp(label, "6\0") == 0)
            gtk_style_context_add_class(context, "six");
        else if (strcmp(label, "7\0") == 0)
            gtk_style_context_add_class(context, "seven");
        else if (strcmp(label, "8\0") == 0)
            gtk_style_context_add_class(context, "eight");

        gtk_button_set_label(GTK_BUTTON(button), label);
        U--;
        R++;
        checkWin();
    }
}

void initializeArrays() {
    board = (int **)malloc(N * sizeof(int *));
    for (int i = 0; i < N; i++) {
        board[i] = (int *)malloc(M * sizeof(int));
        for (int j = 0; j < M; j++) {
            board[i][j] = 0;
        }
    }
}

void plantBombs() {
    srand(time(NULL));
    for (int bomb = 0; bomb < B; bomb++) {
        int row, column;

        do {
            row = rand() % M;
            column = rand() % N;
        } while (board[column][row] == -1);

        board[column][row] = -1;
    }
}

void countTouchingSquares() {
    for (int i = 0; i < N; i++) {
        for (int j = 0; j < M; j++) {
            int touching_squares = 0;
            if (i + 1 < N && board[i+1][j] == -1)
                touching_squares++;
            if (i - 1 >= 0 && board[i-1][j] == -1)
                touching_squares++;
            if (j + 1 < M && board[i][j+1] == -1)
                touching_squares++;
            if (j - 1 >= 0 && board[i][j-1] == -1)
                touching_squares++;
            if (i + 1 < N && j - 1 >= 0 && board[i+1][j-1] == -1)
                touching_squares++;
            if (i + 1 < N && j + 1 < M && board[i+1][j+1] == -1)
                touching_squares++;
            if (i - 1 >= 0 && j - 1 >= 0 && board[i-1][j-1] == -1)
                touching_squares++;
            if (i - 1 >= 0 && j + 1 < M && board[i-1][j+1] == -1)
                touching_squares++;
            if (board[i][j] != -1)
            board[i][j] = touching_squares;
            touching_squares = 0;
        }
    }
}

void* playAudioThread(void* data) {
    struct ThreadData* threadData = (struct ThreadData*)data;
    const char* mp3FilePath = threadData->mp3FilePath;

    char command[256];
    snprintf(command, sizeof(command), "%s \"%s\"", mpg123Path, mp3FilePath);

    int result = system(command);

    free(threadData);

    return NULL;
}

void playAudio(const char* mp3FilePath) {

    struct ThreadData* threadData = (struct ThreadData*)malloc(sizeof(struct ThreadData));
    threadData->mp3FilePath = mp3FilePath;

    pthread_t audioThread;
    pthread_create(&audioThread, NULL, playAudioThread, (void*)threadData);

    pthread_detach(audioThread);
}

void showResultDialog(const char* message, const char* option1, const char* option2) {
    GtkWidget* dialog;
    dialog = gtk_message_dialog_new(NULL, GTK_DIALOG_MODAL, GTK_MESSAGE_INFO, 
    GTK_BUTTONS_NONE, message);

    gtk_message_dialog_format_secondary_text(GTK_MESSAGE_DIALOG(dialog), "Choose an option:");

    gtk_dialog_add_button(GTK_DIALOG(dialog), option1, 1);
    gtk_dialog_add_button(GTK_DIALOG(dialog), option2, 2);

    gtk_window_set_position(GTK_WINDOW(dialog), GTK_WIN_POS_CENTER);

    int result = gtk_dialog_run(GTK_DIALOG(dialog));
    gtk_widget_destroy(dialog);

    if (result == 1) {
        restartGame();
    } else if (result == 2) {
        quitApplication();
        endTime = clock();

        double elapsedTime = (double)(endTime - startTime) / CLOCKS_PER_SEC;
        g_print("Elapsed Time: %.2f seconds\n", elapsedTime);
    }
    g_signal_connect(dialog, "response", G_CALLBACK(resultDialogResponse), NULL);
}

void showModeDialog(const char* message, const char* option1, const char* option2) {
    GtkWidget* dialog;
    dialog = gtk_message_dialog_new(NULL, GTK_DIALOG_MODAL, GTK_MESSAGE_INFO, 
    GTK_BUTTONS_NONE, message);

    gtk_message_dialog_format_secondary_text(GTK_MESSAGE_DIALOG(dialog), "Choose an option:");

    gtk_dialog_add_button(GTK_DIALOG(dialog), option1, 1);
    gtk_dialog_add_button(GTK_DIALOG(dialog), option2, 2);

    gtk_window_set_position(GTK_WINDOW(dialog), GTK_WIN_POS_CENTER);

    int result = gtk_dialog_run(GTK_DIALOG(dialog));
    gtk_widget_destroy(dialog);

    if (result == 1) {
        showChallengeDialog("Choose a challenge you want to face with:",  "First", "Second", "Third", "Fourth");
        return;
    } else if (result == 2) {
        customMode();
        return;
    }
    g_signal_connect(dialog, "response", G_CALLBACK(modeDialogResponse), NULL);
}

void showChallengeDialog(const char* message, const char* option1, const char* option2, const char* option3, const char* option4) {
    GtkWidget* dialog;
    dialog = gtk_message_dialog_new(NULL, GTK_DIALOG_MODAL, GTK_MESSAGE_INFO, 
    GTK_BUTTONS_NONE, message);

    gtk_message_dialog_format_secondary_text(GTK_MESSAGE_DIALOG(dialog), "Choose an option:");

    gtk_dialog_add_button(GTK_DIALOG(dialog), option1, 1);
    gtk_dialog_add_button(GTK_DIALOG(dialog), option2, 2);
    gtk_dialog_add_button(GTK_DIALOG(dialog), option3, 3);
    gtk_dialog_add_button(GTK_DIALOG(dialog), option4, 4);

    gtk_window_set_position(GTK_WINDOW(dialog), GTK_WIN_POS_CENTER);

    int result = gtk_dialog_run(GTK_DIALOG(dialog));
    gtk_widget_destroy(dialog);

    if (result == 1) {
        challengeMode(1);
        return;
    } else if (result == 2) {
        challengeMode(2);
        return;
    } else if (result == 3) {
        challengeMode(3);
        return;
    } else if (result == 4) {
        challengeMode(4);
        return;
    }
    g_signal_connect(dialog, "response", G_CALLBACK(challengeDialogResponse), NULL);
}

void showInfoDialog(const char* infoMessage) {
    GtkWidget* dialog;
    dialog = gtk_message_dialog_new(NULL, GTK_DIALOG_MODAL, GTK_MESSAGE_INFO, 
    GTK_BUTTONS_OK, infoMessage);
    gtk_window_set_position(GTK_WINDOW(dialog), GTK_WIN_POS_CENTER);

    gtk_dialog_run(GTK_DIALOG(dialog));
    gtk_widget_destroy(dialog);
}

void modeDialogResponse(GtkDialog *dialog, gint response_id, gpointer user_data) {
    gtk_widget_destroy(GTK_WIDGET(dialog));
    quitApplication();
}

void resultDialogResponse(GtkDialog *dialog, gint response_id, gpointer user_data) {
    gtk_widget_destroy(GTK_WIDGET(dialog));
    quitApplication();
}

void challengeDialogResponse(GtkDialog *dialog, gint response_id, gpointer user_data) {
    gtk_widget_destroy(GTK_WIDGET(dialog));
    quitApplication();
}

void restartGame() {
    cleanup();
    restartCount++;
    U = N * M;
    R = 0;
    F = 0; 
    GtkWidget* currentWindow = GTK_WIDGET(gtk_application_get_active_window(app));
    gtk_widget_destroy(currentWindow);
    activate(app, NULL);
}

void quitApplication() {
    g_application_quit(G_APPLICATION(app));
}

void setColor(GtkWidget*** widgets, gpointer userData) {
    if (cssProvider == NULL) {
        cssProvider = gtk_css_provider_new();
        GError* error = NULL;

        if (!gtk_css_provider_load_from_path(cssProvider, "minesweeper_style.css", &error)) {
            g_error("Error loading CSS file: %s", error->message);
            g_clear_error(&error);
        }

        gtk_style_context_add_provider_for_screen(gdk_screen_get_default(), 
            GTK_STYLE_PROVIDER(cssProvider), GTK_STYLE_PROVIDER_PRIORITY_USER);
    }
}

void checkWin() {
    if (U == B) {
        char* text = (char*)malloc(100);
        sprintf(text, "You Won! Congratulations! 🎉\n\t    Restart count: %d", restartCount);
        GtkWidget* label = GTK_WIDGET(gtk_grid_get_child_at(GTK_GRID(grid), 0, 0));
        gtk_label_set_text(GTK_LABEL(label), g_strdup_printf("📕 %d  📖 %d  🚩 %d", U, R, F));
        playAudio(mp3FilePathWin);
        const char option1[] = "Restart";
        const char option2[] = "Quit";
        showResultDialog(text, option1, option2);
    }
}

void cleanup() {
    for (int i = 0; i < N; i++) {
        for (int j = 0; j < M; j++) {
            gtk_widget_destroy(buttons[i][j]);
        }
        free(buttons[i]);
    }
    free(buttons);

    // Destroy the main window
    if (window != NULL) {
        gtk_widget_destroy(window);
        window = NULL;
    }

    // Free the board array
    for (int i = 0; i < N; i++) {
        free(board[i]);
    }
    free(board);

    // Reset global variables
    N = 0;
    M = 0;
    B = 0;
    F = 0;
    U = 0;
    R = 0;
    firstPlay = TRUE;

    tempRow = -1;
    tempCol = -1;
}

