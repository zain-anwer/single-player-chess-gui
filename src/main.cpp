#include <SDL2/SDL.h>
#include <cstdint>
#include <iostream>
#include "chess/board.hpp"
#include "utils/audio.hpp"
#include "chess/dynamics.hpp"

#define SWITCH_DELAY_FRAMES 25

using namespace std;

static const SDL_Rect RESTART_BUTTON = {(CHESS_WINDOW_WIDTH - 390) / 2, 435, 250, 56};
static const SDL_Rect QUIT_BUTTON = {RESTART_BUTTON.x + 270, 435, 120, 56};

static const uint8_t FONT[26][7] = {
	{0x0e,0x11,0x11,0x1f,0x11,0x11,0x11}, {0x1e,0x11,0x11,0x1e,0x11,0x11,0x1e},
	{0x0e,0x11,0x10,0x10,0x10,0x11,0x0e}, {0x1e,0x11,0x11,0x11,0x11,0x11,0x1e},
	{0x1f,0x10,0x10,0x1e,0x10,0x10,0x1f}, {0x1f,0x10,0x10,0x1e,0x10,0x10,0x10},
	{0x0e,0x11,0x10,0x17,0x11,0x11,0x0f}, {0x11,0x11,0x11,0x1f,0x11,0x11,0x11},
	{0x0e,0x04,0x04,0x04,0x04,0x04,0x0e}, {0x07,0x02,0x02,0x02,0x12,0x12,0x0c},
	{0x11,0x12,0x14,0x18,0x14,0x12,0x11}, {0x10,0x10,0x10,0x10,0x10,0x10,0x1f},
	{0x11,0x1b,0x15,0x15,0x11,0x11,0x11}, {0x11,0x19,0x15,0x13,0x11,0x11,0x11},
	{0x0e,0x11,0x11,0x11,0x11,0x11,0x0e}, {0x1e,0x11,0x11,0x1e,0x10,0x10,0x10},
	{0x0e,0x11,0x11,0x11,0x15,0x12,0x0d}, {0x1e,0x11,0x11,0x1e,0x14,0x12,0x11},
	{0x0f,0x10,0x10,0x0e,0x01,0x01,0x1e}, {0x1f,0x04,0x04,0x04,0x04,0x04,0x04},
	{0x11,0x11,0x11,0x11,0x11,0x11,0x0e}, {0x11,0x11,0x11,0x11,0x11,0x0a,0x04},
	{0x11,0x11,0x11,0x15,0x15,0x15,0x0a}, {0x11,0x11,0x0a,0x04,0x0a,0x11,0x11},
	{0x11,0x11,0x0a,0x04,0x04,0x04,0x04}, {0x1f,0x01,0x02,0x04,0x08,0x10,0x1f}
};

static void drawText(SDL_Renderer *renderer, const string &text, int x, int y, int scale, SDL_Color color)
{
	SDL_SetRenderDrawColor(renderer, color.r, color.g, color.b, color.a);
	int cursor_x = x;
	for (char character : text)
	{
		if (character == ' ')
		{
			cursor_x += 4 * scale;
			continue;
		}

		if (character >= 'A' && character <= 'Z')
		{
			const uint8_t *glyph = FONT[character - 'A'];
			for (int row = 0; row < 7; row++)
			{
				for (int column = 0; column < 5; column++)
				{
					if (glyph[row] & (1 << (4 - column)))
					{
						SDL_Rect pixel = {cursor_x + column * scale, y + row * scale, scale, scale};
						SDL_RenderFillRect(renderer, &pixel);
					}
				}
			}
		}
		cursor_x += 6 * scale;
	}
}

static int textWidth(const string &text, int scale)
{
	int width = 0;
	for (char character : text)
		width += (character == ' ' ? 4 : 6) * scale;
	return width - scale;
}

static int capturedPieceImageIndex(const string &piece)
{
	if (piece == "King") return 0;
	if (piece == "Queen") return 1;
	if (piece == "Rook") return 2;
	if (piece == "Bishop") return 3;
	if (piece == "Knight") return 4;
	if (piece == "Pawn") return 5;
	return -1;
}

static void loadCapturedPieceTextures(SDL_Renderer *renderer, SDL_Texture *textures[2][6])
{
	for (int color = 0; color < 2; color++)
	{
		for (int piece = 0; piece < 6; piece++)
		{
			SDL_Surface *surface = SDL_LoadBMP(images[color][piece].c_str());
			if (surface == nullptr)
			{
				cerr << "Unable to load captured-piece image " << images[color][piece]
					<< ": " << SDL_GetError() << '\n';
				continue;
			}

			textures[color][piece] = SDL_CreateTextureFromSurface(renderer, surface);
			SDL_FreeSurface(surface);
			if (textures[color][piece] != nullptr)
				SDL_SetTextureScaleMode(textures[color][piece], SDL_ScaleModeLinear);
		}
	}
}

