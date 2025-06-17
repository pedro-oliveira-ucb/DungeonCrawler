// Inclui o cabeçalho da classe CBaseEntity.
#include "CBaseEntity.h"

// Inclui cabeçalhos de contêineres da STL.
#include <unordered_map> // Para mapas hash não ordenados.
#include <set>           // Para conjuntos.
#include <cmath>         // Para funções matemáticas como atan2 e M_PI.

// Inclui o cabeçalho para algoritmos da STL, como std::clamp.
#include <algorithm>

// Inclui cabeçalhos de componentes do jogo.
#include "../../../gameObjects/gameMap/gameMap.h" // Para interagir com o mapa do jogo.
#include "../../../../Globals/Globals.h"         // Para acessar variáveis globais do jogo.

// Inclui cabeçalhos de sistemas do jogo.
#include "../../../Managers/collisionManager/collisionManager.h" // Para gerenciamento de colisões.
#include "../../Events/EventManager.h"                         // Para gerenciamento de eventos.
#include "../../../../Utils/Log/Log.h"                           // Para logging.

// Construtor de cópia.
CBaseEntity::CBaseEntity( const CBaseEntity & other ) :
	entityAnimations( other.entityAnimations ) // Inicializa as animações da entidade com as da outra entidade.
{
	// Copia as propriedades da outra entidade para esta entidade.
	this->Name = other.Name;
	this->entityPosition = other.entityPosition;
	this->health = other.health;
	this->entityType = other.entityType;
	this->entityMovementDirection = other.entityMovementDirection;
	this->entityState = other.entityState;
	this->movementsRequest = other.movementsRequest;
	this->lookingAngle = other.lookingAngle;
	this->movementAngle = other.movementAngle;
	this->movementSpeed = other.movementSpeed;
	this->entityHitbox = other.entityHitbox;

	// Registra a chamada do construtor de cópia no log.
	Log::Print( "[%s] Copy constructor called" , this->GetEntityName( ).c_str( ) );
}

// Construtor que utiliza um builder (padrão de projeto Builder).
CBaseEntity::CBaseEntity( CBaseEntityConstructor builder ) :
	entityAnimations( builder.entityAnimations ) // Inicializa as animações da entidade com as do builder.
{
	// Inicializa as propriedades da entidade com os valores do builder.
	this->health = builder.health;
	this->Name = builder.Name;
	this->entityPosition = builder.entityPosition;
	this->entityType = builder.entityType;
	this->entityMovementDirection = builder.entityMovementDirection;
	this->entityAnimations = builder.entityAnimations;
	this->movementSpeed = builder.movementSpeed;
	this->movementAngle = 0.f; // Ângulo de movimento inicializado como 0.
	this->lookingAngle = 0.0f; // Ângulo de visão inicializado como 0.
	this->entityLookingDirection = DIRECTION::LEFT; // Direção de visão inicializada como esquerda.
	this->entityHitbox = builder.entityHitbox;
}

// Verifica se a entidade está correndo.
bool CBaseEntity::isSprinting( ) const {
	std::scoped_lock lock( cBaseMutex ); // Garante exclusão mútua para acesso à variável 'sprinting'.
	return this->sprinting;
}

// Define se a entidade está correndo.
void  CBaseEntity::setSprinting( bool sprinting ) {
	std::scoped_lock lock( cBaseMutex ); // Garante exclusão mútua para acesso à variável 'sprinting'.
	this->sprinting = sprinting;
}

// Adiciona um estado à entidade (usando operações bitwise).
void  CBaseEntity::addEntityState( CBaseEntityState state ) {
	std::scoped_lock lock( cBaseMutex ); // Garante exclusão mútua para acesso à variável 'entityState'.
	entityState |= state; // Adiciona o estado usando OR bitwise.
}

// Remove um estado da entidade (usando operações bitwise).
void  CBaseEntity::removeEntityState( CBaseEntityState state ) {
	std::scoped_lock lock( cBaseMutex ); // Garante exclusão mútua para acesso à variável 'entityState'.
	entityState &= ~state; // Remove o estado usando AND bitwise com o complemento do estado.
}

