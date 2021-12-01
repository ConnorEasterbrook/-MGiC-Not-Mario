#ifndef NOTMARIOGAME_POWERUP_H
#define NOTMARIOGAME_POWERUP_H

#include "GameObject.h"
#include "Player.h"

class PowerUp : public GameObject
{
 public:
  PowerUp()  = default;
  ~PowerUp() = default;

  void checkCollision(const GameObject& player, bool life_spawn, bool star_spawn);
  void setLifeUp();
  bool collectLifeUp();
  void setStarUp();
  bool collectStarUp();

  bool life_power_up = false;
  bool star_power_up = false;

 private:
};

#endif // NOTMARIOGAME_POWERUP_H
