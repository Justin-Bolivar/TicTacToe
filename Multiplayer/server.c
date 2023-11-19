#include <stdio.h>
#include <stdlib.h>
#include <winsock2.h>

char square[10] = { 'o', '1', '2', '3', '4', '5', '6', '7', '8', '9' };

void displayBoard() {
    printf("\n\n\tTic Tac Toe\n\n");

    printf("CLIENT (X)  -  SERVER (O)\n\n\n");


    printf("     |     |     \n");
    printf("  %c  |  %c  |  %c \n", square[1], square[2], square[3]);

    printf("_____|_____|_____\n");
    printf("     |     |     \n");

    printf("  %c  |  %c  |  %c \n", square[4], square[5], square[6]);

    printf("_____|_____|_____\n");
    printf("     |     |     \n");

    printf("  %c  |  %c  |  %c \n", square[7], square[8], square[9]);

    printf("     |     |     \n\n");
}

int checkWin() {
    if (square[1] == square[2] && square[2] == square[3])
        return 1;
        
    else if (square[4] == square[5] && square[5] == square[6])
        return 1;
        
    else if (square[7] == square[8] && square[8] == square[9])
        return 1;
        
    else if (square[1] == square[4] && square[4] == square[7])
        return 1;
        
    else if (square[2] == square[5] && square[5] == square[8])
        return 1;
        
    else if (square[3] == square[6] && square[6] == square[9])
        return 1;
        
    else if (square[1] == square[5] && square[5] == square[9])
        return 1;
        
    else if (square[3] == square[5] && square[5] == square[7])
        return 1;
        
    else if (square[1] != '1' && square[2] != '2' && square[3] != '3' &&
        square[4] != '4' && square[5] != '5' && square[6] != '6' && square[7] 
        != '7' && square[8] != '8' && square[9] != '9')

        return 0;
    else
        return  - 1;
}

int main() {
    WSADATA wsaData;
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
        printf("WSAStartup failed\n");
        return 1;
    }

    SOCKET serverSocket;
    serverSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (serverSocket == INVALID_SOCKET) {
        printf("Socket creation in server side failed\n");
        WSACleanup();
        return 1;
    }

    struct sockaddr_in serverAddr;
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(6999);
    serverAddr.sin_addr.s_addr = INADDR_ANY;

    if (bind(serverSocket, (struct sockaddr*)&serverAddr, sizeof(serverAddr)) == SOCKET_ERROR) {
        printf("Binding failure at server side\n");
        closesocket(serverSocket);
        WSACleanup();
        return 1;
    }

    listen(serverSocket, 5);

    struct sockaddr_in clientAddr;
    int clientAddrLen = sizeof(clientAddr);
    SOCKET clientSocket = accept(serverSocket, (struct sockaddr*)&clientAddr, &clientAddrLen);
    if (clientSocket == INVALID_SOCKET) {
        printf("Failure at server side in accept\n");
        closesocket(serverSocket);
        WSACleanup();
        return 1;
    }

    int choice;
    char mark;

    int i = checkWin();
    while (i != 1) {
        recv(clientSocket, (char*)&choice, sizeof(choice), 0);
        if (choice == 11) {
            printf("Client wins!\n");
            closesocket(serverSocket);
            break;
        }

        mark = 'X';
        if (choice == 1 && square[1] == '1')
		    square[1] = mark;
		    
		else if (choice == 2 && square[2] == '2')
		    square[2] = mark;
		    
		else if (choice == 3 && square[3] == '3')
		    square[3] = mark;
		    
		else if (choice == 4 && square[4] == '4')
		    square[4] = mark;
		    
		else if (choice == 5 && square[5] == '5')
		    square[5] = mark;
		    
		else if (choice == 6 && square[6] == '6')
		    square[6] = mark;
		    
		else if (choice == 7 && square[7] == '7')
		    square[7] = mark;
		    
		else if (choice == 8 && square[8] == '8')
		    square[8] = mark;
		    
		else if(choice == 9 && square[9] == '9')
		    square[9] = mark;

        send(clientSocket, (const char*)&choice, sizeof(choice), 0);

        displayBoard();
        i = checkWin();
        if (i == 1) {
            displayBoard();
            printf("Server wins!\n");
            break;
        }

        displayBoard();
        printf("SERVER Please Enter Your Choice: ");
        scanf("%d", &choice);
        if (!(choice >= 1 && choice <= 9)) {
            printf("Invalid move \n");
            printf("Please enter a valid choice between 1-9\n");
            continue;
        }

        mark = 'O';
        if (choice == 1 && square[1] == '1')
		    square[1] = mark;
		    
		else if (choice == 2 && square[2] == '2')
		    square[2] = mark;
		    
		else if (choice == 3 && square[3] == '3')
		    square[3] = mark;
		    
		else if (choice == 4 && square[4] == '4')
		    square[4] = mark;
		    
		else if (choice == 5 && square[5] == '5')
		    square[5] = mark;
		    
		else if (choice == 6 && square[6] == '6')
		    square[6] = mark;
		    
		else if (choice == 7 && square[7] == '7')
		    square[7] = mark;
		    
		else if (choice == 8 && square[8] == '8')
		    square[8] = mark;
		    
		else if (choice == 9 && square[9] == '9')
		    square[9] = mark;

        send(clientSocket, (const char*)&choice, sizeof(choice), 0);
    }

    closesocket(serverSocket);
    closesocket(clientSocket);
    WSACleanup();
    return 0;
}