// Verifica se a entidade possui um determinado estado.
bool  CBaseEntity::hasEntityState( CBaseEntityState state ) const {
	return ( entityState & state ) != 0; // Verifica se o bit correspondente ao estado está ativo.
}

// Limpa todos os estados da entidade.
void  CBaseEntity::clearEntityStates( ) {
	std::scoped_lock lock( cBaseMutex ); // Garante exclusão mútua para acesso à variável 'entityState'.
	entityState = 0; // Define o estado como 0 (nenhum estado ativo).
}

// Retorna os estados atuais da entidade como um inteiro de 32 bits.
std::uint32_t CBaseEntity::getEntityStates( ) const {
	return entityState;
}

// Define os estados da entidade.
void  CBaseEntity::setEntityStates( std::uint32_t states ) {
	std::scoped_lock lock( cBaseMutex ); // Garante exclusão mútua para acesso à variável 'entityState'.
	entityState = states;
}

// Retorna o nome da entidade.
std::string CBaseEntity::GetEntityName( ) const {
	return this->Name;
}

// Retorna a posição da entidade.
GVector2D CBaseEntity::getEntityPosition( ) const {
	std::lock_guard<std::mutex> lock( this->cBaseMutex ); // Garante exclusão mútua para acesso à variável 'entityPosition'.
	return this->entityPosition;
}

// Retorna a vida atual da entidade.
int CBaseEntity::getHealth( ) const {
	std::lock_guard<std::mutex> lock( this->cBaseMutex ); // Garante exclusão mútua para acesso à variável 'health'.
	return this->health;
}

// Retorna o tipo da entidade.
CBaseEntityType CBaseEntity::getEntityType( ) const {
	std::lock_guard<std::mutex> lock( this->cBaseMutex ); // Garante exclusão mútua para acesso à variável 'entityType'.
	return this->entityType;
}

// Retorna o ângulo base da direção de visão da entidade.
float CBaseEntity::getEntityLookingDirectionBaseAngle( ) const {
	switch ( getEntityLookingDirection( ) ) { // Determina o ângulo com base na direção de visão.
	case DIRECTION::FORWARD:
		return 270.f; // Para frente.
	case DIRECTION::BACKWARD:
		return 90.f;  // Para trás.
	case DIRECTION::LEFT:
		return 180.f; // Para a esquerda.
	case DIRECTION::RIGHT:
		return 0.f;   // Para a direita.
	}

	return 0.f; // Retorno padrão caso a direção não seja reconhecida.
}

// Retorna a vida máxima da entidade.
int CBaseEntity::getMaxHealth( ) const {
	return this->maxHealth;
}

// Retorna um ponteiro para as animações da entidade.
CBaseEntityAnimation * CBaseEntity::getEntityAnimations( ) {
	return &this->entityAnimations;
}

// Define a vida atual da entidade.
void CBaseEntity::setHealth( int health ) {
	std::lock_guard<std::mutex> lock( this->cBaseMutex ); // Garante exclusão mútua para acesso à variável 'health'.
	this->health = health;
}

// Define a posição da entidade.
void CBaseEntity::setEntityPosition( GVector2D pos ) {
	std::lock_guard<std::mutex> lock( this->cBaseMutex ); // Garante exclusão mútua para acesso à variável 'entityPosition'.
	this->entityPosition = pos;
}

// Define a estamina atual da entidade, limitando-a entre 0 e a estamina máxima.
void CBaseEntity::setCurrentStamina( float stamina )
{
	std::lock_guard<std::mutex> lock( this->cBaseMutex ); // Garante exclusão mútua.
	this->currentStamina = stamina;
	this->currentStamina = std::clamp( this->currentStamina , 0.0f , this->maxStamina ); // Garante que a estamina esteja dentro dos limites.
}

// Define a estamina máxima da entidade.
void CBaseEntity::setEntityMaxStamina( float stamina )
{
	std::lock_guard<std::mutex> lock( this->cBaseMutex ); // Garante exclusão mútua.
	this->maxStamina = stamina;
}

