#include "tank.h"

Tank::Tank(std::shared_ptr<sf::Texture> bodyTexture, std::shared_ptr<sf::Texture> barrelTexture)
{
	// Initialise sprites.
	body = std::make_unique<sf::Sprite>(*bodyTexture);
	barrel = std::make_unique<sf::Sprite>(*barrelTexture);

	// Set sprite origins. For body use the center of the texture. For barrel, hardcoded value.
	body->setOrigin((sf::Vector2f)body->getTextureRect().getCenter());
	barrel->setOrigin({ 6, 2 });	

	currentRadius = orbitRadius;

	sf::Vector2f radialDir = {

		std::cos(orbitAngle.asRadians()),
		std::sin(orbitAngle.asRadians())
	};

	position = orbitCenter + radialDir * currentRadius;
	bodyRotation = orbitAngle + sf::degrees(90);

	body->setPosition(position);
	barrel->setPosition(position);

	// Orientación inicial
	//bodyRotation = orbitAngle;
	body->setRotation(bodyRotation);
	barrel->setRotation(bodyRotation);
}

void Tank::Update(float dt)
{
	sf::Vector2f radialDir = {

		std::cos(orbitAngle.asRadians()),
		std::sin(orbitAngle.asRadians()) 
	};

	switch (movementState) {

		case MovementState::Orbiting:
			
			if (isMoving.left) {
				orbitAngle += sf::degrees(angularSpeed * dt);
			}
			if (isMoving.right) {
				orbitAngle -= sf::degrees(angularSpeed * dt);
				//position = orbitCenter + body_direction * currentRadius;
			}
			
			// If pushing input detected, change to PushingIn state.
			if (isMoving.push) {
				movementState = MovementState::PushingIn;
				//position = orbitCenter + body_direction * currentRadius;
			}
			break;


		case MovementState::PushingIn:
			currentRadius -= pushInSpeed * dt;
			if (currentRadius <= minRadius) {

				currentRadius = minRadius;
				//position = orbitCenter + body_direction * currentRadius;
				movementState = MovementState::ReturningOut;
			}
			break;

		case MovementState::ReturningOut:
			currentRadius += returnSpeed * dt;
			if (currentRadius >= orbitRadius) {
				//position = orbitCenter + body_direction * currentRadius;
				currentRadius = orbitRadius;
				movementState = MovementState::Orbiting;
			}
			break;


	}


	radialDir = {
		std::cos(orbitAngle.asRadians() + sf::degrees(90).asRadians()),
		std::sin(orbitAngle.asRadians() + sf::degrees(90).asRadians())
	};
	position = orbitCenter + radialDir * currentRadius;

	// Rotación tangente
	bodyRotation = orbitAngle + sf::degrees(180);
	// Apply new rotation to tank body and barrel.
	body->setRotation(bodyRotation);
	barrel->setRotation(bodyRotation);
	body->setPosition(position);
	barrel->setPosition(position);
}

const void Tank::Render(sf::RenderWindow &window) {
		window.draw(*body);
		window.draw(*barrel);
}
