#include "crane.h"

void init_crane(struct Crane *crane, SDL_Renderer *renderer) {
	/**
	Initializes the crane by setting its properties with initial values.
	The crane will be positioned at the beginning out of the borders of
	the window.
	**/
	int texture_w, texture_h;

	crane->renderer = renderer;
	crane->texture = IMG_LoadTexture(renderer, "res/crane.png");
	SDL_QueryTexture(crane->texture, NULL, NULL, &texture_w, &texture_h);

	crane->dstrect.w = texture_w;
	crane->dstrect.h = texture_h;
        crane->dstrect.x = -texture_w;
        crane->dstrect.y = 0;

        crane->l_upd_time_vrt =
        crane->last_slide_time = SDL_GetTicks();
        crane->sliding = false;

	crane->current_block = NULL;
}

void draw_pipe(SDL_Renderer *renderer) {
	/**
	Draws the pipe on which the crane slides.
	**/
	SDL_SetRenderDrawColor(renderer, 0, 0, 0, 0);
	int pipe_width = 5;
	for(int i = 0; i < pipe_width; i++){
		SDL_RenderDrawLine(renderer, 0, WINDOW_HEIGHT / 30 + i, WINDOW_WIDTH, WINDOW_HEIGHT / 30 + i);
		SDL_RenderDrawLine(renderer, 0, WINDOW_HEIGHT / 15 + i, WINDOW_WIDTH, WINDOW_HEIGHT / 15 + i);
	}
	SDL_SetRenderDrawColor(renderer, DEFAULT_DRAW_COLOR);
}

void draw_crane(struct Crane *crane) {
	/**
	Draws the crane.
	**/
	SDL_RenderCopy(crane->renderer, crane->texture, NULL, &crane->dstrect);
}

void slide(struct Crane *crane) {
	/**
	Slides the crane from the left of the screen to the right and viceversa.
	**/
        if(!crane->sliding)
                return;

	/// Updates the position of the crane only if the last update time happened
	/// more than 1000/UPD milliseconds ago.
        if(SDL_GetTicks() - crane->l_upd_time_vrt > 1000/UPD){
                crane->l_upd_time_vrt = SDL_GetTicks();
                if(crane->dir == RIGHT) {
                        crane->dstrect.x += VELOCITY;
			if (crane->current_block->dstrect.x > crane->target_x) {
				crane->current_block->dstrect.x = crane->target_x;
				crane->current_block->falling = true;
			}
                } else {
                        crane->dstrect.x -= VELOCITY;
			if (crane->current_block->dstrect.x < crane->target_x) {
				crane->current_block->dstrect.x = crane->target_x;
				crane->current_block->falling = true;
			}
		}
		/// Sets the coordinates of the current block being held by the
		/// crane to be equal to the crane coordinates.
		/// The crate will be drawn between the crane's claws.
		if (crane->current_block->falling != true) {
			crane->current_block->dstrect.x = crane->dstrect.x + (crane->dstrect.w / 2) - (crane->current_block->dstrect.w / 2);
			crane->current_block->dstrect.y = crane->dstrect.y + crane->dstrect.h - (crane->current_block->dstrect.h / 2);
		}
	}
}

void generate_crate(struct Crane *crane, struct Block *blocks[], int *number_of_blocks) {
	/**
	Drops the crate that is being held by the crane.
	**/
        if((crane->dstrect.x > 0 && crane->dstrect.x < WINDOW_HEIGHT)
         || SDL_GetTicks() - crane->last_slide_time < 6000)
                return;

        srand(time(NULL));
	/// Generates a random number and takes a random choice of the crane
	/// direction (so either LEFT or RIGHT). If it is LEFT, then
	/// the crane's x coordinate will be set to the WINDOW_WIDTH initially,
	/// and if the direction is RIGHT, then the x coordinate will be set
	/// to the negative value of the width of the texture. This means that
	/// the crane will be positioned exactly near the borders of the window,
	/// but outside of the window.
        if((crane->dir = rand() % 2) == LEFT)
                crane->dstrect.x = WINDOW_WIDTH;
	else
		crane->dstrect.x = -crane->dstrect.w;

        crane->sliding = true;
        crane->last_slide_time = SDL_GetTicks();
        /**
         * crane->target_x = (rand() % (WINDOW_HEIGHT / BLOCK_SIZE))
         * ...
	 * struct Block
 	crane->target_x = (rand() % (WINDOW_WIDTH / BLOCK_SIZE))
 	printf("The x coordinate of the target point of the crane: ", crane->target_x)
         * **/

	crane->target_x = rand() % WINDOW_WIDTH;
	struct Block *block = create_block();
	init_block(block, crane->target_x, WINDOW_HEIGHT - 64, crane->renderer);
	add_block_to_block_array(block, blocks, number_of_blocks);
	printf("Number of blocks: %d\n", *number_of_blocks);
	crane->current_block = block;
	block->falling = false; /// CAREFUL, YOU HAVE ALREADY ADDED THE BLOCK TO THE ARRAY!!!!

        printf("The x coordinate of the target point of the crane: %d\n", crane->target_x);
}
