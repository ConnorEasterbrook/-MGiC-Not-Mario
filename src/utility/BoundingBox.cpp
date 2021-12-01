#include "utility/BoundingBox.h"

bool BoundingBox::collision(
  float object_xPos, float object_yPos, float object_width, float object_height) const
{
  return this->xPos<object_xPos + object_width&& this->xPos + this->width> object_xPos &&
         this->yPos<object_yPos + object_height&& this->yPos + this->height> object_yPos;
}

[[maybe_unused]] bool BoundingBox::isBetween(float value, float min, float max) const
{
  return (value >= min) && (value <= max);
}
