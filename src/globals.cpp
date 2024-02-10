#include "globals.h"

// Gets an integer number from serial
int32_t getIntInput() {
    char characters[10]{};
    char c;

    for (int i = 0; i < sizeof(characters) / sizeof(char); i++) {
        // Break to avoid overflow
        if (i + 1 == sizeof(characters) / sizeof(char)) {
            characters[i] = '\0';
            printf("\n");
            break;
        }

        // Read char
        c = getchar();

        // Handle enter key
        if (c == '\r' || c == '\n') {
            printf("\n");
            characters[i] = '\0';
            break;
        }

        // Handle backspace
        if (c == '\b') {
            if (i <= 0) {
                printf("\a");
                i -= 1;
            } else {
                printf("\b \b");
                i -= 2;
            }
            continue;
        }

        characters[i] = c;
        printf("%c", c);
    }

    return atoi(characters);
}