#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <winsock2.h>

#pragma comment(lib, "ws2_32.lib")

#define BOARD_SIZE 3

void display_combined_board(char server_board[BOARD_SIZE][BOARD_SIZE], char client_board[BOARD_SIZE][BOARD_SIZE]) {
    printf("\nGame State:\n");

    for (int i = 0; i < BOARD_SIZE; i++) {
        for (int j = 0; j < BOARD_SIZE; j++) {
            printf("[%c]", (server_board[i][j] == ' ') ? client_board[i][j] : server_board[i][j]);
        }
        
        printf("\n");
    }
}


int check_win(char board[BOARD_SIZE][BOARD_SIZE], char marker) {
    // check row
    for (int i = 0; i < BOARD_SIZE; i++) {
        if(board[i][0] == marker && board[i][1] == marker && board[i][2] == marker)  {
            return 1;
        }
    }

    //check col
    for(int i = 0; i < BOARD_SIZE; i++) {
        if(board[0][i] == marker && board[1][i] == marker && board[2][i] == marker) {
            return 1;
        }
    }

    //check diagonals
    if (board[0][0] == marker && board[1][1] == marker && board[2][2] == marker) {
        return 1;
        }
        else if (board[0][2] == marker && board[1][1] == marker && board[2][0] == marker) {
            return 1;
        }

    return 0;
}

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

    if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0) {
        printf("Failed to initialize Winsock\n");
        return 1;
    }

    if ((server_socket = socket(AF_INET, SOCK_STREAM, 0)) == INVALID_SOCKET) {
        printf("Failed to create socket\n");
        return 1;
    }

    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(12345);

    if (bind(server_socket, (struct sockaddr*)&server_addr, sizeof(server_addr)) == SOCKET_ERROR) {
        printf("Bind failed\n");
        return 1;
    }

    if (listen(server_socket, 1) == SOCKET_ERROR) {
        printf("Listen failed\n");
        return 1;
    }

    printf("Server listening on port 12345\n");

    srand((unsigned int)time(NULL));

    if ((client_socket = accept(server_socket, (struct sockaddr*)&client_addr, &client_addr_len)) == INVALID_SOCKET) {
        printf("Accept failed\n");
        return 1;
    }

    printf("Connection from %s\n", inet_ntoa(client_addr.sin_addr));

    int game_over = 0;
    int move;
    char marker;

    while (!game_over) {
        display_combined_board(server_board, client_board);

        move = make_random_move(server_board);
        printf("\nServer (X) chooses position %d\n", move);

        server_board[(move - 1) / BOARD_SIZE][(move - 1) % BOARD_SIZE] = 'X';

        send(client_socket, (char*)&move, sizeof(int), 0);

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

        recv(client_socket, (char*)&move, sizeof(int), 0);
        move = *(int*)buffer;

        client_board[(move - 1) / BOARD_SIZE][(move - 1) % BOARD_SIZE] = 'X';

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

    closesocket(client_socket);

    closesocket(server_socket);
    WSACleanup();

    return 0;
}
