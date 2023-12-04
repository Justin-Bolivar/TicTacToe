#include <stdio.h>
#include <winsock2.h>

#pragma comment(lib, "ws2_32.lib")

#define BOARD_SIZE 3

// Function to display the combined Tic-Tac-Toe board
void display_combined_board(char server_board[BOARD_SIZE][BOARD_SIZE], char client_board[BOARD_SIZE][BOARD_SIZE]) {
    printf("\nCombined Board:\n");
    for (int i = 0; i < BOARD_SIZE; ++i) {
        for (int j = 0; j < BOARD_SIZE; ++j) {
            printf("%c ", (server_board[i][j] == ' ') ? client_board[i][j] : server_board[i][j]);
        }
        printf("\n");
    }
}

// Function to check if a player has won
int check_win(char board[BOARD_SIZE][BOARD_SIZE], char marker) {
    // Check row
    for (int i = 0; i < BOARD_SIZE; i++) {
        if(board[i][0] == marker && board[i][1] == marker && board[i][2] == marker)  {
            return 1; // Win
        }
    }

    //check col
    for(int i = 0; i < BOARD_SIZE; i++) {
        if(board[0][i] == marker && board[1][i] == marker && board[2][i] == marker) {
            return 1;
        }
    }

    // Check diagonals
    if (board[0][0] == marker && board[1][1] == marker && board[2][2] == marker) {
        return 1; // Win
        }
        else if (board[0][2] == marker && board[1][1] == marker && board[2][0] == marker) {
            return 1;
        }

    return 0; // No win
}

int main() {
    WSADATA wsa;
    SOCKET client_socket;
    struct sockaddr_in server_addr;

    int move;
    char server_board[BOARD_SIZE][BOARD_SIZE] = {{' ', ' ', ' '}, {' ', ' ', ' '}, {' ', ' ', ' '}};
    char client_board[BOARD_SIZE][BOARD_SIZE] = {{' ', ' ', ' '}, {' ', ' ', ' '}, {' ', ' ', ' '}};

    // Initialize Winsock
    if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0) {
        printf("Failed to initialize Winsock\n");
        return 1;
    }

    // Create a socket
    if ((client_socket = socket(AF_INET, SOCK_STREAM, 0)) == INVALID_SOCKET) {
        printf("Failed to create socket\n");
        return 1;
    }

    // Set up the server address structure
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = inet_addr("192.168.1.6");
    server_addr.sin_port = htons(12345);

    // Connect to the server
    if (connect(client_socket, (struct sockaddr*)&server_addr, sizeof(server_addr)) == SOCKET_ERROR) {
        printf("Connection failed\n");
        return 1;
    }

    int game_over = 0;
    char marker;

    while (!game_over) {
        // Display the combined board
        display_combined_board(server_board, client_board);

        // Receive the move from the server
        recv(client_socket, (char*)&move, sizeof(int), 0);

        // Update the client's board with the move
        client_board[(move - 1) / BOARD_SIZE][(move - 1) % BOARD_SIZE] = 'X';

        // Check for a win on the client side
        if (check_win(client_board, 'X')) {
            display_combined_board(server_board, client_board);
            printf("\nAI (X) wins!\n");
            game_over = 1;
        } else if (++move > BOARD_SIZE * BOARD_SIZE) {
            display_combined_board(server_board, client_board);
            printf("\nIt's a draw!\n");
            game_over = 1;
            break;
        }

        // Display the updated combined board
        display_combined_board(server_board, client_board);

        // Prompt the player for a move
        printf("\nEnter your move (1-9): ");
        scanf("%d", &move);

        // Send the move to the server
        send(client_socket, (char*)&move, sizeof(int), 0);

        // Update the server's board with the move
        server_board[(move - 1) / BOARD_SIZE][(move - 1) % BOARD_SIZE] = 'O';

        // Check for a win on the server side
        if (check_win(server_board, 'O')) {
            display_combined_board(server_board, client_board);
            printf("\nPlayer (O) wins!\n");
            game_over = 1;
        } else if (++move > BOARD_SIZE * BOARD_SIZE) {
            display_combined_board(server_board, client_board);
            printf("\nIt's a draw!\n");
            game_over = 1;
        }
    }

    Sleep(10000);

    // Close the connection with the server
    closesocket(client_socket);

    // Cleanup
    WSACleanup();

    return 0;
}
