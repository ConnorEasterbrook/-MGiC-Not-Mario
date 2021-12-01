#include "gameObject/Enemy.h"

float Enemy::getSpeed() const
{
  return speed;
}

[[maybe_unused]] void Enemy::setSpeed(float playerSpeed)
{
  speed = playerSpeed;
}

[[maybe_unused]] int Enemy::getEnemyCount() const
{
  return enemy_amount;
}
void Enemy::setEnemyCount(int enemy_count)
{
  enemy_amount = enemy_count;
}

/// Controls Enemy movement
void Enemy::getMovement(Player& player)
{
  float wander_range = 300;
  player.score += 0;

  /// If the player AABB is within the Enemy's left wander_range then move towards the player
  /// location
  if (
    player.sprite()->getX() +
        (player.sprite()->getSprite()->width() * player.sprite()->getSprite()->scale()) >
      x_pos - wander_range &&
    player.sprite()->getX() +
        (player.sprite()->getSprite()->width() * player.sprite()->getSprite()->scale()) <
      x_pos)
  {
    sprite()->setX(sprite()->getX() - (getSpeed()));
  }
  /// If the player AABB is within the Enemy's right wander_range then move towards the player
  /// location
  else if (player.sprite()->getX() < x_pos + wander_range && player.sprite()->getX() > x_pos)
  {
    sprite()->setX(sprite()->getX() + (getSpeed()));
  }
  /// If the player AABB is outside the Enemy's wander_range then move normally
  else if (
    player.sprite()->getX() > x_pos + wander_range ||
    player.sprite()->getX() +
        (player.sprite()->getSprite()->width() * player.sprite()->getSprite()->scale()) <
      x_pos - wander_range)
  {
    /// Right movement
    if (!enemy_direction)
    {
      sprite()->setX(sprite()->getX() + (getSpeed()));

      if (sprite()->getX() > x_pos + wander_range && !enemy_direction)
      {
        enemy_direction = true;
      }
    }
    /// Left movement
    else
    {
      sprite()->setX(sprite()->getX() - (getSpeed()));

      if (sprite()->getX() < x_pos - wander_range && enemy_direction)
      {
        enemy_direction = false;
      }
    }
  }
}

/// For each instance created within the main class, set the xPos of the Enemy
void Enemy::setXPos(float xPos)
{
  x_pos = xPos;
}
