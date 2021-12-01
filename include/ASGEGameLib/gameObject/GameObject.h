#ifndef NOTMARIOGAME_GAMEOBJECT_H
#define NOTMARIOGAME_GAMEOBJECT_H

#include "../utility/SpriteComponent.h"
#include "../utility/Vector2.h"
#include "iostream"
#include <string>

class GameObject
{
 public:
  GameObject()  = default;
  ~GameObject() = default;

  void goRender(ASGE::Renderer* renderer) const;
  bool addSpriteComponent(
    const std::string& texture_file_path, std::unique_ptr<SpriteComponent> unique_sc,
    ASGE::Renderer* renderer, int z_order);
  [[nodiscard]] const std::unique_ptr<SpriteComponent>& sprite() const;

  bool visibility = false;

 private:
  std::unique_ptr<SpriteComponent> sprite_component = nullptr;
};

#endif // NOTMARIOGAME_GAMEOBJECT_H
