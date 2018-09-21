# Boberto

![alt Boberto](./readme_imgs/boberto.png)

Projeto desenvolvido para captura e classificação dos gestos de numerais.

## Capturando gestos

Para a captura dos números foi desenvolvido um programa que faz o _tracking_ de uma _tag_ aruco e gera uma imagem com o gesto feito.

[Código](./colect_data/main.cpp)

## Classificador

Após serem capturados os dados foi treinada uma SVM para a classificação do número corespondende ao gesto que o úsuario está fazendo.

[Código](./classefier/SVM.py)

## Utilitários

Foram desenvolvidos alguns utilitarios durante o projeto:

* [Script para girar as imagens](./utils/flip.py)
* [Script para mudar a resolução das imagens](./utils/changeResolution.py)