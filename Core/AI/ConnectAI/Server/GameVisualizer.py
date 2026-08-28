import tkinter as tk
from tkinter import font as tkfont
from queue import Queue
from GameState import GameState, PieceType


PIECE_COLORS = {
    PieceType.I: "#61d6d6",
    PieceType.J: "#0037da",
    PieceType.L: "#ffa500",
    PieceType.O: "#ffff00",
    PieceType.S: "#13a10e",
    PieceType.T: "#881798",
    PieceType.Z: "#c50f1f",
    PieceType.GHOST: "#555555",
}

CELL_SIZE = 22
BOARD_X = 20
BOARD_Y = 60
CELL_PAD = 1


class GameVisualizer:
    def __init__(self, dataQueue: Queue):
        self.dataQueue = dataQueue
        self.root = tk.Tk()
        self.root.title("Tetris AI - Client Data Viewer")
        self.root.configure(bg="#1e1e1e")
        self.root.resizable(False, False)

        self.boardWidth = GameState.BOARD_WIDTH
        self.boardHeight = GameState.BOARD_HEIGHT

        canvasWidth = BOARD_X * 2 + self.boardWidth * CELL_SIZE + 340
        canvasHeight = BOARD_Y + self.boardHeight * CELL_SIZE + 60
        
        self.canvas = tk.Canvas(self.root, width=canvasWidth, height=canvasHeight, bg="#1e1e1e", highlightthickness=0, )
        self.canvas.pack(padx=5, pady=5)

        self.info_x = BOARD_X + self.boardWidth * CELL_SIZE + 30

        self.DrawBoardBorder()
        self.InitBoardCells()
        self.InitInfoTexts()

    def DrawBoardBorder(self):
        x1 = BOARD_X - 2
        y1 = BOARD_Y - 2
        x2 = BOARD_X + self.boardWidth * CELL_SIZE + 2
        y2 = BOARD_Y + self.boardHeight * CELL_SIZE + 2
        self.canvas.create_rectangle(x1, y1, x2, y2, outline="#666666", width=2)

    def InitBoardCells(self):
        self.cells = []
        
        for y in range(self.boardHeight):
            row = []
            
            for x in range(self.boardWidth):
                cx = BOARD_X + x * CELL_SIZE
                cy = BOARD_Y + y * CELL_SIZE
                
                rect = self.canvas.create_rectangle(
                    cx + CELL_PAD, cy + CELL_PAD,
                    cx + CELL_SIZE - CELL_PAD, cy + CELL_SIZE - CELL_PAD,
                    fill="#2a2a2a",
                    outline="#3a3a3a",
                    width=1,
                )
                row.append(rect)
                
            self.cells.append(row)

    def InitInfoTexts(self):
        mono = tkfont.Font(family="Consolas", size=10)
        mono_b = tkfont.Font(family="Consolas", size=11, weight="bold")
        x = self.info_x
        y = BOARD_Y

        self.canvas.create_text(x, y, text="[ AI Output Data ]", fill="#00cccc", font=mono_b, anchor="nw", )
        y += 30

        def MakeLabel(text, yy):
            self.canvas.create_text(x, yy, text=text, fill="#888888", font=mono, anchor="nw")

        def MakeValue(yy):
            return self.canvas.create_text(x + 120, yy, text="-", fill="#dddddd", font=mono, anchor="nw")

        MakeLabel("Message Size : ", y)
        self.val_size = MakeValue(y)
        y += 22

        MakeLabel("Current Piece : ", y)
        self.val_current = MakeValue(y)
        y += 20

        MakeLabel("Rotation :", y)
        self.val_rotation = MakeValue(y)
        y += 20

        MakeLabel("Position : ", y)
        self.val_position = MakeValue(y)
        y += 25

        MakeLabel("Hold Piece : ", y)
        self.val_hold = MakeValue(y)
        y += 22

        MakeLabel("Next Pieces : ", y)
        self.val_next = MakeValue(y)
        y += 30

        self.canvas.create_text(x, y, text="Raw Bytes (hex):", fill="#aaaaaa", font=mono, anchor="nw")
        y += 20

        self.hex_texts = []
        for _ in range(16):
            t = self.canvas.create_text(x, y, text="", fill="#7aac7a", font=mono, anchor="nw")
            self.hex_texts.append(t)
            y += 15

    def PieceName(self, pieceType):
        try:
            return PieceType(pieceType).name
        except ValueError:
            return f"Unknown({pieceType})"

    def PieceColor(self, pieceType):
        try:
            p = PieceType(pieceType)
            return PIECE_COLORS.get(p, "#555555")
        except ValueError:
            return "#555555"

    def UpdateData(self, state, rawBytes):
        for y in range(self.boardHeight):
            for x in range(self.boardWidth):
                cellValue = state.board[y][x]
                color = self.PieceColor(cellValue)
                self.canvas.itemconfig(self.cells[y][x], fill=color)

        cp = state.currentPiece
        
        self.canvas.itemconfig(self.val_size, text=f"{len(rawBytes)} bytes")
        self.canvas.itemconfig(
            self.val_current,
            text=self.PieceName(cp.type),
            fill=self.PieceColor(cp.type),
        )
        self.canvas.itemconfig(self.val_rotation, text=str(cp.rotation))
        self.canvas.itemconfig(self.val_position, text=f"({cp.x}, {cp.y})")

        hp = state.holdPiece
        self.canvas.itemconfig(
            self.val_hold,
            text=self.PieceName(hp),
            fill=self.PieceColor(hp),
        )

        next_names = " ".join(self.PieceName(p) for p in state.nextPieces)
        self.canvas.itemconfig(self.val_next, text=next_names)

        hex_lines = []
        for i in range(0, len(rawBytes), 16):
            chunk = rawBytes[i:i + 16]
            hex_str = " ".join(f"{b:02X}" for b in chunk)
            hex_lines.append(hex_str)

        for i, t in enumerate(self.hex_texts):
            if i < len(hex_lines):
                self.canvas.itemconfig(t, text=hex_lines[i])
            else:
                self.canvas.itemconfig(t, text="")

    def PollQueue(self):
        try:
            while True:
                state, rawBytes = self.dataQueue.get_nowait()
                self.UpdateData(state, rawBytes)
                
        except Exception:
            pass
        
        self.root.after(16, self.PollQueue)

    def Run(self):
        self.root.after(16, self.PollQueue)
        self.root.mainloop()
