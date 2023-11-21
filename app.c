#include "app.h"
#include <pthread.h>

GtkWidget*** buttons;
GtkApplication* app;
int restartCount = 0;
int N, M, B;
int** board;
int* bombsArray;
GtkCssProvider* cssProvider = NULL;

struct ThreadData {
    const char* mp3FilePath;
};

// Sound files and commands
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

void buttonClicked(GtkWidget* widget, gpointer userData) {
    int index = GPOINTER_TO_INT(userData);
    int row = index / M;
    int col = index % M;

    revealSquare(row, col);
}

void activate(GtkApplication* app, gpointer userData) {
    GtkWidget* window;
    GtkWidget* grid;
    do {
        printf("Enter the board length (N): ");
        if (scanf("%d", &N) != 1 || N < 5) {
            printf("Input is not valid! Please enter a number greater than or equal to 5.\n");
            while (getchar() != '\n');
        }
    } while (N < 5);

    do {
        printf("Enter the board width (M): ");
        if (scanf("%d", &M) != 1 || M < 5) {
            printf("Input is not valid! Please enter a number greater than or equal to 5.\n");
            while (getchar() != '\n');
        }
    } while (M < 5);

    do {
        printf("Enter the bomb count (B): ");
        if (scanf("%d", &B) != 1 || B < M * N / 10 || B > M * N / 5) {
            printf("Input is not valid! Please enter a valid number (M * N / 10 <= B <= M * N / 5).\n");
            while (getchar() != '\n');
        }
    } while (B < M * N / 10 || B > M * N / 5);

    printf("Board length: %d, Board width: %d, Bomb count: %d\n", N, M, B);

    showInfoDialog("The source code can be found at:\nhttps://github.com/UMMAN2005/minesweeper\
    \n\nIf you want to change the source code, please send a pull request.");

    window = gtk_application_window_new(app);
    gtk_window_set_title(GTK_WINDOW(window), "minesweeper");
    gtk_window_set_resizable(GTK_WINDOW(window), FALSE);
    gtk_window_set_position(GTK_WINDOW(window), GTK_WIN_POS_CENTER);
    grid = gtk_grid_new();
    gtk_container_add(GTK_CONTAINER(window), grid);

    GtkWidget* label = gtk_label_new("Good Luck!!!");
    gtk_widget_set_hexpand(label, TRUE);
    gtk_widget_set_halign(label, GTK_ALIGN_CENTER);

    gtk_grid_attach(GTK_GRID(grid), label, 0, 0, N, 3);

    buttons = (GtkWidget***)malloc(N * sizeof(GtkWidget**));
    for (int i = 0; i < N; i++) {
        buttons[i] = (GtkWidget**)malloc(M * sizeof(GtkWidget*));
        for (int j = 0; j < M; j++) {
            buttons[i][j] = gtk_button_new_with_label(NULL);
            gtk_style_context_add_class(gtk_widget_get_style_context(buttons[i][j]), "square");
            gtk_widget_set_size_request(buttons[i][j], 50, 50);
            gtk_style_context_add_class(gtk_widget_get_style_context(buttons[i][j]), "area_button");
            setColor(buttons, NULL);
            g_signal_connect(buttons[i][j], "clicked", G_CALLBACK(buttonClicked), GINT_TO_POINTER(i * M + j));
            gtk_grid_attach(GTK_GRID(grid), buttons[i][j], i, j + 3, 1, 1);
        }
    }

    initializeArrays();
    plantBombs();
    countTouchingSquares();

    gtk_widget_show_all(window);
}


void revealSquare(int row, int col) {
    GtkWidget* button = GTK_WIDGET(buttons[row][col]);

    if (board[row][col] == -1) {
        playAudio(mp3FilePath_1);
        revealAllBombs();
        char* text = (char*)malloc(100);
        sprintf(text, "Game Over! You clicked on a bomb. Try again!\n\t\t\t  Restart count: %d", restartCount);
        showResultDialog(text);
    } else if (board[row][col] == 0) {
        playAudio(mp3FilePath0);
        gtk_button_set_label(GTK_BUTTON(button), "");
        revealEmptySquares(row, col);
    } else {
        char label[2];
        sprintf(label, "%d", board[row][col]);
        if (strcmp(label, "1\0") == 0)
        playAudio(mp3FilePath1);
        else if (strcmp(label, "2\0") == 0)
        playAudio(mp3FilePath2);
        else if (strcmp(label, "3\0") == 0)
        playAudio(mp3FilePath3);
        else if (strcmp(label, "4\0") == 0)
        playAudio(mp3FilePath4);
        else if (strcmp(label, "5\0") == 0)
        playAudio(mp3FilePath5);
        else if (strcmp(label, "6\0") == 0)
        playAudio(mp3FilePath6);
        else if (strcmp(label, "7\0") == 0)
        playAudio(mp3FilePath7);
        else if (strcmp(label, "8\0") == 0)
        playAudio(mp3FilePath8);
        gtk_button_set_label(GTK_BUTTON(button), label);
    }

    gtk_widget_set_sensitive(button, FALSE);
}

void revealAllBombs() {
    for (int i = 0; i < N; i++) {
        for (int j = 0; j < M; j++) {
            if (board[i][j] == -1) {
                GtkWidget* button = GTK_WIDGET(buttons[i][j]);
                gtk_button_set_label(GTK_BUTTON(button), "💣");
                gtk_widget_set_sensitive(button, FALSE);
            }
        }
    }
}

void revealEmptySquares(int row, int col) {
    if (row < 0 || col < 0 || row >= N || col >= M) {
        return;
    }

    GtkWidget* button = GTK_WIDGET(buttons[row][col]);

    if (gtk_widget_get_sensitive(button) == FALSE) {
        return;
    }

    gtk_widget_set_sensitive(button, FALSE);

    if (board[row][col] == 0) {
        gtk_button_set_label(GTK_BUTTON(button), "");

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
        gtk_button_set_label(GTK_BUTTON(button), label);
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

void showResultDialog(const char* message) {
    GtkWidget* dialog;
    dialog = gtk_message_dialog_new(NULL, GTK_DIALOG_MODAL, GTK_MESSAGE_INFO, 
    GTK_BUTTONS_NONE, message);
    gtk_message_dialog_format_secondary_text(GTK_MESSAGE_DIALOG(dialog), "Choose an option:");

    gtk_dialog_add_button(GTK_DIALOG(dialog), "Restart", 1);
    gtk_dialog_add_button(GTK_DIALOG(dialog), "Quit", 2);

    gtk_window_set_position(GTK_WINDOW(dialog), GTK_WIN_POS_CENTER);

    int result = gtk_dialog_run(GTK_DIALOG(dialog));
    gtk_widget_destroy(dialog);

    if (result == 1) {
        restartGame();
    } else if (result == 2) {
        quitApplication();
    }
}

void showInfoDialog(const char* infoMessage) {
    GtkWidget* dialog;
    dialog = gtk_message_dialog_new(NULL, GTK_DIALOG_MODAL, GTK_MESSAGE_INFO, 
    GTK_BUTTONS_OK, infoMessage);
    gtk_window_set_position(GTK_WINDOW(dialog), GTK_WIN_POS_CENTER);

    gtk_dialog_run(GTK_DIALOG(dialog));
    gtk_widget_destroy(dialog);
}

void restartGame() {
    restartCount++;
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

void cleanup() {
    for (int i = 0; i < N; i++) {
        for (int j = 0; j < M; j++) {
            gtk_widget_destroy(buttons[i][j]);
        }
        free(buttons[i]);
    }
    free(buttons);
}
