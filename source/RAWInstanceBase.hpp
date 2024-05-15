#pragma once

#include <jaffarCommon/hash.hpp>
#include <jaffarCommon/exceptions.hpp>
#include <jaffarCommon/file.hpp>
#include <jaffarCommon/serializers/base.hpp>
#include <jaffarCommon/deserializers/base.hpp>
#include <jaffarCommon/serializers/contiguous.hpp>
#include <jaffarCommon/deserializers/contiguous.hpp>

#include "controller.hpp"

namespace rawspace
{

class EmuInstanceBase
{
  public:

  EmuInstanceBase() = default;
  virtual ~EmuInstanceBase() = default;

  inline void advanceState(const std::string &move)
  {
    bool isInputValid = _controller.parseInputString(move);
    if (isInputValid == false) JAFFAR_THROW_LOGIC("Move provided cannot be parsed: '%s'\n", move.c_str());

    advanceStateImpl(_controller);
  }

  inline jaffarCommon::hash::hash_t getStateHash() const
  {
    MetroHash128 hash;
    
    auto workRam = getRamPointer();

    for (size_t i = 0; i < 512; i++) if (i != 0x18E)
      hash.Update(workRam[i]);

    jaffarCommon::hash::hash_t result;
    hash.Finalize(reinterpret_cast<uint8_t *>(&result));
    return result;
  }

  virtual int16_t* getThreadsData() const = 0;
  virtual size_t getThreadsDataSize() const = 0;
  virtual int16_t* getScriptStackData() const = 0;
  virtual size_t getScriptStackDataSize() const = 0;

  void initialize(const std::string& gameDataPath)
  {
    initializeImpl(gameDataPath);
    _stateSize = getStateSizeImpl();
  }

  virtual uint8_t* getPixelsPtr() const = 0;
  virtual size_t getPixelsSize() const = 0;
  virtual uint8_t* getPalettePtr() const = 0;
  virtual size_t getPaletteSize() const = 0;
  virtual void initializeImpl(const std::string& gameDataPath) = 0;
  virtual void initializeVideoOutput() = 0;
  virtual void finalizeVideoOutput() = 0;
  virtual void enableRendering() = 0;
  virtual void disableRendering() = 0;

  void enableStateBlock(const std::string& block) 
  {
    enableStateBlockImpl(block);
    _stateSize = getStateSizeImpl();
    _differentialStateSize = getDifferentialStateSizeImpl();
  }

  void disableStateBlock(const std::string& block)
  {
     disableStateBlockImpl(block);
    _stateSize = getStateSizeImpl();
    _differentialStateSize = getDifferentialStateSizeImpl();
  }

  inline size_t getStateSize() const 
  {
    return _stateSize;
  }

  inline size_t getDifferentialStateSize() const
  {
    return _differentialStateSize;
  }

  // Virtual functions

  virtual void updateRenderer() = 0;
  virtual void serializeState(jaffarCommon::serializer::Base& s) const = 0;
  virtual void deserializeState(jaffarCommon::deserializer::Base& d) = 0;

  virtual void doSoftReset() = 0;
  virtual void doHardReset() = 0;
  virtual std::string getCoreName() const = 0;

  protected:

  virtual void advanceStateImpl(const rawspace::Controller controller) = 0;

  virtual void enableStateBlockImpl(const std::string& block) {};
  virtual void disableStateBlockImpl(const std::string& block) {};

  virtual size_t getStateSizeImpl() const = 0;
  virtual size_t getDifferentialStateSizeImpl() const = 0;
  
  virtual uint8_t* getRamPointer() const = 0;

  // State size
  size_t _stateSize;

  private:

  // Controller class for input parsing
  Controller _controller;

  // Differential state size
  size_t _differentialStateSize;
};

} // namespace rawspace