static void drawCapturedPanel(SDL_Renderer *renderer, const SDL_Rect &panel, const string &owner,
	const vector<string> &captured_pieces, int piece_color, SDL_Texture *textures[2][6])
{
	SDL_SetRenderDrawColor(renderer, 58, 34, 20, SDL_ALPHA_OPAQUE);
	SDL_RenderFillRect(renderer, &panel);
	SDL_Rect inner = {panel.x + 4, panel.y + 4, panel.w - 8, panel.h - 8};
	SDL_SetRenderDrawColor(renderer, 142, 88, 48, SDL_ALPHA_OPAQUE);
	SDL_RenderFillRect(renderer, &inner);

	SDL_SetRenderDrawColor(renderer, 184, 128, 75, SDL_ALPHA_OPAQUE);
	for (int grain = 0; grain < 7; grain++)
	{
		int y = panel.y + 56 + grain * 94;
		SDL_RenderDrawLine(renderer, panel.x + 8, y, panel.x + panel.w - 8, y);
	}

	drawText(renderer, owner, panel.x + (panel.w - textWidth(owner, 2)) / 2, panel.y + 15, 2,
		{250, 229, 196, SDL_ALPHA_OPAQUE});
	SDL_SetRenderDrawColor(renderer, 75, 45, 27, SDL_ALPHA_OPAQUE);
	SDL_RenderDrawLine(renderer, panel.x + 12, panel.y + 45, panel.x + panel.w - 12, panel.y + 45);

	for (int slot = 0; slot < 16; slot++)
	{
		int column = slot % 2;
		int row = slot / 2;

		if (slot >= static_cast<int>(captured_pieces.size()))
			continue;

		int image_index = capturedPieceImageIndex(captured_pieces[slot]);
		if (image_index < 0 || textures[piece_color][image_index] == nullptr)
			continue;

		SDL_Rect piece_image = {panel.x + 14 + column * 84, panel.y + 58 + row * 80,
			CHESS_TILE_SIZE, CHESS_TILE_SIZE};
		SDL_RenderCopy(renderer, textures[piece_color][image_index], nullptr, &piece_image);
	}
}

static void drawCapturedPanels(SDL_Renderer *renderer, ChessInterface &interface,
	SDL_Texture *textures[2][6])
{
	constexpr int panel_width = 184;
	constexpr int panel_height = 720;
	constexpr int board_gap = 18;
	const int panel_y = (CHESS_WINDOW_HEIGHT - panel_height) / 2;
	const SDL_Rect left_panel = {
		CHESS_BOARD_X - CHESS_BOARD_BORDER - board_gap - panel_width,
		panel_y, panel_width, panel_height};
	const SDL_Rect right_panel = {
		CHESS_BOARD_X + CHESS_BOARD_SIZE + CHESS_BOARD_BORDER + board_gap,
		panel_y, panel_width, panel_height};
	vector<string> white_pieces = interface.read_captured_pieces("White");
	vector<string> black_pieces = interface.read_captured_pieces("Black");

	// Black's right is screen-left; White's right is screen-right when the board is oriented to that player.
	drawCapturedPanel(renderer, left_panel, "BLACK", white_pieces, WHITE, textures);
	drawCapturedPanel(renderer, right_panel, "WHITE", black_pieces, BLACK, textures);
}

static void drawGameOverDialog(SDL_Renderer *renderer, int result, int turn)
{
	string title = result == CHECKMATE ? "CHECKMATE" : "STALEMATE";
	string message = result == CHECKMATE ? (turn == WHITE ? "BLACK WINS" : "WHITE WINS") : "DRAW";
	SDL_Rect panel = {(CHESS_WINDOW_WIDTH - 520) / 2, (CHESS_WINDOW_HEIGHT - 260) / 2, 520, 260};

	SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
	SDL_SetRenderDrawColor(renderer, 15, 20, 19, 165);
	SDL_Rect scrim = {0, 0, CHESS_WINDOW_WIDTH, CHESS_WINDOW_HEIGHT};
	SDL_RenderFillRect(renderer, &scrim);
	SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_NONE);

	SDL_SetRenderDrawColor(renderer, 34, 41, 38, SDL_ALPHA_OPAQUE);
	SDL_RenderFillRect(renderer, &panel);
	SDL_Rect inner_panel = {panel.x + 3, panel.y + 3, panel.w - 6, panel.h - 6};
	SDL_SetRenderDrawColor(renderer, 241, 236, 219, SDL_ALPHA_OPAQUE);
	SDL_RenderFillRect(renderer, &inner_panel);

	int title_scale = 4;
	drawText(renderer, title, (CHESS_WINDOW_WIDTH - textWidth(title, title_scale)) / 2, panel.y + 30, title_scale,
		{35, 43, 39, SDL_ALPHA_OPAQUE});
	drawText(renderer, message, (CHESS_WINDOW_WIDTH - textWidth(message, 2)) / 2, panel.y + 85, 2,
		{91, 49, 39, SDL_ALPHA_OPAQUE});

	SDL_SetRenderDrawColor(renderer, 62, 115, 82, SDL_ALPHA_OPAQUE);
	SDL_RenderFillRect(renderer, &RESTART_BUTTON);
	SDL_SetRenderDrawColor(renderer, 73, 76, 70, SDL_ALPHA_OPAQUE);
	SDL_RenderFillRect(renderer, &QUIT_BUTTON);

	string restart_label = "RESTART GAME";
	drawText(renderer, restart_label,
		RESTART_BUTTON.x + (RESTART_BUTTON.w - textWidth(restart_label, 2)) / 2,
		RESTART_BUTTON.y + 21, 2, {255, 255, 255, SDL_ALPHA_OPAQUE});
	drawText(renderer, "QUIT", QUIT_BUTTON.x + (QUIT_BUTTON.w - textWidth("QUIT", 2)) / 2,
		QUIT_BUTTON.y + 21, 2, {255, 255, 255, SDL_ALPHA_OPAQUE});
}

