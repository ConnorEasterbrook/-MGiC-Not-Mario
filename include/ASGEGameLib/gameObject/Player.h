#ifndef NOTMARIOGAME_PLAYER_H
#define NOTMARIOGAME_PLAYER_H

#include "GameObject.h"
#include "PowerUp.h"
#include <Engine/InputEvents.hpp>
#include <Engine/Sprite.hpp>
#include <vector>

class Player : public GameObject
{
 public:
  Player()  = default;
  ~Player() = default;

  [[nodiscard]] int getPlayerLives() const;
  void setPlayerLives(int playerLives);
  void minusPlayerLife();
  void addPlayerLife();
  void setPlayerSpawn(int x_pos, int y_pos);
  void teleportPlayerSpawn();
  [[nodiscard]] float getPlayerSpeed() const;
  [[maybe_unused]] void setPlayerSpeed(float playerSpeed);
  [[nodiscard]] float getGravityForce() const;
  [[maybe_unused]] void setGravityForce(float gravityForce);
  [[nodiscard]] float getJumpStrength() const;
  [[maybe_unused]] void setJumpStrength(float jumpStrength);
  void playerUpdate(float game_window_height, const ASGE::GameTime& us);
  void playerCollision(
    std::vector<std::unique_ptr<ASGE::Sprite>>& collidible_tile, int tile_size, int tile_scale);
  void playerDestruction(
    std::vector<std::unique_ptr<ASGE::Sprite>>& destructible_tile, int tile_size, int tile_scale,
    GameObject& power_up);
  bool enemyCollision(GameObject& enemy);

  // PLAYER MOVEMENT
  bool leftMovementKey                  = false;
  [[maybe_unused]] bool upMovementKey   = false;
  [[maybe_unused]] bool downMovementKey = false;
  bool rightMovementKey                 = false;
  bool jumpKey                          = false;
  bool jumping                          = false;
  bool no_jump                          = false;

  // PLAYER GAMEPLAY VARIABLES
  bool power_up_limiter = false;
  bool spawn_star       = false;
  bool spawn_mushroom   = false;
  bool star_power_up    = false;
  int score             = 0;

 private:
  void playerMovement(float game_window_height, const ASGE::GameTime& us);
  void updatePowerUp(const ASGE::GameTime& us);
  void animationCheck(const ASGE::GameTime& us);

  Vector2 velocity       = { 0, 0 };
  int player_lives       = 4;
  int spawn_xPos         = 0;
  int spawn_yPos         = 0;
  float player_speed     = 10;
  float gravity_force    = 2;
  float jump_strength    = 20;
  double star_power_time = 5;
};

#endif // NOTMARIOGAME_PLAYER_H
