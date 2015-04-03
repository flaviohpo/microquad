![http://s18.postimage.org/v8zd3ori1/quadzinho_artistico.jpg](http://s18.postimage.org/v8zd3ori1/quadzinho_artistico.jpg)

Mini quadcopter designed mainly by Flavio Henrique, [Julio César](http://www.feng.pucrs.br/~jclima/), from PUCRS(Brazil) and http://selivre.wordpress.com/author/fanl1234/Felipe A Neves. The first code was based on [KKmultcopter](http://www.kkmulticopter.com), with several improvements, for example the diagnosis display mounted on it. Currently the whole control loop has changed, a real PI controller is used instead of KKmulticopter control loop. We're having much better results using PI controller, sometimes it is hard to notice that the sensoring is gyro based only. The addition of accelerometer in the control loop is still in queue, we're not having much time to work on this project lately.

The goal in this project is to test some control techniques, FPV systems and obviously, to have fun flying and developing!

With only 145g, the first structure flew very well as shown in the pcture below. The only problem we had was about robustness. This structure has been replaced by a new one with landing gears and some other features.

Current we're working on a new flight controller called OVI-C sponsored by [SkyDrones](http://www.skydrones.com.br). It uses a LPC175x, Cortex M3 from NXP which has gyro 3-axis[L3G4200](L3G4200.md), accel 3-axis[LSM303DLHC](LSM303DLHC.md), magnetometer 3-axis, SDcard, display[NOKIA6100](NOKIA6100.md) and USB[Slave](Slave.md). Two extra boards are going to be designed also, one for indoor navigation and one for outdoor navigation.
ESC and radio receiver/transmitter are going to be developed soon.

Check WIKI for more info.

MSP430 and analog Sensors flight controller
![http://s18.postimage.org/ety9rrp7d/IMG_2879.jpg](http://s18.postimage.org/ety9rrp7d/IMG_2879.jpg)
![http://s14.postimage.org/e95gjursh/IMG_2881.jpg](http://s14.postimage.org/e95gjursh/IMG_2881.jpg)

The first flight
![http://s9.postimage.org/g79yr56bz/Sem_t_tulo.png](http://s9.postimage.org/g79yr56bz/Sem_t_tulo.png)

The first frame
![http://s11.postimage.org/k981tlvcj/quadcontrol.png](http://s11.postimage.org/k981tlvcj/quadcontrol.png)

The second frame
![http://s7.postimage.org/i438up6bf/100_1294.jpg](http://s7.postimage.org/i438up6bf/100_1294.jpg)

Miniquad and its little brother
![http://s13.postimage.org/5cbr28ehj/100_1331.jpg](http://s13.postimage.org/5cbr28ehj/100_1331.jpg)

Some screens
![http://s13.postimage.org/5f082t7iv/100_1334.jpg](http://s13.postimage.org/5f082t7iv/100_1334.jpg)
![http://s8.postimage.org/kqfrtwvhx/100_1342.jpg](http://s8.postimage.org/kqfrtwvhx/100_1342.jpg)
![http://s17.postimage.org/bdlsuvdz3/100_1344.jpg](http://s17.postimage.org/bdlsuvdz3/100_1344.jpg)