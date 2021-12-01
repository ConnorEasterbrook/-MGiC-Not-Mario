#include "NotMarioGame.hpp"

/// Initialises the game.
/// Setup your game and initialise the core components.
/// @param settings
NotMarioGame::NotMarioGame(const ASGE::GameSettings& settings) :
  OGLGame(settings),
  key_callback_id(inputs->addCallbackFnc(ASGE::E_KEY, &NotMarioGame::keyHandler, this)),
  mouse_callback_id(inputs->addCallbackFnc(ASGE::E_MOUSE_CLICK, &NotMarioGame::clickHandler, this))
{
  renderer->setClearColour(ASGE::COLOURS::CORNFLOWERBLUE);
  toggleFPS();

  initCamera();
  initText();
  initMenu();
  initGame();
  initEnd();
}

/// Destroys the game.
NotMarioGame::~NotMarioGame()
{
  // this->inputs->unregisterCallback(static_cast<unsigned int>(key_callback_id));
  // this->inputs->unregisterCallback(static_cast<unsigned int>(mouse_callback_id));
  this->inputs->unregisterCallback(key_callback_id);
  this->inputs->unregisterCallback(mouse_callback_id);
}

void NotMarioGame::initCamera()
{
  camera =
    ASGE::Camera{ static_cast<float>(game_window_width), static_cast<float>(game_window_height) };

  std::cout << "INIT CAMERA" << std::endl;
  camera.lookAt(ASGE::Point2D{ static_cast<float>(game_window_width) / 2.F,
                               static_cast<float>(game_window_height) / 2.F });

  camera2 =
    ASGE::Camera{ static_cast<float>(game_window_width), static_cast<float>(game_window_height) };

  camera2.lookAt(ASGE::Point2D{ static_cast<float>(game_window_width) / 2.F,
                                static_cast<float>(game_window_height) / 2.F });
}

void NotMarioGame::initText()
{
  ASGE::FILEIO::File font_file;
  if (font_file.open("data/text/Super-Mario-64-DS.ttf"))
  {
    auto buffer = font_file.read();

    font_index = renderer->loadFontFromMem(
      "Attack", buffer.as_unsigned_char(), static_cast<unsigned int>(buffer.length), 26);
    renderer->setFont(font_index);
    font_file.close();
  }
}

void NotMarioGame::initMenu()
{
  setupInitSprites(menu_bg, "data/images/Menu_Image.jpg", 1280, 720, -2, 0, 0, 1);
  setupInitSprites(menu_icon, "data/images/Menu_Icon.png", 228, 72, -1, 150, 175, 0.75);

  setupInitSprites(play_dark, "data/images/Play_Dark.png", 228, 72, -1, 50, 150, 1);
  setupInitSprites(play_light, "data/images/Play_Light.png", 228, 72, -1, 50, 150, 1);

  setupInitSprites(exit_dark, "data/images/Exit_Dark.png", 228, 72, -1, 50, 232, 1);
  setupInitSprites(exit_light, "data/images/Exit_Light.png", 228, 72, -1, 50, 232, 1);
}

void NotMarioGame::initGame()
{
  power_up.addSpriteComponent(
    "data/images/Life_Up.png", std::make_unique<SpriteComponent>(), renderer.get(), 8);

  power_up.sprite()->setDimensions(16, 16, 2);
  power_up.sprite()->setX(500);
  power_up.sprite()->setY(
    624 - (power_up.sprite()->getSprite()->height() * power_up.sprite()->getSprite()->scale()));

  initTileInit();
  initPlayer();
  initEnemy();
}

void NotMarioGame::initTileInit()
{
  ASGE::FILEIO::File tile_map;
  if (tile_map.open("/data/map/World_Map.tmx"))
  {
    ASGE::FILEIO::IOBuffer buffer = tile_map.read();
    std::string file_string(buffer.as_char(), buffer.length);

    map.loadFromString(file_string, ".");

    initTileMap();
  }

  std::cout << collidible_cc << " collidible blocks." << std::endl;
  std::cout << destructible_cc << " destructible blocks." << std::endl;
  std::cout << decoration_cc << " decoration blocks." << std::endl;
}

