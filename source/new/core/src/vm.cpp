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

#include <ctime>
#include "vm.h"
#include "mixer.h"
#include "resource.h"
#include "video.h"
#include "serializer.h"
#include "sfxplayer.h"
#include "sys.h"
#include "parts.h"
#include "file.h"

VirtualMachine::VirtualMachine(Mixer *mix, Resource *resParameter, SfxPlayer *ply, Video *vid, System *stub)
	: mixer(mix), res(resParameter), player(ply), video(vid), sys(stub) {
}

void VirtualMachine::init() {

	memset(vmVariables, 0, sizeof(vmVariables));
	vmVariables[0x54] = 0x81;
	vmVariables[VM_VARIABLE_RANDOM_SEED] = 0;
#ifdef BYPASS_PROTECTION
   // these 3 variables are set by the game code
   vmVariables[0xBC] = 0x10;
   vmVariables[0xC6] = 0x80;
   vmVariables[0xF2] = 4000;
   // these 2 variables are set by the engine executable
   vmVariables[0xDC] = 33;
#endif

	player->_markVar = &vmVariables[VM_VARIABLE_MUS_MARK];
}



void VirtualMachine::initForPart(uint16_t partId) {

	player->stop();
	mixer->stopAll();

	//WTF is that ?
	vmVariables[0xE4] = 0x14;

	res->setupPart(partId);

	//Set all thread to inactive (pc at 0xFFFF or 0xFFFE )
	memset((uint8_t *)threadsData, 0xFF, sizeof(threadsData));


	memset((uint8_t *)vmIsChannelActive, 0, sizeof(vmIsChannelActive));
	
	int firstThreadId = 0;
	threadsData[PC_OFFSET][firstThreadId] = 0;	
}

/* 
     This is called every frames in the infinite loop.
*/
void VirtualMachine::checkThreadRequests() {

	//Check if a part switch has been requested.
	if (res->requestedNextPart != 0) {
		initForPart(res->requestedNextPart);
		res->requestedNextPart = 0;
	}

	
	// Check if a state update has been requested for any thread during the previous VM execution:
	//      - Pause
	//      - Jump

	// JUMP:
	// Note: If a jump has been requested, the jump destination is stored
	// in threadsData[REQUESTED_PC_OFFSET]. Otherwise threadsData[REQUESTED_PC_OFFSET] == 0xFFFF

	// PAUSE:
	// Note: If a pause has been requested it is stored in  vmIsChannelActive[REQUESTED_STATE][i]

	for (int threadId = 0; threadId < VM_NUM_THREADS; threadId++) {

		vmIsChannelActive[CURR_STATE][threadId] = vmIsChannelActive[REQUESTED_STATE][threadId];

		uint16_t n = threadsData[REQUESTED_PC_OFFSET][threadId];

		if (n != VM_NO_SETVEC_REQUESTED) {

			threadsData[PC_OFFSET][threadId] = (n == 0xFFFE) ? VM_INACTIVE_THREAD : n;
			threadsData[REQUESTED_PC_OFFSET][threadId] = VM_NO_SETVEC_REQUESTED;
		}
	}
}

void VirtualMachine::hostFrame() {

	// Run the Virtual Machine for every active threads (one vm frame).
	// Inactive threads are marked with a thread instruction pointer set to 0xFFFF (VM_INACTIVE_THREAD).
	// A thread must feature a break opcode so the interpreter can move to the next thread.

	for (int threadId = 0; threadId < VM_NUM_THREADS; threadId++) {

		if (vmIsChannelActive[CURR_STATE][threadId])
			continue;
		
		uint16_t n = threadsData[PC_OFFSET][threadId];

		if (n != VM_INACTIVE_THREAD) {

			// Set the script pointer to the right location.
			// script pc is used in executeThread in order
			// to get the next opcode.
			_scriptPtr.pc = res->segBytecode + n;
			_stackPtr = 0;

			gotoNextThread = false;
			executeThread();

			//Since .pc is going to be modified by this next loop iteration, we need to save it.
			threadsData[PC_OFFSET][threadId] = _scriptPtr.pc - res->segBytecode;

			if (sys->input.quit) {
				break;
			}
		}
		
	}
}

#define COLOR_BLACK 0xFF
#define DEFAULT_ZOOM 0x40


