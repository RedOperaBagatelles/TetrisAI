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
    def __init__(self, data_queue: Queue):
        self.data_queue = data_queue
        self.root = tk.Tk()
        self.root.title("Tetris AI - Client Data Viewer")
        self.root.configure(bg="#1e1e1e")
        self.root.resizable(False, False)

        self.bw = GameState.BOARD_WIDTH
        self.bh = GameState.BOARD_HEIGHT

        canvas_w = BOARD_X * 2 + self.bw * CELL_SIZE + 340
        canvas_h = BOARD_Y + self.bh * CELL_SIZE + 60
        self.canvas = tk.Canvas(
            self.root,
            width=canvas_w,
            height=canvas_h,
            bg="#1e1e1e",
            highlightthickness=0,
        )
        self.canvas.pack(padx=5, pady=5)

        self.info_x = BOARD_X + self.bw * CELL_SIZE + 30

        self._draw_board_border()
        self._init_board_cells()
        self._init_info_texts()

    def _draw_board_border(self):
        x1 = BOARD_X - 2
        y1 = BOARD_Y - 2
        x2 = BOARD_X + self.bw * CELL_SIZE + 2
        y2 = BOARD_Y + self.bh * CELL_SIZE + 2
        self.canvas.create_rectangle(x1, y1, x2, y2, outline="#666666", width=2)

    def _init_board_cells(self):
        self.cells = []
        for y in range(self.bh):
            row = []
            for x in range(self.bw):
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

    def _init_info_texts(self):
        mono = tkfont.Font(family="Consolas", size=10)
        mono_b = tkfont.Font(family="Consolas", size=11, weight="bold")
        x = self.info_x
        y = BOARD_Y

        self.canvas.create_text(
            x, y, text="[ AI Output Data ]",
            fill="#00cccc", font=mono_b, anchor="nw",
        )
        y += 30

        def make_label(text, yy):
            self.canvas.create_text(x, yy, text=text, fill="#888888", font=mono, anchor="nw")

        def make_value(yy):
            return self.canvas.create_text(x + 120, yy, text="-", fill="#dddddd", font=mono, anchor="nw")

        make_label("Message Size:", y)
        self.val_size = make_value(y)
        y += 22

        make_label("Current Piece:", y)
        self.val_current = make_value(y)
        y += 20

        make_label("Rotation:", y)
        self.val_rotation = make_value(y)
        y += 20

        make_label("Position:", y)
        self.val_position = make_value(y)
        y += 25

        make_label("Hold Piece:", y)
        self.val_hold = make_value(y)
        y += 22

        make_label("Next Pieces:", y)
        self.val_next = make_value(y)
        y += 30

        self.canvas.create_text(x, y, text="Raw Bytes (hex):", fill="#aaaaaa", font=mono, anchor="nw")
        y += 20

        self.hex_texts = []
        for _ in range(16):
            t = self.canvas.create_text(x, y, text="", fill="#7aac7a", font=mono, anchor="nw")
            self.hex_texts.append(t)
            y += 15

    def _piece_name(self, pt):
        try:
            return PieceType(pt).name
        except ValueError:
            return f"Unknown({pt})"

    def _piece_color(self, pt):
        try:
            p = PieceType(pt)
            return PIECE_COLORS.get(p, "#555555")
        except ValueError:
            return "#555555"

    def update_data(self, state, raw_bytes):
        for y in range(self.bh):
            for x in range(self.bw):
                cell_val = state.board[y][x]
                color = self._piece_color(cell_val)
                self.canvas.itemconfig(self.cells[y][x], fill=color)

        cp = state.current_piece
        self.canvas.itemconfig(self.val_size, text=f"{len(raw_bytes)} bytes")
        self.canvas.itemconfig(
            self.val_current,
            text=self._piece_name(cp.type),
            fill=self._piece_color(cp.type),
        )
        self.canvas.itemconfig(self.val_rotation, text=str(cp.rotation))
        self.canvas.itemconfig(self.val_position, text=f"({cp.x}, {cp.y})")

        hp = state.hold_piece
        self.canvas.itemconfig(
            self.val_hold,
            text=self._piece_name(hp),
            fill=self._piece_color(hp),
        )

        next_names = " ".join(self._piece_name(p) for p in state.next_pieces)
        self.canvas.itemconfig(self.val_next, text=next_names)

        hex_lines = []
        for i in range(0, len(raw_bytes), 16):
            chunk = raw_bytes[i:i + 16]
            hex_str = " ".join(f"{b:02X}" for b in chunk)
            hex_lines.append(hex_str)

        for i, t in enumerate(self.hex_texts):
            if i < len(hex_lines):
                self.canvas.itemconfig(t, text=hex_lines[i])
            else:
                self.canvas.itemconfig(t, text="")

    def _poll_queue(self):
        try:
            while True:
                state, raw_bytes = self.data_queue.get_nowait()
                self.update_data(state, raw_bytes)
        except Exception:
            pass
        self.root.after(16, self._poll_queue)

    def run(self):
        self.root.after(16, self._poll_queue)
        self.root.mainloop()