void NotMarioGame::initTileMap()
{
  for (const auto& layer : map.getLayers())
  {
    if (layer->getType() == tmx::Layer::Type::Tile)
    {
      auto tile_layer = layer->getLayerAs<tmx::TileLayer>();
      int y           = 0;

      for (unsigned int row = 0; row < layer->getSize().y; row++)
      {
        int x = 0;

#pragma unroll(3)
        for (unsigned int col = 0; col < layer->getSize().x; col++)
        {
          auto tile_info   = tile_layer.getTiles()[row * tile_layer.getSize().x + col];
          const auto* tile = map.getTilesets()[0].getTile(tile_info.ID);

          initTiles(layer, tile, x, y);

          x += tile_size * tile_scale;
        }
        y += tile_size * tile_scale;
      }
    }
  }
}

void NotMarioGame::initTiles(const auto& layer, const auto* tile, int x, int y)
{
  if (tile != nullptr)
  {
    if (layer->getName() == "Ground" || layer->getName() == "Pipes")
    {
      collidible_cc++;

      auto& col_sprite = collidible_tile.emplace_back(renderer->createUniqueSprite());

      if (col_sprite->loadTexture(tile->imagePath))
      {
        tileSpriteSetup(tile, col_sprite, x, y);
      }
    }
    else if (layer->getName() == "Blocks")
    {
      destructible_cc++;

      auto& des_sprite = destructible_tile.emplace_back(renderer->createUniqueSprite());

      if (des_sprite->loadTexture(tile->imagePath))
      {
        tileSpriteSetup(tile, des_sprite, x, y);
      }
    }
    else
    {
      decoration_cc++;

      auto& dec_sprite = decoration_tiles.emplace_back(renderer->createUniqueSprite());

      if (dec_sprite->loadTexture(tile->imagePath))
      {
        tileSpriteSetup(tile, dec_sprite, x, y);
        dec_sprite->scale(static_cast<float>(tile_scale));
      }
    }
  }
}

void NotMarioGame::initPlayer()
{
  player.addSpriteComponent(
    "data/img/mario-luigi.png", std::make_unique<SpriteComponent>(), renderer.get(), 10);

  if (player.sprite()->getSprite() != nullptr)
  {
    player.sprite()->setDimensions(16, 16, 3);
    player.sprite()->setBounds(80, 32);
  }

  player.setPlayerSpawn(70, 600);
  player.teleportPlayerSpawn();
  player.setPlayerLives(2);
}

void NotMarioGame::initEnemy()
{
  enemy->setEnemyCount(enemy_count);

  float add_x = 0;
#pragma unroll(1)
  for (int i = 0; i < enemy_count; i++)
  {
    enemy[i].addSpriteComponent(
      "data/img/enemies.png", std::make_unique<SpriteComponent>(), renderer.get(), 10);

    if (!enemy[i].sprite()->getSprite()->loadTexture("data/img/enemies.png"))
    {
      std::cout << "Enemy texture failed to load." << std::endl;
      return;
    }

    if (enemy[i].sprite()->getSprite() != nullptr)
    {
      enemy[i].sprite()->setDimensions(16, 16, 2);
      enemy[i].sprite()->setBounds(0, 4);

      enemy[i].sprite()->setY(
        624 - (enemy[i].sprite()->getSprite()->height() * enemy[i].sprite()->getSprite()->scale()));

      enemy[i].setSpeed(5);

      enemy[i].sprite()->setX(1000 + add_x);
      enemy[i].setXPos(enemy[i].sprite()->getX());

      add_x += 1000;
    }
  }
}

void NotMarioGame::initEnd()
{
  setupInitSprites(end_screen, "data/images/End_Image.jpg", 1280, 720, -2, 0, 0, 1);
}

/// Processes key inputs.
/// This function is added as a callback to handle the game's keyboard input.
/// For this game, calls to this function are not thread safe, so you may alter
/// the game's state but your code needs to designed to prevent data-races.
/// @param data
/// @see KeyEvent
void NotMarioGame::keyHandler(ASGE::SharedEventData data)
{
  const auto* key = dynamic_cast<const ASGE::KeyEvent*>(data.get());

  if (key->key == ASGE::KEYS::KEY_ESCAPE)
  {
    signalExit();
  }

  if (in_menu)
  {
    menuKeyHandler(key);
  }
  else if (in_game)
  {
    playerKeyHandler(*key);
  }
  else if (in_end_win || in_end_lose)
  {
    if (key->key == ASGE::KEYS::KEY_ENTER && key->action == ASGE::KEYS::KEY_RELEASED)
    {
      in_menu     = true;
      in_end_win  = false;
      in_end_lose = false;
    }
  }
}

