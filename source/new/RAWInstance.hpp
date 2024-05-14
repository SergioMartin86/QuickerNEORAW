#pragma once

#include "../RAWInstanceBase.hpp"
#include <string>
#include <vector>
#include <jaffarCommon/exceptions.hpp>
#include <jaffarCommon/file.hpp>
#include <jaffarCommon/serializers/base.hpp>
#include <jaffarCommon/serializers/contiguous.hpp>
#include <jaffarCommon/deserializers/base.hpp>

namespace rawspace
{

class EmuInstance : public EmuInstanceBase
{
 public:

 EmuInstance(const std::string& gameDataPath) : EmuInstanceBase()
 {
 }

 ~EmuInstance()
 {
 }

  virtual void initializeImpl() override
  {
  }

  void initializeVideoOutput() override
  {
  }

  void finalizeVideoOutput() override
  {
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

  uint8_t* getPalettePtr() const {return nullptr;}
  size_t getPaletteSize() const {return 0;}

  uint8_t* getPixelsPtr() const override { return nullptr; }
  size_t getPixelsSize() const override { return 0; }

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


  std::string getCoreName() const override { return "QuickerRAW"; }

  uint8_t* getRamPointer() const override { return nullptr; }

  void advanceStateImpl(rawspace::Controller controller) override
  {
    const auto& input = controller.getInput();

  }

  private:

};

} // namespace rawspace