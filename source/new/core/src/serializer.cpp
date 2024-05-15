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

#include "serializer.h"


Serializer::Serializer(uint8_t *buffer, Mode mode, uint8_t *ptrBlock, uint16_t saveVer)
	: _buffer(buffer), _mode(mode), _ptrBlock(ptrBlock), _saveVer(saveVer) {
}

void Serializer::saveOrLoadEntries(Entry *entry) {
	debug(DBG_SER, "Serializer::saveOrLoadEntries() _mode=%d", _mode);
	switch (_mode) {
	case SM_SAVE:
		saveEntries(entry);
		break;
	case SM_LOAD:
		loadEntries(entry);
		break;	
	}
	debug(DBG_SER, "Serializer::saveOrLoadEntries() _bytesCount=%d", _bytesCount);
}

void Serializer::saveEntries(Entry *entry) {
	debug(DBG_SER, "Serializer::saveEntries()");
	for (; entry->type != SET_END; ++entry) {
		if (entry->maxVer == CUR_VER) {
			switch (entry->type) {
			case SET_INT:
			{
				saveInt(entry->size, entry->data);
				_bytesCount += entry->size;
			}
				break;
			case SET_ARRAY:
			{
				if (entry->size == Serializer::SES_INT8) {
					if (entry->data != nullptr) if (_buffer != nullptr) memcpy(&_buffer[_bytesCount], entry->data, entry->n);
					_bytesCount += entry->n;
				} else {
					uint8_t *p = (uint8_t *)entry->data;
					for (int i = 0; i < entry->n; ++i) {
						saveInt(entry->size, p);
						p += entry->size;
						_bytesCount += entry->size;
					}
				}
			}
				break;
			case SET_PTR:
			{
			 uint32_t val = *(uint8_t **)(entry->data) - _ptrBlock;
				if (_buffer != nullptr) memcpy(&_buffer[_bytesCount], &val, 4);
				_bytesCount += 4;
				break;
				}
			case SET_END:
				break;
			}
		}
	}
}

void Serializer::loadEntries(Entry *entry) {
	debug(DBG_SER, "Serializer::loadEntries()");
	for (; entry->type != SET_END; ++entry) {
		if (_saveVer >= entry->minVer && _saveVer <= entry->maxVer) {
			switch (entry->type) {
			case SET_INT:
				loadInt(entry->size, entry->data);
				_bytesCount += entry->size;
				break;
			case SET_ARRAY:
				if (entry->size == Serializer::SES_INT8) {
					if (entry->data != nullptr) if (_buffer != nullptr) memcpy(entry->data, &_buffer[_bytesCount], entry->n);
					_bytesCount += entry->n;
				} else {
					uint8_t *p = (uint8_t *)entry->data;
					for (int i = 0; i < entry->n; ++i) {
						loadInt(entry->size, p);
						p += entry->size;
						_bytesCount += entry->size;
					}
				}
				break;
			case SET_PTR:
			{
			 uint32_t val = 0;
				if (_buffer != nullptr) memcpy(&val, &_buffer[_bytesCount], 4);
				*(uint8_t **)(entry->data) = _ptrBlock + val;
				_bytesCount += 4;
			}
				break;
			case SET_END:
				break;				
			}
		}
	}
}

void Serializer::saveInt(uint8_t es, void *p) {
	switch (es) {
	case 1:
	{
	  uint8_t val = *(uint8_t *)p;
		 if (_buffer != nullptr) memcpy(&_buffer[_bytesCount], &val, 1);
	}
		break;
	case 2:
	{
	  uint16_t val = *(uint16_t *)p;
		 if (_buffer != nullptr) memcpy(&_buffer[_bytesCount], &val, 2);
	}
		break;
	case 4:
	{
	  uint32_t val = *(uint32_t *)p;
		 if (_buffer != nullptr) memcpy(&_buffer[_bytesCount], &val, 4);
	}
		break;
	}
}

void Serializer::loadInt(uint8_t es, void *p) {
	switch (es) {
	case 1:
	{
	 uint8_t val = 0;
		if (_buffer != nullptr) memcpy(&val, &_buffer[_bytesCount], 1);
		*(uint8_t *)p = val;
	}
		break;
	case 2:
	{
		uint16_t val = 0;
		if (_buffer != nullptr) memcpy(&val, &_buffer[_bytesCount], 2);
		*(uint16_t *)p = val;
	}
		break;
	case 4:
	{
		uint32_t val = 0;
		if (_buffer != nullptr) memcpy(&val, &_buffer[_bytesCount], 4);
		*(uint32_t *)p = val;
	}
		break;
	}
}
