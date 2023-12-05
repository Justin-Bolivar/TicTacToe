#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <winsock2.h>

#pragma comment(lib, "ws2_32.lib")

#define BOARD_SIZE 3

// Function to display the combined Tic-Tac-Toe board
void display_combined_board(char server_board[BOARD_SIZE][BOARD_SIZE], char client_board[BOARD_SIZE][BOARD_SIZE]) {
    printf("\nGame State:\n");

    for (int i = 0; i < BOARD_SIZE; i++) {
        for (int j = 0; j < BOARD_SIZE; j++) {
            printf("[%c]", (server_board[i][j] == ' ') ? client_board[i][j] : server_board[i][j]);
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

// Function to make a random move for the AI
int make_random_move(char board[BOARD_SIZE][BOARD_SIZE]) {
    int move;
    do {
        move = rand() % (BOARD_SIZE * BOARD_SIZE) + 1;
    } while (board[(move - 1) / BOARD_SIZE][(move - 1) % BOARD_SIZE] != ' ');

    return move;
}

int main() {
    WSADATA wsa;
    SOCKET server_socket, client_socket;
    struct sockaddr_in server_addr, client_addr;
    int client_addr_len = sizeof(client_addr);
    char buffer[1024] = {0};
    char server_board[BOARD_SIZE][BOARD_SIZE] = {{' ', ' ', ' '}, {' ', ' ', ' '}, {' ', ' ', ' '}};
    char client_board[BOARD_SIZE][BOARD_SIZE] = {{' ', ' ', ' '}, {' ', ' ', ' '}, {' ', ' ', ' '}};

    // Initialize Winsock
    if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0) {
        printf("Failed to initialize Winsock\n");
        return 1;
    }

    // Create a socket
    if ((server_socket = socket(AF_INET, SOCK_STREAM, 0)) == INVALID_SOCKET) {
        printf("Failed to create socket\n");
        return 1;
    }

    // Set up the server address structure
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(12345);

    // Bind the socket
    if (bind(server_socket, (struct sockaddr*)&server_addr, sizeof(server_addr)) == SOCKET_ERROR) {
        printf("Bind failed\n");
        return 1;
    }

    // Listen for incoming connections
    if (listen(server_socket, 1) == SOCKET_ERROR) {
        printf("Listen failed\n");
        return 1;
    }

    printf("Server listening on port 12345\n");

    // Seed the random number generator for AI moves
    srand((unsigned int)time(NULL));

    // Accept connections and play Tic-Tac-Toe
    if ((client_socket = accept(server_socket, (struct sockaddr*)&client_addr, &client_addr_len)) == INVALID_SOCKET) {
        printf("Accept failed\n");
        return 1;
    }

    printf("Connection from %s\n", inet_ntoa(client_addr.sin_addr));

    int game_over = 0;
    int move;
    char marker;

    while (!game_over) {
        // Display the combined board
        display_combined_board(server_board, client_board);

        // Server's turn (AI)
        move = make_random_move(server_board);
        printf("\nServer (X) chooses position %d\n", move);

        // Update the server's board with the move
        server_board[(move - 1) / BOARD_SIZE][(move - 1) % BOARD_SIZE] = 'X';

        // Send the move to the client
        send(client_socket, (char*)&move, sizeof(int), 0);

        // Check for a win on the server side
        if (check_win(server_board, 'X')) {
            display_combined_board(server_board, client_board);
            printf("\nServer (X) wins!\n");
            game_over = 1;
            break;
        } else if (move++ > BOARD_SIZE * BOARD_SIZE) {
            display_combined_board(server_board, client_board);
            printf("\nIt's a draw!\n");
            game_over = 1;
            break;
        }

        // Receive the move from the client
        recv(client_socket, (char*)&move, sizeof(int), 0);
        move = *(int*)buffer;

        // Update the client's board with the move
        client_board[(move - 1) / BOARD_SIZE][(move - 1) % BOARD_SIZE] = 'X';

        // Check for a win on the client side
        if (check_win(client_board, 'O')) {
            display_combined_board(server_board, client_board);
            printf("\nPlayer (O) wins!\n");
            game_over = 1;
        } else if (move++ > BOARD_SIZE * BOARD_SIZE) {
            display_combined_board(server_board, client_board);
            printf("\nIt's a draw!\n");
            game_over = 1;
        }
    }

Sleep(10000);

    // Close the connection with the client
    closesocket(client_socket);

    // Cleanup
    closesocket(server_socket);
    WSACleanup();

    return 0;
}
