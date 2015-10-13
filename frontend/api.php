<?php

class PiMobile {

    const PWM_MAX = 1024;

    const GPIO_LEFT_PWM = 19;
    const GPIO_LEFT_A = 6;
    const GPIO_LEFT_B = 13;

    const GPIO_RIGHT_PWM = 18;
    const GPIO_RIGHT_A = 14;
    const GPIO_RIGHT_B = 15;

    private $commandQueue = [];

    public function __construct() {
        return;

        // check if already initialized
        $memcache = new Memcached();
        $memcache->addServer('localhost', 11211);

        if( !$memcache->get('is_pimobile_initialized') ) {
            // init gpio's
            $this->queueGpioCommand('-g mode ' . self::GPIO_LEFT_PWM . ' pwm');
            $this->queueGpioCommand('-g mode ' . self::GPIO_LEFT_A . ' out');
            $this->queueGpioCommand('-g mode ' . self::GPIO_LEFT_B . ' out');

            $this->queueGpioCommand('-g mode ' . self::GPIO_RIGHT_PWM . ' pwm');
            $this->queueGpioCommand('-g mode ' . self::GPIO_RIGHT_A. ' out');
            $this->queueGpioCommand('-g mode ' . self::GPIO_RIGHT_B. ' out');
            $this->setSpeed( self::GPIO_LEFT_PWM, 0 );
            $this->setSpeed( self::GPIO_RIGHT_PWM, 0 );
            $this->executeGpioCommands();
            $memcache->set('is_pimobile_initialized', true);
        }
    }

    /**
     * @return bool
     */
    public function getIsCameraEnabled() {
        @exec('/usr/bin/netstat -tulpn|grep mjpg_streamer',$output,$returnCode);
        return $returnCode === 0;
    }

    /**
     * @param int $gpio GPIO Pin
     * @param int $percentage 0-100
     */
    private function setSpeed( $gpio, $percentage ) {
        $this->queueGpioCommand('-g pwm ' . (int)$gpio, floor( (int)$percentage / 100 * self::PWM_MAX ) );
    }

    /**
     * Set up motor turn direction
     *
     * @param int $gpioA
     * @param int $gpioB
     * @param bool $direction
     */
    private function setDirection( $gpioA, $gpioB, $direction ) {
        $this->queueGpioCommand('-g write ' . (int)$gpioA, (int)$direction );
        $this->queueGpioCommand('-g write ' . (int)$gpioB, (int)!$direction );
    }

    /**
     * @param string $command
     * @param string $parameters
     */
    private function queueGpioCommand( $command, $parameters = '' ) {
        // check memcache if this command is already active
        $ident = sha1( $command );
        $memcache = new Memcached();
        $memcache->addServer('localhost', 11211);
//        echo 'p: ' . $parameters . ' c: ' . $command . ' mc: ' . $memcache->get($ident) . PHP_EOL;
        if( $parameters === '' || ($memcache->get($ident) !== $parameters) ) {
            $this->commandQueue[] = $command . ' ' . $parameters;
            if($parameters !== '') {
                $memcache->set($ident, $parameters);
            }
        }
    }

    /**
     * Executes all gpio commands in the queue
     */
    private function executeGpioCommands() {
        if(count($this->commandQueue)==0) {
            // nothing to do
            return;
        }
        $command = '/usr/bin/gpio ' . implode(' ; /usr/bin/gpio ', $this->commandQueue) . ' ';
        echo $command . PHP_EOL;
        @shell_exec($command);
        $this->commandQueue = [];
    }

    /**
     * @param int $speedVector     -100 (reverse) to 100
     * @param int $directionVector -100 (full left) to 100 (full right)
     */
    public function steer( $speedVector, $directionVector ) {
//        $start = microtime(true);
        $speedLeftMotor = $speedRightMotor = $speed = abs( (int)$speedVector );

        $directionFactor = abs( (int)$directionVector );
        $directionLeftMotor = $directionRightMotor = (bool)max(0, (int)$speedVector + 1);

        // now calculate motor speeds for left and right motor based on $rawSpeed and $direction to enable turning
        // ignore values below 5
        if( $directionFactor > 5 ) {
            if( $directionFactor < 50 ) {
                // values below 50 gradually reduce speed of the opposite motor
                $speedOppositeMotor = (int)floor( $speed - $directionFactor / 50 * $speed );
            } else {
                // values above 50 invert motor direction on opposite motor with increasing speeds
                $speedOppositeMotor = (int)floor( ($directionFactor - 50) / 50 * $speed );

                // now invert motor direction
                if( $directionVector > 0 ) {
                    // turn right
                    $directionRightMotor = !$directionRightMotor;
                } else {
                    $directionLeftMotor = !$directionLeftMotor;
                }
            }

            if( $directionVector > 0 ) {
                // turn right
                $speedRightMotor = $speedOppositeMotor;
            } else {
                // turn left
                $speedLeftMotor = $speedOppositeMotor;
            }
        }

//        var_dump(  $speedVector );
//        var_dump(  $directionVector );
//        var_dump( $directionLeftMotor );
//        var_dump( $directionRightMotor );
//        var_dump( $speedLeftMotor );
//        var_dump( $speedRightMotor );

        $this->setSpeed( self::GPIO_LEFT_PWM, $speedLeftMotor );
        $this->setSpeed( self::GPIO_RIGHT_PWM, $speedRightMotor );

        $this->setDirection( self::GPIO_LEFT_A, self::GPIO_LEFT_B, $directionLeftMotor );
        $this->setDirection( self::GPIO_RIGHT_A, self::GPIO_RIGHT_B, $directionRightMotor );

        $this->executeGpioCommands();

//        var_dump('duration: ' . (microtime(true)-$start));
    }

    /**
     * Process API request
     */
    public function processRequest() {
        $response = [
            'status' => 'unknown method.',
        ];

        if( isset( $_POST['speed'] ) && isset( $_POST['direction'] ) ) {
            $this->steer($_POST['speed'], $_POST['direction']);
            $response['status'] = 'ok';
        } else if( isset( $_POST['camera'] ) ) {
            // Webcam aktivieren / deaktivieren
            if( $_POST['camera'] ) {
                if(!$this->getIsCameraEnabled()) {
                    // Webcam aktivieren falls nicht bereits aktiv
                    @shell_exec('/usr/bin/mjpg_streamer -i "input_uvc.so -d /dev/video0 -f 20 -r 320x240 -q 60 -y" -o "output_http.so -p 8080 -w /usr/share/mjpeg-streamer/www/" > /dev/null 2>&1 &');
                }
                $response['status'] = 'ok';
                $response['reloadCameraImage'] = true;
            } else {
                // Webcam deaktivieren
                @shell_exec('/usr/bin/killall mjpg_streamer');
                $response['status'] = 'ok';
            }
            $response['test'] = $this->getIsCameraEnabled();
        }

        echo json_encode( $response );
    }
}

$piMobile = new PiMobile();
$piMobile->processRequest();
