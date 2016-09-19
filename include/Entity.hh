#ifndef _ENTITY_H_
#define _ENTITY_H_

namespace irr {
  class IrrlichtDevice;
}

class Entity {
public:
  virtual void initialize(irr::IrrlichtDevice* device) = 0;
  virtual void update(double deltaT) = 0;
};

#endif /* _ENTITY_H_ */
