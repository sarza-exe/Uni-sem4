import pygame
import math
import time
import sys

# --- Configuration ---
SCREEN_WIDTH = 800
SCREEN_HEIGHT = 700 # Increased height for UI elements
BOARD_AREA_HEIGHT = 500 # Area dedicated to the puzzle board
CONTROLS_AREA_HEIGHT = SCREEN_HEIGHT - BOARD_AREA_HEIGHT

BG_COLOR = (30, 30, 30)
TILE_COLOR = (70, 70, 180)
TILE_CORRECT_COLOR = (70, 180, 70)
EMPTY_COLOR = (50, 50, 50)
TEXT_COLOR = (230, 230, 230)
BUTTON_COLOR = (80, 80, 80)
BUTTON_HOVER_COLOR = (110, 110, 110)
BUTTON_TEXT_COLOR = (230, 230, 230)
SLIDER_BG_COLOR = (60, 60, 60)
SLIDER_HANDLE_COLOR = (150, 150, 150)
PROGRESS_BAR_BG_COLOR = (60, 60, 60)
PROGRESS_BAR_FG_COLOR = (70, 180, 70)

TILE_MARGIN = 10
ANIMATION_DURATION = 0.1  # seconds for one tile move

# --- Helper Functions ---
def load_solution(filename):
    """Loads puzzle states from a file."""
    states = []
    try:
        with open(filename, 'r') as f:
            for line in f:
                try:
                    state = list(map(int, line.strip().split(',')))
                    states.append(state)
                except ValueError:
                    print(f"Warning: Skipping invalid line in {filename}: {line.strip()}")
    except FileNotFoundError:
        print(f"Error: Solution file '{filename}' not found.")
        return None
    if not states:
        print(f"Error: No valid states found in '{filename}'.")
        return None
    return states

def get_puzzle_dimension(state):
    """Calculates the dimension (n) of the n x n puzzle."""
    length = len(state)
    n = int(math.sqrt(length))
    if n * n != length:
        raise ValueError("Invalid state length for a square puzzle.")
    return n

def get_tile_rect(board_x, board_y, tile_size, grid_r, grid_c):
    """Calculates the screen rectangle for a tile at grid_r, grid_c."""
    x = board_x + grid_c * (tile_size + TILE_MARGIN) + TILE_MARGIN
    y = board_y + grid_r * (tile_size + TILE_MARGIN) + TILE_MARGIN
    return pygame.Rect(x, y, tile_size, tile_size)

def get_target_solved_state(n):
    """Generates the target solved state for an n x n puzzle (0 is blank)."""
    return list(range(1, n * n)) + [0]

# --- UI Element Classes ---
class Button:
    def __init__(self, x, y, width, height, text, font, callback):
        self.rect = pygame.Rect(x, y, width, height)
        self.text = text
        self.font = font
        self.callback = callback
        self.is_hovered = False

    def draw(self, screen):
        color = BUTTON_HOVER_COLOR if self.is_hovered else BUTTON_COLOR
        pygame.draw.rect(screen, color, self.rect, border_radius=5)
        text_surf = self.font.render(self.text, True, BUTTON_TEXT_COLOR)
        text_rect = text_surf.get_rect(center=self.rect.center)
        screen.blit(text_surf, text_rect)

    def handle_event(self, event):
        if event.type == pygame.MOUSEMOTION:
            self.is_hovered = self.rect.collidepoint(event.pos)
        if event.type == pygame.MOUSEBUTTONDOWN:
            if event.button == 1 and self.is_hovered:
                self.callback()
                return True
        return False

class Slider:
    def __init__(self, x, y, width, height, min_val, max_val, initial_val, label=""):
        self.rect = pygame.Rect(x, y, width, height)
        self.min_val = min_val
        self.max_val = max_val
        self.current_val = initial_val
        self.handle_width = 10
        self.handle_rect = pygame.Rect(0, 0, self.handle_width, height + 10) # Taller handle
        self.handle_rect.centery = self.rect.centery
        self._update_handle_pos()
        self.is_dragging = False
        self.label = label
        self.font = pygame.font.Font(None, 24)


    def _update_handle_pos(self):
        # Map current_val to x-coordinate
        ratio = (self.current_val - self.min_val) / (self.max_val - self.min_val)
        self.handle_rect.centerx = self.rect.x + ratio * self.rect.width

    def _update_value_from_pos(self, x_pos):
        ratio = (x_pos - self.rect.x) / self.rect.width
        ratio = max(0, min(1, ratio)) # Clamp between 0 and 1
        self.current_val = self.min_val + ratio * (self.max_val - self.min_val)

    def draw(self, screen):
        # Draw label
        if self.label:
            label_surf = self.font.render(f"{self.label}: {self.current_val:.2f}x", True, TEXT_COLOR)
            label_rect = label_surf.get_rect(midright=(self.rect.left - 10, self.rect.centery))
            screen.blit(label_surf, label_rect)

        pygame.draw.rect(screen, SLIDER_BG_COLOR, self.rect, border_radius=3)
        pygame.draw.rect(screen, SLIDER_HANDLE_COLOR, self.handle_rect, border_radius=3)


    def handle_event(self, event):
        if event.type == pygame.MOUSEBUTTONDOWN:
            if event.button == 1 and self.handle_rect.collidepoint(event.pos):
                self.is_dragging = True
        elif event.type == pygame.MOUSEBUTTONUP:
            if event.button == 1:
                self.is_dragging = False
        elif event.type == pygame.MOUSEMOTION:
            if self.is_dragging:
                self._update_value_from_pos(event.pos[0])
                self._update_handle_pos()
        return self.is_dragging # Return true if event was "consumed" by dragging

    def get_value(self):
        return self.current_val

