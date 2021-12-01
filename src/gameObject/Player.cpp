#include <gameObject/Player.h>

int Player::getPlayerLives() const
{
  return player_lives;
}
void Player::setPlayerLives(int playerLives)
{
  player_lives = playerLives;
}

void Player::minusPlayerLife()
{
  player_lives--;

  teleportPlayerSpawn();
}

void Player::addPlayerLife()
{
  player_lives++;
}

/// Set the player spawn location
void Player::setPlayerSpawn(int x_pos, int y_pos)
{
  spawn_xPos = x_pos;
  spawn_yPos = y_pos;
}
/// Move the player to the set spawn location
void Player::teleportPlayerSpawn()
{
  sprite()->setX(static_cast<float>(spawn_xPos));
  sprite()->setY(static_cast<float>(spawn_yPos));
}

float Player::getPlayerSpeed() const
{
  return player_speed;
}
[[maybe_unused]] void Player::setPlayerSpeed(float playerSpeed)
{
  player_speed = playerSpeed;
}

float Player::getGravityForce() const
{
  return gravity_force;
}
[[maybe_unused]] void Player::setGravityForce(float gravityForce)
{
  gravity_force = gravityForce;
}

float Player::getJumpStrength() const
{
  return jump_strength;
}

[[maybe_unused]] void Player::setJumpStrength(float jumpStrength)
{
  jump_strength = jumpStrength;
}

/// Called from the main update() meaning this works as a Player::update() function
void Player::playerUpdate(float game_window_height, const ASGE::GameTime& us)
{
  playerMovement(game_window_height, us);
  updatePowerUp(us);
}

/// Control Player Movement. Boolean based
void Player::playerMovement(float game_window_height, const ASGE::GameTime& us)
{
  float direction_y = velocity.y * (getPlayerSpeed() / 200);

  if (rightMovementKey && sprite()->getX() < 10126)
  {
    sprite()->setX(sprite()->getX() + getPlayerSpeed());
    sprite()->getSprite()->setFlipFlags(ASGE::Sprite::NORMAL);
  }
  if (leftMovementKey && sprite()->getX() > 0)
  {
    sprite()->setX(sprite()->getX() - getPlayerSpeed());
    sprite()->getSprite()->setFlipFlags(ASGE::Sprite::FLIP_X);
  }

  if (jumpKey && sprite()->getY() < game_window_height && !jumping && !no_jump)
  {
    jumping = true;
  }

  if (!jumping)
  {
    sprite()->setY(sprite()->getY() + static_cast<float>(direction_y * 10));
    velocity.y += static_cast<float>(getGravityForce());
  }
  else
  {
    sprite()->setY(sprite()->getY() - static_cast<float>(getJumpStrength() + (direction_y * 10)));
    velocity.y -= static_cast<float>(getGravityForce());
  }

  if (velocity.y > 10 && !jumping && !no_jump)
  {
    no_jump = true;
  }

  animationCheck(us);
}

/// This works as a powerUp timer
void Player::updatePowerUp(const ASGE::GameTime& us)
{
  if (star_power_up && star_power_time >= 0)
  {
    star_power_time -= us.deltaInSecs();

    sprite()->setDimensions(16, 16, 6);
  }
  else if (star_power_up && star_power_time < 0)
  {
    star_power_up = false;

    sprite()->setDimensions(16, 16, 3);

    star_power_time = 5;
  }
}

/// If player has movement then call the animation function
void Player::animationCheck(const ASGE::GameTime& us)
{
  if (rightMovementKey || leftMovementKey)
  {
    sprite()->setPlayerAnimationCells(us);
  }
}

/// Player Collision with Collidable tiles
void Player::playerCollision(
  std::vector<std::unique_ptr<ASGE::Sprite>>& collidible_tile, int tile_size, int tile_scale)
{
#pragma unroll(3)
  for (auto& collidibles : collidible_tile)
  {
    collidibles->width(static_cast<float>(tile_size * tile_scale));
    collidibles->height(static_cast<float>(tile_size * tile_scale));

    ///  If Player's AABB is inside collidable tile's AABB then stop movement
    if (sprite()
          ->getBoundingBox(sprite()->getSprite()->scale())
          .collision(
            collidibles->xPos(), collidibles->yPos(), collidibles->width(), collidibles->height()))
    {
      jumping    = false;
      no_jump    = false;
      velocity.y = 0;

      /// If player sprite yPos is more than tile (yPos + 30 {Safe Range}) [Lower]
      if (
        sprite()->getY() + (sprite()->getSprite()->height() * sprite()->getSprite()->scale()) >
          collidibles->yPos() + 30 &&
        collidibles->yPos() < 624)
      {
        /// If player sprite width is touching collidible tile then stop relevant movement
        if (
          sprite()->getX() + (sprite()->getSprite()->width() * sprite()->getSprite()->scale()) <=
          collidibles->xPos() + collidibles->width())
        {
          sprite()->setX(
            collidibles->xPos() -
            (sprite()->getSprite()->width() * sprite()->getSprite()->scale()));

          /// Slide down collidible unless touching ground
          if (
            sprite()->getY() + (sprite()->getSprite()->height() * sprite()->getSprite()->scale()) <
            624)
          {
            sprite()->setY(sprite()->getY() + (getGravityForce() * 4));
          }
        }
        else if (sprite()->getX() >= collidibles->xPos())
        {
          sprite()->setX(collidibles->xPos() + collidibles->width());

          /// Slide down collidible unless touching ground
          if (
            sprite()->getY() + (sprite()->getSprite()->height() * sprite()->getSprite()->scale()) <
            624)
          {
            sprite()->setY(sprite()->getY() + (getGravityForce() * 4));
          }
        }
      }
      /// Else set player's yPos to the top of the sprite to mimick ground
      else
      {
        sprite()->setY(
          collidibles->yPos() - (sprite()->getSprite()->height() * sprite()->getSprite()->scale()));
      }
    }
  }
}