void CBaseEntity::setEntityMaxHealth( int health ) {
	std::lock_guard<std::mutex> lock( this->cBaseMutex ); // Garante exclusão mútua.
	this->maxHealth = health;
}

// Define a taxa de regeneração de estamina da entidade.
void CBaseEntity::setStaminaRegenRate( float rate )
{
	std::lock_guard<std::mutex> lock( this->cBaseMutex ); // Garante exclusão mútua.
	this->staminaRegenRate = rate;
}

// Define a taxa de perda de estamina da entidade.
void CBaseEntity::setStaminaLossRate( float rate )
{
	std::lock_guard<std::mutex> lock( this->cBaseMutex ); // Garante exclusão mútua.
	this->staminaLossRate = rate;
}

void CBaseEntity::setMovementSpeed( float speed )
{
	std::lock_guard<std::mutex> lock( this->cBaseMutex ); // Garante exclusão mútua.
	this->movementSpeed = speed;
}

void CBaseEntity::setEntityDefense( float defense )
{
	std::lock_guard<std::mutex> lock( this->cBaseMutex ); // Garante exclusão mútua.
	this->defense = defense; // Define a defesa da entidade.
}

// Aplica dano à entidade.
void CBaseEntity::Hit( int damage ) {
	{
		std::lock_guard<std::mutex> lock( this->cBaseMutex ); // Garante exclusão mútua para modificar 'health' e 'beingHit'.
		damage -= static_cast< int >( floor( ( float ) ( damage ) * ( this->defense / 100.f ) ) );
		damage = std::max( 0 , damage ); // Garante que o dano não seja negativo.
		this->health -= damage; // Reduz a vida.
		this->beingHit = true;  // Define que a entidade está sendo atingida.
	}

	// Dispara eventos com base no estado de vida da entidade.
	if ( this->isAlive( ) )
		EventManager::Get( ).Trigger( this->Name + "_hurt" ); // Evento de "ferido".
	else
		EventManager::Get( ).Trigger( this->Name + "_dead" );  // Evento de "morto".
}

// Verifica se a entidade está sendo atingida.
bool CBaseEntity::isBeingHit( ) const {
	std::lock_guard<std::mutex> lock( this->cBaseMutex ); // Garante exclusão mútua.
	return this->beingHit;
}

// Para a animação/estado de "sendo atingido".
void CBaseEntity::stopBeingHit( ) {
	std::lock_guard<std::mutex> lock( this->cBaseMutex ); // Garante exclusão mútua.
	this->beingHit = false;
}

// Retorna a direção de movimento da entidade.
CBaseEntityMovementDirection CBaseEntity::getEntityMovementDirection( ) const {
	return this->entityMovementDirection;
}

// Define a direção de movimento da entidade.
void CBaseEntity::setEntityMovementDirection( CBaseEntityMovementDirection move ) {
	this->entityMovementDirection = move;
}

// Adiciona uma requisição de movimento à fila.
void CBaseEntity::addMoveRequest( CBaseEntityMovementDirection movement ) {
	std::lock_guard<std::mutex> lock( this->cBaseMutex ); // Garante exclusão mútua para acesso a 'movementsRequest'.
	this->movementsRequest.emplace_back( movement );
}

// Retorna o ângulo de movimento da entidade.
float CBaseEntity::getMovementAngle( )const {
	return this->movementAngle;
}

// Limpa todas as requisições de movimento.
void CBaseEntity::clearMovementRequest( ) {
	std::lock_guard<std::mutex> lock( this->cBaseMutex ); // Garante exclusão mútua para acesso a 'movementsRequest'.
	this->movementsRequest.clear( );
}

// Tabela de consulta para vetores de direção. Melhora a performance e a clareza.
const GVector2D directionVectors[ DIRECTION::DIRECTION_COUNT ] = {
	{0.0f, -1.0f}, // FORWARD (Para cima no eixo Y)
	{0.0f, 1.0f},  // BACKWARD (Para baixo no eixo Y)
	{-1.0f, 0.0f}, // LEFT (Para a esquerda no eixo X)
	{1.0f, 0.0f}   // RIGHT (Para a direita no eixo X)
};

