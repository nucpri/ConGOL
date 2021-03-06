//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// ConGOL.cpp : This file contains the 'main' function. Program execution begins and ends there.                                                            //
//                                                                                                                                                          //
//  PURPOSE:                                                                                                                                                //
//  A recreation of Conway's Game of Life, powered by OpenGL                                                                                                //
//  v1.0: 12.06.2021                                                                                                                                        //
//                                                                                                                                                          //
//  Rules (https://en.wikipedia.org/wiki/Conway%27s_Game_of_Life, as of 01.06.2021):                                                                        //
//                                                                                                                                                          //
//  The universe of the Game of Life is an infinite, two-dimensional orthogonal grid of square cells_, each of which is in one of two possible states:       //
//  live or dead, (or populated and unpopulated, respectively).                                                                                             //
//                                                                                                                                                          //
//  Every cell interacts with its eight neighbours, which are the cells_ that are horizontally, vertically, or diagonally adjacent.                          //
//  At each step in time, the following transitions occur:                                                                                                  //
//  - Any live cell with fewer than two live neighbours dies, as if by underpopulation.                                                                     //
//  - Any live cell with two or three live neighbours lives on to the next generation.                                                                      //
//  - Any live cell with more than three live neighbours dies, as if by overpopulation.                                                                     //
//  - Any dead cell with exactly three live neighbours becomes a live cell, as if by reproduction.                                                          //
//                                                                                                                                                          //
//  These rules, which compare the behavior of the automaton to real life, can be condensed into the following :                                            //
//  - Any live cell with two or three live neighbours survives.                                                                                             //
//  - Any dead cell with three live neighbours becomes a live cell.                                                                                         //
//  - All other live cells_ die in the next generation.Similarly, all other dead cells_ stay dead.                                                            //
//                                                                                                                                                          //
//  The initial pattern constitutes the seed of the system:                                                                                                 //
//  The first generation is created by applying the above rules simultaneously to every cell in the seed; births and deaths occur simultaneously,           //
//  and the discrete moment at which this happens is sometimes called a tick. Each generation is a pure function of the preceding one.                      //
//  The rules continue to be applied repeatedly to create further generations.                                                                              //
//                                                                                                                                                          //
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include "Grid.h"
#include "Utils.h"

#include <fan/graphics/graphics.h>
#include <thread>


//* To-Do *\\
// 
// GENERAL:
// - Zoom +/-
// - Measuring tape (in square units)
// 
//  Known bugs:
//  - Crash if a tick causes a cell generation that overlaps a corner .. corners overlap handling, TBD!
//  - Not a bug, but tickrate works counter-intuitively; lowering increases simulation speed & vice versa


int main()
{
    //  Grid divisor
    int subdivs = 50;

	fan::window_t window;
	window.open(Utils::FloorToPerfectSquare(fan::get_screen_resolution()) - 100, "Conway's Game of Life");
	fan::opengl::context_t context;
	context.init();
  context.bind_to_window(&window);
  context.set_viewport(0, window.get_size());
	window.add_resize_callback(&context, [](fan::window_t*, const fan::vec2i& s, void* userptr) {
		((fan::opengl::context_t*)userptr)->set_viewport(0, s);
	});


  window.set_max_fps(165);
  context.set_vsync(&window, false);
    
    
  fan::set_console_visibility(false);

  Grid grid(&window, &context, subdivs);

	/* Key bindings */
	window.add_key_callback(fan::mouse_left, fan::key_state::press, &grid, [](fan::window_t*, uint16_t key, void* userptr) { 
		((Grid*)userptr)->paintingLive = true;
	});
	window.add_key_callback(fan::mouse_left, fan::key_state::release, &grid, [](fan::window_t*, uint16_t key, void* userptr) { 
		((Grid*)userptr)->paintingLive = false; 
	});

	window.add_key_callback(fan::mouse_right, fan::key_state::press, &grid, [](fan::window_t*, uint16_t key, void* userptr) { 
		((Grid*)userptr)->paintingDead = true; 
	});
	window.add_key_callback(fan::mouse_right, fan::key_state::release, &grid, [](fan::window_t*, uint16_t key, void* userptr) { 
		((Grid*)userptr)->paintingDead = false; 
	});

	// ESC: Close game
	window.add_key_callback(fan::key_escape, fan::key_state::press, nullptr, [](fan::window_t* w, uint16_t key, void* userptr) {
		w->close();
	});

	// F: Toggle FPS
	window.add_key_callback(fan::key_f, fan::key_state::press, &grid, [](fan::window_t* w, uint16_t key, void* userptr) { 
		Grid& grid = *(Grid*)userptr;
		grid.show_fps = !grid.show_fps; grid.window->set_name("Conway's Game of Life"); 
	});

	// Space: Toggle simulation
	window.add_key_callback(fan::key_space, fan::key_state::press, &grid, [](fan::window_t* w, uint16_t key, void* userptr) { 
		((Grid*)userptr)->toggle_simulation(); 
	});

	// Shift+T+ScrollUp: Evolve or forward to next generation depending on if the generation is already recorded or not. 
	// Shift+T+ScrollDown: Devolve to earlier generation if it exists
	window.add_keys_callback(&grid, [](fan::window_t*, uint16_t key, fan::key_state, void* userptr) {
		Grid& grid = *(Grid*)userptr;
		if (grid.window->key_press(fan::key_t) && key == fan::mouse_scroll_up) {
			grid.evolve();
		}
		else if (grid.window->key_press(fan::key_t) && key == fan::mouse_scroll_down) {
			grid.devolve();
		}
	});

  grid.run();
}