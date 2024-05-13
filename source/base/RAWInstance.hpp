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

  EmuInstance(const std::string& gameDataPath) : EmuInstanceBase()
  {
    e = new Engine(stub, gameDataPath.c_str(), "");
  }

  ~EmuInstance()
  {
    delete e;
  }

  virtual void initialize() override
  {
    e->init();
  }

  virtual bool loadROMImpl(const std::string &romFilePath) override
  {
    return true;
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
  }

  void disableRendering() override
  {
  }

  void serializeState(jaffarCommon::serializer::Base& s) const override
  {
  }

  void deserializeState(jaffarCommon::deserializer::Base& d) override
  {
  }

  size_t getStateSizeImpl() const override
  {
    return 0;
  }

  void updateRenderer() override
  {
  }

  inline size_t getDifferentialStateSizeImpl() const override { return getStateSizeImpl(); }

  void enableStateBlockImpl(const std::string& block) override
  {
  }

  void disableStateBlockImpl(const std::string& block) override
  {
  }

  void doSoftReset() override
  {
  }
  
  void doHardReset() override
  {
  }

  std::string getCoreName() const override { return "RAW"; }

  uint8_t* getRamPointer() const override { return (uint8_t*)e->vm.vmVariables; }

  void advanceStateImpl(rawspace::Controller controller) override
  {
    const auto& input = controller.getInput();

		e->vm.checkThreadRequests();

		e->vm.inp_updatePlayer();

		e->vm.hostFrame();
  }

  private:

};

} // namespace rawspace