/// Player collision with destructible tiles
void Player::playerDestruction(
  std::vector<std::unique_ptr<ASGE::Sprite>>& destructible_tile, int tile_size, int tile_scale,
  GameObject& power_up)
{
#pragma unroll(3)
  for (auto& destructible : destructible_tile)
  {
    destructible->width(static_cast<float>(tile_size * tile_scale));
    destructible->height(static_cast<float>(tile_size * tile_scale));

    /// If player AABB is colliding with Destructible block's AABB
    if (sprite()
          ->getBoundingBox(sprite()->getSprite()->scale())
          .collision(
            destructible->xPos(),
            destructible->yPos(),
            destructible->width(),
            destructible->height()))
    {
      jumping = false;
      no_jump = true;

      /// If Player sprite's yPos is more than the tile's (yPos - 20 {Safe Range}) [Lower]
      if (sprite()->getY() >= destructible->yPos() + (destructible->height() - 20))
      {
        velocity.y = 0;
        sprite()->setY(destructible->yPos() + destructible->height() + 20);
        sprite()->setY(sprite()->getY() + (getGravityForce() * 2));
        velocity.y -= static_cast<float>(getGravityForce() * 2);

        score += 10;

        int rng = rand() % 25;

        std::cout << "PowerUp RNG was " << rng << std::endl;

        if (rng > 16 && power_up_limiter)
        {
          spawn_star       = true;
          power_up_limiter = false;
          power_up.sprite()->getSprite()->xPos(
            destructible->xPos() + (destructible->width() / 2) + 100);
          power_up.sprite()->getSprite()->yPos(
            destructible->yPos() - (power_up.sprite()->getSprite()->height()) + 100);
          std::cout << " so STAR POWER UP at "
                    << destructible->yPos() - (power_up.sprite()->getSprite()->height())
                    << std::endl;
        }
        else if (rng == 0 && power_up_limiter)
        {
          // addPlayerLife();
          spawn_mushroom   = true;
          power_up_limiter = false;
          power_up.sprite()->getSprite()->xPos(destructible->xPos() + (destructible->width() / 2));
          power_up.sprite()->getSprite()->yPos(
            destructible->yPos() - (power_up.sprite()->getSprite()->height()));
          std::cout << " so LIFE UP POWER UP at "
                    << destructible->yPos() - (power_up.sprite()->getSprite()->height())
                    << std::endl;
        }
        else
        {
          power_up_limiter = true;
          std::cout << " so no power up!" << std::endl;
        }

        destructible->xPos(-100);
        destructible->yPos(-100);
      }
      /// Else if player sprite yPos is more than tile (yPos + 30 {Safe Range}) [Lower]
      else if (
        sprite()->getY() + (sprite()->getSprite()->height() * sprite()->getSprite()->scale()) >
          destructible->yPos() + 30 &&
        destructible->yPos() < 624)
      {
        /// If player sprite width is touching destructible tile then stop relevant movement
        if (
          sprite()->getX() + (sprite()->getSprite()->width() * sprite()->getSprite()->scale()) <=
          destructible->xPos() + destructible->width())
        {
          sprite()->setX(
            destructible->xPos() -
            (sprite()->getSprite()->width() * sprite()->getSprite()->scale()));
          sprite()->setY(sprite()->getY() + (getGravityForce() * 4));
        }
        else if (sprite()->getX() >= destructible->xPos())
        {
          sprite()->setX(destructible->xPos() + destructible->width());
          sprite()->setY(sprite()->getY() + (getGravityForce() * 4));
        }
      }
      /// Else move player sprite to treat destructible tile as a collidible tile
      else
      {
        sprite()->setY(
          destructible->yPos() -
          (sprite()->getSprite()->height() * sprite()->getSprite()->scale()));
        velocity.y = 0;
        no_jump    = false;
      }
    }
  }
}

/// Checks for collision with Enemy's AABB
bool Player::enemyCollision(GameObject& enemy)
{
  return static_cast<bool>(sprite()
                             ->getBoundingBox(sprite()->getSprite()->scale())
                             .collision(
                               enemy.sprite()->getX(),
                               enemy.sprite()->getY(),
                               enemy.sprite()->getSprite()->width(),
                               enemy.sprite()->getSprite()->height()));
}
