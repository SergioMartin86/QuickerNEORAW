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

#ifndef __LOGIC_H__
#define __LOGIC_H__

#include "intern.h"
#include "resource.h"
#include "video.h"
#include "serializer.h"
#include "sfxplayer.h"
#include "sys.h"
#include "parts.h"
#include "file.h"
#include "mixer.h"

#define VM_NUM_THREADS 64
#define VM_NUM_VARIABLES 256
#define VM_NO_SETVEC_REQUESTED 0xFFFF
#define VM_INACTIVE_THREAD    0xFFFF


enum ScriptVars {
		VM_VARIABLE_RANDOM_SEED          = 0x3C,
		
		VM_VARIABLE_LAST_KEYCHAR         = 0xDA,

		VM_VARIABLE_HERO_POS_UP_DOWN     = 0xE5,

		VM_VARIABLE_MUS_MARK             = 0xF4,

		VM_VARIABLE_SCROLL_Y             = 0xF9, // = 239
		VM_VARIABLE_HERO_ACTION          = 0xFA,
		VM_VARIABLE_HERO_POS_JUMP_DOWN   = 0xFB,
		VM_VARIABLE_HERO_POS_LEFT_RIGHT  = 0xFC,
		VM_VARIABLE_HERO_POS_MASK        = 0xFD,
		VM_VARIABLE_HERO_ACTION_POS_MASK = 0xFE,
		VM_VARIABLE_PAUSE_SLICES         = 0xFF
	};

struct Mixer;
struct Resource;
struct Serializer;
struct SfxPlayer;
struct System;
struct Video;

//For threadsData navigation
#define PC_OFFSET 0
#define REQUESTED_PC_OFFSET 1
#define NUM_DATA_FIELDS 2

//For vmIsChannelActive navigation
#define CURR_STATE 0
#define REQUESTED_STATE 1
#define NUM_THREAD_FIELDS 2

struct VirtualMachine {

	// The type of entries in opcodeTable. This allows "fast" branching
	typedef void (VirtualMachine::*OpcodeStub)();
	static const OpcodeStub opcodeTable[];

	//This table is used to play a sound
	static const uint16_t frequenceTable[];

	Mixer *mixer;
	Resource *res;
	SfxPlayer *player;
	Video *video;
	System *sys;

	int16_t vmVariables[VM_NUM_VARIABLES];
	uint16_t _scriptStackCalls[VM_NUM_THREADS];
	uint16_t threadsData[NUM_DATA_FIELDS][VM_NUM_THREADS];

	// This array is used: 
	//     0 to save the channel's instruction pointer 
	//     when the channel release control (this happens on a break).
	//     1 When a setVec is requested for the next vm frame.
	uint8_t vmIsChannelActive[NUM_THREAD_FIELDS][VM_NUM_THREADS];

	Ptr _scriptPtr;
	uint8_t _stackPtr;
	bool gotoNextThread;
	bool _doRendering = false;

