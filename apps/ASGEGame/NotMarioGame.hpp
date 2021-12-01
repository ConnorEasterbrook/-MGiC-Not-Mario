#pragma once

#include <ASGEGameLib/GComponent.hpp>
#include <ASGEGameLib/gameObject/Enemy.h>
#include <ASGEGameLib/gameObject/GameObject.h>
#include <ASGEGameLib/gameObject/Player.h>
#include <ASGEGameLib/gameObject/PowerUp.h>
#include <ASGEGameLib/utility/SpriteComponent.h>
#include <ASGEGameLib/utility/Vector2.h>
#include <Engine/FileIO.hpp>
#include <Engine/Logger.hpp>
#include <Engine/OGLGame.hpp>
#include <Engine/Sprite.hpp>
#include <random>
#include <stack>
#include <tmxlite/Map.hpp>
#include <tmxlite/TileLayer.hpp>
#include <tmxlite/Tileset.hpp>
#include <utility>
#include <vector>

class NotMarioGame : public ASGE::OGLGame
{
 public:
  explicit NotMarioGame(const ASGE::GameSettings& settings);
  ~NotMarioGame() override;
  NotMarioGame(const NotMarioGame&) = delete;
  NotMarioGame& operator=(const NotMarioGame&) = delete;

 private:
  void initCamera();
  void initText();
  void initMenu();
  void initGame();
  void initTileInit();
  void initTileMap();
  void initTiles(const auto& layer, const auto* tile, int x, int y);
  void initPlayer();
  void initEnemy();
  void initEnd();

  void keyHandler(ASGE::SharedEventData data);
  void menuKeyHandler(const auto* key);
  void playerKeyHandler(const ASGE::KeyEvent& key);
  void clickHandler(ASGE::SharedEventData data);

  void update(const ASGE::GameTime& us) override;
  void updateGame(const ASGE::GameTime& us);
  void updatePlayer(const ASGE::GameTime& us);
  void joystickMenuInput(auto joystick);
  void joystickGameInput(auto joystick);
  void enemyController(const ASGE::GameTime& us);

  void render(const ASGE::GameTime& us) override;
  void renderMenu();
  void renderGame();
  void renderGameUI();
  void renderFinish();

  void setupInitSprites(
    std::unique_ptr<ASGE::Sprite>& sprite, const std::string& texture_file_path, float width,
    float height, int16_t z_order, float x_pos, float y_pos, float scale);
  void tileSpriteSetup(const auto* tile, auto& sprite, int x, int y);
  void
  setupTextRender(ASGE::Text& text, int16_t z_order, float x_pos, float y_pos, ASGE::Colour colour);

  [[maybe_unused]] std::vector<std::unique_ptr<GameComponent>> game_components;
  std::string key_callback_id{};
  std::string mouse_callback_id{};

  float game_window_width  = static_cast<float>(ASGE::SETTINGS.window_width);
  float game_window_height = static_cast<float>(ASGE::SETTINGS.window_height);

  ASGE::Camera camera;
  ASGE::Camera camera2;
  tmx::Map map;
  int font_index{};

  // MENU STATES
  bool in_menu = true;
  bool in_game = false;

  // MENU VARIABLES
  int menu_option                          = 0;
  std::unique_ptr<ASGE::Sprite> menu_bg    = nullptr;
  std::unique_ptr<ASGE::Sprite> menu_icon  = nullptr;
  std::unique_ptr<ASGE::Sprite> play_dark  = nullptr;
  std::unique_ptr<ASGE::Sprite> play_light = nullptr;
  std::unique_ptr<ASGE::Sprite> exit_dark  = nullptr;
  std::unique_ptr<ASGE::Sprite> exit_light = nullptr;

  // TILE VARIABLES
  int tile_size       = 16;
  int tile_scale      = 3;
  int collidible_cc   = 0;
  int destructible_cc = 0;
  int decoration_cc   = 0;
  std::vector<std::unique_ptr<ASGE::Sprite>> decoration_tiles;
  std::vector<std::unique_ptr<ASGE::Sprite>> collidible_tile;
  std::vector<std::unique_ptr<ASGE::Sprite>> destructible_tile;

  // GAMEPLAY STATES
  bool lock_screen = false;

  // GAMEPLAY VARIABLES
  Player player;
  Enemy enemy[8];
  int enemy_count = 8;

  PowerUp power_up;
  Vector2 velocity = { 0, 0 };
  ASGE::Text score_text{};

  // END SCREEN VARIABLES
  bool in_end_win                          = false;
  bool in_end_lose                         = false;
  std::unique_ptr<ASGE::Sprite> end_screen = nullptr;
  ASGE::Text end_text{};
};
