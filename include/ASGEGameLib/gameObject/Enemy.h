#ifndef NOTMARIOGAME_ENEMY_H
#define NOTMARIOGAME_ENEMY_H

#include "GameObject.h"
#include "Player.h"

class Enemy : public GameObject
{
 public:
  Enemy()  = default;
  ~Enemy() = default;

  [[nodiscard]] float getSpeed() const;
  [[maybe_unused]] void setSpeed(float playerSpeed);
  [[maybe_unused]] [[nodiscard]] int getEnemyCount() const;
  void setEnemyCount(int enemy_count);
  void getMovement(Player& player);
  void setXPos(float xPos);

  bool enemy_life  = true;
  int enemy_amount = 0;

 private:
  float speed          = 10;
  bool enemy_direction = false;
  float x_pos;
};

#endif // NOTMARIOGAME_ENEMY_H