	VirtualMachine(Mixer *mix, Resource *res, SfxPlayer *ply, Video *vid, System *stub);
	void init();
	
inline void op_movConst() {
	uint8_t variableId = _scriptPtr.fetchByte();
	int16_t value = _scriptPtr.fetchWord();
	vmVariables[variableId] = value;
}

inline void op_mov() {
	uint8_t dstVariableId = _scriptPtr.fetchByte();
	uint8_t srcVariableId = _scriptPtr.fetchByte();	
	vmVariables[dstVariableId] = vmVariables[srcVariableId];
}

inline void op_add() {
	uint8_t dstVariableId = _scriptPtr.fetchByte();
	uint8_t srcVariableId = _scriptPtr.fetchByte();
	vmVariables[dstVariableId] += vmVariables[srcVariableId];
}

inline void op_addConst() {
	if (res->currentPartId == 0x3E86 && _scriptPtr.pc == res->segBytecode + 0x6D48) {
		warning("op_addConst() hack for non-stop looping gun sound bug");
		// the script 0x27 slot 0x17 doesn't stop the gun sound from looping, I 
		// don't really know why ; for now, let's play the 'stopping sound' like 
		// the other scripts do
		//  (0x6D43) jmp(0x6CE5)
		//  (0x6D46) break
		//  (0x6D47) VAR(6) += -50
		snd_playSound(0x5B, 1, 64, 1);
	}
	uint8_t variableId = _scriptPtr.fetchByte();
	int16_t value = _scriptPtr.fetchWord();
	vmVariables[variableId] += value;
}

inline void op_call() {

	uint16_t offset = _scriptPtr.fetchWord();
	uint8_t sp = _stackPtr;

	_scriptStackCalls[sp] = _scriptPtr.pc - res->segBytecode;
	if (_stackPtr == 0xFF) {
		error("op_call() ec=0x%X stack overflow", 0x8F);
	}
	++_stackPtr;
	_scriptPtr.pc = res->segBytecode + offset ;
}

inline void op_ret() {
	if (_stackPtr == 0) {
		error("op_ret() ec=0x%X stack underflow", 0x8F);
	}	
	--_stackPtr;
	uint8_t sp = _stackPtr;
	_scriptPtr.pc = res->segBytecode + _scriptStackCalls[sp];
}

inline void op_pauseThread() {
	gotoNextThread = true;
}

inline void op_jmp() {
	uint16_t pcOffset = _scriptPtr.fetchWord();
	_scriptPtr.pc = res->segBytecode + pcOffset;	
}

inline void op_setSetVect() {
	uint8_t threadId = _scriptPtr.fetchByte();
	uint16_t pcOffsetRequested = _scriptPtr.fetchWord();
	threadsData[REQUESTED_PC_OFFSET][threadId] = pcOffsetRequested;
}

inline void op_jnz() {
	uint8_t i = _scriptPtr.fetchByte();
	--vmVariables[i];
	if (vmVariables[i] != 0) {
		op_jmp();
	} else {
		_scriptPtr.fetchWord();
	}
}

inline void op_condJmp() {
	uint8_t opcode = _scriptPtr.fetchByte();
  const uint8_t var = _scriptPtr.fetchByte();
  int16_t b = vmVariables[var];
	int16_t a;

	if (opcode & 0x80) {
		a = vmVariables[_scriptPtr.fetchByte()];
	} else if (opcode & 0x40) {
    a = _scriptPtr.fetchWord();
	} else {
    a = _scriptPtr.fetchByte();
	}

	// Check if the conditional value is met.
	bool expr = false;
	switch (opcode & 7) {
	case 0:	// jz
		expr = (b == a);
#ifdef BYPASS_PROTECTION
      if (res->currentPartId == 16000) {
        //
        // 0CB8: jmpIf(VAR(0x29) == VAR(0x1E), @0CD3)
        // ...
        //
        if (b == 0x29 && (opcode & 0x80) != 0) {
          // 4 symbols
          vmVariables[0x29] = vmVariables[0x1E];
          vmVariables[0x2A] = vmVariables[0x1F];
          vmVariables[0x2B] = vmVariables[0x20];
          vmVariables[0x2C] = vmVariables[0x21];
          // counters
          vmVariables[0x32] = 6;
          vmVariables[0x64] = 20;
          warning("Script::op_condJmp() bypassing protection");
          expr = true;
        }
      }
#endif
		break;
	case 1: // jnz
		expr = (b != a);
		break;
	case 2: // jg
		expr = (b > a);
		break;
	case 3: // jge
		expr = (b >= a);
		break;
	case 4: // jl
		expr = (b < a);
		break;
	case 5: // jle
		expr = (b <= a);
		break;
	default:
		warning("op_condJmp() invalid condition %d", (opcode & 7));
		break;
	}

	if (expr) {
		op_jmp();
	} else {
		_scriptPtr.fetchWord();
	}

}

inline void op_setPalette() {
	uint16_t paletteId = _scriptPtr.fetchWord();
	video->paletteIdRequested = paletteId >> 8;
}

inline void op_resetThread() {

	uint8_t threadId = _scriptPtr.fetchByte();
	uint8_t i =        _scriptPtr.fetchByte();

	// FCS: WTF, this is cryptic as hell !!
	//int8_t n = (i & 0x3F) - threadId;  //0x3F = 0011 1111
	// The following is so much clearer

	//Make sure i within [0-VM_NUM_THREADS-1]
	i = i & (VM_NUM_THREADS-1) ;
	int8_t n = i - threadId;

	if (n < 0) {
		warning("op_resetThread() ec=0x%X (n < 0)", 0x880);
		return;
	}
	++n;
	uint8_t a = _scriptPtr.fetchByte();


	if (a == 2) {
		uint16_t *p = &threadsData[REQUESTED_PC_OFFSET][threadId];
		while (n--) {
			*p++ = 0xFFFE;
		}
	} else if (a < 2) {
		uint8_t *p = &vmIsChannelActive[REQUESTED_STATE][threadId];
		while (n--) {
			*p++ = a;
		}
	}
}

inline void op_selectVideoPage() {
	uint8_t frameBufferId = _scriptPtr.fetchByte();
	video->changePagePtr1(frameBufferId);
}

inline void op_fillVideoPage() {
	uint8_t pageId = _scriptPtr.fetchByte();
	uint8_t color = _scriptPtr.fetchByte();
	video->fillPage(pageId, color);
}

inline void op_copyVideoPage() {
	uint8_t srcPageId = _scriptPtr.fetchByte();
	uint8_t dstPageId = _scriptPtr.fetchByte();
	video->copyPage(srcPageId, dstPageId, vmVariables[VM_VARIABLE_SCROLL_Y]);
}


uint32_t lastTimeStamp = 0;
inline void op_blitFramebuffer() {

	uint8_t pageId = _scriptPtr.fetchByte();
	inp_handleSpecialKeys();

  int32_t delay = sys->getTimeStamp() - lastTimeStamp;
  int32_t timeToSleep = vmVariables[VM_VARIABLE_PAUSE_SLICES] * 20 - delay;

  // The bytecode will set vmVariables[VM_VARIABLE_PAUSE_SLICES] from 1 to 5
  // The virtual machine hence indicate how long the image should be displayed.

  if (timeToSleep > 0) {
    //sys->sleep(timeToSleep);
  }

  lastTimeStamp = sys->getTimeStamp();

	//WTF ?
	vmVariables[0xF7] = 0;

	if (_doRendering == true) video->updateDisplay(pageId);
}

inline void op_killThread() {
	_scriptPtr.pc = res->segBytecode + 0xFFFF;
	gotoNextThread = true;
}

inline void op_drawString() {
	uint16_t stringId = _scriptPtr.fetchWord();
	uint16_t x = _scriptPtr.fetchByte();
	uint16_t y = _scriptPtr.fetchByte();
	uint16_t color = _scriptPtr.fetchByte();


	video->drawString(color, x, y, stringId);
}

inline void op_sub() {
	uint8_t i = _scriptPtr.fetchByte();
	uint8_t j = _scriptPtr.fetchByte();
	vmVariables[i] -= vmVariables[j];
}

inline void op_and() {
	uint8_t variableId = _scriptPtr.fetchByte();
	uint16_t n = _scriptPtr.fetchWord();
	vmVariables[variableId] = (uint16_t)vmVariables[variableId] & n;
}

inline void op_or() {
	uint8_t variableId = _scriptPtr.fetchByte();
	uint16_t value = _scriptPtr.fetchWord();
	vmVariables[variableId] = (uint16_t)vmVariables[variableId] | value;
}

inline void op_shl() {
	uint8_t variableId = _scriptPtr.fetchByte();
	uint16_t leftShiftValue = _scriptPtr.fetchWord();
	vmVariables[variableId] = (uint16_t)vmVariables[variableId] << leftShiftValue;
}

inline void op_shr() {
	uint8_t variableId = _scriptPtr.fetchByte();
	uint16_t rightShiftValue = _scriptPtr.fetchWord();
	vmVariables[variableId] = (uint16_t)vmVariables[variableId] >> rightShiftValue;
}

inline void op_playSound() {
	uint16_t resourceId = _scriptPtr.fetchWord();
	uint8_t freq = _scriptPtr.fetchByte();
	uint8_t vol = _scriptPtr.fetchByte();
	uint8_t channel = _scriptPtr.fetchByte();
	snd_playSound(resourceId, freq, vol, channel);
}

inline void op_updateMemList() {

	uint16_t resourceId = _scriptPtr.fetchWord();

	if (resourceId == 0) {
		player->stop();
		mixer->stopAll();
		res->invalidateRes();
	} else {
		res->loadPartsOrMemoryEntry(resourceId);
	}
}

inline void op_playMusic() {
	// uint16_t resNum = _scriptPtr.fetchWord();
	// uint16_t delay = _scriptPtr.fetchWord();
	// uint8_t pos = _scriptPtr.fetchByte();
	// snd_playMusic(resNum, delay, pos);
}

	void initForPart(uint16_t partId);
	void checkThreadRequests();
	void hostFrame();
	void executeThread();

	void inp_updatePlayer(bool up, bool down, bool left, bool right, bool fire);
	void inp_handleSpecialKeys();
	
	void snd_playSound(uint16_t resNum, uint8_t freq, uint8_t vol, uint8_t channel);
	void snd_playMusic(uint16_t resNum, uint16_t delay, uint8_t pos);
	
	void saveOrLoad(Serializer &ser);
};

#endif
