
<p align="center">
  <img src="https://img.shields.io/badge/GAMEPROJECT-2D%20CUSTOM%20ENGINE-1f1f1f?style=for-the-badge&logo=raylib&logoColor=white" alt="GAMEPROJECT Logo">
</p>

<h1 align="center">🎮 Dungeon Crawler</h1>
<p align="center">
  <em>Jogo 2D em C++ com Raylib — motor de jogo personalizado, arquitetura modular e desempenho otimizado.</em>
</p>

<p align="center">
  <img src="https://img.shields.io/badge/C++-00599C?style=for-the-badge&logo=cplusplus&logoColor=white">
  <img src="https://img.shields.io/badge/Raylib-1f1f1f?style=for-the-badge&logo=raylib&logoColor=white">
  <img src="https://img.shields.io/badge/Custom%20Game%20Engine-4B8BBE?style=for-the-badge">
  <img src="https://img.shields.io/badge/State%20Machine-2F855A?style=for-the-badge">
  <img src="https://img.shields.io/badge/Multithreaded-553C9A?style=for-the-badge">
</p>

---

**Dungeon Crawler** é um jogo 2D desenvolvido em **C++** com **Raylib**, utilizando um **motor de jogo personalizado**.  
Sua arquitetura é organizada em módulos independentes, com gerenciamento de estados, processamento multithread e renderização otimizada.  
O projeto segue padrões clássicos de desenvolvimento de jogos, oferecendo um fluxo robusto e escalável.

---

## 🚀 Funcionalidades

- 🖥 **Motor de jogo customizado** – Sem uso de engines prontas como Unity ou Godot.
- 🧩 **Gerenciamento de Estados (State Machine)** – Menu, gameplay, game over.
- ⚡ **Execução Multithread** – Separação entre lógica, IA e renderização para melhor desempenho.
- 🗺 **Geração Procedural de Mapas** – Criação dinâmica de cenários.
- 🎨 **Renderização em Camadas** – Mapa, entidades, HUD, menus e efeitos visuais.
- 🎯 **Sistema de Entidades e Componentes** – Estrutura modular para jogadores, inimigos, itens e ataques.
- 🔊 **Suporte a Áudio e Música** – Implementado via Raylib.

---

## 📂 Estrutura do Projeto

```bash
gameProject/
├── Game/ # Lógica principal e sistemas do jogo
│ ├── gameInitializer/ # Inicialização de recursos, entidades e mapa
│ ├── gameRender/ # Renderização (mapa, HUD, entidades, menus)
│ ├── gameThreads/ # Threads de atualização paralela
│ ├── gameState/ # Estados do jogo (menu, gameplay, game over)
│ ├── Handlers/ # Controle de entidades, ataques e itens
│ ├── Managers/ # Gerenciamento de recursos, colisões e estados
│ ├── SDK/ # Classes base e utilitários fundamentais
│ └── gameObjects/ # Objetos principais (player, inimigos, etc.)
├── Globals/ # Dados globais e configurações
├── Process/ # Gerenciamento de threads
├── Utils/ # Funções e ferramentas auxiliares
├── main.cpp # Ponto de entrada (loop principal)
```

---

## 🛠 Tecnologias Utilizadas

- **Linguagem:** C++ (`.cpp`, `.hpp`, `.h`)
- **Biblioteca Gráfica e Multimídia:** [Raylib](https://www.raylib.com/)
- **Arquitetura:** Motor de jogo personalizado
- **Padrões de Projeto:**
  - Singleton
  - State Machine
  - ECS (Entity-Component System) – parcialmente implementado

---

## 🖥 Como Compilar e Executar

### 1️⃣ Pré-requisitos
- [Raylib](https://www.raylib.com/) instalado e configurado.
- Compilador C++ (GCC, Clang ou MSVC).
- Visual Studio (opcional, projeto inclui `.sln`).

### 2️⃣ Compilação (GCC/Clang)
```sh
g++ -std=c++17 -I<raylib_include_path> -L<raylib_lib_path> -o game main.cpp -lraylib
```

### 3️⃣ Execução
```sh
./game
```
---
## 📈 Possíveis Melhorias
```
🔹 Implementar Injeção de Dependência para reduzir acoplamento.
🔹 Substituir sistema de colisão por Box2D ou Chipmunk2D.
🔹 Adicionar sistema de scripts (Lua) para acelerar o desenvolvimento.
🔹 Evoluir arquitetura para um ECS puro (ex.: EnTT).
🔹 Criar editor de níveis para mapas manuais.
```
---

* 💡 GAMEPROJECT foi criado como um estudo de arquitetura de motores de jogo e pode servir como base para projetos 2D complexos.*
