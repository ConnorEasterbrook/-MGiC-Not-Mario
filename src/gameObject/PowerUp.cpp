#include "gameObject/PowerUp.h"

void PowerUp::checkCollision(const GameObject& player, bool life_spawn, bool star_spawn)
{
  if (player.sprite()
        ->getBoundingBox(sprite()->getSprite()->scale())
        .collision(
          sprite()->getSprite()->xPos(),
          sprite()->getSprite()->yPos(),
          sprite()->getSprite()->width() * sprite()->getSprite()->scale(),
          sprite()->getSprite()->height() * sprite()->getSprite()->scale()))
  {
    if (life_spawn)
    {
      setLifeUp();
    }
    else if (star_spawn)
    {
      setStarUp();
    }
  }
}

void PowerUp::setLifeUp()
{
  life_power_up = true;
}
bool PowerUp::collectLifeUp()
{
  return life_power_up;
}

void PowerUp::setStarUp()
{
  star_power_up = true;
}
bool PowerUp::collectStarUp()
{
  return star_power_up;
}
