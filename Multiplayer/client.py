import socket
import tkinter as tk
import json  # Import the json module for data serialization

HOST = '127.0.0.1'  # Replace with the server's IP address
PORT = 12345

class TicTacToeClient:
    def __init__(self, master):
        self.master = master
        self.master.title("Tic-Tac-Toe")
        self.board = [[" " for _ in range(3)] for _ in range(3)]
        self.create_board_buttons()
        self.connect_to_server()

    def create_board_buttons(self):
        self.buttons = []
        for i in range(3):
            for j in range(3):
                button = tk.Button(self.master, text="", width=10, height=3,
                                   command=lambda row=i, col=j: self.on_button_click(row, col))
                button.grid(row=i, column=j)
                self.buttons.append(button)

    def connect_to_server(self):
        self.client_socket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
        self.client_socket.connect((HOST, PORT))

    def on_button_click(self, row, col):
        if self.board[row][col] == " ":
            self.board[row][col] = "X"
            self.buttons[row * 3 + col].config(text="X", state=tk.DISABLED)
            self.send_move(row, col)
            self.check_winner()

    def send_move(self, row, col):
        move = {"row": row, "col": col}  # Use a dictionary for more structured data
        self.client_socket.send(json.dumps(move).encode())

    def receive_board(self):
        data = self.client_socket.recv(1024)
        if not data:
            return
        received_board = json.loads(data.decode())  # Use json.loads to convert data to a Python object
        self.update_board(received_board)

    def update_board(self, new_board):
        self.board = new_board
        for i in range(3):
            for j in range(3):
                self.buttons[i * 3 + j].config(text=new_board[i][j])

    def check_winner(self):
        # Implement the logic to check for a winner or a draw
        # Update the GUI and notify the server if needed
        pass

if __name__ == "__main__":
    root = tk.Tk()
    app = TicTacToeClient(root)
    
    while True:
        app.receive_board()
    
    root.mainloop()