void NotMarioGame::menuKeyHandler(const auto* key)
{
  if (key->key == ASGE::KEYS::KEY_ENTER && key->action == ASGE::KEYS::KEY_RELEASED)
  {
    if (menu_option == 0)
    {
      in_menu = false;
      in_game = true;
    }
    else if (menu_option == 1)
    {
      signalExit();
    }
  }
  if (key->key == ASGE::KEYS::KEY_DOWN && key->action == ASGE::KEYS::KEY_RELEASED)
  {
    if (menu_option >= 1)
    {
      menu_option--;
    }
    else
    {
      menu_option++;
    }
  }
  if (key->key == ASGE::KEYS::KEY_UP && key->action == ASGE::KEYS::KEY_RELEASED)
  {
    if (menu_option <= 0)
    {
      menu_option++;
    }
    else
    {
      menu_option--;
    }
  }
}

void NotMarioGame::playerKeyHandler(const ASGE::KeyEvent& key)
{
  if (player.visibility)
  {
    if (key.key == ASGE::KEYS::KEY_LEFT && key.action == ASGE::KEYS::KEY_PRESSED)
    {
      player.leftMovementKey = true;
    }
    if (key.key == ASGE::KEYS::KEY_LEFT && key.action == ASGE::KEYS::KEY_RELEASED)
    {
      player.leftMovementKey = false;
    }
    if (key.key == ASGE::KEYS::KEY_RIGHT && key.action == ASGE::KEYS::KEY_RELEASED)
    {
      player.rightMovementKey = false;
    }
    if (key.key == ASGE::KEYS::KEY_RIGHT && key.action == ASGE::KEYS::KEY_PRESSED)
    {
      player.rightMovementKey = true;
    }
    if (key.key == ASGE::KEYS::KEY_SPACE && key.action == ASGE::KEYS::KEY_PRESSED)
    {
      player.jumpKey = true;
    }
    if (key.key == ASGE::KEYS::KEY_SPACE && key.action == ASGE::KEYS::KEY_RELEASED)
    {
      player.jumpKey = false;
    }

    if (key.key == ASGE::KEYS::KEY_P && key.action == ASGE::KEYS::KEY_RELEASED)
    {
      std::cout << player.star_power_up << std::endl;
    }
  }
}

/// @brief   Processes any click inputs
/// @details This function is added as a callback to handle the game's
///          mouse button input. For this game, calls to this function
///          are thread safe, so you may alter the game's state as you
///          see fit.
/// @param   data The event data relating to key input.
/// @see     ClickEvent
/// @return  void
void NotMarioGame::clickHandler(ASGE::SharedEventData data)
{
  const auto* click = dynamic_cast<const ASGE::ClickEvent*>(data.get());

  double x_pos = click->xpos;
  double y_pos = click->ypos;

  Logging::DEBUG("x_pos: " + std::to_string(x_pos));
  Logging::DEBUG("y_pos: " + std::to_string(y_pos));
}

/// Updates your game and all it's components using a fixed
/// timestep. This allows calculations to resolve correctly and
/// stop physics simulations from imploding
/// https://gamedev.stackexchange.com/questions/1589/when-should-i-use-a-fixed-or-variable-time-step
/// @param us
void NotMarioGame::update(const ASGE::GameTime& us)
{
  camera.update(us);
  auto joystick = inputs->getGamePad(0);

  if (in_game)
  {
    player.visibility = true;

#pragma unroll(1)
    for (int i = 0; i < enemy_count; i++)
    {
      if (enemy[i].enemy_life)
      {
        enemy[i].visibility = true;
      }
    }

    updateGame(us);
    if (joystick.is_connected)
    {
      joystickGameInput(joystick);
    }
  }
  else if (in_menu && joystick.is_connected)
  {
    joystickMenuInput(joystick);
  }
  else if (joystick.is_connected)
  {
    if (joystick.buttons[1] != 0)
    {
      in_end_win  = false;
      in_end_lose = false;
      in_menu     = true;
    }
  }
}

void NotMarioGame::updateGame(const ASGE::GameTime& us)
{
  updatePlayer(us);
  enemyController(us);

  score_text.setString(
    "Player Score: " + std::to_string(player.score) +
    "\nLives: " + std::to_string(player.getPlayerLives()));

  if (player.sprite()->getX() >= 640 && player.sprite()->getX() < 9486 && in_game)
  {
    camera.lookAt(ASGE::Point2D{ static_cast<float>(player.sprite()->getX()),
                                 static_cast<float>(game_window_height) / 2.F });
    lock_screen = true;
  }
  else if (player.sprite()->getX() >= 9486 && in_game)
  {
    camera.lookAt(ASGE::Point2D{ static_cast<float>(game_window_width) / 2.F,
                                 static_cast<float>(game_window_height) / 2.F });
    lock_screen = true;

    in_end_win = true;
    in_game    = false;
  }
  else
  {
    lock_screen = false;
  }

  if (player.getPlayerLives() <= 0)
  {
    in_end_lose = true;
    in_game     = false;
  }
}

