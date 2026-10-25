# Media Center

Projeto pessoal de um media center para Linux, desenvolvido para transformar um mini PC em um servidor de reprodução de mídia conectado a uma TV via HDMI.

A proposta é utilizar o celular como controle remoto e interface de gerenciamento, permitindo navegar pela biblioteca de filmes, iniciar a reprodução e controlar o player pelo navegador.

## Arquitetura

- **C e GStreamer:** reprodução e controle de áudio e vídeo.
- **Unix Domain Sockets:** comunicação entre o servidor de mídia e os processos locais.
- **Python e Flask:** API HTTP que recebe as solicitações da interface.
- **JavaScript, HTML e CSS:** interface web responsiva para dispositivos móveis.
- **SQLite:** armazenamento dos metadados da biblioteca de mídia.

## Funcionalidades implementadas

- [x] Reprodução de vídeos por meio do GStreamer.
- [x] Comunicação entre processos via socket Unix.
- [x] API HTTP para integração com o player.
- [x] Consulta do estado, posição e duração da reprodução.
- [x] Controles de pausa e retomada.
- [x] Avanço, retrocesso e busca por posição.
- [x] Interface web para controle do player.
- [ ] Biblioteca de mídia completa integrada ao banco de dados.
- [ ] Descoberta e cadastro automático de arquivos de mídia.
- [ ] Melhorias de estabilidade e tratamento de erros.
- [ ] Instalação e inicialização automatizadas no mini PC.

## Como funciona

O mini PC executa o servidor de mídia e fica conectado à TV via HDMI.

O usuário acessa a interface web pelo celular. A aplicação Flask recebe os comandos HTTP e os encaminha ao servidor C por meio de um socket Unix. O servidor C controla a reprodução utilizando o GStreamer.

## Estado do projeto

**Em desenvolvimento.**

O projeto está sendo desenvolvido incrementalmente, com foco em Linux, programação de sistemas, integração entre processos e desenvolvimento de aplicações de mídia.

## Tecnologias

C · GStreamer · Python · Flask · JavaScript · HTML · CSS · SQLite · Linux · CMake

## Objetivos futuros

- Aprimorar a experiência de uso em celulares.
- Desenvolver uma biblioteca de mídia persistente.
- Facilitar a instalação e execução no mini PC.
- Melhorar o gerenciamento e a descoberta de arquivos.
- Tornar a comunicação e o controle de reprodução mais robustos.
