/* Raw - Another World Interpreter
 * Copyright (C) 2004 Gregory Montoir
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.

 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.

 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
 */

#include "engine.h"
#include "file.h"
#include "serializer.h"
#include "sys.h"
#include "parts.h"

Engine::Engine(System *paramSys, const char *dataDir, const char *saveDir)
	: sys(paramSys), vm(&mixer, &res, &player, &video, sys), mixer(sys), res(&video, dataDir), 
	player(&mixer, &res, sys), video(&res, sys), _dataDir(dataDir), _saveDir(saveDir), _stateSlot(0) {
}

void Engine::run() {

	while (!sys->input.quit) {

		vm.checkThreadRequests();

		vm.inp_updatePlayer(false, false, false, false, false);

		processInput();

		vm.hostFrame();
	}


}

Engine::~Engine(){

	finish();
	sys->destroy();
}


void Engine::init() {


	// //Init system
	// sys->init("Out Of This World");

	video.init();

	res.allocMemBlock();

	res.readEntries();

	vm.init();

	mixer.init();

	player.init();

	uint16_t part = GAME_PART1;  // This game part is the protection screen
#ifdef BYPASS_PROTECTION
  part = GAME_PART2;
#endif
  vm.initForPart(part);



  // Try to cheat here. You can jump anywhere but the VM crashes afterward.
	// Starting somewhere is probably not enough, the variables and calls return are probably missing.
	//vm.initForPart(GAME_PART2); // Skip protection screen and go directly to intro
	//vm.initForPart(GAME_PART3); // CRASH
	//vm.initForPart(GAME_PART4); // Start directly in jail but then crash
	//vm.initForPart(GAME_PART5);   //CRASH
	//vm.initForPart(GAME_PART6);   // Start in the battlechar but CRASH afteward
	//vm.initForPart(GAME_PART7); //CRASH
	//vm.initForPart(GAME_PART8); //CRASH
	//vm.initForPart(GAME_PART9); // Green screen not doing anything
}

void Engine::finish() {
	player.free();
	mixer.free();
	res.freeMemBlock();
}

void Engine::processInput() {
	// if (sys->input.load) {
	// 	loadGameState(_stateSlot);
	// 	sys->input.load = false;
	// }
	// if (sys->input.save) {
	// 	saveGameState(_stateSlot, "quicksave");
	// 	sys->input.save = false;
	// }
	// if (sys->input.stateSlot != 0) {
	// 	int8_t slot = _stateSlot + sys->input.stateSlot;
	// 	if (slot >= 0 && slot < MAX_SAVE_SLOTS) {
	// 		_stateSlot = slot;
	// 		debug(DBG_INFO, "Current game state slot is %d", _stateSlot);
	// 	}
	// 	sys->input.stateSlot = 0;
	// }
}

void Engine::makeGameStateName(uint8_t slot, char *buf) {
	sprintf(buf, "raw.s%02d", slot);
}

size_t Engine::saveGameState(uint8_t* buffer) {
		Serializer s(buffer, Serializer::SM_SAVE, res._memPtrStart);
		vm.saveOrLoad(s);
		if (_storeNonVMState == true)
		{
			res.saveOrLoad(s);
			video.saveOrLoad(s);
			player.saveOrLoad(s);
			mixer.saveOrLoad(s);
		}
		return s._bytesCount;
}

void Engine::loadGameState(uint8_t* buffer) {
			Serializer s(buffer, Serializer::SM_LOAD, res._memPtrStart);
			vm.saveOrLoad(s);

			if (_storeNonVMState == true)
			{
	 		res.saveOrLoad(s);
				video.saveOrLoad(s);
				player.saveOrLoad(s);
				mixer.saveOrLoad(s);
			}
}