// Processa o movimento da entidade.
void CBaseEntity::move( ) {
	std::lock_guard<std::mutex> lock( this->cBaseMutex ); // Garante exclusão mútua para modificar dados da entidade.

	// Calcula o tempo delta desde o último movimento.
	double currentGameTime = Globals::Get( ).getGame( )->getCurrentGameTime( );
	float deltaTime = ( this->lastMoveTime == 0.0 ) ? 0.0f : ( currentGameTime - this->lastMoveTime );
	this->lastMoveTime = currentGameTime; // Atualiza o tempo do último movimento.

	// Se não houver requisições de movimento, retorna.
	if ( this->movementsRequest.empty( ) ) {
		return;
	}

	// Define a velocidade final do movimento.
	float finalMoveSpeed = this->movementSpeed;

	// Dobra a velocidade se a entidade estiver correndo.
	if ( this->entityState & CBaseEntityState::RUNNING ) {
		finalMoveSpeed *= 2.0f;
	}

	GVector2D finalMovement( 0.0f , 0.0f ); // Vetor para o movimento final.

	// Processa as requisições de movimento.
	bool moveFlags[ DIRECTION::DIRECTION_COUNT ] = { false }; // Flags para cada direção.
	for ( const auto & move : this->movementsRequest ) {
		// Verifica se a requisição de movimento é válida e define a flag correspondente.
		if ( move >= CBaseEntityMovementDirection::MOVEMENT_FORWARD && move <= CBaseEntityMovementDirection::MOVEMENT_RIGHT ) {
			moveFlags[ static_cast< int >( move ) ] = true;
		}
	}
	this->movementsRequest.clear( ); // Limpa as requisições após processá-las.

	// Adiciona os vetores de direção correspondentes às flags ativas.
	if ( moveFlags[ DIRECTION::FORWARD ] ) finalMovement += directionVectors[ DIRECTION::FORWARD ];
	if ( moveFlags[ DIRECTION::BACKWARD ] ) finalMovement += directionVectors[ DIRECTION::BACKWARD ];
	if ( moveFlags[ DIRECTION::LEFT ] ) finalMovement += directionVectors[ DIRECTION::LEFT ];
	if ( moveFlags[ DIRECTION::RIGHT ] ) finalMovement += directionVectors[ DIRECTION::RIGHT ];

	// Calcula o comprimento do vetor de movimento.
	float movementLength = finalMovement.length( );

	// Se houver movimento (comprimento > 0).
	if ( movementLength > 0.0f ) {
		// Ajusta a velocidade final pelo deltaTime.
		finalMoveSpeed *= deltaTime;
		// Calcula o vetor de movimento final normalizado e multiplicado pela velocidade.
		finalMovement = GVector2D( finalMovement.x * finalMoveSpeed , finalMovement.y * finalMoveSpeed );

		// Calcula a nova posição.
		GVector2D newPos = this->entityPosition + finalMovement;
		// Verifica se a entidade pode se mover para a nova posição.
		if ( CollisionManager::Get( ).CanMoveTo( this , newPos ) ) {
			// Atualiza a posição da entidade.
			this->entityPosition = newPos;
			// Calcula e define o ângulo de movimento em graus.
			this->movementAngle = std::atan2( finalMovement.y , finalMovement.x ) * ( 180.0f / M_PI );
		}
	}
}

// Verifica se há requisições de movimento pendentes.
bool CBaseEntity::hasMovementRequest( ) const {
	std::lock_guard<std::mutex> lock( this->cBaseMutex ); // Garante exclusão mútua.
	return !this->movementsRequest.empty( );
}

// Método virtual para atualização da lógica da entidade (pode ser sobrescrito por classes derivadas).
void CBaseEntity::updateEntity( ) {
	// Lógica de atualização específica da entidade.
}