class ProgressBar:
    def __init__(self, x, y, width, height):
        self.rect = pygame.Rect(x, y, width, height)
        self.progress = 0  # 0.0 to 1.0

    def set_progress(self, progress):
        self.progress = max(0, min(1, progress))

    def draw(self, screen):
        pygame.draw.rect(screen, PROGRESS_BAR_BG_COLOR, self.rect, border_radius=3)
        fill_width = self.progress * self.rect.width
        fill_rect = pygame.Rect(self.rect.x, self.rect.y, fill_width, self.rect.height)
        pygame.draw.rect(screen, PROGRESS_BAR_FG_COLOR, fill_rect, border_radius=3)

# --- Main Game Class ---
class PuzzleReplay:
    def __init__(self, solution_file):
        pygame.init()
        self.screen = pygame.display.set_mode((SCREEN_WIDTH, SCREEN_HEIGHT))
        pygame.display.set_caption("Puzzle Replay")
        self.clock = pygame.time.Clock()
        self.font_large = pygame.font.Font(None, 48)
        self.font_small = pygame.font.Font(None, 30)
        self.font_tile = None # Will be set based on tile size

        self.solution_states = load_solution(solution_file)
        if not self.solution_states:
            self.running = False
            return

        self.n = get_puzzle_dimension(self.solution_states[0])
        self.target_solved_state = get_target_solved_state(self.n)

        self.current_state_index = 0
        self.is_playing = False
        self.play_pause_button_text = "Play"

        # Animation variables
        self.is_animating = False
        self.animation_start_time = 0
        self.anim_start_state = None
        self.anim_end_state = None
        self.moving_tile_value = -1
        self.moving_tile_start_rect = None
        self.moving_tile_end_rect = None

        self.board_x = 0
        self.board_y = 0
        self.tile_size = 0
        self._calculate_board_layout()

        self._init_ui()
        self.running = True

    def _calculate_board_layout(self):
        # Calculate tile size and board position to center it
        max_board_dim = min(SCREEN_WIDTH - 20, BOARD_AREA_HEIGHT - 20) # Max space for board
        self.tile_size = (max_board_dim - (self.n + 1) * TILE_MARGIN) // self.n

        board_width = self.n * self.tile_size + (self.n + 1) * TILE_MARGIN
        board_height = self.n * self.tile_size + (self.n + 1) * TILE_MARGIN

        self.board_x = (SCREEN_WIDTH - board_width) // 2
        self.board_y = (BOARD_AREA_HEIGHT - board_height) // 2

        # Adjust font size for tiles
        try:
            self.font_tile = pygame.font.Font(None, int(self.tile_size * 0.6))
        except: # Fallback if system font fails for some reason
            self.font_tile = pygame.font.SysFont("arial", int(self.tile_size * 0.5))


    def _init_ui(self):
        btn_w, btn_h = 120, 40
        btn_y = BOARD_AREA_HEIGHT + 20
        spacing = 20

        self.btn_start = Button(spacing, btn_y, btn_w, btn_h, "Go to Start", self.font_small, self.go_to_start)
        self.btn_prev = Button(spacing + btn_w + spacing, btn_y, btn_w, btn_h, "Prev", self.font_small, self.prev_state)
        self.btn_play_pause = Button(spacing + 2 * (btn_w + spacing), btn_y, btn_w, btn_h, self.play_pause_button_text, self.font_small, self.toggle_play_pause)
        self.btn_next = Button(spacing + 3 * (btn_w + spacing), btn_y, btn_w, btn_h, "Next", self.font_small, self.next_state)

        self.buttons = [self.btn_start, self.btn_prev, self.btn_play_pause, self.btn_next]

        slider_y = btn_y + btn_h + 30
        slider_width = 200
        self.speed_slider = Slider(SCREEN_WIDTH - slider_width - spacing - 70, slider_y, slider_width, 20, 0.25, 5.0, 1.0, "Speed") # Min 0.25x, Max 5x

        progress_bar_y = slider_y + 20 + 30
        self.progress_bar = ProgressBar(spacing, progress_bar_y, SCREEN_WIDTH - 2 * spacing, 20)


    def go_to_start(self):
        if self.is_animating: return
        self.current_state_index = 0
        self.is_playing = False
        self._update_play_pause_button()

    def prev_state(self):
        if self.is_animating: return
        if self.current_state_index > 0:
            self._start_animation(self.current_state_index, self.current_state_index - 1)
        self.is_playing = False
        self._update_play_pause_button()

    def next_state(self, auto_triggered=False):
        if self.is_animating and not auto_triggered : return # User click ignored if animating
        if self.is_animating and auto_triggered: # if auto triggered and already animating, let current finish
            if self.current_state_index < len(self.solution_states) - 1:
                # Queue next animation if playing
                self.queued_next_state = self.current_state_index + 1
            else:
                self.is_playing = False
                self._update_play_pause_button()
            return

        if self.current_state_index < len(self.solution_states) - 1:
            self._start_animation(self.current_state_index, self.current_state_index + 1)
        else: # Reached end
            self.is_playing = False
            self._update_play_pause_button()

        if not auto_triggered: # User clicked next
            self.is_playing = False
            self._update_play_pause_button()


    def toggle_play_pause(self):
        if self.is_animating and not self.is_playing: # If paused during animation, resume
            pass # Animation will continue

        self.is_playing = not self.is_playing
        self._update_play_pause_button()
        if self.is_playing and not self.is_animating and self.current_state_index < len(self.solution_states) - 1:
            # Start playing from current state if not at end and not already animating
            self.last_auto_step_time = time.time() # Reset timer for playback
            self.next_state(auto_triggered=True)


    def _update_play_pause_button(self):
        self.play_pause_button_text = "Pause" if self.is_playing else "Play"
        self.btn_play_pause.text = self.play_pause_button_text

    def _find_moving_tile(self, state1_flat, state2_flat):
        """ Finds the tile that moves and its start/end grid positions. """
        pos0_state1 = state1_flat.index(0)
        pos0_state2 = state2_flat.index(0)

        r0_s1, c0_s1 = divmod(pos0_state1, self.n)
        r0_s2, c0_s2 = divmod(pos0_state2, self.n)

        # The tile that moved is the one at the blank's new position in the old state.
        # It moves from its original position to the blank's old position.
        if pos0_state1 != pos0_state2: # If 0 moved
            moved_tile_value = state1_flat[pos0_state2] # Value of tile that is now where 0 was
            start_grid_r, start_grid_c = r0_s2, c0_s2 # This tile started at new blank pos
            end_grid_r, end_grid_c = r0_s1, c0_s1 # And moved to old blank pos
            return moved_tile_value, (start_grid_r, start_grid_c), (end_grid_r, end_grid_c)
        return None, None, None


    def _start_animation(self, from_idx, to_idx):
        self.is_animating = True
        self.animation_start_time = time.time()
        self.anim_start_state_idx = from_idx
        self.anim_end_state_idx = to_idx

        start_flat = self.solution_states[from_idx]
        end_flat = self.solution_states[to_idx]

        moved_val, start_grid_pos, end_grid_pos = self._find_moving_tile(start_flat, end_flat)

        if moved_val is not None:
            self.moving_tile_value = moved_val
            self.moving_tile_start_rect = get_tile_rect(self.board_x, self.board_y, self.tile_size, start_grid_pos[0], start_grid_pos[1])
            self.moving_tile_end_rect = get_tile_rect(self.board_x, self.board_y, self.tile_size, end_grid_pos[0], end_grid_pos[1])
        else: # Should not happen if states are consecutive steps of a solution
            self.is_animating = False
            self.current_state_index = to_idx # Just snap to the state


    def _update_animation(self):
        if not self.is_animating:
            return

        elapsed_time = time.time() - self.animation_start_time
        progress = min(elapsed_time / ANIMATION_DURATION, 1.0)

        if progress >= 1.0:
            self.is_animating = False
            self.current_state_index = self.anim_end_state_idx
            self.moving_tile_value = -1 # Reset
            # If playing, trigger next step after a delay based on speed
            if self.is_playing:
                self.last_auto_step_time = time.time() # Reset timer for next step
        return progress


    def _draw_board(self, state_flat, animation_progress=None):
        # Draw static part of the board (from anim_start_state if animating)
        base_state_to_draw = state_flat
        if self.is_animating and self.anim_start_state_idx is not None:
            base_state_to_draw = self.solution_states[self.anim_start_state_idx]


        for i, tile_val in enumerate(base_state_to_draw):
            r, c = divmod(i, self.n)
            tile_rect = get_tile_rect(self.board_x, self.board_y, self.tile_size, r, c)

            if self.is_animating and tile_val == self.moving_tile_value:
                continue # Skip drawing the moving tile here, will be drawn separately

            if tile_val == 0:
                pygame.draw.rect(self.screen, EMPTY_COLOR, tile_rect, border_radius=3)
            else:
                is_correct = (tile_val == self.target_solved_state[i])
                color = TILE_CORRECT_COLOR if is_correct else TILE_COLOR
                pygame.draw.rect(self.screen, color, tile_rect, border_radius=3)

                if self.font_tile:
                    text_surf = self.font_tile.render(str(tile_val), True, TEXT_COLOR)
                    text_rect = text_surf.get_rect(center=tile_rect.center)
                    self.screen.blit(text_surf, text_rect)

        # Draw the moving tile if animating
        if self.is_animating and self.moving_tile_value != -1 and animation_progress is not None:
            curr_x = self.moving_tile_start_rect.x + (self.moving_tile_end_rect.x - self.moving_tile_start_rect.x) * animation_progress
            curr_y = self.moving_tile_start_rect.y + (self.moving_tile_end_rect.y - self.moving_tile_start_rect.y) * animation_progress
            animated_rect = pygame.Rect(curr_x, curr_y, self.tile_size, self.tile_size)

            # Determine if moving tile is correct in its *final* animated position
            final_pos_idx = self.solution_states[self.anim_end_state_idx].index(self.moving_tile_value)
            is_correct_at_end = (self.moving_tile_value == self.target_solved_state[final_pos_idx])
            color = TILE_CORRECT_COLOR if is_correct_at_end else TILE_COLOR

            pygame.draw.rect(self.screen, color, animated_rect, border_radius=3)
            if self.font_tile:
                text_surf = self.font_tile.render(str(self.moving_tile_value), True, TEXT_COLOR)
                text_rect = text_surf.get_rect(center=animated_rect.center)
                self.screen.blit(text_surf, text_rect)


    def _draw_ui(self):
        # Draw a background for the control panel
        controls_bg_rect = pygame.Rect(0, BOARD_AREA_HEIGHT, SCREEN_WIDTH, CONTROLS_AREA_HEIGHT)
        pygame.draw.rect(self.screen, (40,40,40), controls_bg_rect)


        for btn in self.buttons:
            btn.draw(self.screen)
        self.speed_slider.draw(self.screen)
        self.progress_bar.draw(self.screen)


    def run(self):
        self.last_auto_step_time = time.time()
        self.queued_next_state = None # For smoother playback

        while self.running:
            animation_progress = None
            for event in pygame.event.get():
                if event.type == pygame.QUIT:
                    self.running = False

                # Give UI elements priority for events, especially slider dragging
                slider_consumed_event = self.speed_slider.handle_event(event)
                if slider_consumed_event: # If slider is being dragged, don't process button clicks
                    continue

                for btn in self.buttons:
                    if btn.handle_event(event):
                        break # Event handled by a button

            # --- Update ---
            if self.is_animating:
                animation_progress = self._update_animation()
            elif self.is_playing: # Not animating, but in play mode
                # Calculate delay based on speed slider
                # Speed value is multiplier. 1.0x means 1 step per ANIMATION_DURATION + some base pause.
                # A higher speed value means shorter delay.
                base_pause_duration = 0.1 # Small pause even at high speeds between animations
                effective_animation_time = ANIMATION_DURATION / self.speed_slider.get_value()
                time_for_next_step = self.last_auto_step_time + effective_animation_time + base_pause_duration

                if time.time() >= time_for_next_step:
                    if self.current_state_index < len(self.solution_states) - 1:
                        self.next_state(auto_triggered=True)
                        self.last_auto_step_time = time.time() # Reset timer after starting animation
                    else:
                        self.is_playing = False # Reached end
                        self._update_play_pause_button()

            # Update progress bar
            if self.solution_states and len(self.solution_states) > 1:
                current_prog_idx = self.current_state_index
                if self.is_animating and animation_progress is not None: # Smooth progress during animation
                    current_prog_idx = self.anim_start_state_idx + (self.anim_end_state_idx - self.anim_start_state_idx) * animation_progress

                self.progress_bar.set_progress(current_prog_idx / (len(self.solution_states) - 1))
            else:
                self.progress_bar.set_progress(0)


            # --- Draw ---
            self.screen.fill(BG_COLOR)
            current_display_state = self.solution_states[self.current_state_index]
            self._draw_board(current_display_state, animation_progress)
            self._draw_ui()

            pygame.display.flip()
            self.clock.tick(60) # Target 60 FPS

        pygame.quit()

# --- Main Execution ---
if __name__ == '__main__':

    solution_file = sys.argv[1]

    replay_game = PuzzleReplay(solution_file)
    if replay_game.running:
        replay_game.run()