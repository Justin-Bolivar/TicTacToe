#include <stdio.h>
#include <winsock2.h>

#pragma comment(lib, "ws2_32.lib")

#define PORT 12345
#define BUFFER_SIZE 1024

void playGame(SOCKET clientSocket);
int checkWinner(char board[3][3]);

int main() {
    WSADATA wsaData;
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
        perror("Error initializing Winsock");
        return 1;
    }

    SOCKET serverSocket, clientSocket;
    struct sockaddr_in serverAddr, clientAddr;
    int addrSize = sizeof(struct sockaddr_in);

    // Create socket
    if ((serverSocket = socket(AF_INET, SOCK_STREAM, 0)) == INVALID_SOCKET) {
        perror("Error creating socket");
        return 1;
    }

    // Configure server address struct
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_addr.s_addr = INADDR_ANY;
    serverAddr.sin_port = htons(PORT);

    // Bind the socket
    if (bind(serverSocket, (struct sockaddr*)&serverAddr, sizeof(serverAddr)) == SOCKET_ERROR) {
        perror("Error binding socket");
        return 1;
    }

    // Listen for incoming connections
    if (listen(serverSocket, 5) == SOCKET_ERROR) {
        perror("Error listening for connections");
        return 1;
    }

    printf("Server listening on port %d...\n", PORT);

    // Accept incoming connections
    while (1) {
        clientSocket = accept(serverSocket, (struct sockaddr*)&clientAddr, &addrSize);
        if (clientSocket == INVALID_SOCKET) {
            perror("Error accepting connection");
            continue;
        }

        printf("Connection accepted from %s:%d\n", inet_ntoa(clientAddr.sin_addr), ntohs(clientAddr.sin_port));

        // Start a new thread or process to handle the game for this client
        playGame(clientSocket);
    }

    closesocket(serverSocket);
    WSACleanup();

    return 0;
}

void playGame(SOCKET clientSocket) {
    char board[3][3] = { { ' ', ' ', ' ' }, { ' ', ' ', ' ' }, { ' ', ' ', ' ' } };
    char buffer[BUFFER_SIZE];
    int row, col;
    int currentPlayer = 1;  // 1 for Player X, 2 for Player O

    while (1) {
        // Send the current state of the board to the client
        send(clientSocket, (const char*)board, sizeof(board), 0);

        // Receive the move from the client
        recv(clientSocket, buffer, sizeof(buffer), 0);
        sscanf(buffer, "%d %d", &row, &col);

        // Update the board
        if (currentPlayer == 1 && board[row][col] == ' ') {
            board[row][col] = 'X';
            currentPlayer = 2;
        }
        else if (currentPlayer == 2 && board[row][col] == ' ') {
            board[row][col] = 'O';
            currentPlayer = 1;
        }

        // Check for a winner or a draw
        int winner = checkWinner(board);
        if (winner == 1) {
            printf("Player X wins!\n");
            break;
        }
        else if (winner == 2) {
            printf("Player O wins!\n");
            break;
        }
        else if (winner == 0) {
            printf("It's a draw!\n");
            break;
        }
    }

    closesocket(clientSocket);
}

int checkWinner(char board[3][3]) {
    // Check rows and columns
    for (int i = 0; i < 3; i++) {
        if (board[i][0] == board[i][1] && board[i][1] == board[i][2] && board[i][0] != ' ')
            return (board[i][0] == 'X') ? 1 : 2;

        if (board[0][i] == board[1][i] && board[1][i] == board[2][i] && board[0][i] != ' ')
            return (board[0][i] == 'X') ? 1 : 2;
    }

    // Check diagonals
    if (board[0][0] == board[1][1] && board[1][1] == board[2][2] && board[0][0] != ' ')
        return (board[0][0] == 'X') ? 1 : 2;

    if (board[0][2] == board[1][1] && board[1][1] == board[2][0] && board[0][2] != ' ')
        return (board[0][2] == 'X') ? 1 : 2;

    // Check for a draw
    int draw = 1;
    for (int i = 0; i < 3; i++) {
        for (int j = 0; j < 3; j++) {
            if (board[i][j] == ' ')
                draw = 0;
        }
    }

    if (draw)
        return 0;

    return -1; // No winner yet
}