// Determina o tipo de animação com base no estado e direção da entidade.
CBaseEntityAnimationType CBaseEntity::getAnimationTypeBasedOnStateAndDirection( uint32_t states , DIRECTION direction ) {
	// Converte a direção em um índice (0 = FORWARD, 1 = BACKWARD, 2 = LEFT, 3 = RIGHT).
	int dirIndex = 0;
	switch ( direction ) {
	case FORWARD:  dirIndex = 0; break;
	case BACKWARD: dirIndex = 1; break;
	case LEFT:     dirIndex = 2; break;
	case RIGHT:    dirIndex = 3; break;
	}

	// Define a prioridade dos estados para determinar a animação.
	// PRIORIDADE: DEAD > HURT > ATTACKING > RUNNING > MOVING > STOPPED

	// Estado: DEAD (Morto)
	if ( states & DEAD ) {
		int idx = static_cast< int >( DEAD_FORWARD ) + dirIndex; // Calcula o índice da animação de morte.
		return static_cast< CBaseEntityAnimationType >( idx );
	}
	// Estado: HURT (Ferido)
	if ( states & HURT ) {
		int idx = static_cast< int >( HURT_FORWARD ) + dirIndex; // Calcula o índice da animação de ferido.
		return static_cast< CBaseEntityAnimationType >( idx );
	}
	// Estado: ATTACKING (Atacando)
	if ( states & ATTACKING ) {
		bool running = states & RUNNING; // Verifica se está correndo.
		bool walking = states & MOVING;  // Verifica se está andando.
		int baseAnim = static_cast< int >( ATTACKING_FORWARD ); // Animação base de ataque.
		if ( running ) {
			baseAnim = static_cast< int >( ATTACKING_RUNNING_FORWARD ); // Animação de ataque correndo.
		}
		else if ( walking ) {
			baseAnim = static_cast< int >( ATTACKING_WALKING_FORWARD ); // Animação de ataque andando.
		}
		int idx = baseAnim + dirIndex; // Calcula o índice final da animação de ataque.
		return static_cast< CBaseEntityAnimationType >( idx );
	}
	// Estado: RUNNING (Correndo)
	if ( states & RUNNING ) {
		int idx = static_cast< int >( RUNNING_FORWARD ) + dirIndex; // Calcula o índice da animação de corrida.
		return static_cast< CBaseEntityAnimationType >( idx );
	}
	// Estado: MOVING (Movendo/Andando)
	if ( states & MOVING ) {
		int idx = static_cast< int >( WALKING_FORWARD ) + dirIndex; // Calcula o índice da animação de caminhada.
		return static_cast< CBaseEntityAnimationType >( idx );
	}
	// Estado: STOPPED (Parado/Idle)
	int idx = static_cast< int >( IDLE_FORWARD ) + dirIndex; // Calcula o índice da animação de parado.
	return static_cast< CBaseEntityAnimationType >( idx );
}

// Verifica se a entidade está viva.
bool CBaseEntity::isAlive( )  const {
	return this->health > 0;
}

// Verifica se a animação de morte terminou.
bool CBaseEntity::deathAnimationFinished( ) const
{
	std::lock_guard<std::mutex> lock( this->cBaseMutex ); // Garante exclusão mútua.
	return this->finishedDeathAnimation;
}

// Define se a animação de morte terminou.
void CBaseEntity::setDeathAnimationFinished( bool finished )
{
	std::lock_guard<std::mutex> lock( this->cBaseMutex ); // Garante exclusão mútua.
	this->finishedDeathAnimation = finished;
}

