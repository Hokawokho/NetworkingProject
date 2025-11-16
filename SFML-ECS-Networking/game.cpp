#include "game.h"
#include "utils.h"

Game::Game()
{
	LoadTextures();
	// Initialis sprite for background.
	background = std::make_unique<sf::Sprite>(*textures["ground_sand"]);
	background->setTextureRect(sf::IntRect({ 0, 0 }, { 640, 480 }));

	// We're not loading the tank by default any more.
}

void Game::LoadTextures() {
	textures["ground_sand"] = std::make_shared<sf::Texture>("Assets/tileSand1.png");
	textures["ground_sand"]->setRepeated(true);

	textures["barrel_black"] = std::make_shared<sf::Texture>("Assets/blackBarrel.png");
	textures["body_black"] = std::make_shared<sf::Texture>("Assets/blackTank.png");
	textures["barrel_blue"] = std::make_shared<sf::Texture>("Assets/blueBarrel.png");
	textures["body_blue"] =	std::make_shared<sf::Texture>("Assets/blueTank.png");
	textures["barrel_green"] = std::make_shared<sf::Texture>("Assets/greenBarrel.png");
	textures["body_green"] = std::make_shared<sf::Texture>("Assets/greenTank.png");
}


void Game::ResolveTankCollisions()
{
	for (std::size_t i = 0; i < tanks.size(); ++i)
	{
		for (std::size_t j = i + 1; j < tanks.size(); ++j)
		{
			auto& a = *tanks[i];
			auto& b = *tanks[j];

			sf::Vector2f ca = a.GetColliderCenter();
			sf::Vector2f cb = b.GetColliderCenter();

			sf::Vector2f diff = cb - ca;
			float dist2 = diff.x * diff.x + diff.y * diff.y;

			float ra = a.GetColliderRadius();
			float rb = b.GetColliderRadius();
			float minDist = ra + rb;
			float minDist2 = minDist * minDist;

			if (dist2 < minDist2)  // hay colisión
			{
				//Utils::printMsg("Colisión entre tanque " + std::to_string(i) + " y tanque " + std::to_string(j));

				float dist = std::sqrt(dist2);
				if (dist == 0.f)
				{
					// Están exactamente encima: fuerza una dirección arbitraria
					diff = { 1.f, 0.f };
					dist = 1.f;
				}

				sf::Vector2f normal = diff / dist;              // de A hacia B
				float penetration = minDist - dist;             // lo que se solapan

				// Caso 1: ambos orbitando -> se separan y se sincroniza la órbita
				if (a.IsOrbiting() && b.IsOrbiting())
				{
					/*Utils::printMsg("Tanque VOLVIENDO SI QUE ENTRA");*/
					sf::Vector2f correction = normal * (penetration / 2.f);
					a.ApplyCollisionCorrection(-correction);
					b.ApplyCollisionCorrection(correction);

				}
				else
				{
					// Si alguno está empujando hacia dentro, forzarlo a volver
					/*if (a.GetMovementState() == Tank::MovementState::PushingIn) {
						a.ForceReturnOut(); 
						Utils::printMsg("Tanque VOLVIENDO SI QUE ENTRA 23232323");
					
					}
					if (b.IsPushingIn()) b.ForceReturnOut();*/

					if (a.GetMovementState() != Tank::MovementState::PushingIn && b.GetMovementState() == Tank::MovementState::PushingIn)
					{
						//b.ApplyCollisionCorrection(normal * penetration);
						//b.ForceReturnOut();
						Utils::printMsg("Tanque " + std::to_string(i) + " impactado. Vida restante: " + std::to_string(a.hits));
						a.LowerHit();
						b.ForceReturnOut();
						a.ForceReturnOut();
						// si B fuera a necesitar sincronizar órbita, hacerlo; si no, no hace daño
						//if (b.IsOrbiting()) b.ForceReturnOut();
					}
					else if (a.GetMovementState() == Tank::MovementState::PushingIn && b.GetMovementState() != Tank::MovementState::PushingIn)
					{
						//a.ApplyCollisionCorrection(-normal * penetration);
						//a.ForceReturnOut();
						Utils::printMsg("Tanque " + std::to_string(j) + " impactado. Vida restante: " + std::to_string(b.hits));
						b.LowerHit();
						a.ForceReturnOut();
						b.ForceReturnOut();
						//if (a.IsOrbiting()) a.ForceReturnOut();
					}
					else if (a.GetMovementState() == Tank::MovementState::PushingIn && b.GetMovementState() == Tank::MovementState::PushingIn)
					{
						b.ForceReturnOut();
						a.ForceReturnOut();
					}
					//else
					//{
					//	// Ninguno orbitando: comportamiento clásico (dividir corrección)
					//	sf::Vector2f correction = normal * (penetration / 2.f);
					//	Utils::printMsg("NO ESTA ENTRANDO A LOS ANTERIORES" + std::to_string(a.IsPushingIn()) + "  yyy  " + std::to_string(b.IsPushingIn()));
					//	//a.ApplyCollisionCorrection(-correction);
					//	//b.ApplyCollisionCorrection(correction);
					//}

				}
			}
		}
	}
}