void VirtualMachine::executeThread() {

	while (!gotoNextThread) {
		uint8_t opcode = _scriptPtr.fetchByte();

		// 1000 0000 is set
		if (opcode & 0x80) 
		{
			uint16_t off = ((opcode << 8) | _scriptPtr.fetchByte()) * 2;
			res->_useSegVideo2 = false;
			int16_t x = _scriptPtr.fetchByte();
			int16_t y = _scriptPtr.fetchByte();
			int16_t h = y - 199;
			if (h > 0) {
				y = 199;
				x += h;
			}

			// This switch the polygon database to "cinematic" and probably draws a black polygon
			// over all the screen.
			video->setDataBuffer(res->segCinematic, off);
			video->readAndDrawPolygon(COLOR_BLACK, DEFAULT_ZOOM, Point(x,y));

			continue;
		} 

		// 0100 0000 is set
		if (opcode & 0x40) 
		{
			int16_t x, y;
			uint16_t off = _scriptPtr.fetchWord() * 2;
			x = _scriptPtr.fetchByte();

			res->_useSegVideo2 = false;

			if (!(opcode & 0x20)) 
			{
				if (!(opcode & 0x10))  // 0001 0000 is set
				{
					x = (x << 8) | _scriptPtr.fetchByte();
				} else {
					x = vmVariables[x];
				}
			} 
			else 
			{
				if (opcode & 0x10) { // 0001 0000 is set
					x += 0x100;
				}
			}

			y = _scriptPtr.fetchByte();

			if (!(opcode & 8))  // 0000 1000 is set
			{
				if (!(opcode & 4)) { // 0000 0100 is set
					y = (y << 8) | _scriptPtr.fetchByte();
				} else {
					y = vmVariables[y];
				}
			}

			uint16_t zoom = _scriptPtr.fetchByte();

			if (!(opcode & 2))  // 0000 0010 is set
			{
				if (!(opcode & 1)) // 0000 0001 is set
				{
					--_scriptPtr.pc;
					zoom = 0x40;
				} 
				else 
				{
					zoom = vmVariables[zoom];
				}
			} 
			else 
			{
				
				if (opcode & 1) { // 0000 0001 is set
					res->_useSegVideo2 = true;
					--_scriptPtr.pc;
					zoom = 0x40;
				}
			}
			video->setDataBuffer(res->_useSegVideo2 ? res->_segVideo2 : res->segCinematic, off);
			video->readAndDrawPolygon(0xFF, zoom, Point(x, y));

			continue;
		} 
		 
		
		if (opcode > 0x1A) 
		{
			error("VirtualMachine::executeThread() ec=0x%X invalid opcode=0x%X", 0xFFF, opcode);
		} 
		else 
		{
			(this->*opcodeTable[opcode])();
		}
		
	}
}

void VirtualMachine::inp_updatePlayer(bool up, bool down, bool left, bool right, bool fire) {

	// sys->processEvents();

	if (res->currentPartId == 0x3E89) {
		char c = sys->input.lastChar;
		if (c == 8 || /*c == 0xD |*/ c == 0 || (c >= 'a' && c <= 'z')) {
			vmVariables[VM_VARIABLE_LAST_KEYCHAR] = c & ~0x20;
			sys->input.lastChar = 0;
		}
	}

	int16_t lr = 0;
	int16_t m = 0;
	int16_t ud = 0;

	if (right) {
		lr = 1;
		m |= 1;
	}
	if (left) {
		lr = -1;
		m |= 2;
	}
	if (down) {
		ud = 1;
		m |= 4;
	}

	vmVariables[VM_VARIABLE_HERO_POS_UP_DOWN] = ud;

	if (up) {
		vmVariables[VM_VARIABLE_HERO_POS_UP_DOWN] = -1;
	}

	if (up) { // inpJump
		ud = -1;
		m |= 8;
	}

	vmVariables[VM_VARIABLE_HERO_POS_JUMP_DOWN] = ud;
	vmVariables[VM_VARIABLE_HERO_POS_LEFT_RIGHT] = lr;
	vmVariables[VM_VARIABLE_HERO_POS_MASK] = m;
	int16_t button = 0;

	if (fire) { // inpButton
		button = 1;
		m |= 0x80;
	}

	vmVariables[VM_VARIABLE_HERO_ACTION] = button;
	vmVariables[VM_VARIABLE_HERO_ACTION_POS_MASK] = m;
}

void VirtualMachine::inp_handleSpecialKeys() {

	if (sys->input.pause) {

		if (res->currentPartId != GAME_PART1 && res->currentPartId != GAME_PART2) {
			sys->input.pause = false;
			while (!sys->input.pause) {
				sys->processEvents();
				sys->sleep(200);
			}
		}
		sys->input.pause = false;
	}

	if (sys->input.code) {
		sys->input.code = false;
		if (res->currentPartId != GAME_PART_LAST && res->currentPartId != GAME_PART_FIRST) {
			res->requestedNextPart = GAME_PART_LAST;
		}
	}

	// XXX
	if (vmVariables[0xC9] == 1) {
		warning("VirtualMachine::inp_handleSpecialKeys() unhandled case (vmVariables[0xC9] == 1)");
	}

}

void VirtualMachine::snd_playSound(uint16_t resNum, uint8_t freq, uint8_t vol, uint8_t channel) {

	
	MemEntry *me = &res->_memList[resNum];

	if (me->state != MEMENTRY_STATE_LOADED)
		return;

	
	if (vol == 0) {
		mixer->stopChannel(channel);
	} else {
		MixerChunk mc;
		memset(&mc, 0, sizeof(mc));
		mc.data = me->bufPtr + 8; // skip header
		mc.len = READ_BE_UINT16(me->bufPtr) * 2;
		mc.loopLen = READ_BE_UINT16(me->bufPtr + 2) * 2;
		if (mc.loopLen != 0) {
			mc.loopPos = mc.len;
		}
		assert(freq < 40);
		mixer->playChannel(channel & 3, &mc, frequenceTable[freq], MIN(vol, 0x3F));
	}
	
}

void VirtualMachine::snd_playMusic(uint16_t resNum, uint16_t delay, uint8_t pos) {


	if (resNum != 0) {
		player->loadSfxModule(resNum, delay, pos);
		player->start();
	} else if (delay != 0) {
		player->setEventsDelay(delay);
	} else {
		player->stop();
	}
}

void VirtualMachine::saveOrLoad(Serializer &ser) {
	Serializer::Entry entries[] = {
		SE_ARRAY(vmVariables, 0x100, Serializer::SES_INT16, VER(1)),
		SE_ARRAY(_scriptStackCalls, 0x100, Serializer::SES_INT16, VER(1)),
		SE_ARRAY(threadsData, 0x40 * 2, Serializer::SES_INT16, VER(1)),
		SE_ARRAY(vmIsChannelActive, 0x40 * 2, Serializer::SES_INT8, VER(1)),
		SE_END()
	};
	ser.saveOrLoadEntries(entries);
}