static void restartGame(Board &board, ChessInterface &interface, Dynamics &dynamics)
{
	dynamics.flipBoard(board);
	dynamics.pawn_promotion = false;
	board.resetBoard();
	interface.reset_game();
	dynamics.src_i = dynamics.src_j = dynamics.dest_i = dynamics.dest_j = -1;
}

int main()
{
	SDL_Renderer* renderer;
	SDL_Window* window;

	ChessInterface interface;

	if (SDL_Init(SDL_INIT_EVERYTHING) != 0)
	{
		cerr << "SDL initialization failed: " << SDL_GetError() << '\n';
		return 1;
	}
	
	SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "1");

	window = SDL_CreateWindow("-- Chess --", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
		CHESS_WINDOW_WIDTH, CHESS_WINDOW_HEIGHT, SDL_WINDOW_SHOWN);
	if (window == nullptr)
	{
		cerr << "SDL window creation failed: " << SDL_GetError() << '\n';
		SDL_Quit();
		return 1;
	}

	renderer = SDL_CreateRenderer(window,-1,SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
	if (renderer == nullptr)
	{
		cerr << "Accelerated SDL renderer unavailable: " << SDL_GetError() << '\n';
		renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_SOFTWARE);
	}
	if (renderer == nullptr)
	{
		cerr << "SDL renderer creation failed: " << SDL_GetError() << '\n';
		SDL_DestroyWindow(window);
		SDL_Quit();
		return 1;
	}

	SDL_Texture *captured_piece_textures[2][6] = {};
	loadCapturedPieceTextures(renderer, captured_piece_textures);
	
	Board B1;

	bool running = true;

	Dynamics D(&interface);

	Audio audio;

	SDL_Point p;

	int select_result = 0;
	int switch_timer = 0;
	bool switch_pending = false;
	bool game_over = false;


	while(running)
	{
		SDL_Event event;
		while (SDL_PollEvent(&event))
		{
			if (event.type == SDL_QUIT)
				running = false;

			if (!switch_pending && game_over && event.type == SDL_KEYDOWN)
			{
				if (event.key.keysym.sym == SDLK_ESCAPE)
					running = false;
				else if (event.key.keysym.sym == SDLK_RETURN)
				{
					restartGame(B1, interface, D);
					select_result = INVALID_CHOICE;
					game_over = false;
				}
			}
			
			if (!switch_pending && event.type == SDL_MOUSEBUTTONDOWN)
			{
				if (event.button.button == SDL_BUTTON_LEFT)
				{
					if (game_over)
					{
						SDL_Point click = {event.button.x, event.button.y};
						if (SDL_PointInRect(&click, &RESTART_BUTTON))
						{
							restartGame(B1, interface, D);
							select_result = INVALID_CHOICE;
							game_over = false;
						}
						else if (SDL_PointInRect(&click, &QUIT_BUTTON))
							running = false;
					}
					else
					{
						p.x = event.button.x;
						p.y = event.button.y;
						select_result = D.select(&p, B1);
						audio.playSound(select_result);

						if (select_result == VALID_MOVE || select_result == VALID_CAPTURE ||
							select_result == CHECKMATE || select_result == STALEMATE)
						{
							switch_timer = 0;
							switch_pending = true;
						}
					}
				}
			}				
		}

		if (switch_pending && ++switch_timer > SWITCH_DELAY_FRAMES)
		{
			D.flipBoard(B1);
			switch_pending = false;
			if (select_result == CHECKMATE || select_result == STALEMATE)
				game_over = true;
		}

		B1.drawBoard(renderer);
		drawCapturedPanels(renderer, interface, captured_piece_textures);
		if (game_over)
			drawGameOverDialog(renderer, select_result, interface.turn);
		SDL_RenderPresent(renderer);
	}

	for (int color = 0; color < 2; color++)
		for (int piece = 0; piece < 6; piece++)
			if (captured_piece_textures[color][piece] != nullptr)
				SDL_DestroyTexture(captured_piece_textures[color][piece]);
	audio.shutdown();
		
	SDL_DestroyRenderer(renderer);
			
	SDL_DestroyWindow(window);
		
	SDL_Quit();
				
	return 0;
}