void Game::HandleEvents(const std::optional<sf::Event> event)
{
	// Handle key press events passed from window..
	if (const auto* keyPressed = event->getIf<sf::Event::KeyPressed>()) {
		if (keyPressed->scancode == sf::Keyboard::Scancode::W) {
			tanks.at(0)->isMoving.push = true;
		}
		if (keyPressed->scancode == sf::Keyboard::Scancode::A) {
			tanks.at(0)->isMoving.left = true;
			tanks.at(0)->isMoving.right = false;
		}
		else if (keyPressed->scancode == sf::Keyboard::Scancode::D) {
			tanks.at(0)->isMoving.left = false;
			tanks.at(0)->isMoving.right = true;
		}

		// P2
		if (keyPressed->scancode == sf::Keyboard::Scancode::J)
			tanks.at(1)->isMoving.left = true;
		if (keyPressed->scancode == sf::Keyboard::Scancode::L)
			tanks.at(1)->isMoving.right = true;
		if (keyPressed->scancode == sf::Keyboard::Scancode::I)
			tanks.at(1)->isMoving.push = true;
	}

	// Handle key release events passed from window.
	else if (const auto* keyReleased = event->getIf<sf::Event::KeyReleased>()) {
		if (keyReleased->scancode == sf::Keyboard::Scancode::W)
			tanks.at(0)->isMoving.push = false;
		if (keyReleased->scancode == sf::Keyboard::Scancode::A)
			tanks.at(0)->isMoving.left = false;
		if (keyReleased->scancode == sf::Keyboard::Scancode::D)
			tanks.at(0)->isMoving.right = false;


		// P2
		if (keyReleased->scancode == sf::Keyboard::Scancode::J)
			tanks.at(1)->isMoving.left = false;
		if (keyReleased->scancode == sf::Keyboard::Scancode::L)
			tanks.at(1)->isMoving.right = false;
		if (keyReleased->scancode == sf::Keyboard::Scancode::I)
			tanks.at(1)->isMoving.push = false;
	}
}

void Game::Update(float dt)
{
	for (int i = 0; i < tanks.size(); i++) {
		tanks.at(i)->Update(dt);
	}

	ResolveTankCollisions();
}





// Tank data now includes player id for Observer to update multiple players.
void Game::NetworkUpdate(float dt, TankMessage data) {
	// Force position updates from network data.
	tanks.at(data.id)->position = data.position;
	// Update tank with new position.
	// NOTE: This assumets no inputs were detected and so the tank will only move according to 
	// network updates. This is not ideal and prone to unexpected behaviour if game is extended
	// to be fully multiplayer. 
	tanks.at(data.id)->Update(dt);
}

void Game::Render(sf::RenderWindow& window)
{
	window.draw(*background);
	for (int i = 0; i < tanks.size(); i++) {
		tanks.at(i)->Render(window);
	}
}

void Game::AddTank(std::string body_tex, std::string barrel_tex, sf::Vector2f position, sf::Angle initialAngle)
{
	std::unique_ptr<Tank> tank = std::make_unique<Tank>(Tank(textures[body_tex], textures[barrel_tex]));
	tank->position = position;
	tank->orbitCenter = tank->orbitCenter;
	tank->orbitAngle = initialAngle;
	tank->currentRadius = tank->orbitRadius;

	tanks.push_back(std::move(tank));
}

TankMessage Game::GetNetworkUpdate()
{
	// This is hardcoded for the local tan for Player.
	// First value can be anything as we don't care about player ID for local Player updates.
	// For time use -1.f as a way of checking for errors later. The time can never be negative.
	return {-1.f, 0, tanks.at(0)->position};
}
