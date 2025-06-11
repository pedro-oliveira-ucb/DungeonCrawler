#pragma once

// Enum para identificar os tipos de estado (opcional, mas �til)
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


class gameStateManager; // Declara��o avan�ada para evitar depend�ncia circular

class gameState {

	float alpha = 255.0f; // Vari�vel para controlar a transi��o entre estados
	float transitionDuration = 2.5; // Velocidade de transi��o 
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

	// Chamado quando o estado � iniciado
	virtual void OnEnter( gameStateManager * manager ) = 0;

	// Chamado quando o estado � finalizado
	virtual void OnExit( gameStateManager * manager ) = 0;

	// Lida com a entrada do usu�rio (teclado, mouse, etc.)
	virtual void HandleInput( gameStateManager * manager ) = 0;

	// Atualiza a l�gica do estado (movimento de entidades, colis�es, etc.)
	// deltaTime � o tempo desde o �ltimo frame
	virtual void Update( gameStateManager * manager , float deltaTime ) = 0;

	// Desenha o estado na tela
	virtual void Render( gameStateManager * manager ) = 0;

	// (Opcional) Retorna o tipo do estado
	virtual gameStateType GetType( ) const = 0;
};