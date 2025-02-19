# Conversor Analógico-Digital (ADC)

Embarcatech: Unidade 4 - Microcontroladores / Capítulo 8 - ADC

## **Descrição**
  Este projeto foi desenvolvido como tarefa na capacitação promovida pela EmbarcaTech e demonstra o uso do módulo de conversão analógico-digital ADC, PWM, display SSD1306 e I2C aplicados ao microcontrolador RP2040 no kit de desenvolvimento BitDogLab.

## **Funcionalidades do projeto**
- O LED RGB iniciará apagado e permanecerá assim quando o eixo X e Y do Joystick estiverem no centro;
- Com a variação do eixo X, o LED Vermelho irá brilhar menos ou mais de acordo com a 'intensidade' do sinal ADC;
- Com a variação do eixo Y, o LED Azul irá brilhar menos ou mais de acordo com a 'intensidade' do sinal ADC;
- Ao pressionar o botão A, o PWM será desativado e a intensidade dos LEDS Azul e Vermelho não irão mais variar. Apenas quando pressionado novamente, o PWM será reativado.
- Ao mesmo tempo um pequeno quadrado branco será exibido no display SSD1306 replicando a movimentação dos eixos X e Y do Joystick;
- Ao pressionar o botão do Joystick, uma animação de borda será exibida no display juntamente com o quadrado.

## **Requisitos pra instalação e execução**
- Ambiente de desenvolvimento VSCode configurado com Pico SDK.
- Kit de desenvolvimento BitDogLab.

## **Vídeo de demonstração**
- https://photos.app.goo.gl/df8aspyo8PeBYQcZ8
