#pragma once

#include "../RAWInstanceBase.hpp"
#include <string>
#include <vector>
#include <jaffarCommon/exceptions.hpp>
#include <jaffarCommon/file.hpp>
#include <jaffarCommon/serializers/base.hpp>
#include <jaffarCommon/serializers/contiguous.hpp>
#include <jaffarCommon/deserializers/base.hpp>

namespace raw
{

class EmuInstance : public EmuInstanceBase
{
 public:

 EmuInstance() : EmuInstanceBase()
 {
 }

 ~EmuInstance()
 {
 }

  virtual void initialize() override
  {
  }

  virtual bool loadROMImpl(const std::string &romFilePath) override
  {
    return true;
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

  uint8_t* getRamPointer() const override { return nullptr; }

  void advanceStateImpl(raw::Controller controller) override
  {
    const auto& input = controller.getInput();

  }

  private:

};

} // namespace raw