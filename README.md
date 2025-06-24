# 🎮 Dungeon Crawler: Demonstração de Thread Safety e Semaforização

## 📋 Sumário

1. [Introdução](#-introdução)
2. [Visão Geral Técnica](#-visão-geral-técnica)
3. [Arquitetura Detalhada](#-arquitetura-detalhada)
   - [Diagrama de Componentes](#diagrama-de-componentes)
   - [Fluxo de Execução](#fluxo-de-execução)
4. [Implementação de Concorrência](#-implementação-de-concorrência)
   - [Thread Safety](#thread-safety)
   - [Semaforização](#semaforização)
   - [Prevenção de Deadlocks](#prevenção-de-deadlocks)
5. [Estrutura do Projeto](#-estrutura-do-projeto)
6. [Game Design](#-game-design)
   - [Mecânicas de Jogo](#mecânicas-de-jogo)
   - [Interface do Usuário](#interface-do-usuário)
   - [Controles Detalhados](#controles-detalhados)
7. [Compilação e Execução](#-compilação-e-execução)
   - [Requisitos do Sistema](#requisitos-do-sistema)
   - [Passo a Passo](#passo-a-passo)
   - [Opções de Configuração](#opções-de-configuração)
8. [Guia de Desenvolvimento](#-guia-de-desenvolvimento)
9. [Resolução de Problemas](#-resolução-de-problemas)
10. [Licença e Créditos](#-licença-e-créditos)

---

## 🔍 Introdução

Este projeto foi desenvolvido para a disciplina de **Sistemas Operacionais**, oferecendo uma implementação prática de conceitos críticos de concorrência em um ambiente de jogo interativo. Através de um **Dungeon Crawler** com gráficos 2D, são demonstrados os desafios e soluções associados à programação multithread em um sistema de entretenimento em tempo real.

O jogo apresenta um ambiente gerado proceduralmente onde o jogador navega através de masmorras, enfrenta inimigos e coleta itens, enquanto componentes de sistema paralelos gerenciam diferentes aspectos da experiência como física, IA, renderização e carregamento de recursos.


## 📜 Licença e Créditos

Este projeto é disponibilizado sob licença educacional para demonstração de conceitos de Sistemas Operacionais.

**Créditos:**
- Raylib: Copyright (c) 2013-2023 Ramon Santamaria (@raysan5)
- Recursos Gráficos: [Créditos de Terceiros]
- Efeitos Sonoros: [Créditos de Terceiros]

---

*Desenvolvido como um projeto acadêmico para demonstração prática de conceitos de Thread Safety e Semaforização em jogos digitais. Não é destinado para fins comerciais.*
