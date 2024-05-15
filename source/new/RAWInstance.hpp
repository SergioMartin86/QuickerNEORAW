#pragma once

#include "../RAWInstanceBase.hpp"
#include <string>
#include <vector>
#include <jaffarCommon/exceptions.hpp>
#include <jaffarCommon/file.hpp>
#include <jaffarCommon/serializers/base.hpp>
#include <jaffarCommon/serializers/contiguous.hpp>
#include <jaffarCommon/deserializers/base.hpp>

#include <engine.h>
#include <sys.h>

extern thread_local System *stub ;//= System_SDL_create();
extern thread_local Engine* e;

namespace rawspace
{

class EmuInstance : public EmuInstanceBase
{
 public:

  EmuInstance() : EmuInstanceBase()
  {
  }

  ~EmuInstance()
  {
    delete e;
  }

  int16_t* getThreadsData() const override { return (int16_t*)e->vm._scriptStackCalls; }
  size_t getThreadsDataSize() const override { return VM_NUM_THREADS * sizeof(int16_t*); }
  int16_t* getScriptStackData() const override { return (int16_t*)e->vm.threadsData; }
  size_t getScriptStackDataSize() const override { return NUM_DATA_FIELDS * VM_NUM_THREADS * sizeof(int16_t*); }

  virtual void initializeImpl(const std::string& gameDataPath) override
  {
    e = new Engine(stub, gameDataPath.c_str(), "");
    e->init();
  }

  void initializeVideoOutput() override
  {
    stub->init("");
  }

  void finalizeVideoOutput() override
  {
    stub->destroy();
  }

  void enableRendering() override
  {
    e->vm._doRendering = true;
    e->video._doRendering = true;
  }

  void disableRendering() override
  {
    e->vm._doRendering = false;
    e->video._doRendering = false;
  }

  uint8_t* getPixelsPtr() const override { return stub->getPixelsPtr(); }
  size_t getPixelsSize() const override { return stub->getPixelsSize(); }
  uint8_t* getPalettePtr() const override { return stub->getPalettePtr(); }
  size_t getPaletteSize() const override { return stub->getPaletteSize(); }

  void serializeState(jaffarCommon::serializer::Base& s) const override
  {
    e->saveGameState(s.getOutputDataBuffer());
    s.pushContiguous(nullptr, _stateSize);
  }

  void deserializeState(jaffarCommon::deserializer::Base& d) override
  {
    e->loadGameState((uint8_t*)(uint64_t)d.getInputDataBuffer());
    d.popContiguous(nullptr, _stateSize);
  }

  size_t getStateSizeImpl() const override
  {
    return e->saveGameState(nullptr);
  }

  void updateRenderer() override
  {
    stub->applyPalette();
    stub->updateRenderer();
  }

  inline size_t getDifferentialStateSizeImpl() const override { return getStateSizeImpl(); }

void enableStateBlockImpl(const std::string& block)
  { 
    bool recognizedBlock = false;
    
    if (block == "NVS") { e->_storeNonVMState = true; recognizedBlock = true; }

    if (recognizedBlock == false) { fprintf(stderr, "Unrecognized block type: %s\n", block.c_str()); exit(-1);}
  };


  void disableStateBlockImpl(const std::string& block)
  { 
    bool recognizedBlock = false;
    
    if (block == "NVS") { e->_storeNonVMState = false; recognizedBlock = true; }

    if (recognizedBlock == false) { fprintf(stderr, "Unrecognized block type: %s\n", block.c_str()); exit(-1);}
  };

  void doSoftReset() override
  {
  }
  
  void doHardReset() override
  {
  }

  std::string getCoreName() const override { return "QuickerRAW"; }

  uint8_t* getRamPointer() const override { return (uint8_t*)e->vm.vmVariables; }

  void advanceStateImpl(rawspace::Controller controller) override
  {
    const auto& input = controller.getInput();

		e->vm.checkThreadRequests();

		e->vm.inp_updatePlayer(input.buttonUp, input.buttonDown, input.buttonLeft, input.buttonRight, input.buttonFire);

		e->vm.hostFrame();
  }

  private:

};

} // namespace rawspace