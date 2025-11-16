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
	prevPosition = position;
	
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
				prevPosition = position;
				orbitAngle += sf::degrees(angularSpeed * dt);
			}
			if (isMoving.right) {
				prevPosition = position;
				orbitAngle -= sf::degrees(angularSpeed * dt);
				//position = orbitCenter + body_direction * currentRadius;
			}
			
			// If pushing input detected, change to PushingIn state.
			if (isMoving.push) {
				prevPosition = position;
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
			sf::Vector2f toTank = prevPosition - orbitCenter;
			float angleRad = std::atan2(toTank.y, toTank.x);
			orbitAngle = sf::radians(angleRad);
			currentRadius += returnSpeed * dt;

			if (hits > 2.0f) {
				if (currentRadius >= orbitRadius) {
					currentRadius = orbitRadius;


					movementState = MovementState::Orbiting;
				}
			}
			if (hits > 1.0f) {
				if (currentRadius >= orbitRadius2) {
					currentRadius = orbitRadius2;


					movementState = MovementState::Orbiting;
				}
			}
			if (hits > 0.0f) {
				if (currentRadius >= orbitRadius3) {
					currentRadius = orbitRadius3;


					movementState = MovementState::Orbiting;
				}
			}
			break;


	}


	radialDir = {
		std::cos(orbitAngle.asRadians()),
		std::sin(orbitAngle.asRadians())
	};

	position = orbitCenter + radialDir * currentRadius;
	
	if (!hasInitialPrev)
	{
		prevPosition = position;
		hasInitialPrev = true;
	}


	// Rotación tangente
	bodyRotation = orbitAngle + sf::degrees(90);
	// Apply new rotation to tank body and barrel.
	body->setRotation(bodyRotation);
	barrel->setRotation(bodyRotation);
	body->setPosition(position);
	barrel->setPosition(position);
}

//ES PER A QUAN CHOQUEN ORBITANT (ES POT LLEVAR A POSTERIORI)
void Tank::ApplyCollisionCorrection(const sf::Vector2f& correction)
{
	// 1) Mover la posición
	position += correction;

	// 2) Recalcular radio y ángulo de la órbita a partir de la nueva posición
	sf::Vector2f toTank = position - orbitCenter;
	float len2 = toTank.x * toTank.x + toTank.y * toTank.y;
	float len = std::sqrt(len2);
	if (len == 0.f)
	{
		// Evitar NaN: si por alguna razón quedó exactamente en el centro
		len = 1.f;
		toTank = { 1.f, 0.f };
	}

	currentRadius = len;
	float angleRad = std::atan2(toTank.y, toTank.x);
	orbitAngle = sf::radians(angleRad);

	// 3) Actualizar rotación y sprites  // el offset que estés usando
	bodyRotation = orbitAngle + sf::degrees(90);
	body->setRotation(bodyRotation);
	barrel->setRotation(bodyRotation);
	body->setPosition(position);
	barrel->setPosition(position);
}


const void Tank::Render(sf::RenderWindow &window) {
		window.draw(*body);
		window.draw(*barrel);


			sf::CircleShape colliderShape;
			colliderShape.setRadius(colliderRadius);
			colliderShape.setOrigin({colliderRadius, colliderRadius});
			colliderShape.setPosition(position);
			colliderShape.setFillColor(sf::Color(255, 0, 0, 40)); // rojo transparente
			colliderShape.setOutlineColor(sf::Color::Red);
			colliderShape.setOutlineThickness(2.f);
			window.draw(colliderShape);
		
}