void NotMarioGame::updatePlayer(const ASGE::GameTime& us)
{
#pragma unroll(1)
  for (int i = 0; i < enemy_count; i++)
  {
    if (player.enemyCollision(enemy[i]) && enemy[i].enemy_life)
    {
      if (!player.star_power_up && !player.jumping)
      {
        player.minusPlayerLife();

        std::cout << "DEATH CAM" << std::endl;
        camera.lookAt(ASGE::Point2D{ static_cast<float>(game_window_width) / 2.F,
                                     static_cast<float>(game_window_height) / 2.F });
      }
      else
      {
        player.score += 100;
        enemy[i].visibility = false;
        enemy[i].enemy_life = false;
      }
    }
  }

  player.playerUpdate(game_window_height, us);
  player.playerCollision(collidible_tile, tile_size, tile_scale);
  player.playerDestruction(destructible_tile, tile_size, tile_scale, power_up);

  power_up.checkCollision(player, player.spawn_mushroom, player.spawn_star);

  if (player.sprite()->getSprite()->yPos() > game_window_height)
  {
    player.minusPlayerLife();

    std::cout << "DEATH CAM" << std::endl;
    camera.lookAt(ASGE::Point2D{ static_cast<float>(game_window_width) / 2.F,
                                 static_cast<float>(game_window_height) / 2.F });
  }
}

void NotMarioGame::joystickMenuInput(auto joystick)
{
  if (joystick.buttons[0] != 0)
  {
    if (menu_option == 0)
    {
      in_menu = false;
      in_game = true;
    }
    else if (menu_option == 1)
    {
      signalExit();
    }
  }

  if (joystick.axis[1] > -0.2)
  {
    if (menu_option <= 0)
    {
      menu_option = 1;
    }
  }
  if (joystick.axis[1] < 0.2)
  {
    if (menu_option >= 1)
    {
      menu_option = 0;
    }
  }
}

void NotMarioGame::joystickGameInput(auto joystick)
{
  if (joystick.axis[0] > 0.2)
  {
    player.rightMovementKey = true;
    player.leftMovementKey  = false;
  }

  if (joystick.axis[0] < -0.2)
  {
    player.leftMovementKey  = true;
    player.rightMovementKey = false;
  }

  if (joystick.axis[0] >= -0.2 && joystick.axis[0] <= 0.2)
  {
    player.rightMovementKey = false;
    player.leftMovementKey  = false;
  }

  player.jumpKey =
    static_cast<bool>(joystick.buttons[0] != 0 && !player.jumping && !player.no_jump);
}

void NotMarioGame::enemyController(const ASGE::GameTime& us)
{
#pragma unroll(1)
  for (int i = 0; i < enemy_count; i++)
  {
    enemy[i].getMovement(player);
    enemy[i].sprite()->setEnemyAnimationCells(us);
  }
}

/// Simulates the visuals and renders your game scenes.
/// These use a variable time-step, which allows for extra smoothing and
/// interpolation based solely on the framerate of the game.
/// @param us
void NotMarioGame::render(const ASGE::GameTime& /*us*/)
{
  if (in_menu)
  {
    renderMenu();
  }
  else if (in_game)
  {
    renderGame();
    renderGameUI();

    player.goRender(renderer.get());

#pragma unroll(1)
    for (int i = 0; i < enemy_count; i++)
    {
      enemy[i].goRender(renderer.get());
    }

    if (player.spawn_star)
    {
      power_up.sprite()->getSprite()->loadTexture("data/images/Star_Up.png");
      power_up.visibility = true;
      power_up.goRender(renderer.get());

      if (power_up.collectStarUp())
      {
        player.spawn_star      = false;
        player.star_power_up   = true;
        power_up.visibility    = false;
        power_up.star_power_up = false;
        std::cout << "Collected Star" << std::endl;
      }
    }
    if (player.spawn_mushroom)
    {
      power_up.sprite()->getSprite()->loadTexture("data/images/Life_Up.png");
      power_up.visibility = true;
      power_up.goRender(renderer.get());

      if (power_up.collectLifeUp())
      {
        player.spawn_mushroom = false;
        power_up.visibility   = false;
        player.addPlayerLife();
        power_up.life_power_up = false;
        std::cout << "Collected Mushroom" << std::endl;
      }
    }
  }
  else if (in_end_win || in_end_lose)
  {
    renderFinish();
  }
}

