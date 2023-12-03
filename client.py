import tkinter as tk
from tkinter import messagebox
import socket
import threading

BOARD_SIZE = 3

class TicTacToeClient:
    def __init__(self, root, server_ip, server_port):
        self.root = root
        self.server_ip = server_ip
        self.server_port = server_port

        self.client_socket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
        self.client_socket.connect((self.server_ip, self.server_port))

        self.board = [[' ' for _ in range(BOARD_SIZE)] for _ in range(BOARD_SIZE)]

        self.buttons = [[None for _ in range(BOARD_SIZE)] for _ in range(BOARD_SIZE)]

        self.root.title("Tic-Tac-Toe")
        self.create_board()

        self.receive_moves_thread = threading.Thread(target=self.receive_moves)
        self.receive_moves_thread.start()

    def create_board(self):
        for i in range(BOARD_SIZE):
            for j in range(BOARD_SIZE):
                button = tk.Button(self.root, text='', font=('normal', 20), width=5, height=2,
                                   command=lambda row=i, col=j: self.make_move(row, col))
                button.grid(row=i, column=j)
                self.buttons[i][j] = button

    def make_move(self, row, col):
        if self.board[row][col] == ' ':
            move = row * BOARD_SIZE + col + 1
            self.send_move(move)

    def send_move(self, move):
        self.client_socket.sendall(move.to_bytes(4, byteorder='big'))
        self.update_board(move, 'O')

    def receive_moves(self):
        while True:
            try:
                move_bytes = self.client_socket.recv(4)
                if not move_bytes:
                    break
                move = int.from_bytes(move_bytes, byteorder='big')
                self.root.after(0, self.update_board, move, 'X')  # Schedule GUI update on the main thread
            except Exception as e:
                print("Error receiving move:", str(e))
                break

    def update_board(self, move, marker):
        row = (move - 1) // BOARD_SIZE
        col = (move - 1) % BOARD_SIZE
        self.board[row][col] = marker
        self.buttons[row][col]['text'] = marker

        if self.check_win(self.board, marker):
            self.show_winner(marker)
        elif all(self.board[i][j] != ' ' for i in range(BOARD_SIZE) for j in range(BOARD_SIZE)):
            self.show_draw()

    def check_win(self, board, marker):
        # Check rows and columns
        for i in range(BOARD_SIZE):
            if all(board[i][j] == marker for j in range(BOARD_SIZE)) or \
               all(board[j][i] == marker for j in range(BOARD_SIZE)):
                return True

        # Check diagonals
        if all(board[i][i] == marker for i in range(BOARD_SIZE)) or \
           all(board[i][BOARD_SIZE - i - 1] == marker for i in range(BOARD_SIZE)):
            return True

        return False

    def show_winner(self, marker):
        messagebox.showinfo("Game Over", f"{marker} wins!")
        self.reset_game()

    def show_draw(self):
        messagebox.showinfo("Game Over", "It's a draw!")
        self.reset_game()

    def reset_game(self):
        self.client_socket.close()
        self.root.destroy()

def main():
    root = tk.Tk()
    server_ip = "192.168.1.6"  # Change this to your server's IP
    server_port = 12345
    tic_tac_toe_client = TicTacToeClient(root, server_ip, server_port)
    root.mainloop()

if __name__ == "__main__":
    main()
