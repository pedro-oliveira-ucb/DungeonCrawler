#pragma once

// Enum para identificar os tipos de estado (opcional, mas útil)
enum class gameStateType {
	NONE ,
	MAIN_MENU ,
	LOADING ,
	IN_GAME ,
	CUTSCENE ,
	PAUSE_MENU
};
enum class gameStateTransitionState{ 
	NO_TRANSITION,
	EXIT_FINISHED,
	EXIT_IN_PROGRESS ,
	ENTER_FINISHED ,
	ENTER_IN_PROGRESS ,
};


class gameStateManager; // Declaração avançada para evitar dependência circular

class gameState {

	float alpha = 255.0f; // Variável para controlar a transição entre estados
	float transitionDuration = 2.5; // Velocidade de transição 
	bool entering = true;
	bool exiting = false;
	gameStateTransitionState onStateFadeIn( float delta );
	gameStateTransitionState onStateFadeOut( float delta );

public:
	virtual ~gameState( ) = default;
	gameStateTransitionState updateStateTransition( float );
	void renderTransition( gameStateManager * );
	void setExiting( bool );
	void setEntering( bool );

	// Chamado quando o estado é iniciado
	virtual void OnEnter( gameStateManager * manager ) = 0;

	// Chamado quando o estado é finalizado
	virtual void OnExit( gameStateManager * manager ) = 0;

	// Lida com a entrada do usuário (teclado, mouse, etc.)
	virtual void HandleInput( gameStateManager * manager ) = 0;

	// Atualiza a lógica do estado (movimento de entidades, colisões, etc.)
	// deltaTime é o tempo desde o último frame
	virtual void Update( gameStateManager * manager , float deltaTime ) = 0;

	// Desenha o estado na tela
	virtual void Render( gameStateManager * manager ) = 0;

	// (Opcional) Retorna o tipo do estado
	virtual gameStateType GetType( ) const = 0;
};