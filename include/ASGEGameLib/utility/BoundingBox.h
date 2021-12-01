#ifndef NOTMARIOGAME_BOUNDINGBOX_H
#define NOTMARIOGAME_BOUNDINGBOX_H

struct BoundingBox
{
  [[nodiscard]] bool
  collision(float object_xPos, float object_yPos, float object_width, float object_height) const;
  [[maybe_unused]] [[nodiscard]] bool isBetween(float value, float min, float max) const;

  float xPos   = 0;
  float yPos   = 0;
  float width  = 0;
  float height = 0;
} __attribute__((aligned(16)));

#endif // NOTMARIOGAME_BOUNDINGBOX_H
