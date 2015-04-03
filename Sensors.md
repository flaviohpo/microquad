# Sensores #

## Giroscópio ##

O giroscópio utilizado precisa ser de aproximadamente 300º/s, isto melhora a sensibilidade no controle. O ponto negativo é que limita a velocidade máxima de giro em 300º/s. Se o quad receber uma perturbação maior q isso ( 300º/s ) o giroscópio vai saturar e podem ocorrer problemas, inclusive queda.

Na primeira versão do hardware foram usados os giroscópios LPR510AL e  LPY510AL na placa com giroscópio analógico e um ITG3205 na placa com giroscópio digital (I2C). O software para a versão digital não foi implementado, somente para os analógicos. Recentemente um dos giroscópios analógicos se tornou obsoleto.

Atualmente estamos utilizando o L3G4200 http://www.st.com/internet/analog/product/250373.jsp, gyro tri-axis SPI/I2C da ST Microelectronics. O desempenho deste sensor tem se mostra muito melhor do que os antigos LPR510AL e LPY510AL. Além do L3G4200, a ST também oferece o L3GD20 http://www.st.com/internet/analog/product/252443.jsp e num futuro próximo o LSM333 http://www.st.com/internet/analog/product/253513.jsp

## Accelerometer ##

O acelerômetro usado na primeira versão da placa foi um MMA7361, que também é analógico.

No OVI-C foi usado o LSM303DLHC http://www.st.com/internet/analog/product/251940.jsp, que possui Acelerômetro e Magnetômetro no mesmo encapsulamento. A comunicação é I2C. O desempenho deste magnetômentro é um relativamente pobre, porém isso não influencia de maneira significativamente o desempenho geral do quadcóptero uma vez que os valores lidos são filtrados e a amostragem não precisa ser tão rápida quanto a do giroscópio.