void NotMarioGame::renderMenu()
{
  renderer->setProjectionMatrix(camera2.getView());

  renderer->render(*menu_bg);

  if (menu_option == 0)
  {
    renderer->render(*play_light);
    renderer->render(*exit_dark);

    menu_icon->yPos(160);
    renderer->render(*menu_icon);
  }
  else if (menu_option == 1)
  {
    renderer->render(*play_dark);
    renderer->render(*exit_light);

    menu_icon->yPos(242);
    renderer->render(*menu_icon);
  }
}

void NotMarioGame::renderGame()
{
  renderer->setProjectionMatrix(camera.getView());

#pragma unroll(1)
  for (auto& decorations : decoration_tiles)
  {
    decorations->setGlobalZOrder(-1);
    renderer->render(*decorations);
  }

#pragma unroll(1)
  for (auto& collidibles : collidible_tile)
  {
    collidibles->setGlobalZOrder(0);
    renderer->render(*collidibles);
  }

#pragma unroll(1)
  for (auto& destructibles : destructible_tile)
  {
    destructibles->setGlobalZOrder(0);
    renderer->render(*destructibles);
  }
}

void NotMarioGame::renderGameUI()
{
  score_text.setFont(renderer->getFont(font_index));
  score_text.setZOrder(10);

  if (lock_screen)
  {
    score_text.setPositionX(player.sprite()->getX() - 500);
  }
  else
  {
    score_text.setPositionX(146);
  }
  score_text.setPositionY(75);

  score_text.setScale(0.75);
  score_text.setColour(ASGE::COLOURS::BLACK);
  renderer->renderText(score_text);
}

void NotMarioGame::renderFinish()
{
  renderer->setProjectionMatrix(camera2.getView());

  end_text.setFont(renderer->getFont(font_index));
  setupTextRender(end_text, 10, 50, 600, ASGE::COLOURS::YELLOW);

  if (in_end_win)
  {
    end_text.setString("YOU WIN\nPlayer Score: " + std::to_string(player.score));
  }
  else
  {
    end_text.setString("YOU LOSE\nPlayer Score: " + std::to_string(player.score));
  }

  renderer->renderText(end_text);
  renderer->render(*end_screen);
}

void NotMarioGame::setupInitSprites(
  std::unique_ptr<ASGE::Sprite>& sprite, const std::string& texture_file_path, float width,
  float height, int16_t z_order, float x_pos, float y_pos, float scale)
{
  sprite = renderer->createUniqueSprite();
  sprite->loadTexture(texture_file_path);

  if (!sprite->loadTexture(texture_file_path))
  {
    std::cout << "SPRITE FAILED TO INITIALISE" << std::endl;
    return;
  }

  sprite->height(height);
  sprite->width(width);
  sprite->setGlobalZOrder(z_order);
  sprite->xPos(x_pos);
  sprite->yPos(y_pos);
  sprite->scale(scale);

  sprite->setMagFilter(ASGE::Texture2D::MagFilter::NEAREST);
}

void NotMarioGame::tileSpriteSetup(const auto* tile, auto& sprite, int x, int y)
{
  sprite->srcRect()[0] = static_cast<float>(tile->imagePosition.x);
  sprite->srcRect()[1] = static_cast<float>(tile->imagePosition.y);
  sprite->srcRect()[2] = static_cast<float>(tile->imageSize.x);
  sprite->srcRect()[3] = static_cast<float>(tile->imageSize.y);

  sprite->width(static_cast<float>(tile->imageSize.x));
  sprite->height(static_cast<float>(tile->imageSize.y));

  sprite->setMagFilter(ASGE::Texture2D::MagFilter::NEAREST);

  sprite->xPos(static_cast<float>(x));
  sprite->yPos(static_cast<float>(y));
}

void NotMarioGame::setupTextRender(
  ASGE::Text& text, int16_t z_order, float x_pos, float y_pos, ASGE::Colour colour)
{
  text.setZOrder(z_order);
  text.setPositionX(x_pos);
  text.setPositionY(y_pos);
  text.setColour(colour);
}