// Define o ângulo de visão da entidade em graus e atualiza a direção de visão.
void CBaseEntity::setLookingAngle( float degress )
{
	std::lock_guard<std::mutex> lock( this->cBaseMutex ); // Garante exclusão mútua.
	// Se a entidade não estiver viva, não faz nada.
	if ( !this->isAlive( ) )
		return;

	// Determina a direção de visão com base no ângulo.
	if ( degress > -45 && degress <= 45 )
		this->entityLookingDirection = ( DIRECTION::RIGHT );    // Direita
	else if ( degress > 45 && degress <= 135 )
		this->entityLookingDirection = ( DIRECTION::BACKWARD ); // Para trás
	else if ( degress > 135 || degress <= -135 )
		this->entityLookingDirection = ( DIRECTION::LEFT );     // Esquerda
	else
		this->entityLookingDirection = ( DIRECTION::FORWARD );  // Para frente

	this->lookingAngle.setDegrees( degress ); // Define o ângulo de visão.
}

// Retorna o ângulo de visão da entidade.
GAngle CBaseEntity::getLookingAngle( ) const
{
	std::lock_guard<std::mutex> lock( this->cBaseMutex ); // Garante exclusão mútua.
	return this->lookingAngle;
}

int CBaseEntity::getMovementSpeed( ) const
{
	std::lock_guard<std::mutex> lock( this->cBaseMutex ); // Garante exclusão mútua.
	return this->movementSpeed;
}

// Retorna a hitbox da entidade.
CBaseEntityHitbox CBaseEntity::getHitbox( ) const
{
	return this->entityHitbox;
}

// Retorna a direção de visão da entidade.
DIRECTION CBaseEntity::getEntityLookingDirection( ) const
{
	std::lock_guard<std::mutex> lock( this->cBaseMutex ); // Garante exclusão mútua.
	return this->entityLookingDirection;
}

// Retorna a estamina atual da entidade.
float CBaseEntity::getCurrentStamina( ) const
{
	std::lock_guard<std::mutex> lock( this->cBaseMutex ); // Garante exclusão mútua.
	return this->currentStamina;
}

// Retorna a estamina máxima da entidade.
float CBaseEntity::getMaxStamina( ) const
{
	std::lock_guard<std::mutex> lock( this->cBaseMutex ); // Garante exclusão mútua.
	return this->maxStamina;
}

// Retorna a taxa de regeneração de estamina da entidade.
float CBaseEntity::getStaminaRegenRate( ) const
{
	std::lock_guard<std::mutex> lock( this->cBaseMutex ); // Garante exclusão mútua.
	return this->staminaRegenRate;
}

// Retorna a taxa de perda de estamina da entidade.
float CBaseEntity::getStaminaLossRate( ) const
{
	std::lock_guard<std::mutex> lock( this->cBaseMutex ); // Garante exclusão mútua.
	return this->staminaLossRate;
}

float CBaseEntity::getEntityDefense( ) const
{
	std::lock_guard<std::mutex> lock( this->cBaseMutex ); // Garante exclusão mútua.
	return this->defense;
}

// Define a direção de visão da entidade.
void CBaseEntity::setEntityLookingDirection( DIRECTION direction )
{
	std::lock_guard<std::mutex> lock( this->cBaseMutex ); // Garante exclusão mútua.
	this->entityLookingDirection = direction;
}

// Retorna uma string representando os estados atuais da entidade.
std::string CBaseEntity::getEntityStateAsString( ) const {
	std::string result; // String para armazenar os estados.

	// Se nenhum estado estiver definido, retorna "Undefined".
	if ( entityState == 0 ) {
		return "Undefined";
	}

	// Concatena os nomes dos estados ativos.
	if ( entityState & STOPPED ) {
		result += "STOPPED";
	}

	if ( entityState & MOVING ) {
		if ( !result.empty( ) ) result += " | "; // Adiciona separador se necessário.
		result += "MOVING";
	}

	if ( entityState & RUNNING ) {
		if ( !result.empty( ) ) result += " | ";
		result += "RUNNING";
	}

	if ( entityState & ATTACKING ) {
		if ( !result.empty( ) ) result += " | ";
		result += "ATTACKING";
	}

	if ( entityState & HURT ) {
		if ( !result.empty( ) ) result += " | ";
		result += "HURT";
	}

	if ( entityState & DEAD ) {
		if ( !result.empty( ) ) result += " | ";
		result += "DEAD";
	}

	return result; // Retorna a string com os estados.
}

